#pragma once

#include <algorithm>
#include <random>
#include <vector>

namespace sudoku_ga {

/*
 * RandomGenerator - A simple wrapper around C++'s random number facilities
 * 
 * This is a "singleton" - there's only one instance shared across the program.
 * Access it with RandomGenerator::instance() or the shortcut rng().
 * 
 * Why a singleton? We want all random operations to use the same generator
 * so results are reproducible if we set a seed.
 */
class RandomGenerator {
public:
    // Get the single shared instance
    static RandomGenerator& instance() {
        static RandomGenerator rng;
        return rng;
    }

    // Set the seed for reproducible results (useful for debugging)
    void seed(unsigned int s) {
        engine_.seed(s);
    }

    // Random integer between min and max (inclusive on both ends)
    int rand_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine_);
    }

    // Random decimal between 0.0 and 1.0 (not including 1.0)
    double rand_double() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(engine_);
    }

    // Randomly reorder a vector
    template<typename T>
    void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), engine_);
    }

    // Get two different random indices from 0 to max_index
    // Useful for picking two cells to swap
    std::pair<int, int> two_distinct_indices(int max_index) {
        int i = rand_int(0, max_index);
        int j = rand_int(0, max_index - 1);
        if (j >= i) ++j;  // Make sure j != i
        return {i, j};
    }

    // Pick k different random indices from 0 to n-1
    // Used for tournament selection
    std::vector<int> sample_indices(int n, int k) {
        std::vector<int> indices(n);
        for (int i = 0; i < n; ++i) indices[i] = i;
        shuffle(indices);
        indices.resize(k);
        return indices;
    }

private:
    // Private constructor - use instance() instead
    RandomGenerator() : engine_(std::random_device{}()) {}
    
    // Prevent copying (there should only be one instance)
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;

    // Mersenne Twister - a high-quality random number generator
    std::mt19937 engine_;
};

// Shortcut to get the global random generator
inline RandomGenerator& rng() {
    return RandomGenerator::instance();
}

} // namespace sudoku_ga
