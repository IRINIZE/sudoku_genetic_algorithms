#pragma once

#include "Population.hpp"
#include "SudokuGrid.hpp"

namespace sudoku_ga {

/*
 * SolverParams - Configuration for the genetic algorithm
 * 
 * These defaults work reasonably well, but you can tweak them.
 * Higher values generally mean slower but more thorough search.
 */
struct SolverParams {
    int population_size = 150;        // How many candidates per generation
    int max_generations = 100000;     // Give up after this many generations
    double crossover_rate = 0.3;      // Probability of combining two parents (30%)
    double mutation_rate = 0.3;       // Probability of mutating each sub-block (30%)
    int tournament_size = 3;          // How many candidates compete in selection
    int local_search_candidates = 2;  // How many mutations to try in local search
    bool use_local_search = true;     // Enable the hill-climbing optimization
    bool elitism = true;              // Always keep the best solution
    int report_interval = 1000;       // Print progress every N generations (0 = quiet)
};

/*
 * SolverResult - What you get back after solving (or trying to solve)
 */
struct SolverResult {
    bool solved = false;              // Did we find a perfect solution?
    int generations = 0;              // How many generations did it take?
    int best_fitness = 0;             // Best fitness we achieved
    Chromosome best_individual;       // The best solution (or attempt)
    double elapsed_seconds = 0.0;     // How long did it take?
};

/*
 * Solver - The main genetic algorithm driver
 * 
 * Usage:
 *   SudokuGrid puzzle("003020600...");
 *   Solver solver;
 *   SolverResult result = solver.solve(puzzle);
 *   if (result.solved) { ... }
 */
class Solver {
public:
    // You can pass custom params, or use the defaults
    explicit Solver(const SolverParams& params = SolverParams{});

    // Run the genetic algorithm on a puzzle
    SolverResult solve(const SudokuGrid& puzzle);

    // Access the parameters (read or modify)
    const SolverParams& params() const { return params_; }
    SolverParams& params() { return params_; }

private:
    SolverParams params_;

    // Runs one generation: selection -> crossover -> mutation -> replacement
    void run_generation(Population& population);

    // Prints progress to console
    void print_progress(int generation, const Population& population);
};

} // namespace sudoku_ga

