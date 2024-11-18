#include "states.h"
#include "values.h"
#include "leds.h"
#include "graphic_display.h"
#include "timecode_display.h"
#include "encoder.h"
#include <Ticker.h>


#define nl "\n"


static ColorInput colorInput;
static uint8_t inputMustReset = false;
static Ticker blinker{};


enum class Change {
    ALLOWED,
    WAITING_FOR_TIMECODE,
    WAITING_FOR_INPUT_RESET
};


static Change changeAllowed() {
    auto tcOK = true;
    if (inputMustReset && colorInput.inputs.none()) inputMustReset = false;
    auto timecode = states::timecode().get();
    using enum states::MainValue::Value;
    switch (states::mainState()->value) {
        case STARTED:
        case INPUT_FIELD_OPENED:
        case INPUT_FIELD_SOLVED:
            tcOK = timecode == values::TC_0B00;
            if (!tcOK)
                log_d("Waiting for timecode: %d", values::TC_0B00);
            break;
        case CANDLES_PLACED:
        case CANDLES_SOLVED:
        case BOOK_BINARY_SOLVED:
            tcOK = timecode == values::TC_0B01;
            if (!tcOK)
                log_d("Waiting for timecode: %d", values::TC_0B01);
            break;
        case MAZE_ACTIVE:
        case MAZE_SOLVED:
            tcOK = timecode == values::TC_0B10;
            if (!tcOK)
                log_d("Waiting for timecode: %d", values::TC_0B10);
            break;
        case ARCADE_UNLOCKED:
        case ALL_ITEMS_SCANNED:
            tcOK = timecode == values::TC_0B11;
            if (!tcOK)
                log_d("Waiting for timecode: %d", values::TC_0B11);
            break;
        default: break; // No restrictions
    }
    using enum Change;
    // change allowed if timecode is set and input must not be reset
    if (tcOK && !inputMustReset) {
        log_d("Change allowed");
        return ALLOWED;
    }
    if (!tcOK) {
        return WAITING_FOR_TIMECODE;
    }
    log_d("Waiting for input reset");
    return WAITING_FOR_INPUT_RESET;
}

static void setDisplayAndLEDs(states::MainValue::Value value = states::mainState()->value, bool leds = true) {
    using namespace states;
    const char *text = nullptr;
    auto type = graphic_display::Type::NORMAL;
    colorInput.colors = ColorInput::all("black");
    using enum Change;
    switch (changeAllowed()) {
        case WAITING_FOR_TIMECODE:
            text = "Waiting for sync...";
            colorInput.colors = ColorInput::all("off");
            break;
        case WAITING_FOR_INPUT_RESET:
            if (inputMustReset == 2) text = "All candles must be" nl "switched in the" nl "correct order...";
            else text = "All switches must be" nl "off to continue...";
            colorInput.sync("red");
            break;
        case ALLOWED:
            using enum MainValue::Value;
            switch (value) {
                case IDLE:
                    text = "Waiting for the game" nl "to start...";
                    break;
                case INPUT_FIELD_SOLVED - 1:
                    type = graphic_display::Type::CODE;
                    text = values::INPUT_CODE;
                    break;
                case CANDLES_SOLVED - 1:
                    colorInput.colors = values::CANDLES_COLORS;
                    break;
                case BOOK_BINARY_SOLVED - 1:
                    colorInput.sync();
                    break;
                case MAZE_SOLVED - 1:
                    type = graphic_display::Type::MAZE;
                    text = visitedMaze().string();
                    colorInput.sync("yellow");
                    colorInput[ColorInput::M].color = "";
                    break;
                case MAZE_SOLVED:
                    text = ""; // Prevent clearing
                    break;
                case GAME_LOST:
                    text = "Game over!" nl "You lost!";
                    colorInput.colors = ColorInput::all("red");
                    break;
                case GAME_WON:
                    text = "Congratulations!" nl "You won the game!";
                    colorInput.colors = ColorInput::all("green");
                    break;
                default: break; // Nothing to draw
            }
            break; // No overwriting
    }
    graphic_display::clear();
    if (text) draw(text, type);
    drawTimer();
    if (leds) leds::set(colorInput.colors);
}

static void blink(auto color) {
    int count = 0;
    auto colors = ColorInput::all(color);
    leds::set(colors);
    blinker.attach_ms(150, [count, colors]() mutable {
        if (count >= 5) {
            blinker.detach();
            setDisplayAndLEDs();
        } else {
            leds::set(count % 2 ? colors : ColorInput::all("black"));
            ++count;
        }
    });
}


