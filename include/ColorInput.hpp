#ifndef COLOR_INPUT_HPP
#define COLOR_INPUT_HPP

#include "Color.hpp";
#include <array>
#include <bitset>


struct ColorInput {
    enum Position : uint8_t { OL = 0, IL = 1, M = 2, IR = 3, OR = 4 };

    static constexpr auto SIZE = static_cast<std::underlying_type_t<Position>>(5);
    using Colors = std::array<Color, SIZE>;
    using Input = std::bitset<SIZE>;

    class {
        using Array = std::array<Position, SIZE>;
        const Array positions_ = []<auto... I>(std::index_sequence<I...>) consteval {
            return Array{static_cast<Position>(I)...};
        }(std::make_index_sequence<SIZE>{});

    public:
        [[nodiscard]] constexpr Array::const_iterator begin() const { return positions_.begin(); }
        [[nodiscard]] constexpr Array::const_iterator end() const { return positions_.end(); }
    } const static positions;

    Colors colors{};
    Input inputs{};
    bool shouldSync = false;

    struct Proxy {
        const Position position;
        Input::reference input;
        Colors::reference color;
        explicit operator bool() const { return input & position; }

    private:
        friend struct ColorInput;

        explicit Proxy(Position position, ColorInput &colorInput)
            : position(position), input(colorInput.inputs[position]), color(colorInput.colors[position]) {}
    };

    Proxy operator[](Position position) { return Proxy{position, *this}; }
    Proxy operator&(Position position) { return Proxy{position, *this}; }
    bool operator==(const ColorInput &other) const = default;

    void sync() {
        if (!shouldSync) return;
        for (auto i : positions) {
            colors[i] = inputs.test(i) ? "white" : "black";
        }
    }
};

constexpr decltype(ColorInput::positions) ColorInput::positions{};


#endif //COLOR_INPUT_HPP
