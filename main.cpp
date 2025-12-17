#include <iostream>
#include "TurnPuzzle.h"

int main() {
    std::cout << "Turn Puzzle Generator" << std::endl;
    std::cout << "=====================" << std::endl;
    
    const int maxAttempts = 100;
    bool foundDifferentSolution = false;
    
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        std::cout << "\n--- Attempt " << attempt << " ---" << std::endl;
        
        // Create a 8x8 puzzle
        TurnPuzzle puzzle(10);
        
        // Generate the solution
        puzzle.generateSolution();
        
        // Mark cells as HEAD and TAIL
        puzzle.markCells();
        
        // Export to SVG
        if (attempt == 1) {
            puzzle.exportToSVG("solution.svg");
        }
        
        std::cout << "Testing for different solution..." << std::endl;
        
        // Try to find a different solution
        bool hasDifferentSolution = puzzle.solvePuzzle();
        
        if (hasDifferentSolution) {
            std::cout << "\n✓ Found puzzle with different solution on attempt " << attempt << "!" << std::endl;
            puzzle.exportToSVG("solution.svg");
            foundDifferentSolution = true;
            break;
        } else {
            std::cout << "✗ No different solution found for this puzzle" << std::endl;
        }
    }
    
    if (!foundDifferentSolution) {
        std::cout << "\n✗ No puzzle with different solution found after " << maxAttempts << " attempts" << std::endl;
    }
    
    std::cout << "\nDone!" << std::endl;
    return 0;
}
