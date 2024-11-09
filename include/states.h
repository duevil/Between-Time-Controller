#ifndef STATES_H
#define STATES_H

#include "Maze.hpp"
#include "ColorInput.hpp"
#include <functional>


namespace states {
    enum class Type { MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS };

    struct MainValue {
        enum Value : uint8_t {
            IDLE               = 0,
            STARTED            = 1,
            INPUT_FIELD_OPENED = 2,
            INPUT_FIELD_SOLVED = 3,
            CANDLES_SOLVED     = 4,
            BOOK_BINARY_SOLVED = 5,
            MAZE_ACTIVE        = 6,
            MAZE_SOLVED        = 7,
            ARCADE_UNLOCKED    = 8,
            ALL_ITEMS_SCANNED  = 9,
            GAME_WON           = 10,
            GAME_LOST          = 11
        } value;

        MainValue() : value(IDLE) {}
        explicit(false) MainValue(auto v) : value(static_cast<Value>(v)) {}
        [[nodiscard]] bool changeAllowed() const;
        bool operator==(const MainValue &other) const { return value == other.value; }
        explicit operator Value() const { return value; }
    };

    template<Type type>
    using ValueType = std::conditional_t<
        type == Type::MAIN, MainValue, std::conditional_t<
            type == Type::TIMECODE, uint16_t, std::conditional_t<
                type == Type::CANDLES, std::bitset<4>, std::conditional_t<
                    type == Type::MAZE_POSITION, Maze::Position, std::conditional_t<
                        type == Type::SCANNED_ITEMS, std::bitset<8>, void>>>>>;

    template<Type type>
    struct StateValue {
        using value_t = ValueType<type>;
        using Callback = std::function<void(value_t)>;
        StateValue() = default;
        virtual ~StateValue() = 0;
        void setCallback(const Callback &callback) { this->callback = callback; }
        const value_t &get() const { return this->value; }

        void set(const value_t &value) {
            if (this->value != value) {
                this->value = value;
                callback(this->value);
                onChange(this->value);
            }
        }

        StateValue &operator=(value_t value) {
            set(value);
            return *this;
        }

    protected:
        virtual void onChange(value_t &value) = 0;

    private:
        Callback callback{};
        value_t value{};
    };

    template<Type type> StateValue<type>::~StateValue() = default;

    StateValue<Type::MAIN> &mainState();
    StateValue<Type::TIMECODE> &timecode();
    StateValue<Type::CANDLES> &candles();
    StateValue<Type::MAZE_POSITION> &mazePosition();
    StateValue<Type::SCANNED_ITEMS> &scannedItems();

    using ColorCallback = std::function<void(const ColorInput::Colors &)>;
    void setColorCallback(ColorCallback callback);
    void processInput(const ColorInput &input);
    void reset();
}


#endif //STATES_H
