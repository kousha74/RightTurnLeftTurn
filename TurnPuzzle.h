#ifndef TURNPUZZLE_H
#define TURNPUZZLE_H

#include <vector>
#include <string>
#include <fstream>
#include "Path.h"
#include "Cell.h"
#include "Edge.h"
#include "DataTypes.h"

// Direction enum for cell connections (bitmask)
enum Direction {
    NONE = 0,
    UP = 1,
    RIGHT = 2,
    DOWN = 4,
    LEFT = 8
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
    bool findPaths(std::vector<Path>& paths);
    void markCells();
    bool GeneratePuzzle();  // Generates solution, marks cells, and checks for different solution
    void solvePuzzle();  // Tries to find different valid solutions
    int FindDifferentSolution(int solutionNumber);  // Tries to find a different valid solution, returns edge index or -1
    TurnPuzzleTypes::SolveOutput SolveCells();  // Calls Solve() on each cell
    bool isSolved();  // Checks if puzzle is solved (HEAD degree=1, others degree=2)
    
private:
    // Private member variables
    int gridSize;
    std::vector<Cell*> cells;  // Flat array of all cells
    std::vector<Edge*> edges;               // All edges
    std::vector<EdgeState> originalSolution;  // Original solution edge states
    std::ofstream logFile;  // Log file for debugging
    
    // Helper functions
    void initializeGrid();
    void initializeEdges();
    Cell* getCell(int row, int col) const;  // Access cell by row/col
    void SaveEdgeStates(std::vector<EdgeState>& edgeStates);
    void RestoreEdgeStates(const std::vector<EdgeState>& edgeStates);
    int FindDifferentEdge(const std::vector<EdgeState>& first, const std::vector<EdgeState>& second);
    bool canAddEdge(const Edge& edge);
    bool tryConnectHeadToTail(Cell* head, Cell* tail, std::vector<Cell*>& unpairedHeads, std::vector<Cell*>& unpairedTails);
    bool findPathBetween(Cell* start, Cell* end, Path& path);
};

#endif // TURNPUZZLE_H
