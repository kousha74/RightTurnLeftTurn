# RightTurnLeftTurn

A turn puzzle generator that creates path-based puzzles where paths can only turn in one direction (either all left turns OR all right turns, never mixed).

## Features

- Generates random puzzles on an NxN grid
- Prevents closed loops in solutions
- Marks cells as HEAD or TAIL for puzzle presentation
- Exports puzzles to SVG format for visualization
- Includes constraint-solving framework

## Building

The project uses CMake and includes a convenient build script:

```bash
./build.sh
```

This will:
1. Create a `build` directory if it doesn't exist
2. Run CMake to configure the project
3. Compile all source files
4. Generate the `HelloWorld` executable in the `build` directory

## Running

After building, run the program with:

```bash
./build/HelloWorld
```

This will:
- Generate an 8x8 turn puzzle
- Mark cells as HEAD or TAIL
- Print the solution grid to the console
- Export the puzzle to `solution.svg`
- Test the solver algorithm

## Requirements

- C++17 compatible compiler (g++, clang++)
- CMake 3.10 or higher
- Standard build tools (make)

## Project Structure

- `Cell.h/cpp` - Grid cell representation with connections
- `Path.h/cpp` - Path class with turn type detection
- `TurnPuzzle.h/cpp` - Main puzzle generator and solver
- `DataTypes.h` - Centralized type definitions
- `main.cpp` - Entry point and example usage