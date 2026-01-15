#include "GeneticOperations.hpp"
#include "RandomUtils.hpp"

#include <algorithm>

namespace sudoku_ga {

// ============================================================================
// CROSSOVER
// ============================================================================

std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1,
                                             const Chromosome& parent2) {
    // Start both children as copies of parent1
    Chromosome child1(parent1);
    Chromosome child2(parent1);
    
    // Child 1: for each band of 3 rows, pick whichever parent has better scores
    for (int band = 0; band < 3; ++band) {
        int score1 = parent1.grid().get_row_band_score(band);
        int score2 = parent2.grid().get_row_band_score(band);
        
        if (score2 > score1) {
            // Parent 2 is better for this band - use their rows
            child1.grid().copy_row_band_from(parent2.grid(), band);
        }
        // Otherwise we keep parent1's band (already there from the copy)
    }
    
    // Child 2: same idea, but with column stacks instead of row bands
    for (int stack = 0; stack < 3; ++stack) {
        int score1 = parent1.grid().get_column_stack_score(stack);
        int score2 = parent2.grid().get_column_stack_score(stack);
        
        if (score2 > score1) {
            child2.grid().copy_column_stack_from(parent2.grid(), stack);
        } else {
            child2.grid().copy_column_stack_from(parent1.grid(), stack);
        }
    }
    
    // Update fitness for the new children
    child1.recalculate_fitness();
    child2.recalculate_fitness();
    
    return {std::move(child1), std::move(child2)};
}

// ============================================================================
// MUTATION
// ============================================================================

// Swap two random non-fixed cells within one sub-block
void mutate_subblock(Chromosome& chrom, int subblock_index) {
    // Get list of cells we're allowed to change
    auto positions = chrom.grid().get_subblock_non_fixed_positions(subblock_index);
    
    // Need at least 2 cells to do a swap
    if (positions.size() < 2) {
        return;
    }
    
    // Pick two different random positions
    auto [idx1, idx2] = rng().two_distinct_indices(static_cast<int>(positions.size()) - 1);
    auto [r1, c1] = positions[idx1];
    auto [r2, c2] = positions[idx2];
    
    // Do the swap
    int temp = chrom.grid().get(r1, c1);
    chrom.grid().set(r1, c1, chrom.grid().get(r2, c2));
    chrom.grid().set(r2, c2, temp);
}

// Apply mutation to each sub-block with the given probability
void mutate(Chromosome& chrom, double mutation_rate) {
    bool mutated = false;
    
    // Go through all 9 sub-blocks
    for (int block = 0; block < SudokuGrid::NUM_SUBBLOCKS; ++block) {
        // Roll the dice - should we mutate this block?
        if (rng().rand_double() < mutation_rate) {
            mutate_subblock(chrom, block);
            mutated = true;
        }
    }
    
    // Only recalculate fitness if we actually changed something
    if (mutated) {
        chrom.recalculate_fitness();
    }
}


// ============================================================================
// LOCAL SEARCH (hill climbing)
// ============================================================================

// Try a few random mutations and keep the best result
Chromosome local_search(const Chromosome& parent, int num_candidates) {
    Chromosome best = parent;
    int best_fitness = parent.fitness();
    
    for (int i = 0; i < num_candidates; ++i) {
        // Make a copy and mutate a random sub-block
        Chromosome candidate = parent;
        int block = rng().rand_int(0, SudokuGrid::NUM_SUBBLOCKS - 1);
        mutate_subblock(candidate, block);
        candidate.recalculate_fitness();
        
        // Keep it if it's better

        if (candidate.fitness() > best_fitness) {
            best = std::move(candidate);
            best_fitness = best.fitness();
        }
    }
    

    return best;
}

} // namespace sudoku_ga
