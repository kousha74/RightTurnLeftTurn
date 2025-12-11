#ifndef PATH_H
#define PATH_H

#include <vector>
#include "Cell.h"

// Enum to represent turn types
enum TurnType {
    STRAIGHT,
    RIGHT_ONLY,
    LEFT_ONLY,
    RIGHT_LEFT_MIXED
};

class Path {
public:
    std::vector<Cell*> cells;
    TurnType turnType;
    bool dirty;
    
    Path();
    
    void addCell(Cell* cell);
    int getLength() const;
    void calculateTurnType();
};

#endif // PATH_H
