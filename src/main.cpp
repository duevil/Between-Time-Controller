#include "secrets.h"
#include "mqtt_params.h"
#include "mqtt.h"
#include "leds.h"
#include "graphic_display.h"
#include "timecode_display.h"
#include "input.h"
#include "encoder.h"
#include "values.h"
#include "states.h"
#include <WiFi.h>


// TODO: add comments


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

    mqtt::setup();
    mqtt::setClientID(CLIENT_ID);
    mqtt::setServer(MQTT_SERVER);
    mqtt::setOnConnect(states::subscribeToTopics);

    leds::setup();
    graphic_display::setup();
    timecode_display::setup();
    timecode_display::set(0);

    encoder::setup();
    encoder::setCallback([](encoder::Event e) {
        using namespace timecode_display;
        auto i = static_cast<uint16_t>(abs(encoder::get()));
        if (e == encoder::Event::PRESS) {
            toggleBlinking(!isBlinking());
            if (!isBlinking()) {
                states::timecode() = values::TIMECODES[i % values::TC_SIZE];
            }
        } else if (isBlinking()) {
            set(values::TIMECODES[i % values::TC_SIZE]);
        } else {
            encoder::set(states::timecode().get());
        }
    });

    input::setup();
    input::setCallback(states::processInput);

    states::reset();
    states::processInput(0);
}

void loop() {
    // TODO: remove for release
    if (static auto last = millis(); millis() - last > 5000) {
        last = millis();
        log_d("[%lu] Free heap: %lu", millis(), ESP.getFreeHeap());
    }
    mqtt::loop();
    input::loop();
    encoder::loop();
}
