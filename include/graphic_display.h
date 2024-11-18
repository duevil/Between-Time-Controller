#ifndef GRAPHIC_DISPLAY_H
#define GRAPHIC_DISPLAY_H

#include <Arduino.h>


namespace graphic_display {
    constexpr inline  auto PIN_CS = SS;
    constexpr inline auto PIN_DC = GPIO_NUM_17;
    constexpr inline auto PIN_RST = GPIO_NUM_16;

    enum class Type { NORMAL, CODE, MAZE};

    void setup();
    void clear();
    void draw(const char *s, Type type);
}


#endif //GRAPHIC_DISPLAY_H
