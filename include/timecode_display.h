#ifndef TIMECODE_DISPLAY_H
#define TIMECODE_DISPLAY_H

#include <Arduino.h>


namespace timecode_display {
#ifdef WOKWI
    constexpr inline auto PIN_CLK = GPIO_NUM_16;
    constexpr inline auto PIN_DIO = GPIO_NUM_17;
#else
    constexpr inline auto PIN_CLK = GPIO_NUM_12;
    constexpr inline auto PIN_DIO = GPIO_NUM_14;
#endif

    void setup();
    void set(uint16_t timecode);
    void toggleBlinking(bool enable = false);
    bool isBlinking();
}


#endif //TIMECODE_DISPLAY_H
