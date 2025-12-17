#ifndef EDGE_H
#define EDGE_H

// Forward declaration
class Cell;

// Enum to represent edge state
enum EdgeState {
    UNDECIDED,
    INCLUDED,
    EXCLUDED,
    DELETED
};

class Edge {
public:
    Cell* cell1;
    Cell* cell2;
    EdgeState state;
    
    Edge(Cell* c1, Cell* c2);
    
    bool isUndecided() const;
    bool isIncluded() const;
    bool isExcluded() const;
    bool isDeleted() const;
    
    void setState(EdgeState newState);
};

#endif // EDGE_H
