#ifndef INPUT_H
#define INPUT_H

#include "ColorInput.hpp"
#include <Arduino.h>
#include <functional>


namespace input {
    constexpr auto PINS(ColorInput::Position position) {
        switch (position) {
            case ColorInput::OL: return GPIO_NUM_32;
            case ColorInput::IL: return GPIO_NUM_33;
            case ColorInput::M: return GPIO_NUM_25;
            case ColorInput::IR: return GPIO_NUM_26;
            case ColorInput::OR: return GPIO_NUM_27;
            default: return GPIO_NUM_NC;
        }
    }

    using Callback = std::function<void(const ColorInput::Input &)>;

    void setup();
    void loop();
    void setCallback(Callback callback);
    const ColorInput::Input &get();
}


#endif //INPUT_H
