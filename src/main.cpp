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

    graphic_display::setup();
    draw("Booting...\nPlease wait...", graphic_display::Type::NORMAL);
    timecode_display::setup();
    timecode_display::set(0);
    leds::setup();

    encoder::setup();
    encoder::setCallback([](encoder::Event e) {
        using namespace timecode_display;
        auto i = static_cast<uint16_t>(abs(encoder::get()));
        if (states::gameRunning() && e == encoder::Event::PRESS) {
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

    mqtt::setup();
    mqtt::setClientID(CLIENT_ID);
    mqtt::setServer(MQTT_SERVER);
    mqtt::setOnConnect(states::subscribeToTopics);

    // TODO: replace with WiFi manager
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        log_e("WiFi Failed!");
        while (!WiFi.isConnected()) delay(1000);
    } else {
        log_i("Connected to %s at %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
}

void loop() {
    if (static auto last = millis(); millis() - last > 20000) {
        last = millis();
        log_d("Heartbeat");
    }
    mqtt::loop();
    input::loop();
    encoder::loop();
}
