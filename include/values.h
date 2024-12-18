#ifndef VALUES_H
#define VALUES_H

#include "Maze.hpp"
#include "ColorInput.hpp"


namespace values {
    constexpr inline int TC_SIZE = 32;
    constexpr inline int TCI_0B00 = 07 % TC_SIZE;
    constexpr inline int TCI_0B01 = 15 % TC_SIZE;
    constexpr inline int TCI_0B10 = 23 % TC_SIZE;
    constexpr inline int TCI_0B11 = 31 % TC_SIZE;
    constexpr static unsigned short TIMECODES[TC_SIZE] = {
        0x4bd6, 0x66cd, 0xcf1f, 0x9887, 0x59aa, 0x2968, 0xca16, 0xdc3e,
        0x8fb4, 0x249f, 0x40eb, 0x4804, 0x1adf, 0xaa0e, 0x0ef7, 0x4001,
        0xac5c, 0xa06b, 0xb2a4, 0x1802, 0xbb97, 0x04f6, 0xc9b2, 0x14ea,
        0x138d, 0x323c, 0x5153, 0xe873, 0xbdb3, 0xa8a0, 0x5418, 0x19da,
    };
    constexpr inline auto &TC_0B00 = TIMECODES[TCI_0B00];
    constexpr inline auto &TC_0B01 = TIMECODES[TCI_0B01];
    constexpr inline auto &TC_0B10 = TIMECODES[TCI_0B10];
    constexpr inline auto &TC_0B11 = TIMECODES[TCI_0B11];
    constexpr inline ColorInput::Position INPUT_FIELD_OPEN_POS = ColorInput::IR;
    constexpr inline ColorInput::Input INPUT_FIELD_OPENED = 1 << INPUT_FIELD_OPEN_POS;
    constexpr inline auto INPUT_CODE = "485361";
    constexpr static ColorInput::Colors CANDLES_COLORS = {
        "blue", "black", "green", "magenta", "cyan"
    };
    constexpr static ColorInput::Position CANDLES_ORDER[] = {
        ColorInput::IR, ColorInput::OL, ColorInput::M, ColorInput::OR
    };
    constexpr inline ColorInput::Input BOOK_BINARY = 22;
    constexpr static Maze MAZE = {
        0x355519B3,
        0x6935C2CA,
        0xBA693C3C,
        0xAA3C6DAB,
        0x2CA393CA,
        0x2D6CA658,
        0xA3554D3C,
        0xC655554D,
    };
    constexpr inline Maze::Position MAZE_START = {0, 0};
    constexpr inline Maze::Position MAZE_END = {Maze::SIZE_X - 1, Maze::SIZE_Y - 1};
}


#endif //VALUES_H
