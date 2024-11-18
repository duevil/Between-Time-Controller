#include "states.h"
#include "mqtt.h"
#include "mqtt_params.h"
#include "values.h"
#include <variant>
#include <ranges>
#include <bits/ranges_algobase.h>

using namespace states;
using enum Type;


struct : VisitedMaze {
    [[nodiscard]] const char *string() const override { return str; }

    void operator+=(Maze::Position pos) override {
        static auto lastPos = values::MAZE_START;
#ifdef SHOW_MAZE
        auto cell = values::MAZE[pos];
        str[cell.y * (Maze::SIZE_X + 1) + cell.x] = static_cast<char>(cell.value + 48);
        if (lastPos == pos) return;
        str[lastPos.y * (Maze::SIZE_X + 1) + lastPos.x] -= 16;
#else
        str[pos.y * (Maze::SIZE_X + 1) + pos.x] = 34;
        if (lastPos == pos) return;
        str[lastPos.y * (Maze::SIZE_X + 1) + lastPos.x] = 33;
#endif
        lastPos = pos;
    }

    void clear() {
        constexpr static auto SIZE = (Maze::SIZE_X + 1) * Maze::SIZE_Y + 1;
        for (auto i = 0; i < SIZE; ++i) {
            if (i == SIZE - 1) str[i] = '\0';
            else if (i % (Maze::SIZE_X + 1) == Maze::SIZE_X) str[i] = '\n';
            else str[i] = ' ';
        }
    }

private:
    char str[(Maze::SIZE_X + 1) * Maze::SIZE_Y + 1]{};
} visitedMaze_;


template<Type type>
struct SV final : State<type> {
    using value_t = typename State<type>::value_t;
    const mqtt::Topic &topic{
        mqtt::Topic::create((MQTT_ROOT + [] {
            using std::string_literals::operator""s;
            if constexpr (type == MAIN) return "main"s;
            if constexpr (type == TIMECODE) return "timecode"s;
            if constexpr (type == CANDLES) return "candles"s;
            if constexpr (type == MAZE_POSITION) return "maze"s;
            if constexpr (type == SCANNED_ITEMS) return "arcade"s;
            return ""s;
        }()).c_str(), onReceive)
    };

    static void onReceive(const uint8_t *payload, unsigned int length);
    ~SV() override = default;
    void onChange(value_t &value) override;
};


inline auto &stateValues() {
    static auto stateValues_ = []<auto... type>(std::integer_sequence<Type, type...>) {
        return std::unordered_map<Type, std::variant<SV<type>...>>{{type, SV<type>{}}...};
    }(std::integer_sequence<Type, MAIN, TIMECODE, CANDLES, MAZE_POSITION, SCANNED_ITEMS>{});
    return stateValues_;
}

template<Type type>
static SV<type> &get() { return get<SV<type>>(stateValues().at(type)); }


template<Type type> void SV<type>::onReceive(const uint8_t *payload, unsigned int length) {
    SV &sv = get<type>();
    std::string data{reinterpret_cast<const char *>(payload), length};
    auto val = std::stoi(data);
    sv.set(value_t{val});
}

template<Type type> void SV<type>::onChange(value_t &value) {
    auto data = [&value] {
        if /**/ constexpr (type == MAIN) return value.value;
        else if constexpr (type == TIMECODE) return value;
        else if constexpr (type == CANDLES || type == SCANNED_ITEMS) return value.to_ulong();
        else if constexpr (type == MAZE_POSITION) return value.toInt();
        else return 0;
    }();
    static_cast<void>(topic.publish(std::to_string(data).c_str()));
    ::onChange<type>(value);
}


State<MAIN> &states::mainState() { return get<MAIN>(); }
State<TIMECODE> &states::timecode() { return get<TIMECODE>(); }
State<CANDLES> &states::candles() { return get<CANDLES>(); }
State<MAZE_POSITION> &states::mazePosition() { return get<MAZE_POSITION>(); }
State<SCANNED_ITEMS> &states::scannedItems() { return get<SCANNED_ITEMS>(); }
VisitedMaze &states::visitedMaze() { return visitedMaze_; }

void states::subscribeToTopics() {
    for (auto &s_ : stateValues() | std::views::values) std::visit([](auto &s) { s.topic.subscribe(); }, s_);
}

void states::reset() {
    for (auto &s_ : stateValues() | std::views::values) std::visit([](auto &s) { s.set({}); }, s_);
    visitedMaze_.clear();
    visitedMaze_ += values::MAZE_START;
}
