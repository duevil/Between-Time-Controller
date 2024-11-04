#include "mqtt.h"
#include "leds.h"
#include "input.h"
#include "timecode_display.h"
#include "secrets.h"
#include <WiFi.h>

#include "states.h"

#ifdef WOKWI
//! @brief Client ID
//! @note Base64 encoding of 'between-time-controller' to ensure uniqueness for connecting to public MQTT brokers
constexpr auto CLIENT_ID = "YmV0d2Vlbi10aW1lLWNvbnRyb2xsZXI";
//! @brief MQTT topic path root
//! @note Base64 encoding of 'between-time' to obscure path for reduced visibility in public MQTT brokers
constexpr auto MQTT_ROOT = "YmV0d2Vlbi10aW1l/";
#else
//! @brief Client ID;
constexpr auto CLIENT_ID = "between-time-controller";
//! @brief MQTT topic path root
constexpr auto MQTT_ROOT = "between-time/";
#endif

const char *makeCStr(auto...);
template<states::Type type> states::ValueType<type> receiveValue(const uint8_t *, unsigned int,
                                                                 states::StateValue<type> &);
template<states::Type type> void publishValue(const mqtt::Topic &, const states::ValueType<type> &);


// U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 17, /* reset=*/ 16);

mqtt::Topic topicMain{
    makeCStr(MQTT_ROOT, "main"),
    [](const uint8_t *payload, unsigned int length) { receiveValue(payload, length, states::mainState()); }
};
mqtt::Topic topicTimecode{
    makeCStr(MQTT_ROOT, "timecode"),
    [](const uint8_t *payload, unsigned int length) {
        auto val = receiveValue(payload, length, states::timecode());
        timecode_display::set(val);
    }
};
mqtt::Topic topicCandles{
    makeCStr(MQTT_ROOT, "candles"),
    [](const uint8_t *payload, unsigned int length) { receiveValue(payload, length, states::candles()); }
};
mqtt::Topic topicMazePosition{
    makeCStr(MQTT_ROOT, "maze"),
    [](const uint8_t *payload, unsigned int length) { receiveValue(payload, length, states::mazePosition()); }
};
mqtt::Topic topicScannedItems{
    makeCStr(MQTT_ROOT, "arcade"),
    [](const uint8_t *payload, unsigned int length) { receiveValue(payload, length, states::scannedItems()); }
};


void setup() {
    Serial.begin(115200);
    Serial.println("Hello, ESP32!");

    // TODO: replace with WiFi manager
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        log_e("WiFi Failed!");
        while (!WiFi.isConnected()) delay(1000);
    } else {
        log_i("Connected to %s at %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }

    leds::setup();

    input::setup();
    input::setCallback(states::processInput);
    input::setSync(true);

    mqtt::setup();
    mqtt::setClientID(CLIENT_ID);
    mqtt::setServer("test.mosquitto.org");
    mqtt::setOnConnect([] {
        static_cast<void>(topicMain.subscribe());
        static_cast<void>(topicTimecode.subscribe());
        static_cast<void>(topicCandles.subscribe());
        static_cast<void>(topicMazePosition.subscribe());
        static_cast<void>(topicScannedItems.subscribe());
    });

    timecode_display::setup();
    timecode_display::set(0);

    states::setColorCallback(leds::setAll);
    using enum states::Type;
    states::mainState().setCallback([](auto value) { publishValue<MAIN>(topicMain, value); });
    states::timecode().setCallback([](auto value) { publishValue<TIMECODE>(topicTimecode, value); });
    states::candles().setCallback([](auto value) { publishValue<CANDLES>(topicCandles, value); });
    states::mazePosition().setCallback([](auto value) { publishValue<MAZE_POSITION>(topicMazePosition, value); });
    states::scannedItems().setCallback([](auto value) { publishValue<SCANNED_ITEMS>(topicScannedItems, value); });
}

void loop() {
    // TODO: remove for release
    if (static auto last = millis(); millis() - last > 1000) {
        last = millis();
        log_d("[%lu] Free heap: %lu", millis(), ESP.getFreeHeap());
    }
    mqtt::loop();
    input::loop();
}


const char *makeCStr(auto... args) {
    auto str = (String{} + ... + args);
    auto c_str = new char[str.length() + 1];
    strcpy(c_str, str.c_str());
    return c_str;
}

template<states::Type type>
states::ValueType<type> receiveValue(const uint8_t *payload,
                                     unsigned int length,
                                     states::StateValue<type> &stateValue) {
    String data{reinterpret_cast<const char*>(payload), length};
    using namespace states;
    using enum Type;
    using v_t = ValueType<type>;
    if /**/ constexpr (type == MAIN) stateValue = static_cast<v_t>(data.toInt());
    else if constexpr (type == TIMECODE) stateValue = static_cast<v_t>(strtoul(data.c_str(), nullptr, 16));
    else if constexpr (type == CANDLES || type == SCANNED_ITEMS) stateValue = data.toInt();
    else if constexpr (type == MAZE_POSITION) stateValue = v_t{data.toInt()};
    return stateValue.get();
}

template<states::Type type>
void publishValue(const mqtt::Topic &topic, const states::ValueType<type> &value) {
    String payload;
    using enum states::Type;
    if /**/ constexpr (type == MAIN) payload = String{static_cast<states::MainValue::Value>(value)};
    else if constexpr (type == TIMECODE) payload = String{value, 16};
    else if constexpr (type == CANDLES || type == SCANNED_ITEMS) payload = String{value.to_string().c_str()};
    else if constexpr (type == MAZE_POSITION) payload = String{value.toInt(), 16};
    static_cast<void>(topic.publish(payload.c_str()));
}
