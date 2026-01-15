#include "Chromosome.hpp"
#include "RandomUtils.hpp"

#include <algorithm>
#include <bitset>

namespace sudoku_ga {

// Default constructor - empty grid
Chromosome::Chromosome() 
    : grid_()
    , cached_fitness_(0)
{}

// Create from a puzzle - copies the grid but doesn't fill empty cells yet
Chromosome::Chromosome(const SudokuGrid& initial_puzzle)
    : grid_(initial_puzzle)
    , cached_fitness_(0)
{}

// Copy constructor - duplicate the grid and fitness
Chromosome::Chromosome(const Chromosome& other)
    : grid_(other.grid_)
    , cached_fitness_(other.cached_fitness_)
{}

// Copy assignment
Chromosome& Chromosome::operator=(const Chromosome& other) {
    if (this != &other) {
        grid_ = other.grid_;
        cached_fitness_ = other.cached_fitness_;
    }
    return *this;
}

// Update the cached fitness value by recalculating from the grid
void Chromosome::recalculate_fitness() {
    cached_fitness_ = grid_.get_total_score();
}

// Fill one 3x3 sub-block with the digits it's missing
// The digits are placed in random order to create diversity
void Chromosome::fill_subblock_random(int subblock_index) {
    auto [top, left] = SudokuGrid::subblock_top_left(subblock_index);
    
    // First, figure out which digits are already in the sub-block (the fixed ones)
    std::bitset<10> present;  // present[d] = true if digit d is already there
    for (int r = top; r < top + SudokuGrid::SUBBLOCK_SIZE; ++r) {
        for (int c = left; c < left + SudokuGrid::SUBBLOCK_SIZE; ++c) {
            int val = grid_.get(r, c);
            if (val != 0) {
                present[val] = true;
            }
        }
    }
    
    // Collect the digits that are missing (need to be filled in)
    std::vector<int> missing;
    for (int d = 1; d <= 9; ++d) {
        if (!present[d]) {
            missing.push_back(d);
        }
    }
    
    // Shuffle them so each chromosome gets a different configuration
    rng().shuffle(missing);
    
    // Place the shuffled digits into the empty cells
    size_t idx = 0;
    for (int r = top; r < top + SudokuGrid::SUBBLOCK_SIZE; ++r) {
        for (int c = left; c < left + SudokuGrid::SUBBLOCK_SIZE; ++c) {
            if (grid_.get(r, c) == 0) {
                grid_.set(r, c, missing[idx++]);
            }
        }
    }
}

// Initialize all empty cells in the grid
// After this, every sub-block will contain digits 1-9 exactly once
void Chromosome::initialize_random() {
    for (int block = 0; block < SudokuGrid::NUM_SUBBLOCKS; ++block) {
        fill_subblock_random(block);
    }
    recalculate_fitness();
}

// Print the chromosome (grid + fitness info)
std::ostream& operator<<(std::ostream& os, const Chromosome& chrom) {
    os << chrom.grid_;
    os << "Fitness: " << chrom.fitness() << " / " << SudokuGrid::MAX_SCORE;
    if (chrom.is_solution()) {
        os << " [SOLVED]";
    }
    os << '\n';
    return os;
}

} // namespace sudoku_ga