template<>
void states::onChange<states::Type::MAIN>(const ValueType<Type::MAIN> &value) {
    log_i("Main state changed to %d", value.value);
    inputMustReset = false;
    using enum MainValue::Value;
    switch (value.value) {
        case IDLE:
            reset();
            break;
        case INPUT_FIELD_OPENED - 1:
        case CANDLES_SOLVED - 1:
        case BOOK_BINARY_SOLVED - 1:
        case MAZE_SOLVED - 1:
            inputMustReset = true;
            break;
        default: break; // No action required
    }
    setDisplayAndLEDs(value.value, false);
    switch (value.value) {
        case IDLE:
            // we're not setting leds above, so we need to do it here
            leds::set(ColorInput::all("black"));
            break;
        case STARTED:
            blink("white");
            break;
        case INPUT_FIELD_OPENED:
        case INPUT_FIELD_SOLVED:
        case CANDLES_PLACED:
        case CANDLES_SOLVED:
        case BOOK_BINARY_SOLVED:
        case MAZE_ACTIVE:
        case MAZE_SOLVED:
        case ARCADE_UNLOCKED:
        case ALL_ITEMS_SCANNED:
            blink("cyan");
            break;
        case GAME_LOST:
            blink("red");
            break;
        case GAME_WON:
            blink("green");
            break;
    }
}

template<>
void states::onChange<states::Type::TIMECODE>(const ValueType<Type::TIMECODE> &value) {
    log_i("Timecode changed to %d", value);
    for (auto i = 0; i < values::TC_SIZE; ++i) {
        if (value != values::TIMECODES[i]) continue;
        encoder::set(i);
        break;
    }
    setDisplayAndLEDs();
    timecode_display::set(value);
}

template<>
void states::onChange<states::Type::CANDLES>(const ValueType<Type::CANDLES> &value) {
    log_i("Candles changed to %s", value.to_string().c_str());
    if (!value.all()) return;
    mainState() = MainValue::CANDLES_SOLVED;
}

template<>
void states::onChange<states::Type::MAZE_POSITION>(const ValueType<Type::MAZE_POSITION> &value) {
    log_i("Maze position changed to {%d, %d}", value.x, value.y);
    visitedMaze() += value;
    setDisplayAndLEDs();
    if (value != values::MAZE_END) return;
    mainState() = MainValue::MAZE_SOLVED;
}

template<>
void states::onChange<states::Type::SCANNED_ITEMS>(const ValueType<Type::SCANNED_ITEMS> &value) {
    log_i("Scanned items changed to %s", value.to_string().c_str());
    log_e("Not implemented");
}


void states::processInput(const ColorInput::Input &input) {
    colorInput.inputs = input;
    if (!gameRunning()) return;
    setDisplayAndLEDs();
    if (changeAllowed() != Change::ALLOWED) return;
    using enum MainValue::Value;
    switch (mainState()->value) {
        case INPUT_FIELD_OPENED - 1: {
            if (input == values::INPUT_FIELD_OPENED) mainState() = INPUT_FIELD_OPENED;
            break;
        }
        case CANDLES_SOLVED - 1: {
            auto ci_ = colorInput;
            auto candles_ = candles().get();
            for (auto i = 0; i < candles()->count() + 1; ++i) {
                auto proxy = ci_ & values::CANDLES_ORDER[i];
                if (!proxy) break; // No candle or wrong order
                candles_[i] = true;
                proxy.input = false;
            }
            // Only correct candles are set
            if (ci_.inputs.none()) {
                candles() = candles_;
                break;
            }
            inputMustReset = 2;
            setDisplayAndLEDs();
            return;
        }
        case BOOK_BINARY_SOLVED - 1: {
            if (input == values::BOOK_BINARY) mainState() = BOOK_BINARY_SOLVED;
            break;
        }
        case MAZE_SOLVED - 1: {
            auto &pRef = mazePosition().get();
            auto p = pRef;
            using enum Maze::Direction;
            using enum ColorInput::Position;
            using namespace values;
            if (colorInput & OL && p.x > MAZE_START.x && !(MAZE[pRef] & WEST)) p = p + WEST;
            if (colorInput & IL && p.y > MAZE_START.y && !(MAZE[pRef] & NORTH)) p = p + NORTH;
            if (colorInput & IR && p.y < MAZE_END.y && !(MAZE[pRef] & SOUTH)) p = p + SOUTH;
            if (colorInput & OR && p.x < MAZE_END.x && !(MAZE[pRef] & EAST)) p = p + EAST;
            mazePosition() = p;
            break;
        }
        default: break; // No action required
    }
}

void states::drawTimer(const char *value) {
    draw(value, graphic_display::Type::TIMER);
    log_d("Timer: %s", value);
}
