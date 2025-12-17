#ifndef CELL_H
#define CELL_H

#include <vector>
#include "DataTypes.h"

// Forward declaration
class Edge;

// Enum to represent cell types
enum CellType {
    UNMARKED,
    HEAD,
    TAIL
};

class Cell {
private:
    int degree;

public:
    static int idCounter;
    int id;
    int row;
    int col;
    bool visited;
    CellType cellType;
    std::vector<Edge*> edges;
    
    Cell(int r, int c);
    
    void addEdge(Edge* edge);
    TurnPuzzleTypes::SolveOutput Solve();
    
    // Getter and setter for degree
    int getDegree() const;
    void setDegree(int deg);
};

#endif // CELL_H
