#ifndef LEDS_H
#define LEDS_H

#include "ColorInput.hpp"
#include <Arduino.h>


namespace leds {
    constexpr inline auto PIN = GPIO_NUM_4; // TODO: define the GPIO pin number

    struct Ref : Color {
        const ColorInput::Position position;
        void set(Color color) const;
        void off() const;
        Ref &operator=(Color color);
        explicit(false) Ref(ColorInput::Position position);
    };

    using Array = std::array<Ref, ColorInput::SIZE>;

    void setup();
    Array get();
    Ref get(ColorInput::Position position);
    void setAll(const ColorInput::Colors &colors);
    void set(ColorInput::Position position, Color color);
    void off();
}


#endif //LEDS_H
