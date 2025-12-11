#ifndef TURNPUZZLE_H
#define TURNPUZZLE_H

#include <vector>
#include <string>
#include "Path.h"
#include "Cell.h"

// Forward declarations
class Edge;

// Enum to represent edge state
enum EdgeState {
    UNDECIDED,
    INCLUDED,
    EXCLUDED
};

// Enum to represent connections in each cell
enum Direction {
    NONE = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 4,
    LEFT = 8
};

// Edge class
class Edge {
public:
    Cell* cell1;
    Cell* cell2;
    EdgeState state;
    
    Edge(Cell* c1, Cell* c2) : cell1(c1), cell2(c2), state(UNDECIDED) {}
    
    bool isUndecided() const { return state == UNDECIDED; }
    bool isIncluded() const { return state == INCLUDED; }
    bool isExcluded() const { return state == EXCLUDED; }
    
    void setState(EdgeState newState) {
        if (state == newState) return;
        
        // Handle degree changes
        if (state == INCLUDED) {
            // Changing from INCLUDED to something else - decrease degree
            cell1->degree--;
            cell2->degree--;
        }
        
        if (newState == INCLUDED) {
            // Changing to INCLUDED - increase degree
            cell1->degree++;
            cell2->degree++;
        }
        
        state = newState;
    }
};

class TurnPuzzle {
public:
    // Constructor
    TurnPuzzle(int size);
    
    // Destructor
    ~TurnPuzzle();
    
    // Member functions
    void generateSolution();
    void exportToSVG(const std::string& filename) const;
    int getSize() const;
    void printSolution() const;
    void resetVisitedFlags();
    void findPath(Cell* startCell, Path& path);
    
private:
    // Private member variables
    int gridSize;
    std::vector<std::vector<Cell*>> cells;  // 2D grid of cells
    std::vector<Edge*> edges;               // All edges
    
    // Legacy - keeping for compatibility
    std::vector<std::vector<int>> grid; // Each cell stores a bitmask of connections
    std::vector<std::vector<int>> pathId; // Each cell stores which path it belongs to (-1 if none)
    
    // Path tracking
    int nextPathId;
    std::vector<TurnType> pathTypes; // Turn type for each path
    
    // Helper functions
    void initializeGrid();
    void initializeEdges();
    int getConnectionCount(int row, int col) const;
    bool hasConnection(int row, int col, int direction) const;
    bool canAddEdge(const Edge& edge);
    int getOppositeDirection(int direction) const;
    int getRightTurn(int direction) const;
    int getLeftTurn(int direction) const;
    int getDirectionBetween(int r1, int c1, int r2, int c2) const;
    TurnType getTurnType(int fromDir, int toDir) const;
    TurnType getPathTurnType(int pathIdx) const;
    void updatePathType(int pathIdx);
};

#endif // TURNPUZZLE_H
