#include "Cell.h"

// Initialize static counter
int Cell::idCounter = 0;

Cell::Cell(int r, int c) : row(r), col(c), degree(0), visited(false) {
    id = idCounter++;
}

void Cell::addEdge(Edge* edge) {
    edges.push_back(edge);
}
