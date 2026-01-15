#include "Solver.hpp"
#include "SudokuGrid.hpp"

#include <iostream>

/*
 * Example: Solving a Sudoku puzzle with a Genetic Algorithm
 * 
 * The puzzle below looks like this:
 * 
 *     . . . | 2 6 . | 7 . 1
 *     6 8 . | . 7 . | . 9 .
 *     1 9 . | . . 4 | 5 . .
 *     ------+-------+------
 *     8 2 . | 1 . . | . 4 .
 *     . . 4 | 6 . 2 | 9 . .
 *     . 5 . | . . 3 | . 2 8
 *     ------+-------+------
 *     . . 9 | 3 . . | . 7 4
 *     . 4 . | . 5 . | . 3 6
 *     7 . 3 | . 1 8 | . . .
 */

int main() {
    // Create the puzzle from a string (81 characters, 0 = empty)
    auto puzzle = sudoku_ga::SudokuGrid(
        "000260701"
        "680070090"
        "190004500"
        "820100040"
        "004602900"
        "050003028"
        "009300074"
        "040050036"
        "703018000"
    );
    
    // Create a solver with default parameters
    // You can customize: population_size, mutation_rate, etc.
    auto solver = sudoku_ga::Solver();
    
    // Run the genetic algorithm
    auto result = solver.solve(puzzle);

    // Show results
    if (result.solved) {
        std::cout << "Solved in " << result.generations << " generations!\n";
        std::cout << "Time: " << result.elapsed_seconds << " seconds\n\n";
        std::cout << "Solution:\n" << result.best_individual.grid();
    } else {
        std::cout << "No solution found after " << result.generations << " generations.\n";
        std::cout << "Best fitness achieved: " << result.best_fitness << " / 162\n\n";
        std::cout << "Best attempt:\n" << result.best_individual.grid();
    }

    return 0;
}
