#include "TurnPuzzle.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include "DataTypes.h"

// Constructor
TurnPuzzle::TurnPuzzle(int size) : gridSize(size) {
    std::cout << "TurnPuzzle created with grid size: " << gridSize << "x" << gridSize << std::endl;
    initializeGrid();
    initializeEdges();
}

// Destructor
TurnPuzzle::~TurnPuzzle() {
    // Clean up cells
    for (Cell* cell : cells) {
        delete cell;
    }
    
    // Clean up edges
    for (Edge* edge : edges) {
        delete edge;
    }
    
    std::cout << "TurnPuzzle destroyed" << std::endl;
}

void TurnPuzzle::initializeGrid() {
    // Create cells in flat array
    cells.resize(gridSize * gridSize);
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            cells[i * gridSize + j] = new Cell(i, j);
        }
    }
}

Cell* TurnPuzzle::getCell(int row, int col) const {
    return cells[row * gridSize + col];
}

void TurnPuzzle::resetVisitedFlags() {
    for (Cell* cell : cells) {
        cell->visited = false;
    }
}

void TurnPuzzle::findPath(Cell* startCell, Path& path) {
    // If degree is 0, add just this cell
    if (startCell->getDegree() == 0) {
        path.addCell(startCell);
        return;
    }
    
    // If degree is 2, can't use as starting point
    if (startCell->getDegree() == 2) {
        return;
    }
    
    // If degree is 1, trace the path
    if (startCell->getDegree() == 1) {
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

bool TurnPuzzle::findPaths(std::vector<Path>& paths) {
    // Clear the paths vector
    paths.clear();
    
    // Reset visited flags
    resetVisitedFlags();
    
    // Find all HEAD cells and trace paths from them
    for (Cell* cell : cells) {
        // If this is a HEAD cell, trace the path from it
        if (cell->cellType == HEAD) {
            Path path;
            findPath(cell, path);
            
            // Only add non-empty paths
            if (path.getLength() > 0) {
                // Check if the last cell is a TAIL cell
                if (path.cells[path.getLength() - 1]->cellType != TAIL) {
                    return false;
                }
                
                // Calculate turn type and check if it's mixed
                path.calculateTurnType();
                if (path.turnType == RIGHT_LEFT_MIXED) {
                    return false;
                }
                
                paths.push_back(path);
            }
            
            // Reset visited flags for next HEAD cell
            resetVisitedFlags();
        }
    }
    
    return true;
}

void TurnPuzzle::initializeEdges() {
    // Create all horizontal edges (connecting cells left-right)
    for (int row = 0; row < gridSize; row++) {
        for (int col = 0; col < gridSize - 1; col++) {
            Cell* cell1 = getCell(row, col);
            Cell* cell2 = getCell(row, col + 1);
            Edge* edge = new Edge(cell1, cell2);
            edges.push_back(edge);
            cell1->addEdge(edge);
            cell2->addEdge(edge);
        }
    }
    
    // Create all vertical edges (connecting cells up-down)
    for (int row = 0; row < gridSize - 1; row++) {
        for (int col = 0; col < gridSize; col++) {
            Cell* cell1 = getCell(row, col);
            Cell* cell2 = getCell(row + 1, col);
            Edge* edge = new Edge(cell1, cell2);
            edges.push_back(edge);
            cell1->addEdge(edge);
            cell2->addEdge(edge);
        }
    }
    
    std::cout << "Initialized " << cells.size() << " cells and " 
              << edges.size() << " edges" << std::endl;
}

int TurnPuzzle::getSize() const {
    return gridSize;
}



bool TurnPuzzle::canAddEdge(const Edge& edge) {
    // Only consider UNDECIDED edges
    if (!edge.isUndecided()) return false;
    
    // Check degree constraint: neither cell can have more than 2 connections
    if (edge.cell1->getDegree() >= 2) return false;
    if (edge.cell2->getDegree() >= 2) return false;
    
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
    if (edge.cell1->getDegree() == 1 && edge.cell2->getDegree() == 1) {
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

void TurnPuzzle::markCells() {
    std::cout << "Marking cells as HEAD and TAIL..." << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    resetVisitedFlags();
    
    // Collect all cells with degree 1 (endpoints)
    std::vector<Cell*> endpoints;
    for (Cell* cell : cells) {
        if (cell->getDegree() == 1) {
            endpoints.push_back(cell);
        }
    }
    
    // Shuffle endpoints for random selection
    std::shuffle(endpoints.begin(), endpoints.end(), gen);
    
    // Process each unvisited endpoint
    for (Cell* startCell : endpoints) {
        if (!startCell->visited && startCell->getDegree() == 1) {
            Path path;
            findPath(startCell, path);
            
            if (path.getLength() > 0) {
                // Mark first cell as HEAD
                path.cells[0]->cellType = HEAD;
                
                // Middle cells remain UNMARKED (no need to mark them)
                
                // Mark last cell as TAIL
                if (path.getLength() > 1) {
                    path.cells[path.getLength() - 1]->cellType = TAIL;
                } else {
                    // If path has only one cell, mark it as both HEAD and TAIL (just HEAD)
                    path.cells[0]->cellType = HEAD;
                }
            }
        }
    }
    
    std::cout << "Cells marked!" << std::endl;
    
    // Save the original solution edge states
    SaveEdgeStates(originalSolution);
}

void TurnPuzzle::printSolution() const {
    std::cout << "\nGrid with included edges:" << std::endl;
    
    // Print cell degrees
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            Cell* cell = getCell(i, j);
            std::cout << "(" << i << "," << j << "):deg=" << cell->getDegree() << " ";
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
    
    // Draw cell centers based on their type
    // HEAD cells: solid circles
    file << "  <g fill=\"#2196F3\">\n";
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            Cell* cell = getCell(i, j);
            if (cell->cellType == HEAD) {
                int centerX = padding + j * cellSize + cellSize / 2;
                int centerY = padding + i * cellSize + cellSize / 2;
                file << "    <circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"6\"/>\n";
            }
        }
    }
    file << "  </g>\n";
    
    // UNMARKED cells with degree > 0: small filled circles (optional visualization)
    file << "  <g fill=\"#CCCCCC\">\n";
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            Cell* cell = getCell(i, j);
            if (cell->cellType == UNMARKED && cell->getDegree() > 0) {
                int centerX = padding + j * cellSize + cellSize / 2;
                int centerY = padding + i * cellSize + cellSize / 2;
                file << "    <circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"3\"/>\n";
            }
        }
    }
    file << "  </g>\n";
    
    // TAIL cells: empty circles (stroke only)
    file << "  <g fill=\"none\" stroke=\"#2196F3\" stroke-width=\"2\">\n";
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            Cell* cell = getCell(i, j);
            if (cell->cellType == TAIL) {
                int centerX = padding + j * cellSize + cellSize / 2;
                int centerY = padding + i * cellSize + cellSize / 2;
                file << "    <circle cx=\"" << centerX << "\" cy=\"" << centerY << "\" r=\"6\"/>\n";
            }
        }
    }
    file << "  </g>\n";
    
    file << "</svg>\n";
    file.close();
    
    std::cout << "SVG exported to: " << filename << std::endl;
}

