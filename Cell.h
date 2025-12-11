#ifndef CELL_H
#define CELL_H

#include <vector>

// Forward declaration
class Edge;

// Enum to represent cell types
enum CellType {
    UNMARKED,
    HEAD,
    MIDDLE,
    TAIL
};

class Cell {
public:
    static int idCounter;
    int id;
    int row;
    int col;
    int degree;
    bool visited;
    CellType cellType;
    std::vector<Edge*> edges;
    
    Cell(int r, int c);
    
    void addEdge(Edge* edge);
};

#endif // CELL_H
