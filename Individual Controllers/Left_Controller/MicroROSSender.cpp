#include "MicroROSSender.h"

void MicroROSSender::begin() {

    agent_ip = "10.145.116.65";
    agent_port = 8888;

    allocator = rcl_get_default_allocator();

    sensor_msgs__msg__Joy__init(&msg);

    msg.axes.data = axes_buf;
    msg.axes.size = AXES;
    msg.axes.capacity = AXES;

    msg.buttons.data = buttons_buf;
    msg.buttons.size = BUTTONS;
    msg.buttons.capacity = BUTTONS;

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    wifi_connected = false;
    ros_ready = false;

    last_wifi_attempt = millis();
    last_ros_attempt = millis();
}

void MicroROSSender::saveWiFi(const String& ssid, const String& pass)
{
    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);
    prefs.end();
}

bool MicroROSSender::loadWiFi(String &ssid, String &pass)
{
    prefs.begin("wifi", true);

    ssid = prefs.getString("ssid", "");
    pass = prefs.getString("pass", "");

    prefs.end();

    return ssid.length() > 0;
}

void MicroROSSender::openConfigPortal()
{
    wifi_connected = false;
    ros_ready = false;
    WiFi.disconnect(true);
  
    WiFiManager wm;

    bool ok = wm.startConfigPortal("Ponsse Comforthandle Left");

    if (ok) {
        wifi_connected = true;
        saveWiFi(WiFi.SSID(), WiFi.psk());
    } else {
        return;
    }
    
}

void MicroROSSender::update()
{

  Serial.println("updating wifi");

    unsigned long now = millis();
    // -------------------------------------------------
    // WIFI CONNECTION
    // -------------------------------------------------
    Serial.print("ssid :");
    Serial.println(ssid);
    Serial.print("password :");
    Serial.println(password);

    if (WiFi.status() == WL_CONNECTED) {
      
        if (!wifi_connected) {
          wifi_connected = true;
          wifi_ready = false;
          wifi_ready_time = now;
        }

        if (!wifi_ready && (now - wifi_ready_time > 300)) {
          wifi_ready = true;
        }
        
    }

    else if (!loadWiFi(ssid, password)) {
      return;
    }
    
    else {
        Serial.println("wifi not connected");
        wifi_connected = false;
        wifi_ready = false;
        ros_ready = false;
        
        if (now - last_wifi_attempt > 5000) {
            last_wifi_attempt = now;
            WiFi.begin(ssid.c_str(), password.c_str());
        }
        return;
    }

    IPAddress ip;
    if (!ip.fromString(agent_ip)) {
        Serial.println("Bad agent IP");
        return;
    }

    Serial.println(WiFi.localIP());
    Serial.println(agent_ip);
    // -------------------------------------------------
    // ROS INIT
    // -------------------------------------------------

    if (!ros_ready && wifi_ready && WiFi.status() == WL_CONNECTED && 
    WiFi.localIP() != IPAddress(0,0,0,0) && now - last_ros_attempt > 3000 && !ROS_transport_initialized) 
    {
        Serial.println("connecting ROS");
        last_ros_attempt = now;
        
        set_microros_wifi_transports(
            (char*)ssid.c_str(),
            (char*)password.c_str(),
            (char*)agent_ip,
            agent_port
        );
        
        delay(200);
        
        Serial.println("transport set");
        rcl_ret_t ret;

        ret = rclc_support_init(
            &support,
            0,
            NULL,
            &allocator
        );
        Serial.printf("support init: %d\n", ret);

        if (ret != RCL_RET_OK) {
            Serial.println("support failed");
            return;
        }

        ret = rclc_node_init_default(
            &node,
            "left_joystick_node",
            "",
            &support
        );
        Serial.printf("node init: %d\n", ret);

        if (ret != RCL_RET_OK) {
            Serial.println("node failed");
            return;
        }

        ret = rclc_publisher_init_default(
            &pub,
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Joy),
            "/left_joystick/joy"
        );
        Serial.printf("pub init: %d\n", ret);

        if (ret != RCL_RET_OK) {
            return;
        }

        if (rmw_uros_ping_agent(100, 5) != RMW_RET_OK) {
            return;
        }
        ROS_transport_initialized = true;

        ros_ready = true;
        Serial.println("ROS connected");
    }
    Serial.println("ROS should be connected");
}

void MicroROSSender::send(const JoystickState& state) {

    if (!ros_ready) return;

    msg.axes.data[0] = (float)state.x1;
    msg.axes.data[1] = (float)state.y1;
    msg.axes.data[2] = (float)state.x2;
    msg.axes.data[3] = (float)state.y2;

    for (int i = 0; i < BUTTONS; i++) {
        msg.buttons.data[i] = (state.buttons >> i) & 0x01;
    }

    rcl_publish(&pub, &msg, NULL);
}
