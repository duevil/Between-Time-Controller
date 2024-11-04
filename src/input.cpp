#include "input.h"
#include <Bounce2.h>

using namespace input;

static std::unordered_map<ColorInput::Position, Bounce> bounces{};
static ColorInput value{};
static Callback changeCallback{};


void input::setup() {
    for (auto i : ColorInput::positions) {
        bounces[i] = Bounce{};
        bounces[i].attach(PINS(i), INPUT_PULLUP);
        bounces[i].interval(5);
    }
}

void input::loop() {
    auto newValue = value;
    for (auto i : ColorInput::positions) {
        bounces[i].update();
        newValue[i].input = !bounces[i].read();
    }
    if (newValue != value) {
        newValue.sync();
        value = newValue;
        log_i("Input change: %s", value.inputs.to_string().c_str());
        if (changeCallback) changeCallback(value);
    }
}

void input::setCallback(Callback callback) { changeCallback = std::move(callback); }

const ColorInput &input::get() { return value; }

void input::setSync(bool sync) { value.shouldSync = sync; }
