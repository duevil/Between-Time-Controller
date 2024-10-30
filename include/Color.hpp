#ifndef COLOR_HPP
#define COLOR_HPP

#include <string_view>
#include <WString.h>

template<typename T, class Base> requires std::is_same_v<T, Base> ||
                                          std::is_arithmetic_v<T> ||
                                          std::is_same_v<T, const char *> ||
                                          std::is_same_v<T, String>
constexpr inline bool is_color_value = true;

struct Color {
    const uint32_t value;
    constexpr explicit(false) Color(auto value);
    [[nodiscard]] constexpr const char *toString() const;
    constexpr bool operator==(const Color &) const = default;

    consteval explicit Color(int r, int g, int b) : value{static_cast<uint32_t>((r << 16) | (g << 8) | b)} {}

    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color CYAN;
    static const Color MAGENTA;
    static const Color WHITE;
    static const Color BLACK;

private:
    template<typename T> requires is_color_value<T, Color>
    static constexpr uint32_t toColorT(T value);
};


template<typename T> requires is_color_value<T, Color>
constexpr uint32_t Color::toColorT(T value) {
    constexpr auto fromString = [](const char *color) {
        auto str = std::string_view{color};
        if (str == "red") return Color::RED;
        if (str == "green") return Color::GREEN;
        if (str == "blue") return Color::BLUE;
        if (str == "yellow") return Color::YELLOW;
        if (str == "cyan") return Color::CYAN;
        if (str == "magenta") return Color::MAGENTA;
        if (str == "white" || str == "on") return Color::WHITE;
        return Color::BLACK;
    };
    if constexpr (std::is_same_v<T, Color>) return value.value;
    if constexpr (std::is_arithmetic_v<T>) return static_cast<uint32_t>(value);
    if constexpr (std::is_same_v<T, const char *>) return fromString(value).value;
    if constexpr (std::is_same_v<T, String>) return fromString(value.c_str()).value;
}

constexpr Color::Color(auto value) : value{toColorT(value)} {}

constexpr const char *Color::toString() const {
    auto value = this->value;
    if (value == RED.value) return "red";
    if (value == GREEN.value) return "green";
    if (value == BLUE.value) return "blue";
    if (value == YELLOW.value) return "yellow";
    if (value == CYAN.value) return "cyan";
    if (value == MAGENTA.value) return "magenta";
    if (value == WHITE.value) return "white";
    return "black";
}

constexpr Color Color::RED = Color{255, 0, 0};
constexpr Color Color::GREEN = Color{0, 255, 0};
constexpr Color Color::BLUE = Color{0, 0, 255};
constexpr Color Color::YELLOW = Color{255, 255, 0};
constexpr Color Color::CYAN = Color{0, 255, 255};
constexpr Color Color::MAGENTA = Color{255, 0, 255};
constexpr Color Color::WHITE = Color{255, 255, 255};
constexpr Color Color::BLACK = Color{0, 0, 0};

#endif //COLOR_HPP