bool TurnPuzzle::solvePuzzle() {
    std::cout << "Solving puzzle..." << std::endl;
    
    // Clear all edge states to start fresh
    for (Edge* edge : edges) {
        edge->setState(UNDECIDED);
    }
    
    std::vector<EdgeState> differentSolution;
    return FindDifferentSolution(differentSolution);
}


void TurnPuzzle::SaveEdgeStates(std::vector<EdgeState>& edgeStates) {
    edgeStates.clear();
    for (Edge* edge : edges) {
        edgeStates.push_back(edge->state);
    }
}

void TurnPuzzle::RestoreEdgeStates(const std::vector<EdgeState>& edgeStates) {
    for (size_t i = 0; i < edges.size() && i < edgeStates.size(); ++i) {
        edges[i]->setState(edgeStates[i]);
    }
}

bool TurnPuzzle::FindDifferentSolution(std::vector<EdgeState>& edgeStates) {
    TurnPuzzleTypes::SolveOutput result;
    
    while (true) {
        result = SolveCells();
        
        if (result == TurnPuzzleTypes::SolveOutput::SOLVE_FAILED) {
            break;
        }
        
        std::vector<Path> paths;
        if (!findPaths(paths)) {
            result = TurnPuzzleTypes::SolveOutput::SOLVE_FAILED;
            break;
        }
        
        if (result == TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE) {
            break;
        }
        
        // If SOLVE_UPDATED, continue the loop
    }
    
    if (result == TurnPuzzleTypes::SolveOutput::SOLVE_FAILED)
    {
        return false;
    }

    // Check if puzzle is solved
    if (isSolved()) {        
        // Check if it's different from the original solution
        std::vector<EdgeState> currentStates;
        SaveEdgeStates(currentStates);
        
        if (currentStates != originalSolution) {
            std::cout << "Found different solution!" << std::endl;
            SaveEdgeStates(edgeStates);
            exportToSVG("Solution2.svg");
            return true;
        }
        
        std::cout << "Found solution but it matches the original" << std::endl;
    }
    
    // Find an undecided edge
    Edge* undecidedEdge = nullptr;
    for (Edge* edge : edges) {
        if (edge->isUndecided()) {
            undecidedEdge = edge;
            break;
        }
    }
    
    // If no undecided edge found, we're done (no different solution)
    if (undecidedEdge == nullptr) {
        return false;
    }
    
    // Save current edge states
    std::vector<EdgeState> savedStates;
    SaveEdgeStates(savedStates);
    
    // Try marking the edge as INCLUDED
    undecidedEdge->setState(INCLUDED);
    if (FindDifferentSolution(edgeStates)) {
        return true;
    }
    
    // Restore edge states and try EXCLUDED
    RestoreEdgeStates(savedStates);
    undecidedEdge->setState(EXCLUDED);
    if (FindDifferentSolution(edgeStates)) {
        return true;
    }
    
    // Restore edge states before returning
    RestoreEdgeStates(savedStates);
    
    return false;
}

