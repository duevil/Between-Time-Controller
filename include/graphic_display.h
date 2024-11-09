#ifndef GRAPHIC_DISPLAY_H
#define GRAPHIC_DISPLAY_H

#include <Maze.hpp>
#include <Arduino.h>


namespace graphic_display {
    constexpr inline  auto PIN_CS = SS;
    constexpr inline auto PIN_DC = GPIO_NUM_17;
    constexpr inline auto PIN_RST = GPIO_NUM_16;

    void setup();
    void clear();
    void setCursor(uint8_t x, uint8_t y);
    void drawStr(uint8_t, uint8_t, const char *);
    void drawInputField(/* TODO */);
    void drawCode(const char* code);
    void drawMaze(const Maze::Position &position);
}


#endif //GRAPHIC_DISPLAY_H
