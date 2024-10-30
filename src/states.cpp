#include "states.h"
#include "puzzle_values.h"


using namespace states;


Candles::Candles() : Candles::BaseType() {
    *this += [](const ValueType &value) {
        if (main == Main::INPUT_FIELD_SOLVED && value.all()) {
            main = Main::CANDLES_SOLVED;
        }
    };
}

inline Candles &Candles::operator=(const ValueType &value) {
    BaseType::operator=(value);
    return *this;
}

inline void Candles::setC1(bool val) { *this = static_cast<ValueType>(*this).set(0, val); }

inline void Candles::setC2(bool val) { *this = static_cast<ValueType>(*this).set(1, val); }

inline void Candles::setC3(bool val) { *this = static_cast<ValueType>(*this).set(2, val); }

inline void Candles::setC4(bool val) { *this = static_cast<ValueType>(*this).set(3, val); }


MazePosition::MazePosition() : MazePosition::BaseType() {
    *this += [](const ValueType &value) {
        if (main == Main::MAZE_ACTIVE && value == puzzle_values::MAZE_END) {
            main = Main::MAZE_SOLVED;
        }
    };
}

inline MazePosition &MazePosition::operator=(const ValueType &value) {
    BaseType::operator=(value);
    return *this;
}

void MazePosition::move(maze::Direction direction) {
    // TODO
}


ScannedItems::ScannedItems() : ScannedItems::BaseType() {
    *this += [](const ValueType &value) {
        if (main == Main::ARCADE_UNLOCKED && value.all()) {
            main = Main::ALL_ITEMS_SCANNED;
        }
    };
}

inline ScannedItems &ScannedItems::operator=(const ValueType &value) {
    BaseType::operator=(value);
    return *this;
}

inline void ScannedItems::scanI1() { *this = get() | ValueType{1 << 0}; }

inline void ScannedItems::scanI2() { *this = get() | ValueType{1 << 1}; }

inline void ScannedItems::scanI3() { *this = get() | ValueType{1 << 2}; }

inline void ScannedItems::scanI4() { *this = get() | ValueType{1 << 3}; }

inline void ScannedItems::scanI5() { *this = get() | ValueType{1 << 4}; }

inline void ScannedItems::scanI6() { *this = get() | ValueType{1 << 5}; }

inline void ScannedItems::scanI7() { *this = get() | ValueType{1 << 6}; }

inline void ScannedItems::scanI8() { *this = get() | ValueType{1 << 7}; }


inline void states::resetAll() {
    main = Main::IDLE;
    timecode = 0;
    candles = Candles::ValueType{};
    mazePosition = maze::Position{0, 0};
    scannedItems = ScannedItems::ValueType{};
}
