#ifndef SWITCHES_H
#define SWITCHES_H

#include <Arduino.h>
#include <utility>
#include <bitset>
#include <functional>

namespace switches {
    enum class S : uint8_t { OL, IL, M, IR, OR, MAX };
    constexpr inline auto MAX = std::to_underlying(S::MAX);

    constexpr std::array<uint8_t, MAX> PINS = {
            /* OL = */ GPIO_NUM_32, /* IL = */ GPIO_NUM_33, /* M = */ GPIO_NUM_25,
            /* IR = */ GPIO_NUM_26, /* OR = */ GPIO_NUM_27
    };

    using Value = std::bitset<MAX>;
    using Callback = std::function<void(const Value &)>;

    void setup();
    void loop();
    void setCallback(Callback callback);
    const Value &get();
    bool operator&(switches::Value value, switches::S index);
}

#endif //SWITCHES_H
