#ifndef STATES_H
#define STATES_H

#include "Maze.hpp"
#include "ColorInput.hpp"


namespace states {
    enum class Type { MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS };

    struct MainValue {
        enum Value : uint8_t {
            IDLE               = 0,
            STARTED            = 1,
            INPUT_FIELD_OPENED = 2,
            INPUT_FIELD_SOLVED = 3,
            CANDLES_PLACED     = 4,
            CANDLES_SOLVED     = 5,
            BOOK_BINARY_SOLVED = 6,
            MAZE_ACTIVE        = 7,
            MAZE_SOLVED        = 8,
            ARCADE_UNLOCKED    = 9,
            ALL_ITEMS_SCANNED  = 10,
            GAME_WON           = 11,
            GAME_LOST          = 12
        } value;

        MainValue() : value(IDLE) {}
        explicit(false) MainValue(auto v) : value(static_cast<Value>(v)) {}
        bool operator==(const MainValue &other) const { return value == other.value; }
        explicit operator Value() const { return value; }
    };

    struct VisitedMaze {
        virtual ~VisitedMaze() = default;
        [[nodiscard]] virtual const char *string() const = 0;
        virtual void operator+=(Maze::Position pos) = 0;
    };

    template<Type type>
    using ValueType = std::conditional_t<
        type == Type::MAIN, MainValue, std::conditional_t<
            type == Type::TIMECODE, uint16_t, std::conditional_t<
                type == Type::CANDLES, std::bitset<4>, std::conditional_t<
                    type == Type::MAZE_POSITION, Maze::Position, std::conditional_t<
                        type == Type::SCANNED_ITEMS, std::bitset<8>, void>>>>>;

    template<Type type>
    struct State {
        using value_t = ValueType<type>;
        State() = default;
        virtual ~State() = 0;
        [[nodiscard]] const value_t &get() const { return this->value; }
        const value_t &operator*() const { return get(); }
        const value_t *operator->() { return &this->value; }

        void set(const value_t &value) {
            if (this->value == value) return;
            this->value = value;
            onChange(this->value);
        }

        State &operator=(value_t value) {
            set(value);
            return *this;
        }

    protected:
        virtual void onChange(value_t &value) = 0;

    private:
        value_t value{};
    };

    template<Type type> State<type>::~State() = default;


    State<Type::MAIN> &mainState();
    State<Type::TIMECODE> &timecode();
    State<Type::CANDLES> &candles();
    State<Type::MAZE_POSITION> &mazePosition();
    State<Type::SCANNED_ITEMS> &scannedItems();
    VisitedMaze &visitedMaze();
    const char *timer();

    bool gameRunning();
    void subscribeToTopics();
    void reset();
    void processInput(const ColorInput::Input &input);
    void drawTimer(const char *value = timer());
    template<Type type>
    void onChange(const ValueType<type> &value);
}


#endif //STATES_H
