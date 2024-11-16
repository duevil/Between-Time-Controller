#ifndef MAZE_HPP
#define MAZE_HPP


struct Maze {
    static constexpr auto SIZE_X = 8;
    static constexpr auto SIZE_Y = 8;

    enum class Direction { NORTH, EAST, SOUTH, WEST };

    struct Position {
        unsigned char x;
        unsigned char y;

        constexpr Position() : x(0), y(0) {}
        constexpr Position(int x, int y) : x(x), y(y) {}
        constexpr explicit Position(auto p) : x(p >> 0 & 0xF), y(p >> 4 & 0xF) {}

        [[nodiscard]] constexpr auto toInt() const { return y << 4 | x << 0; }

        friend constexpr Position operator+(const Position &position, Direction direction) {
            using enum Direction;
            switch (direction) {
                case NORTH: return {position.x, static_cast<unsigned char>(position.y - 1)};
                case EAST: return {static_cast<unsigned char>(position.x + 1), position.y};
                case SOUTH: return {position.x, static_cast<unsigned char>(position.y + 1)};
                case WEST: return {static_cast<unsigned char>(position.x - 1), position.y};
                default: return {0, 0};
            }
        }

        friend constexpr bool operator==(const Position &lhs, const Position &rhs) = default;
    };

    struct Cell : Position {
        unsigned char value;

        constexpr Cell(Position position, unsigned char value) : Position(position), value(value) {}
        constexpr bool operator &(Direction direction) const { return value & 1 << static_cast<int>(direction); }
    };

    const unsigned int maze[8];

    [[nodiscard]] constexpr Cell operator[](const Position &position) const {
        if (position.x > 7 || position.y > 7) return {{}, 0};
        return {position, static_cast<unsigned char>(maze[position.y] >> position.x * 4 & 0xF)};
    }
};


#endif //MAZE_HPP
