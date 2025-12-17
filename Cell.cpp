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
    // Special case: cells with degree 0
    if (degree == 0) {
        int undecidedCount = 0;
        Edge* undecidedEdge = nullptr;
        
        for (Edge* edge : edges) {
            if (edge->isUndecided()) {
                undecidedCount++;
                undecidedEdge = edge;
            }
        }
        
        if (undecidedCount == 0) {
            // No undecided edges and degree is 0 - FAIL
            return TurnPuzzleTypes::SolveOutput::SOLVE_FAILED;
        }
        
        if (undecidedCount == 1) {
            // Exactly one undecided edge - mark it as INCLUDED
            undecidedEdge->setState(INCLUDED);
            return TurnPuzzleTypes::SolveOutput::SOLVE_UPDATED;
        }
        
        // More than one undecided edge - no change yet
        return TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE;
    }
    
    // Determine maximum allowed degree based on cell type
    int maxDegree;
    if (cellType == HEAD) {
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
    
    
    return TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE;
}

int Cell::getDegree() const {
    return degree;
}

void Cell::setDegree(int deg) {
    degree = deg;
}
