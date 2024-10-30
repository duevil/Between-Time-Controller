#include "leds.h"
#include <Adafruit_NeoPixel.h>

using namespace leds;

static Adafruit_NeoPixel pixels = Adafruit_NeoPixel(MAX, PIN);

template<std::size_t... I> static constexpr auto createArray(std::index_sequence<I...>) {
    return Array{static_cast<N>(I)...};
}


Ref::Ref(N index) : Color(pixels.getPixelColor(std::to_underlying(index))), index(index) {}

void Ref::set(Color color) const { leds::set(index, color); }

void Ref::off() const { set(0); }

Ref &Ref::operator=(Color color) {
    set(color);
    return *this;
}


void leds::setup() {
    pixels.begin();
    off();
}

Array leds::get() { return createArray(std::make_index_sequence<MAX>{}); }

Ref leds::get(N index) { return Ref{index}; }

void leds::set(const Colors &colors) {
    for (auto i = 0; i < MAX; ++i) {
        pixels.setPixelColor(static_cast<int16_t>(i), colors[i].value);
    }
    pixels.show();
}

void leds::set(N index, Color color) {
    if (index == N::MAX) return;
    pixels.setPixelColor(std::to_underlying(index), color.value);
    pixels.show();
}

void leds::off() {
    pixels.clear();
    pixels.show();
}
