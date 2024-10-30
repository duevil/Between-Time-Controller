#include "states.h"
#include "mqtt.h"
#include "leds.h"
#include "switches.h"
#include "secrets.h"
#include <WiFi.h>


// U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 17, /* reset=*/ 16);

#ifdef WOKWI
#define MQTT_ROOT "YmV0d2Vlbi10aW1lLWNvbnRyb2xsZXI/"
#else
#define "between-time/"
#endif

std::vector<mqtt::Topic> ledTopics = [] {
    std::vector<mqtt::Topic> topics;
    for (auto i = 0; i < leds::MAX; ++i) {
        auto topicName = MQTT_ROOT "leds/" + String(i + 1);
        auto *c_topicName = new char[topicName.length() + 1];
        strcpy(c_topicName, topicName.c_str());
        const auto topic = mqtt::Topic{c_topicName, [i](uint8_t *payload, unsigned int length) {
            leds::set(static_cast<leds::N>(i), {String{reinterpret_cast<char *>(payload), length}});
        }};
        topics.push_back(topic);
    }
    return topics;
}();
mqtt::Topic switchesTopic{MQTT_ROOT "switches"};

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, ESP32!");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        log_e("WiFi Failed!");
        while (!WiFi.isConnected()) delay(1000);
    } else {
        log_i("Connected to %s at %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }

    leds::setup();

    switches::setup();
    switches::setCallback([](const switches::Value &val) {
        switchesTopic.publish(val.to_string().c_str());
        for (auto i = 0; i < leds::MAX; ++i) {
            auto b = val & static_cast<switches::S>(i);
            ledTopics[i].publish(b ? "red" : "off");
        }
    });

    mqtt::setup();
    mqtt::setServer("test.mosquitto.org");
    mqtt::setOnConnect([] { for (const auto &topic: ledTopics) topic.subscribe(); });
}

void loop() {
    if (static auto last = millis(); millis() - last > 1000) {
        last = millis();
        log_d("[%lu] Free heap: %lu", millis(), ESP.getFreeHeap());
    }
    mqtt::loop();
    switches::loop();
}
