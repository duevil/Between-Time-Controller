#ifndef COLOR_HPP
#define COLOR_HPP

#include <string_view>


struct Color {
    template<typename T, class Base> requires std::is_same_v<T, Base> ||
                                              std::is_arithmetic_v<T> ||
                                              std::is_same_v<T, const char*> ||
                                              std::is_same_v<T, std::string_view>
    static constexpr bool is_color_value = true;
    long unsigned int value;
    constexpr explicit(false) Color(auto value);
    [[nodiscard]] constexpr const char *toString() const;
    constexpr bool operator==(const Color &) const = default;
    constexpr Color() : value(0) {}
    consteval explicit Color(int r, int g, int b) : value{static_cast<long unsigned int>(r << 16 | g << 8 | b)} {}

    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color CYAN;
    static const Color MAGENTA;
    static const Color WHITE;
    static const Color BLACK;

private:
    template<typename T> requires Color::is_color_value<T, Color>
    static constexpr uint32_t toColorT(T value);
};


// definitions:

template<typename T> requires Color::is_color_value<T, Color>
constexpr uint32_t Color::toColorT(T value) {
    constexpr auto fromString = [](const char *color) {
        auto str = std::string_view{color};
        if (str == "red") return RED;
        if (str == "green") return GREEN;
        if (str == "blue") return BLUE;
        if (str == "yellow") return YELLOW;
        if (str == "cyan") return CYAN;
        if (str == "magenta") return MAGENTA;
        if (str == "white" || str == "on") return WHITE;
        return BLACK;
    };
    if constexpr (std::is_same_v<T, Color>) return value.value;
    if constexpr (std::is_arithmetic_v<T>) return static_cast<long unsigned int>(value);
    if constexpr (std::is_same_v<T, const char*>) return fromString(value).value;
    if constexpr (std::is_same_v<T, std::string_view>) return fromString(value.c_str()).value;
    return 0;
}

constexpr Color::Color(auto value) : value{toColorT(value)} {}

constexpr Color Color::RED = Color{255, 0, 0};
constexpr Color Color::GREEN = Color{0, 255, 0};
constexpr Color Color::BLUE = Color{0, 0, 255};
constexpr Color Color::YELLOW = Color{255, 255, 0};
constexpr Color Color::CYAN = Color{0, 255, 255};
constexpr Color Color::MAGENTA = Color{255, 0, 255};
constexpr Color Color::WHITE = Color{255, 255, 255};
constexpr Color Color::BLACK = Color{0, 0, 0};

constexpr const char *Color::toString() const {
    switch (value) {
        case RED.value: return "red";
        case GREEN.value: return "green";
        case BLUE.value: return "blue";
        case YELLOW.value: return "yellow";
        case CYAN.value: return "cyan";
        case MAGENTA.value: return "magenta";
        case WHITE.value: return "white";
        case BLACK.value: return "black";
        default: return "unknown";
    }
}


#endif //COLOR_HPP
