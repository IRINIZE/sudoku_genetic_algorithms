#pragma once

#include "SudokuGrid.hpp"

namespace sudoku_ga {

/*
 * Chromosome - A candidate solution in our genetic algorithm
 * 
 * Each chromosome wraps a SudokuGrid and adds:
 * - Fitness calculation (how good is this solution?)
 * - Random initialization (fill empty cells while keeping sub-blocks valid)
 * - Comparison operators (so we can sort by fitness)
 * 
 * The key insight: we always keep sub-blocks valid (containing 1-9 exactly once),
 * so the fitness only needs to measure rows and columns.
 */
class Chromosome {
public:
    // Empty chromosome - you'll need to set up the grid yourself
    Chromosome();

    // Create a chromosome from a puzzle. The fixed cells are preserved,
    // but empty cells aren't filled yet - call initialize_random() for that.
    explicit Chromosome(const SudokuGrid& initial_puzzle);

    // Copy constructor and assignment - makes a full copy
    Chromosome(const Chromosome& other);
    Chromosome& operator=(const Chromosome& other);

    ~Chromosome() = default;

    // Get the underlying grid (const or non-const)
    SudokuGrid& grid() { return grid_; }
    const SudokuGrid& grid() const { return grid_; }

    // --- Fitness ---
    // Returns the fitness score. The result is cached so calling it
    // multiple times is cheap. Call recalculate_fitness() after modifying the grid.
    int fitness() const { return cached_fitness_; }
    
    // Call this after you modify the grid to update the cached fitness
    void recalculate_fitness();
    
    // Quick check: is this a perfect solution?
    bool is_solution() const { return fitness() == SudokuGrid::MAX_SCORE; }

    // Fill all empty cells randomly, but keep each 3x3 sub-block valid.
    // This is how we create the initial population.
    void initialize_random();

    // Comparison by fitness (higher = better)
    // These let us use std::sort and similar algorithms.
    bool operator<(const Chromosome& other) const { return fitness() < other.fitness(); }
    bool operator>(const Chromosome& other) const { return fitness() > other.fitness(); }
    bool operator==(const Chromosome& other) const { return fitness() == other.fitness(); }

    // Print the grid and fitness score
    friend std::ostream& operator<<(std::ostream& os, const Chromosome& chrom);

private:
    SudokuGrid grid_;
    int cached_fitness_;  // Stored fitness value (update with recalculate_fitness())

    // Fills one sub-block with the digits it's missing, in random order
    void fill_subblock_random(int subblock_index);
};

} // namespace sudoku_ga


