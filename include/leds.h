#ifndef LEDS_H
#define LEDS_H

#include <Color.hpp>
#include <Arduino.h>
#include <utility>
#include <array>

namespace leds {
    enum class N : uint8_t { L1, L2, L3, L4, L5, MAX };
    constexpr inline auto MAX = std::to_underlying(N::MAX);
    constexpr inline auto PIN = GPIO_NUM_4; // TODO: define the GPIO pin number

    struct Ref : public Color {
        const N index;
        void set(Color color) const;
        void off() const;
        Ref &operator=(Color color);
        explicit(false) Ref(N index);
    };

    using Array = std::array<Ref, MAX>;
    using Colors = std::array<Color, MAX>;

    void setup();
    Array get();
    Ref get(N index);
    void set(const Colors &colors);
    void set(N index, Color color);
    void off();
}

#endif //LEDS_H
