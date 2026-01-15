#include "Solver.hpp"
#include "GeneticOperations.hpp"
#include "RandomUtils.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

namespace sudoku_ga {

Solver::Solver(const SolverParams& params)
    : params_(params)
{}

// Print current progress to the console
void Solver::print_progress(int generation, const Population& population) {
    if (params_.report_interval > 0 && generation % params_.report_interval == 0) {
        std::cout << "Generation " << generation 
                  << " | Best: " << population.best_fitness()
                  << " | Avg: " << population.average_fitness()
                  << " | Worst: " << population.worst_fitness()
                  << std::endl;
    }
}

// This is the heart of the GA - one generation of evolution
void Solver::run_generation(Population& population) {
    std::vector<Chromosome> new_generation;
    new_generation.reserve(population.size());
    
    // Elitism: copy the best individual directly to the next generation
    // This ensures we never lose our best solution
    if (params_.elitism) {
        new_generation.push_back(population.get_best());
    }
    
    // Fill the rest of the new generation with offspring
    while (new_generation.size() < population.size()) {
        // Step 1: Select two parents using tournament selection
        auto [parent1, parent2] = population.select_parents(params_.tournament_size);
        
        // Step 2: Maybe do crossover (combine the parents)
        if (rng().rand_double() < params_.crossover_rate) {
            // Do crossover - create two children from the parents
            auto [child1, child2] = crossover(*parent1, *parent2);
            
            // Step 3: Apply mutation to the children
            mutate(child1, params_.mutation_rate);
            mutate(child2, params_.mutation_rate);
            
            // Step 4: Optional local search (try to improve the children)
            if (params_.use_local_search && params_.local_search_candidates > 1) {
                child1 = local_search(child1, params_.local_search_candidates);
                child2 = local_search(child2, params_.local_search_candidates);
            }
            
            // Add children to new generation
            new_generation.push_back(std::move(child1));
            if (new_generation.size() < population.size()) {
                new_generation.push_back(std::move(child2));
            }
        } else {
            // No crossover - just copy parents and mutate them
            Chromosome child1 = *parent1;
            Chromosome child2 = *parent2;
            
            mutate(child1, params_.mutation_rate);
            mutate(child2, params_.mutation_rate);
            
            if (params_.use_local_search && params_.local_search_candidates > 1) {
                child1 = local_search(child1, params_.local_search_candidates);
                child2 = local_search(child2, params_.local_search_candidates);
            }
            
            new_generation.push_back(std::move(child1));
            if (new_generation.size() < population.size()) {
                new_generation.push_back(std::move(child2));
            }
        }
    }
    
    // Out with the old, in with the new
    population.replace_generation(std::move(new_generation));
}

// Main solving loop
SolverResult Solver::solve(const SudokuGrid& puzzle) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    SolverResult result;
    
    // Create the initial population
    Population population(puzzle, params_.population_size);
    
    // Maybe we got lucky and one of the random initializations is already a solution?
    if (population.has_solution()) {
        result.solved = true;
        result.generations = 0;
        result.best_fitness = SudokuGrid::MAX_SCORE;
        result.best_individual = *population.get_solution();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.elapsed_seconds = std::chrono::duration<double>(end_time - start_time).count();
        return result;
    }
    
    // Show starting point
    print_progress(0, population);
    
    // Main evolution loop
    for (int gen = 1; gen <= params_.max_generations; ++gen) {
        // Evolve one generation
        run_generation(population);
        
        // Did we find a solution?
        if (population.has_solution()) {
            result.solved = true;
            result.generations = gen;
            result.best_fitness = SudokuGrid::MAX_SCORE;
            result.best_individual = *population.get_solution();
            
            if (params_.report_interval > 0) {
                std::cout << "Solution found at generation " << gen << "!" << std::endl;
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            result.elapsed_seconds = std::chrono::duration<double>(end_time - start_time).count();
            return result;
        }
        
        // Show progress
        print_progress(gen, population);
    }
    
    // We ran out of generations without finding a perfect solution
    result.solved = false;
    result.generations = params_.max_generations;
    result.best_fitness = population.best_fitness();
    result.best_individual = population.get_best();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

} // namespace sudoku_ga

