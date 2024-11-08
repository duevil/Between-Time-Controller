#ifndef MAZE_H
#define MAZE_H


namespace maze {
    enum class Direction { NORTH, EAST, SOUTH, WEST };

    struct Position {
        unsigned char x;
        unsigned char y;

        constexpr Position() : x(0), y(0) {}
        constexpr Position(auto x, auto y) : x(x), y(y) {}
        constexpr explicit Position(auto p) : x(p >> 0 & 0xF), y(p >> 4 & 0xF) {}

        [[nodiscard]] auto toInt() const { return y << 4 | x << 0; }

        friend Position operator+(const Position &position, Direction direction) {
            using enum Direction;
            switch (direction) {
                case NORTH: return {position.x, static_cast<unsigned char>(position.y - 1)};
                case EAST: return {static_cast<unsigned char>(position.x + 1), position.y};
                case SOUTH: return {position.x, static_cast<unsigned char>(position.y + 1)};
                case WEST: return {static_cast<unsigned char>(position.x - 1), position.y};
                default: return {0, 0};
            }
        }

        friend bool operator==(const Position &lhs, const Position &rhs) = default;
    };
}


#endif //MAZE_H
