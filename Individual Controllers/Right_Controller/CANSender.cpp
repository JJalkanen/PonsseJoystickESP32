#include "CANSender.h"

// -----------------------------
// J1939 ID builder
// -----------------------------

uint32_t CANSender::makeJ1939Id(uint32_t pgn)
{
    uint8_t priority = 6;
    uint8_t src = 0x23;

    uint8_t dp = (pgn >> 16) & 0x01;
    uint8_t pf = (pgn >> 8) & 0xFF;
    uint8_t ps = pgn & 0xFF;

    uint32_t id = 0;

    id |= (priority << 26);
    id |= (dp << 24);
    id |= (pf << 16);

    if (pf < 240)
        id |= (ps << 8);   // PDU1
    else
        id |= (ps << 8);   // PDU2 broadcast

    id |= src;

    return id;
}

// -----------------------------
// Init CAN (ESP32 TWAI)
// -----------------------------
void CANSender::begin()
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        GPIO_NUM_15,   // TX (change if needed)
        GPIO_NUM_16,   // RX (change if needed)
        TWAI_MODE_NORMAL
    );

    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();

    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        twai_start();
    }
}

// -----------------------------
// Public update
// -----------------------------
void CANSender::send(const JoystickState& s)
{
    sendAxes(s);
    sendButtons(s);
    twai_status_info_t info;
    twai_get_status_info(&info);
    last_tx = millis();
}

// -----------------------------
// AXES PGN 0x00FF00
// -----------------------------
void CANSender::sendAxes(const JoystickState& s)
{
    twai_message_t msg = {};
    msg.extd = 1;
    msg.rtr  = 0;
    msg.data_length_code = 8;
    msg.identifier = makeJ1939Id(0x00FF02);

    auto pack16 = [&](uint16_t v, int i)
    {
        msg.data[i]     = v & 0xFF;
        msg.data[i + 1] = (v >> 8) & 0xFF;
    };

    pack16(s.x1, 0);
    pack16(s.y1, 2);
    pack16(s.x2, 4);
    pack16(s.y2, 6);

    twai_transmit(&msg, pdMS_TO_TICKS(10));
}

// -----------------------------
// BUTTONS PGN 0x00FF01
// -----------------------------
void CANSender::sendButtons(const JoystickState& s)
{
    twai_message_t msg = {};
    msg.extd = 1;
    msg.rtr  = 0;
    msg.data_length_code = 8;
    msg.identifier = makeJ1939Id(0x00FF03);

    uint32_t b = s.buttons;

    msg.data[0] = b & 0xFF;
    msg.data[1] = (b >> 8) & 0xFF;
    msg.data[2] = (b >> 16) & 0xFF;
    msg.data[3] = (b >> 24) & 0xFF;

    // rest unused
    msg.data[4] = 0;
    msg.data[5] = 0;
    msg.data[6] = 0;
    msg.data[7] = 0;

    twai_transmit(&msg, pdMS_TO_TICKS(10));
}
