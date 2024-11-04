#include "leds.h"
#include <Adafruit_NeoPixel.h>
#include <ColorInput.hpp>

using namespace leds;

static Adafruit_NeoPixel pixels{ColorInput::SIZE, PIN};


Ref::Ref(ColorInput::Position position) : Color(pixels.getPixelColor(position)), position(position) {}

void Ref::set(Color color) const { leds::set(position, color); }

void Ref::off() const { set(0); }

Ref &Ref::operator=(Color color) {
    set(color);
    return *this;
}


void leds::setup() {
    pixels.begin();
    off();
}

Array leds::get() {
    return []<auto... I>(std::index_sequence<I...>) {
        return Array{static_cast<ColorInput::Position>(I)...};
    }(std::make_index_sequence<ColorInput::SIZE>{});
}

Ref leds::get(ColorInput::Position position) { return Ref{position}; }

void leds::setAll(const ColorInput::Colors &colors) {
    log_d("Setting all leds");
    for (auto i : ColorInput::positions) {
        log_d("Setting led at position [%d] to: %s", i, colors[i].toString());
        pixels.setPixelColor(i, colors[i].value);
    }
    pixels.show();
}

void leds::set(ColorInput::Position position, Color color) {
    log_d("Setting led at position [%d] to: %s", position, color.toString());
    pixels.setPixelColor(position, color.value);
    pixels.show();
}

void leds::off() {
    log_d("Clearing all leds");
    pixels.clear();
    pixels.show();
}
