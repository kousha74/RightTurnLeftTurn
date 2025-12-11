#include "TurnPuzzle.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <queue>
#include <set>

// Constructor
TurnPuzzle::TurnPuzzle(int size) : gridSize(size), nextPathId(0) {
    std::cout << "TurnPuzzle created with grid size: " << gridSize << "x" << gridSize << std::endl;
    initializeGrid();
    initializeEdges();
}

// Destructor
TurnPuzzle::~TurnPuzzle() {
    // Clean up cells
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            delete cells[i][j];
        }
    }
    
    // Clean up edges
    for (Edge* edge : edges) {
        delete edge;
    }
    
    std::cout << "TurnPuzzle destroyed" << std::endl;
}

void TurnPuzzle::initializeGrid() {
    // Create cells
    cells.resize(gridSize);
    for (int i = 0; i < gridSize; i++) {
        cells[i].resize(gridSize);
        for (int j = 0; j < gridSize; j++) {
            cells[i][j] = new Cell(i, j);
        }
    }
}

void TurnPuzzle::resetVisitedFlags() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            cells[i][j]->visited = false;
        }
    }
}

void TurnPuzzle::findPath(Cell* startCell, Path& path) {
    // If degree is 0, add just this cell
    if (startCell->degree == 0) {
        path.addCell(startCell);
        return;
    }
    
    // If degree is 2, can't use as starting point
    if (startCell->degree == 2) {
        return;
    }
    
    // If degree is 1, trace the path
    if (startCell->degree == 1) {
        Cell* currentCell = startCell;
        
        while (currentCell != nullptr) {
            // Mark as visited and add to path
            currentCell->visited = true;
            path.addCell(currentCell);
            
            // Find the next unvisited cell connected by an INCLUDED edge
            Cell* nextCell = nullptr;
            for (Edge* edge : currentCell->edges) {
                if (edge->isIncluded()) {
                    // Get the other cell
                    Cell* otherCell = (edge->cell1 == currentCell) ? edge->cell2 : edge->cell1;
                    
                    if (!otherCell->visited) {
                        nextCell = otherCell;
                        break;
                    }
                }
            }
            
            // Move to next cell (or nullptr if we've reached the end)
            currentCell = nextCell;
        }
    }
}

void TurnPuzzle::initializeEdges() {
    // Create all horizontal edges (connecting cells left-right)
    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize - 1; col++) {
            Edge* edge = new Edge(cells[row][col], cells[row][col + 1]);
            edges.push_back(edge);
            cells[row][col]->addEdge(edge);
            cells[row][col + 1]->addEdge(edge);
        }
    }
    
    // Create all vertical edges (connecting cells up-down)
    for (int row = 0; row < gridSize - 1; row++) {
        for (int col = 0; col < gridSize; col++) {
            Edge* edge = new Edge(cells[row][col], cells[row + 1][col]);
            edges.push_back(edge);
            cells[row][col]->addEdge(edge);
            cells[row + 1][col]->addEdge(edge);
        }
    }
    
    std::cout << "Initialized " << cells.size() * cells[0].size() << " cells and " 
              << edges.size() << " edges" << std::endl;
}

int TurnPuzzle::getSize() const {
    return gridSize;
}

bool TurnPuzzle::hasConnection(int row, int col, int direction) const {
    return (grid[row][col] & direction) != 0;
}

int TurnPuzzle::getConnectionCount(int row, int col) const {
    int count = 0;
    if (hasConnection(row, col, UP)) count++;
    if (hasConnection(row, col, RIGHT)) count++;
    if (hasConnection(row, col, DOWN)) count++;
    if (hasConnection(row, col, LEFT)) count++;
    return count;
}

int TurnPuzzle::getOppositeDirection(int direction) const {
    if (direction == UP) return DOWN;
    if (direction == DOWN) return UP;
    if (direction == LEFT) return RIGHT;
    if (direction == RIGHT) return LEFT;
    return NONE;
}

int TurnPuzzle::getRightTurn(int direction) const {
    if (direction == UP) return RIGHT;
    if (direction == RIGHT) return DOWN;
    if (direction == DOWN) return LEFT;
    if (direction == LEFT) return UP;
    return NONE;
}

int TurnPuzzle::getLeftTurn(int direction) const {
    if (direction == UP) return LEFT;
    if (direction == LEFT) return DOWN;
    if (direction == DOWN) return RIGHT;
    if (direction == RIGHT) return UP;
    return NONE;
}

int TurnPuzzle::getDirectionBetween(int r1, int c1, int r2, int c2) const {
    if (r2 < r1) return UP;
    if (r2 > r1) return DOWN;
    if (c2 < c1) return LEFT;
    if (c2 > c1) return RIGHT;
    return NONE;
}



