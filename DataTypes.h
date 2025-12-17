// DataTypes.h
// Central small header for reusable puzzle-related types.
// Placed in a namespace to avoid colliding with the project's existing plain enums.

#pragma once

#include <cstdint>

namespace TurnPuzzleTypes {

// Edge state for candidate edges
enum class EdgeState : uint8_t {
    UNDECIDED = 0,
    INCLUDED,
    EXCLUDED
};

// Direction bitmask for cell connections (fits 1 byte)
enum Direction : uint8_t {
    NONE = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 4,
    LEFT = 8
};

// Turn type of a path
enum class TurnType : uint8_t {
    STRAIGHT = 0,
    RIGHT_ONLY,
    LEFT_ONLY,
    RIGHT_LEFT_MIXED
};

enum class SolveOutput : uint8_t {
    SOLVE_NO_CHANGE = 0,
    SOLVE_UPDATED,
    SOLVE_FAILED
};

} // namespace TurnPuzzleTypes
