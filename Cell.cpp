#include "Cell.h"
#include "Edge.h"

// Initialize static counter
int Cell::idCounter = 0;

Cell::Cell(int r, int c) : row(r), col(c), degree(0), visited(false), cellType(UNMARKED) {
    id = idCounter++;
}

void Cell::addEdge(Edge* edge) {
    edges.push_back(edge);
}

TurnPuzzleTypes::SolveOutput Cell::Solve() {
    // Determine maximum allowed degree based on cell type
    int maxDegree;
    if (cellType == HEAD || cellType == TAIL) {
        maxDegree = 1;
    } else { // UNMARKED
        maxDegree = 2;
    }
    
    // Check if degree exceeds limit (FAIL condition)
    if (degree > maxDegree) {
        return TurnPuzzleTypes::SolveOutput::SOLVE_FAILED;
    }
    
    // Check if degree equals limit (EXCLUDE remaining undecided edges)
    if (degree == maxDegree) {
        bool updated = false;
        for (Edge* edge : edges) {
            if (edge->isUndecided()) {
                edge->setState(EXCLUDED);
                updated = true;
            }
        }
        return updated ? TurnPuzzleTypes::SolveOutput::SOLVE_UPDATED 
                       : TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE;
    }
    
    // Check if remaining undecided edges must all be included
    // degree < maxDegree at this point
    int needed = maxDegree - degree;
    int undecidedCount = 0;
    
    for (Edge* edge : edges) {
        if (edge->isUndecided()) {
            undecidedCount++;
        }
    }
    
    if (undecidedCount < needed) {
        // Not enough edges to reach required degree (FAIL)
        return TurnPuzzleTypes::SolveOutput::SOLVE_FAILED;
    }

    if (undecidedCount == needed) {
        // Mark all undecided edges as INCLUDED
        for (Edge* edge : edges) {
            if (edge->isUndecided()) {
                edge->setState(INCLUDED);
            }
        }
        return TurnPuzzleTypes::SolveOutput::SOLVE_UPDATED;
    }
    
    return TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE;
}

int Cell::getDegree() const {
    return degree;
}

void Cell::setDegree(int deg) {
    degree = deg;
}
