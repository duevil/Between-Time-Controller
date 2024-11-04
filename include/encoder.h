#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <functional>


namespace encoder {
#ifdef WOKWI
    constexpr inline auto PIN_CLK = GPIO_NUM_14;
    constexpr inline auto PIN_DT = GPIO_NUM_12;
    constexpr inline auto PIN_SW = GPIO_NUM_13;
#else
    constexpr inline auto PIN_CLK = GPIO_NUM_39;
    constexpr inline auto PIN_DT = GPIO_NUM_34;
    constexpr inline auto PIN_SW = GPIO_NUM_35;
#endif

    enum class Event { ROTATE_CCW, ROTATE_CW, PRESS, };

    using EncoderCallback = std::function<void(Event)>;

    void setup();
    void setCallback(const EncoderCallback &cb);
    int64_t get();
    void set(int64_t value);
    void loop();
}


#endif //ENCODER_H
