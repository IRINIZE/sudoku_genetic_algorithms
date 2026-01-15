#pragma once

#include "Chromosome.hpp"
#include "SudokuGrid.hpp"

#include <vector>

namespace sudoku_ga {

/*
 * Population - A collection of candidate solutions
 * 
 * In a genetic algorithm, we maintain a "population" of individuals that
 * evolve over time. Each generation, we:
 * 1. Select the fittest individuals as parents
 * 2. Create children through crossover and mutation
 * 3. Replace the old population with the new one
 * 
 * This class handles storage and selection of chromosomes.
 */
class Population {
public:
    // Empty population (you'll add individuals later)
    Population();

    // Create a population of the given size, all starting from the same puzzle
    // but with different random initializations
    Population(const SudokuGrid& puzzle, int size);

    // Access individuals by index (like an array)
    Chromosome& operator[](size_t index) { return individuals_[index]; }
    const Chromosome& operator[](size_t index) const { return individuals_[index]; }

    size_t size() const { return individuals_.size(); }
    bool empty() const { return individuals_.empty(); }

    // These let you use range-based for loops: for (auto& chrom : population)
    auto begin() { return individuals_.begin(); }
    auto end() { return individuals_.end(); }
    auto begin() const { return individuals_.begin(); }
    auto end() const { return individuals_.end(); }

    // Find the chromosome with the highest/lowest fitness
    const Chromosome& get_best() const;
    Chromosome& get_best();
    const Chromosome& get_worst() const;

    // --- Selection ---
    // Tournament selection: pick a few random individuals, return the best one.
    // This is how we choose parents - fitter individuals are more likely to win.
    Chromosome& tournament_select(int tournament_size);

    // Select two different parents for crossover
    std::pair<Chromosome*, Chromosome*> select_parents(int tournament_size);

    // --- Generation management ---
    // Replace all individuals with a new set (used at the end of each generation)
    void replace_generation(std::vector<Chromosome> new_generation);

    // --- Statistics ---
    int best_fitness() const;
    int worst_fitness() const;
    double average_fitness() const;

    // Check if we've found a solution
    bool has_solution() const;
    const Chromosome* get_solution() const;  // Returns nullptr if no solution

private:
    std::vector<Chromosome> individuals_;
};

} // namespace sudoku_ga
