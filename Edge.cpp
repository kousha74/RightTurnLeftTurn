#include "Edge.h"
#include "Cell.h"

Edge::Edge(Cell* c1, Cell* c2) : cell1(c1), cell2(c2), state(UNDECIDED) {}

bool Edge::isUndecided() const {
    return state == UNDECIDED;
}

bool Edge::isIncluded() const {
    return state == INCLUDED;
}

bool Edge::isExcluded() const {
    return state == EXCLUDED;
}

void Edge::setState(EdgeState newState) {
    if (state == newState) return;
    
    // Handle degree changes
    if (state == INCLUDED) {
        // Changing from INCLUDED to something else - decrease degree
        cell1->setDegree(cell1->getDegree() - 1);
        cell2->setDegree(cell2->getDegree() - 1);
    }
    
    if (newState == INCLUDED) {
        // Changing to INCLUDED - increase degree
        cell1->setDegree(cell1->getDegree() + 1);
        cell2->setDegree(cell2->getDegree() + 1);
    }
    
    state = newState;
}
