#ifndef TIMECODE_DISPLAY_H
#define TIMECODE_DISPLAY_H

#include <Arduino.h>


namespace timecode_display {
    constexpr uint8_t PIN_CLK = GPIO_NUM_16;
    constexpr uint8_t PIN_DIO = GPIO_NUM_17;

    void setup();
    void set(uint16_t timecode);
    void toggleBlinking(bool enable = false);
    bool isBlinking();
}


#endif //TIMECODE_DISPLAY_H
