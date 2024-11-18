#include "leds.h"
#include <Adafruit_NeoPixel.h>

using namespace leds;

static Adafruit_NeoPixel pixels{ColorInput::SIZE, PIN};


void leds::setup() {
    log_d("LEDs setup");
    pixels.begin();
    set(ColorInput::all("off"));
}

void leds::set(const ColorInput::Colors &colors) {
    log_d("Setting all leds");
    for (auto i : ColorInput::POSITIONS) {
        log_d("Setting led at position [%d] to: %s", i, colors[i].toString());
        pixels.setPixelColor(ColorInput::SIZE - 1 - i, colors[i].value);
    }
    pixels.show();
}
