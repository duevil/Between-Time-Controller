#ifndef STATES_H
#define STATES_H

#include "Observable.hpp"
#include <bitset>
#include "maze.h"

namespace states {
    enum class Type { MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS };
    enum class Main : uint8_t {
        IDLE = 0,
        STARTED = 1,
        INPUT_FIELD_OPENED = 2,
        INPUT_FIELD_SOLVED = 3,
        CANDLES_SOLVED = 4,
        BOOK_BINARY_SOLVED = 5,
        MAZE_ACTIVE = 6,
        MAZE_SOLVED = 7,
        ARCADE_UNLOCKED = 8,
        ALL_ITEMS_SCANNED = 9,
        GAME_WON = 10,
        GAME_LOST = 11
    };

    template<Type TYPE, typename T>
    struct StateValue : public Observable<T> {
        using ValueType = T;
        using BaseType = StateValue;

        const Type type = TYPE;
        ~StateValue() override = default;

        StateValue &operator=(const T &value) override {
            Observable<T>::operator=(value);
            return *this;
        }
    };

    struct Candles : public StateValue<Type::CANDLES, std::bitset<4>> {
        Candles();
        Candles &operator=(const ValueType &value) override;
        void setC1(bool val);
        void setC2(bool val);
        void setC3(bool val);
        void setC4(bool val);
    };

    struct MazePosition : public StateValue<Type::MAZE_POSITION, maze::Position> {
        MazePosition();
        MazePosition &operator=(const ValueType &value) override;
        void move(maze::Direction direction);
    };

    struct ScannedItems : public StateValue<Type::SCANNED_ITEMS, std::bitset<8>> {
        ScannedItems();
        ScannedItems &operator=(const ValueType &value) override;
        void scanI1();
        void scanI2();
        void scanI3();
        void scanI4();
        void scanI5();
        void scanI6();
        void scanI7();
        void scanI8();
    };

    inline StateValue<Type::MAIN, Main> main{};
    inline StateValue<Type::TIMECODE, uint16_t> timecode{};
    inline Candles candles{};
    inline MazePosition mazePosition{};
    inline ScannedItems scannedItems{};

    void resetAll();
}

#endif //STATES_H
