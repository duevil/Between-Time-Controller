#ifndef MAZE_H
#define MAZE_H

#include <cstdint>

namespace maze {
    enum class Direction { NORTH, EAST, SOUTH, WEST };

    struct Position {
        uint8_t x;
        uint8_t y;

        [[nodiscard]] auto toInt() const { return y << 4 | x; }

        friend Position operator+(const Position &position, Direction direction) {
            switch (direction) {
                using
                enum Direction;
                case NORTH:
                    return {position.x, static_cast<uint8_t>(position.y - 1)};
                case EAST:
                    return {static_cast<uint8_t>(position.x + 1), position.y};
                case SOUTH:
                    return {position.x, static_cast<uint8_t>(position.y + 1)};
                case WEST:
                    return {static_cast<uint8_t>(position.x - 1), position.y};
            }
        }

        friend bool operator==(const Position &lhs, const Position &rhs) = default;
    };
}

#endif //MAZE_H
