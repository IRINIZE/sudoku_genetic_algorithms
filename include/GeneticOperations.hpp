#pragma once

#include "Chromosome.hpp"
#include <utility>

namespace sudoku_ga {

/*
 * CROSSOVER
 * 
 * Combines two parent chromosomes to create two children.
 * The idea: take the best parts from each parent.
 * 
 * Child 1 gets the best "row bands" (groups of 3 rows) from either parent.
 * Child 2 gets the best "column stacks" (groups of 3 columns) from either parent.
 * 
 * "Best" means whichever parent has more unique digits in that region.
 */
std::pair<Chromosome, Chromosome> crossover(const Chromosome& parent1, 
                                             const Chromosome& parent2);

/*
 * MUTATION
 * 
 * Randomly swaps two non-fixed cells within a sub-block.
 * This preserves the sub-block constraint (still has digits 1-9),
 * but changes the row/column configuration.
 * 
 * The mutation_rate controls how likely each sub-block is to be mutated.
 * With rate=0.3, each of the 9 sub-blocks has a 30% chance.
 */
void mutate(Chromosome& chrom, double mutation_rate);

// Mutate just one specific sub-block (used by mutate() and local_search())
void mutate_subblock(Chromosome& chrom, int subblock_index);

/*
 * LOCAL SEARCH (hill climbing)
 * 
 * A simple optimization: try a few random mutations and keep the best one.
 * This helps the GA converge faster by exploiting good solutions.
 * 
 * num_candidates = how many variations to try (usually 2-3)
 */
Chromosome local_search(const Chromosome& parent, int num_candidates);

} // namespace sudoku_ga
