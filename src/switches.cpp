#include "switches.h"
#include <Bounce2.h>
#include <unordered_set>

using namespace switches;

static std::array<Bounce, MAX> bounces{};
static Value value{};
static Callback callback{};

void switches::setup() {
    for (size_t i = 0; i < MAX; i++) {
        bounces[i].attach(PINS[i], INPUT_PULLUP);
        bounces[i].interval(5);
    }
}

void switches::loop() {
    Value newValue;
    for (size_t i = 0; i < MAX; i++) {
        bounces[i].update();
        newValue.set(i, !bounces[i].read());
    }
    if (newValue != value) {
        value = newValue;
        if (callback) callback(value);
    }
}

void switches::setCallback(Callback callback) { ::callback = std::move(callback); }

const Value &switches::get() { return value; }

bool switches::operator&(Value value, S index) {
    if (index == S::MAX) return false;
    return value.test(std::to_underlying(index));
}