bool TurnPuzzle::isSolved() {
    for (Cell* cell : cells) {
        int degree = cell->getDegree();
        
        // HEAD and TAIL cells must have degree 1
        if (cell->cellType == HEAD || cell->cellType == TAIL) {
            if (degree != 1) {
                return false;
            }
        }
        // All other cells (UNMARKED) must have degree 2
        else {
            if (degree != 2) {
                return false;
            }
        }
    }
    
    // Verify paths are valid and cover the whole grid
    std::vector<Path> paths;
    if (!findPaths(paths)) {
        return false;
    }
    
    // Count total cells in all paths
    int totalPathCells = 0;
    for (const Path& path : paths) {
        totalPathCells += path.getLength();
    }
    
    // Check if paths cover the whole grid
    if (totalPathCells != static_cast<int>(cells.size())) {
        return false;
    }
    
    return true;
}

bool TurnPuzzle::tryConnectHeadToTail(Cell* head, Cell* tail, std::vector<Cell*>& unpairedHeads, std::vector<Cell*>& unpairedTails) {
    // Base case: all heads are paired
    if (unpairedHeads.empty()) {
        std::cout << "Successfully paired all HEADs and TAILs!" << std::endl;
        return true;
    }
    
    // Pick the first unpaired head
    Cell* currentHead = unpairedHeads[0];
    
    // Try to connect it to each unpaired tail
    for (size_t i = 0; i < unpairedTails.size(); i++) {
        Cell* currentTail = unpairedTails[i];
        
        std::cout << "Trying to connect HEAD at (" << currentHead->row << "," << currentHead->col 
                  << ") to TAIL at (" << currentTail->row << "," << currentTail->col << ")" << std::endl;
        
        // Try to find a valid path between them
        Path testPath;
        if (findPathBetween(currentHead, currentTail, testPath)) {
            testPath.calculateTurnType();
            
            std::cout << "Found path with " << testPath.getLength() << " cells, turn type: " << testPath.turnType << std::endl;
            
            // Check if the path has valid turn type (not mixed)
            if (testPath.turnType != RIGHT_LEFT_MIXED) {
                // Mark the edges in this path as INCLUDED
                std::vector<Edge*> pathEdges;
                for (int j = 0; j < testPath.getLength() - 1; j++) {
                    Cell* from = testPath.cells[j];
                    Cell* to = testPath.cells[j + 1];
                    
                    // Find the edge between these cells
                    for (Edge* edge : edges) {
                        if ((edge->cell1 == from && edge->cell2 == to) ||
                            (edge->cell1 == to && edge->cell2 == from)) {
                            pathEdges.push_back(edge);
                            edge->setState(INCLUDED);
                            break;
                        }
                    }
                }
                
                // Remove this head and tail from unpaired lists
                std::vector<Cell*> newUnpairedHeads = unpairedHeads;
                std::vector<Cell*> newUnpairedTails = unpairedTails;
                newUnpairedHeads.erase(newUnpairedHeads.begin());
                newUnpairedTails.erase(newUnpairedTails.begin() + i);
                
                // Recursively try to pair the rest
                if (tryConnectHeadToTail(nullptr, nullptr, newUnpairedHeads, newUnpairedTails)) {
                    return true; // Success!
                }
                
                // Backtrack: undo the edge inclusions
                for (Edge* edge : pathEdges) {
                    edge->setState(UNDECIDED);
                }
            }
        }
    }
    
    // Failed to find a valid pairing
    std::cout << "Failed to connect HEAD at (" << currentHead->row << "," << currentHead->col << ")" << std::endl;
    return false;
}

