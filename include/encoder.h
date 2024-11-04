#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <functional>


namespace encoder {
    constexpr auto PIN_CLK = GPIO_NUM_14;
    constexpr auto PIN_DT = GPIO_NUM_12;
    constexpr auto PIN_SW = GPIO_NUM_13;

    enum class Event { ROTATE_CCW, ROTATE_CW, PRESS, };

    using EncoderCallback = std::function<void(Event)>;

    void setup();
    void setCallback(const EncoderCallback &cb);
    int64_t get();
    void set(int64_t value);
    void loop();
}


#endif //ENCODER_H
