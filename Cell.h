#ifndef CELL_H
#define CELL_H

#include <vector>

// Forward declaration
class Edge;

class Cell {
public:
    static int idCounter;
    int id;
    int row;
    int col;
    int degree;
    bool visited;
    std::vector<Edge*> edges;
    
    Cell(int r, int c);
    
    void addEdge(Edge* edge);
};

#endif // CELL_H
