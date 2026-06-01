#include "BLE_HID.h"

// --------------------------------------------------
// Static members
// --------------------------------------------------
NimBLECharacteristic* BLEGamepad::inputReport = nullptr;
bool BLEGamepad::connected = false;

// --------------------------------------------------
// Callbacks
// --------------------------------------------------
class Callbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer*, NimBLEConnInfo&) override {
        BLEGamepad::connected = true;
        Serial.println("BLE connected");
    }

    void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override {
        BLEGamepad::connected = false;
        Serial.println("BLE disconnected");
        NimBLEDevice::startAdvertising();
    }
};

// --------------------------------------------------
// Report packing (same as working version)
// --------------------------------------------------
static void fill_gamepad_report(uint8_t* r, const JoystickState& s)
{
    auto put16 = [&](int16_t v, int i) {
        r[i]     = v & 0xFF;
        r[i + 1] = (v >> 8) & 0xFF;
    };

    // AXES (must already be -32768..32767)
    put16(s.x1, 0);
    put16(s.y1, 2);
    put16(s.x2, 4);
    put16(s.y2, 6);

    // BUTTONS (19 bits)
    uint32_t b = s.buttons & 0x7FFFF;

    r[8]  = b & 0xFF;
    r[9]  = (b >> 8) & 0xFF;
    r[10] = (b >> 16) & 0xFF;
}

// --------------------------------------------------
// BEGIN
// --------------------------------------------------
void BLEGamepad::begin(const char* name)
{
    NimBLEDevice::deinit(true);
    delay(200);
    NimBLEDevice::init(name);
    NimBLEDevice::setDeviceName(name);
    NimBLEServer* server = NimBLEDevice::createServer();
    server->setCallbacks(new Callbacks());

    NimBLEService* hid = server->createService("1812");

    // HID info
    NimBLECharacteristic* hidInfo =
        hid->createCharacteristic("2A4A", NIMBLE_PROPERTY::READ);

    uint8_t info[] = {0x11, 0x01, 0x00, 0x01};
    hidInfo->setValue(info, sizeof(info));

    // Report map
    const uint8_t reportMap[] = {
    0x05, 0x01,        // Generic Desktop
    0x09, 0x05,        // Gamepad
    0xA1, 0x01,        // Application Collection

    // --------------------
    // 4 Axes (16-bit signed)
    // --------------------
    0x09, 0x30,        // X
    0x09, 0x31,        // Y
    0x09, 0x32,        // Z
    0x09, 0x33,        // Rx

    0x16, 0x00, 0x80,  // Logical Min (-32768)
    0x26, 0xFF, 0x7F,  // Logical Max (32767)
    0x75, 0x10,        // 16-bit
    0x95, 0x04,        // 4 reports
    0x81, 0x02,        // Input (Data,Var,Abs)

    // --------------------
    // 19 Buttons
    // --------------------
    0x05, 0x09,        // Button page
    0x19, 0x01,
    0x29, 0x13,        // 19 buttons
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x13,
    0x81, 0x02,

    // padding
    0x75, 0x01,
    0x95, 0x05,
    0x81, 0x03,

    0xC0
};

    NimBLECharacteristic* reportMapChar =
        hid->createCharacteristic("2A4B", NIMBLE_PROPERTY::READ);

    reportMapChar->setValue(reportMap, sizeof(reportMap));

    // Protocol mode
    NimBLECharacteristic* protocol =
        hid->createCharacteristic(
            "2A4E",
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE_NR
        );

    uint8_t mode = 1;
    protocol->setValue(&mode, 1);

    // Input report
    inputReport = hid->createCharacteristic(
        "2A4D",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    inputReport->createDescriptor("2902");

    NimBLEDescriptor* ref =
        inputReport->createDescriptor("2908");

    uint8_t refVal[] = {0x01, 0x01};
    ref->setValue(refVal, sizeof(refVal));

    hid->start();

    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID("1812");
    adv->setName(name);
    adv->start();

    Serial.println("BLE Gamepad ready");
}

// --------------------------------------------------
// SEND
// --------------------------------------------------
void BLEGamepad::send(const JoystickState& js)
{
    if (!connected) return;

    uint8_t report[12];
    fill_gamepad_report(report,js);

    inputReport->setValue(report, sizeof(report));
    inputReport->notify();
}
