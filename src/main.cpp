#include "mqtt_params.h"
#include "mqtt.h"
#include "leds.h"
#include "graphic_display.h"
#include "timecode_display.h"
#include "input.h"
#include "encoder.h"
#include "values.h"
#include "states.h"
#include "wifi_manager.h"

#define nl "\n"


void setup() {
    Serial.begin(115200);
    Serial.println("Hello, ESP32!");

    graphic_display::setup();
    draw("Booting..." nl "Please wait.", graphic_display::Type::NORMAL);
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
    input::setCallback([](auto &input) {
        states::processInput(input);
        input::setTickEnabled(states::mainState()->value == states::MainValue::MAZE_SOLVED - 1);
    });

    states::reset();
    states::processInput(0);

    mqtt::setup();
    mqtt::setClientID(CLIENT_ID);
    mqtt::setOnConnect([] {
        graphic_display::clear();
        draw("Waiting for the game" nl "to start...", graphic_display::Type::NORMAL);
        states::subscribeToTopics();
    });

    graphic_display::clear();
    draw("Waiting for WiFi..." nl "Please set" nl "WiFi credentials" nl "using config portal.",
         graphic_display::Type::NORMAL);

    String text;
    if (!wifi_manager::setup(mqtt::getServer(), mqtt::setServer)) {
        text = "Error connecting" nl "to WiFi!";
    } else {
        text = "Waiting for" nl "MQTT connection..." nl nl + wifi_manager::ip();
    }
    graphic_display::clear();
    draw(text.c_str(), graphic_display::Type::NORMAL);
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