bool TurnPuzzle::findPathBetween(Cell* start, Cell* end, Path& path) {
    // Use BFS to find a path between start and end
    resetVisitedFlags();
    
    std::queue<std::vector<Cell*>> queue;
    queue.push({start});
    start->visited = true;
    
    while (!queue.empty()) {
        std::vector<Cell*> currentPath = queue.front();
        queue.pop();
        
        Cell* current = currentPath.back();
        
        // Check if we reached the end
        if (current == end) {
            // Build the path object
            for (Cell* cell : currentPath) {
                path.addCell(cell);
            }
            return true;
        }
        
        // Check if current cell can accept more connections
        if (current->getDegree() >= 2) {
            continue;
        }
        
        // Explore adjacent cells through edges
        for (Edge* edge : current->edges) {
            if (edge->isIncluded()) {
                continue; // Skip already used edges
            }
            
            Cell* neighbor = (edge->cell1 == current) ? edge->cell2 : edge->cell1;
            
            // Skip if already visited or degree would exceed 2
            if (neighbor->visited || neighbor->getDegree() >= 2) {
                continue;
            }
            
            // All UNMARKED cells can be traversed - we only connect HEAD to TAIL
            // No need to skip any cell type
            
            // Create new path with this neighbor
            std::vector<Cell*> newPath = currentPath;
            newPath.push_back(neighbor);
            
            neighbor->visited = true;
            queue.push(newPath);
        }
    }
    
    return false; // No path found
}

TurnPuzzleTypes::SolveOutput TurnPuzzle::SolveCells() {
    bool anyUpdated = false;
    
    // Iterate through all cells in the grid
    for (Cell* cell : cells) {
        TurnPuzzleTypes::SolveOutput result = cell->Solve();
        
        // If any cell fails, return SOLVE_FAILED immediately
        if (result == TurnPuzzleTypes::SolveOutput::SOLVE_FAILED) {
            return TurnPuzzleTypes::SolveOutput::SOLVE_FAILED;
        }
        
        // Track if any cell was updated
        if (result == TurnPuzzleTypes::SolveOutput::SOLVE_UPDATED) {
            anyUpdated = true;
        }
    }
    
    // If any cell was updated, return SOLVE_UPDATED
    // Otherwise return SOLVE_NO_CHANGE
    return anyUpdated ? TurnPuzzleTypes::SolveOutput::SOLVE_UPDATED 
                      : TurnPuzzleTypes::SolveOutput::SOLVE_NO_CHANGE;
}
