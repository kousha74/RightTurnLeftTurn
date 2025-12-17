#include <iostream>
#include "TurnPuzzle.h"

int main() {
    std::cout << "Turn Puzzle Generator" << std::endl;
    std::cout << "=====================" << std::endl;
    
    const int maxAttempts = 1;
    bool foundDifferentSolution = false;
    
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        std::cout << "\n--- Attempt " << attempt << " ---" << std::endl;
        
        // Create a 8x8 puzzle
        TurnPuzzle puzzle(6);
        
        // Generate puzzle and test for different solution
        bool hasDifferentSolution = puzzle.GeneratePuzzle();
        
        if (hasDifferentSolution) {
            std::cout << "\n✓ Found puzzle with different solution on attempt " << attempt << "!" << std::endl;
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
