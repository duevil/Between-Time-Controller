#ifndef COLOR_INPUT_HPP
#define COLOR_INPUT_HPP

#include "Color.hpp";
#include <array>
#include <bitset>


struct ColorInput {
    enum Position : uint8_t { OL = 4, IL = 3, M = 2, IR = 1, OR = 0 };

    static constexpr auto SIZE = static_cast<std::underlying_type_t<Position>>(5);
    using Colors = std::array<Color, SIZE>;
    using Input = std::bitset<SIZE>;

    class {
        using Array = std::array<Position, SIZE>;
        const Array positions = []<auto... I>(std::index_sequence<I...>) consteval {
            return Array{static_cast<Position>(SIZE - I - 1)...};
        }(std::make_index_sequence<SIZE>{});

    public:
        [[nodiscard]] constexpr Array::const_iterator begin() const { return positions.begin(); }
        [[nodiscard]] constexpr Array::const_iterator end() const { return positions.end(); }
    } const static POSITIONS;

    Colors colors{};
    Input inputs{};

    struct Proxy {
        const Position position;
        Input::reference input;
        Colors::reference color;
        explicit operator bool() const { return input; }

    private:
        friend struct ColorInput;

        explicit Proxy(Position position, ColorInput &colorInput)
            : position(position), input(colorInput.inputs[position]), color(colorInput.colors[position]) {}
    };

    Proxy operator[](Position position) { return Proxy{position, *this}; }
    Proxy operator&(Position position) { return Proxy{position, *this}; }
    bool operator==(const ColorInput &other) const = default;

    void sync(Color on = "on") {
        for (auto i : POSITIONS) {
            colors[i] = inputs.test(i) ? on : "off";
        }
    }

    static constexpr Colors all(const Color &color) {
        Colors colors{};
        for (auto i : POSITIONS) {
            colors[i] = color;
        }
        return colors;
    }
};

constexpr decltype(ColorInput::POSITIONS) ColorInput::POSITIONS{};


#endif //COLOR_INPUT_HPP
