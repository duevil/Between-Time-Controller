#include "states.h"
#include "values.h"
#include <ranges>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <variant>

#define _nl "\n"

using namespace states;
using enum Type;


template<Type type>
struct SV final : StateValue<type> {
    using value_t = typename StateValue<type>::value_t;
    ~SV() override = default;
    void onChange(value_t &value) override;
};

struct MazePositionHash {
    std::size_t operator()(Maze::Position pos) const {
        return std::hash<int>{}(pos.toInt());
    }
};

static auto stateValues = []<auto... types>(std::integer_sequence<Type, types...>) {
    return std::unordered_map<Type, std::variant<SV<types>...>>{{types, SV<types>{}}...};
}(std::integer_sequence<Type, MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS>{});
static ColorInput ci;
static bool inputMustReset = false;
static std::unordered_set<Maze::Position, MazePositionHash> visitedMazePositions{};

static bool changeAllowed();
static std::string maze_string();
static void processInput();


#pragma region Definitions


template<Type type>
static SV<type> &get() { return get<SV<type>>(stateValues.at(type)); }


StateValue<MAIN> &states::mainState() { return get<MAIN>(); }
StateValue<TIMECODE> &states::timecode() { return get<TIMECODE>(); }
StateValue<CANDLES> &states::candles() { return get<CANDLES>(); }
StateValue<MAZE_POSITION> &states::mazePosition() { return get<MAZE_POSITION>(); }
StateValue<SCANNED_ITEMS> &states::scannedItems() { return get<SCANNED_ITEMS>(); }

static ColorCallback colorCallback;
static DrawCallback drawCallback;
void states::setColorCallback(ColorCallback callback) { colorCallback = std::move(callback); }
void states::setDrawCallback(DrawCallback callback) { drawCallback = std::move(callback); }


#pragma endregion
#pragma region Logic


template<> void SV<MAIN>::onChange(value_t &value) {
    drawCallback(nullptr);
    switch (value.value) {
            using enum MainValue::Value;
        case IDLE:
            drawCallback("Waiting for the game" _nl "to start...");
            [[fallthrough]]
        case STARTED:
            // reset when changing to IDLE or STARTED
            reset();
            break;
        case INPUT_FIELD_SOLVED - 1:
            drawCallback(values::INPUT_CODE);
            break;
        case GAME_WON:
            drawCallback("Congratulations!" _nl "You won the game!");
            break;
        case GAME_LOST:
            drawCallback("Game over!" _nl "You lost!");
            break;
        default: break; // No action required
    }
    ci.inputs.reset();
    processInput();
    inputMustReset = true;
}

template<> void SV<TIMECODE>::onChange(value_t &value) {
    // TODO: currently, item scanning is skipped
    if (value == values::TC_0B11 && mainState()->value == MainValue::ARCADE_UNLOCKED) scannedItems() = 0b10101010;
    processInput();
}

template<> void SV<CANDLES>::onChange(value_t &value) {
    if (value != 0b1111) return;
    mainState() = MainValue::CANDLES_SOLVED;
}

template<> void SV<MAZE_POSITION>::onChange(value_t &value) {
    visitedMazePositions.insert(value);
    drawCallback(maze_string().c_str());
    if (value != values::MAZE_END) return;
    mainState() = MainValue::MAZE_SOLVED;
}

template<> void SV<SCANNED_ITEMS>::onChange(value_t &value) {
    if (value != 0b10101010) return;
    mainState() = MainValue::ALL_ITEMS_SCANNED;
}


void states::setInput(const ColorInput::Input &input) {
    ci.inputs = input;
    processInput();
}

void states::reset() {
    for (auto &svv : std::views::values(stateValues)) {
        std::visit([](auto &sv) { sv = {}; }, svv);
    }
    visitedMazePositions.clear();
    visitedMazePositions.reserve(Maze::SIZE_X * Maze::SIZE_Y);
    visitedMazePositions.insert(values::MAZE_START); // Start position is always visited
}


static bool changeAllowed() {
    auto tcOK = true;
    if (inputMustReset && ci.inputs.none()) inputMustReset = false;
    auto timecode = get<TIMECODE>().get();
    using enum MainValue::Value;
    switch (mainState()->value) {
        case INPUT_FIELD_OPENED - 1:
            tcOK = timecode == values::TC_0B00;
            break;
        case CANDLES_SOLVED - 1:
        case BOOK_BINARY_SOLVED - 1:
            tcOK = timecode == values::TC_0B01;
            break;
        case MAZE_SOLVED - 1:
            tcOK = timecode == values::TC_0B10;
            break;
        case ALL_ITEMS_SCANNED - 1:
            tcOK = timecode == values::TC_0B11;
            break;
        default: break; // No restrictions
    }
    // change allowed if timecode is set and input must not be reset
    if (tcOK && !inputMustReset) return true;
    if (!tcOK) {
        drawCallback("Waiting for sync...");
    } else if (!inputMustReset) {
        ci.sync();
        drawCallback("Turn all switches" _nl "off to continue...");
        colorCallback(ci.colors);
    }
    return false;
}

static std::string maze_string() {
    std::ostringstream oss;
    for (auto y = 0; y < Maze::SIZE_Y; ++y) {
        for (auto x = 0; x < Maze::SIZE_X; ++x) {
            // TODO: uncomment visited check
            if (auto pos = Maze::Position{x, y}; true /*visitedMazePositions.contains(pos)*/) {
                oss << static_cast<char>(values::MAZE[pos].value + 32);
            } else {
                oss << ' ';
            }
        }
        oss << '\n';
    }
    return oss.str();
}


static void processInput() {
    if (!changeAllowed()) return;
    switch (mainState()->value) {
        case MainValue::INPUT_FIELD_OPENED - 1:
            // Processing action to open input field
            if (ci & values::INPUT_FIELD_OPEN) mainState() = MainValue::INPUT_FIELD_OPENED;
            break;
        case MainValue::CANDLES_SOLVED - 1: {
            // Processing action to solve input field
            ci.colors = values::CANDLES_COLORS;
            auto candles_ = candles().get();
            for (auto i = 0; i < candles()->size(); ++i) {
                // TODO: check if candles are switched on in the correct order
                candles_[i] = static_cast<bool>(ci & values::CANDLES_ORDER[i]);
            }
            candles() = candles_;
            break;
        }
        case MainValue::BOOK_BINARY_SOLVED - 1:
            // Processing action to solve book binary
            ci.sync();
            if (ci.inputs == values::BOOK_BINARY) mainState() = MainValue::BOOK_BINARY_SOLVED;
            break;
        case MainValue::MAZE_SOLVED - 1: {
            // Processing action to move in maze
            ci.sync();
            ci[ColorInput::M].color = "";
            using enum Maze::Direction;
            using enum ColorInput::Position;
            auto mazePos = mazePosition().get();
            // move west
            if (ci & OL && mazePos.x > 0) mazePos = mazePos + WEST;
            // move north
            if (ci & IL && mazePos.y > 0) mazePos = mazePos + NORTH;
            // move south
            if (ci & IR && mazePos.y < Maze::SIZE_Y) mazePos = mazePos + SOUTH;
            // move east
            if (ci & OR && mazePos.x < Maze::SIZE_X) mazePos = mazePos + EAST;
            mazePosition() = mazePos;
            break;
        }
        default: break; // Inputs have no effect
    }
    colorCallback(ci.colors);
}


#pragma endregion
