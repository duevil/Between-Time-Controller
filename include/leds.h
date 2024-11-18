#ifndef LEDS_H
#define LEDS_H

#include "ColorInput.hpp"
#include <Arduino.h>


namespace leds {
#ifdef WOKWI
    constexpr inline auto PIN = GPIO_NUM_4;
#else
    constexpr inline auto PIN = GPIO_NUM_13;
#endif

    void setup();
    void set(const ColorInput::Colors &colors);
}


#endif //LEDS_H
