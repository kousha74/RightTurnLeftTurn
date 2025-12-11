#include <iostream>
#include "TurnPuzzle.h"

int main() {
    std::cout << "Turn Puzzle Generator" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Create a 6x6 puzzle
    TurnPuzzle puzzle(8);
    
    // Generate the solution
    puzzle.generateSolution();
    
    // Print the solution to console
    puzzle.printSolution();
    
    // Export to SVG
    puzzle.exportToSVG("solution.svg");
    
    std::cout << "\nDone!" << std::endl;
    return 0;
}
