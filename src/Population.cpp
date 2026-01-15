#include "Population.hpp"
#include "RandomUtils.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace sudoku_ga {

Population::Population() = default;

// Create N chromosomes from the same puzzle, each with different random fills
Population::Population(const SudokuGrid& puzzle, int size) {
    individuals_.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        Chromosome chrom(puzzle);
        chrom.initialize_random();  // Fill empty cells randomly
        individuals_.push_back(std::move(chrom));
    }
}

// Find the chromosome with the highest fitness
const Chromosome& Population::get_best() const {
    if (individuals_.empty()) {
        throw std::runtime_error("Population is empty");
    }
    
    // std::max_element returns an iterator to the largest element
    // We provide a custom comparator that compares by fitness
    return *std::max_element(individuals_.begin(), individuals_.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness() < b.fitness();
        });
}

Chromosome& Population::get_best() {
    if (individuals_.empty()) {
        throw std::runtime_error("Population is empty");
    }
    
    return *std::max_element(individuals_.begin(), individuals_.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness() < b.fitness();
        });
}

// Find the chromosome with the lowest fitness
const Chromosome& Population::get_worst() const {
    if (individuals_.empty()) {
        throw std::runtime_error("Population is empty");
    }
    
    return *std::min_element(individuals_.begin(), individuals_.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness() < b.fitness();
        });
}

// Tournament selection: pick a few random individuals, return the fittest
// This gives fitter individuals a better chance of being selected as parents
Chromosome& Population::tournament_select(int tournament_size) {
    if (individuals_.empty()) {
        throw std::runtime_error("Population is empty");
    }
    
    // Make sure tournament size is sensible
    tournament_size = std::min(tournament_size, static_cast<int>(individuals_.size()));
    tournament_size = std::max(tournament_size, 1);
    
    // Pick random individuals for the tournament
    auto indices = rng().sample_indices(static_cast<int>(individuals_.size()), tournament_size);
    
    // Find the fittest among them
    int best_idx = indices[0];
    for (int idx : indices) {
        if (individuals_[idx].fitness() > individuals_[best_idx].fitness()) {
            best_idx = idx;
        }
    }
    
    return individuals_[best_idx];
}

// Select two different parents for crossover
std::pair<Chromosome*, Chromosome*> Population::select_parents(int tournament_size) {
    if (individuals_.size() < 2) {
        throw std::runtime_error("Population must have at least 2 individuals");
    }
    
    // Pick first parent
    Chromosome* parent1 = &tournament_select(tournament_size);
    
    // Pick second parent, making sure it's different
    Chromosome* parent2 = nullptr;
    int max_attempts = 10;
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        parent2 = &tournament_select(tournament_size);
        if (parent2 != parent1) {
            break;
        }
    }
    
    // Fallback: if we keep getting the same one, just find any different one
    if (parent2 == parent1) {
        for (auto& ind : individuals_) {
            if (&ind != parent1) {
                parent2 = &ind;
                break;
            }
        }
    }
    
    return {parent1, parent2};
}

// Replace the entire population with a new generation
void Population::replace_generation(std::vector<Chromosome> new_generation) {
    individuals_ = std::move(new_generation);
}

int Population::best_fitness() const {
    if (individuals_.empty()) return 0;
    return get_best().fitness();
}

int Population::worst_fitness() const {
    if (individuals_.empty()) return 0;
    return get_worst().fitness();
}

// Calculate the mean fitness across all individuals
double Population::average_fitness() const {
    if (individuals_.empty()) return 0.0;
    
    // std::accumulate adds up all the fitness values
    double sum = std::accumulate(individuals_.begin(), individuals_.end(), 0.0,
        [](double acc, const Chromosome& c) {
            return acc + c.fitness();
        });
    
    return sum / individuals_.size();
}

// Check if any chromosome has a perfect score
bool Population::has_solution() const {
    return std::any_of(individuals_.begin(), individuals_.end(),
        [](const Chromosome& c) { return c.is_solution(); });
}

// Return a pointer to a solved chromosome, or nullptr if none exists
const Chromosome* Population::get_solution() const {
    for (const auto& ind : individuals_) {
        if (ind.is_solution()) {
            return &ind;
        }
    }
    return nullptr;
}

} // namespace sudoku_ga
