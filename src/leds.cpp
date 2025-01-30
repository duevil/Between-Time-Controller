#include "leds.h"
#include <Adafruit_NeoPixel.h>

using namespace leds;

static Adafruit_NeoPixel pixels{ColorInput::SIZE, PIN};


void leds::setup() {
    log_d("LEDs setup");
    pixels.begin();
    set(ColorInput::all("off"));
    pixels.setBrightness(15);
}

void leds::set(const ColorInput::Colors &colors) {
    for (auto i : ColorInput::POSITIONS) {
        log_v("Setting led at position [%d] to: %s", i, colors[i].toString());
        pixels.setPixelColor(ColorInput::SIZE - i - 1, colors[i].value);
    }
    pixels.show();
}
