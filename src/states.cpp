#include "states.h"
#include <ranges>
#include <variant>

using namespace states;
using enum Type;


template<Type type>
struct SV final : StateValue<type> {
    using value_t = typename StateValue<type>::value_t;
    ~SV() override = default;
    void onChange(value_t &value) override;
};

static auto stateValues = []<auto... types>(std::integer_sequence<Type, types...>) {
    return std::unordered_map<Type, std::variant<SV<types>...>>{{types, SV<types>{}}...};
}(std::integer_sequence<Type, MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS>{});


#pragma region Definitions


template<Type type>
static SV<type> &get() { return get<SV<type>>(stateValues.at(type)); }


bool MainValue::changeAllowed() const {
    // TODO: change values to reflect the proper timecodes
    auto timecode = get<TIMECODE>().get();
    switch (value) {
        case INPUT_FIELD_OPENED:
        case INPUT_FIELD_SOLVED: return timecode == 0;
        case CANDLES_SOLVED:
        case BOOK_BINARY_SOLVED: return timecode == 1;
        case MAZE_ACTIVE:
        case MAZE_SOLVED: return timecode == 2;
        case ARCADE_UNLOCKED:
        case ALL_ITEMS_SCANNED: return timecode == 3;
        default: return true;
    }
}


StateValue<MAIN> &states::mainState() { return get<MAIN>(); }
StateValue<TIMECODE> &states::timecode() { return get<TIMECODE>(); }
StateValue<CANDLES> &states::candles() { return get<CANDLES>(); }
StateValue<MAZE_POSITION> &states::mazePosition() { return get<MAZE_POSITION>(); }
StateValue<SCANNED_ITEMS> &states::scannedItems() { return get<SCANNED_ITEMS>(); }

static ColorCallback colorCallback;
void states::setColorCallback(ColorCallback callback) { colorCallback = std::move(callback); }


#pragma endregion
#pragma region Logic


template<> void SV<MAIN>::onChange(value_t &value) {
    // TODO: state handling
}

template<> void SV<TIMECODE>::onChange(value_t &value) {
    // TODO: state handling
}

template<> void SV<CANDLES>::onChange(value_t &value) {
    // TODO: state handling
}

template<> void SV<MAZE_POSITION>::onChange(value_t &value) {
    // TODO: state handling
}

template<> void SV<SCANNED_ITEMS>::onChange(value_t &value) {
    // TODO: state handling
}


void states::processInput(const ColorInput &input) {
    // TODO: input processing and state updating chain
    colorCallback(input.colors);
}

void states::reset() {
    for (auto &svv : std::views::values(stateValues)) {
        std::visit([](auto &sv) { sv = {}; }, svv);
    }
}


#pragma endregion