bool TurnPuzzle::canAddEdge(const Edge& edge) {
    // Only consider UNDECIDED edges
    if (!edge.isUndecided()) return false;
    
    // Check degree constraint: neither cell can have more than 2 connections
    if (edge.cell1->degree >= 2) return false;
    if (edge.cell2->degree >= 2) return false;
    
    // Find the path connected to each endpoint
    Path path1;
    Path path2;
    
    resetVisitedFlags();
    
    findPath(edge.cell1, path1);
    findPath(edge.cell2, path2);
    
    // If either path has zero cells, return false
    if (path1.getLength() == 0 || path2.getLength() == 0) {
        return false;
    }
    
    // Check if both cells are part of the same path (would create a closed loop)
    // This happens when both endpoints have degree 1 and the paths share cells
    if (edge.cell1->degree == 1 && edge.cell2->degree == 1) {
        // Check if cell2 appears in path1 (meaning they're the same path)
        for (Cell* cell : path1.cells) {
            if (cell == edge.cell2) {
                // Both endpoints are part of the same path - would create a loop
                return false;
            }
        }
    }
    
    // Combine the paths: path1 in reverse, then path2
    Path combinedPath;
    
    // Add path1 in reverse order
    for (int i = path1.getLength() - 1; i >= 0; i--) {
        combinedPath.addCell(path1.cells[i]);
    }
    
    // Add path2 in normal order
    for (int i = 0; i < path2.getLength(); i++) {
        combinedPath.addCell(path2.cells[i]);
    }
    
    // Calculate the turn type of the combined path
    combinedPath.calculateTurnType();
    
    // Return true only if the path doesn't have mixed turns
    return combinedPath.turnType != RIGHT_LEFT_MIXED;
}

void TurnPuzzle::generateSolution() {
    std::cout << "Generating solution..." << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<Edge*> addableEdges;
    
    do {
        // Clear the list and find all edges that can be added using canAddEdge
        addableEdges.clear();
        for (Edge* edge : edges) {
            if (canAddEdge(*edge)) {
                addableEdges.push_back(edge);
            }
        }
        
        // If edges can be added, pick one randomly
        if (!addableEdges.empty()) {
            std::uniform_int_distribution<> dis(0, addableEdges.size() - 1);
            Edge* selectedEdge = addableEdges[dis(gen)];
            
            // Set the edge state to INCLUDED (this will update cell degrees automatically)
            selectedEdge->setState(INCLUDED);
        }
        
    } while (!addableEdges.empty());
    

}

void TurnPuzzle::printSolution() const {
    std::cout << "\nGrid with included edges:" << std::endl;
    
    // Print cell degrees
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            std::cout << "(" << i << "," << j << "):deg=" << cells[i][j]->degree << " ";
        }
        std::cout << std::endl;
    }
    
    // Print included edges
    std::cout << "\nIncluded edges:" << std::endl;
    int edgeCount = 0;
    for (const Edge* edge : edges) {
        if (edge->isIncluded()) {
            edgeCount++;
            std::cout << "Edge " << edgeCount << ": (" << edge->cell1->row << "," << edge->cell1->col 
                     << ") <-> (" << edge->cell2->row << "," << edge->cell2->col << ")" << std::endl;
        }
    }
    std::cout << "Total included edges: " << edgeCount << std::endl;
}

void TurnPuzzle::exportToSVG(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    
    const int cellSize = 60;
    const int padding = 40;
    const int width = gridSize * cellSize + 2 * padding;
    const int height = gridSize * cellSize + 2 * padding;
    const int lineThickness = 3;
    
    // SVG header
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg width=\"" << width << "\" height=\"" << height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    file << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    
    // Draw grid
    file << "  <g stroke=\"#cccccc\" stroke-width=\"1\">\n";
    for (int i = 0; i <= gridSize; i++) {
        int pos = padding + i * cellSize;
        file << "    <line x1=\"" << padding << "\" y1=\"" << pos 
             << "\" x2=\"" << (padding + gridSize * cellSize) << "\" y2=\"" << pos << "\"/>\n";
        file << "    <line x1=\"" << pos << "\" y1=\"" << padding 
             << "\" x2=\"" << pos << "\" y2=\"" << (padding + gridSize * cellSize) << "\"/>\n";
    }
    file << "  </g>\n";
    
    // Draw included edges
    file << "  <g stroke=\"#2196F3\" stroke-width=\"" << lineThickness << "\" stroke-linecap=\"round\">\n";
    for (const Edge* edge : edges) {
        if (edge->isIncluded()) {
            int x1 = padding + edge->cell1->col * cellSize + cellSize / 2;
            int y1 = padding + edge->cell1->row * cellSize + cellSize / 2;
            int x2 = padding + edge->cell2->col * cellSize + cellSize / 2;
            int y2 = padding + edge->cell2->row * cellSize + cellSize / 2;
            
            file << "    <line x1=\"" << x1 << "\" y1=\"" << y1 
                 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\"/>\n";
        }
    }
    file << "  </g>\n";
    
    // Draw cell centers
    file << "  <g fill=\"#2196F3\">\n";
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int centerX = padding + j * cellSize + cellSize / 2;
            int centerY = padding + i * cellSize + cellSize / 2;
            file << "    <circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"4\"/>\n";
        }
    }
    file << "  </g>\n";
    
    file << "</svg>\n";
    file.close();
    
    std::cout << "SVG exported to: " << filename << std::endl;
}
