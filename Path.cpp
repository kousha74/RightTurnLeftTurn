#include "Path.h"
#include "TurnPuzzle.h"

Path::Path() : turnType(STRAIGHT), dirty(true) {
}

void Path::addCell(Cell* cell) {
    cells.push_back(cell);
    dirty = true;
}

int Path::getLength() const {
    return cells.size();
}

void Path::calculateTurnType() {
    // If not dirty, return the cached value
    if (!dirty) {
        return;
    }
    
    dirty = false;
    // If path has less than 3 cells, it's straight (no turns possible)
    if (cells.size() < 3) {
        turnType = STRAIGHT;
        return;
    }
    
    bool hasRightTurn = false;
    bool hasLeftTurn = false;
    
    // Check each consecutive triple of cells to detect turns
    for (size_t i = 0; i < cells.size() - 2; i++) {
        Cell* cell1 = cells[i];
        Cell* cell2 = cells[i + 1];
        Cell* cell3 = cells[i + 2];
        
        // Calculate direction from cell1 to cell2
        int dir1Row = cell2->row - cell1->row;
        int dir1Col = cell2->col - cell1->col;
        
        // Calculate direction from cell2 to cell3
        int dir2Row = cell3->row - cell2->row;
        int dir2Col = cell3->col - cell2->col;
        
        // If directions are the same, no turn
        if (dir1Row == dir2Row && dir1Col == dir2Col) {
            continue;
        }
        
        // Determine if it's a right or left turn
        // Using cross product: positive = left turn, negative = right turn
        int crossProduct = dir1Row * dir2Col - dir1Col * dir2Row;
        
        if (crossProduct > 0) {
            hasLeftTurn = true;
        } else if (crossProduct < 0) {
            hasRightTurn = true;
        }
        
        // If we have both types, we can stop early
        if (hasRightTurn && hasLeftTurn) {
            break;
        }
    }
    
    // Set the turn type based on what we found
    if (hasRightTurn && hasLeftTurn) {
        // Mixed turns - shouldn't happen in a valid puzzle, default to STRAIGHT
        turnType = RIGHT_LEFT_MIXED;
    } else if (hasRightTurn) {
        turnType = RIGHT_ONLY;
    } else if (hasLeftTurn) {
        turnType = LEFT_ONLY;
    } else {
        turnType = STRAIGHT;
    }
}
