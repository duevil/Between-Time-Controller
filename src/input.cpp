#include "input.h"
#include <Bounce2.h>

using namespace input;

static std::unordered_map<ColorInput::Position, Bounce> bounces{};
static ColorInput::Input value{};
static Callback changeCallback{};
static bool tickEnabled = false;


void input::setup() {
    log_d("Input setup");
    for (auto i : ColorInput::POSITIONS) {
        bounces[i] = Bounce{};
        bounces[i].attach(PINS(i), INPUT_PULLUP);
        bounces[i].interval(150);
    }
}

void input::loop() {
    static auto lastTick = millis();
    auto newValue = value;
    for (auto i : ColorInput::POSITIONS) {
        bounces[i].update();
        newValue[i] = !bounces[i].read();
    }
    if (newValue != value) {
        value = newValue;
        log_i("Input change: %s", value.to_string().c_str());
        if (changeCallback) {
            changeCallback(value);
            lastTick = millis();
        }
        return;
    }
    if (changeCallback && tickEnabled && !value.none() && millis() - lastTick >= 1000) {
        changeCallback(value);
        lastTick = millis();
    }
}

void input::setCallback(Callback callback) { changeCallback = std::move(callback); }

const ColorInput::Input &input::get() { return value; }

void input::setTickEnabled(bool enabled) { tickEnabled = enabled; }
