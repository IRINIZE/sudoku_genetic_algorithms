#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace sudoku_ga {

/*
 * SudokuGrid - Represents a 9x9 Sudoku board
 * 
 * This class stores the puzzle state and provides methods to:
 * - Read and write cell values
 * - Track which cells are "fixed" (given in the original puzzle)
 * - Calculate fitness scores for the genetic algorithm
 * - Copy regions between grids (used during crossover)
 * 
 * The grid is laid out like this, with 9 sub-blocks numbered 0-8:
 * 
 *     0 | 1 | 2
 *    ---+---+---
 *     3 | 4 | 5
 *    ---+---+---
 *     6 | 7 | 8
 */
class SudokuGrid {
public:
    // Basic Sudoku constants
    static constexpr int SIZE = 9;
    static constexpr int SUBBLOCK_SIZE = 3;
    static constexpr int NUM_SUBBLOCKS = 9;
    
    // A perfect solution scores 162 (9 unique digits in each of 9 rows + 9 columns)
    static constexpr int MAX_SCORE = 162;

    // Creates an empty grid (all zeros)
    SudokuGrid();
    
    // Creates a grid from a string like "003020600900305..."
    // Use '0' or '.' for empty cells, '1'-'9' for filled cells
    explicit SudokuGrid(const std::string& puzzle);

    // --- Basic cell access ---
    int get(int row, int col) const;
    void set(int row, int col, int value);
    
    // Fixed cells are the ones given in the original puzzle.
    // The GA should never modify these.
    bool is_fixed(int row, int col) const;

    // --- Fitness scoring ---
    // These count how many unique digits (1-9) appear in a row/column.
    // A perfect row or column scores 9.
    int get_row_score(int row) const;
    int get_column_score(int col) const;
    int get_total_score() const;
    
    // Scores for "bands" (3 consecutive rows) and "stacks" (3 consecutive columns).
    // Used during crossover to compare which parent has better regions.
    // band_index and stack_index can be 0, 1, or 2.
    int get_row_band_score(int band_index) const;
    int get_column_stack_score(int stack_index) const;

    // --- Sub-block helpers ---
    // Get positions of non-fixed cells in a sub-block (for mutation)
    std::vector<std::pair<int, int>> get_subblock_non_fixed_positions(int subblock_index) const;
    
    // Convert sub-block index (0-8) to grid coordinates of its top-left corner
    static std::pair<int, int> subblock_top_left(int subblock_index);

    // --- Crossover helpers ---
    // Copy 3 rows at a time from another grid
    void copy_row_band_from(const SudokuGrid& other, int band_index);
    
    // Copy 3 columns at a time from another grid
    void copy_column_stack_from(const SudokuGrid& other, int stack_index);

    // Returns true when the puzzle is completely solved
    bool is_solved() const;

    // For printing the grid nicely
    friend std::ostream& operator<<(std::ostream& os, const SudokuGrid& grid);

private:
    // The actual 9x9 grid of values (0 means empty)
    std::array<std::array<int, SIZE>, SIZE> grid_;
    
    // Tracks which cells came from the original puzzle
    std::array<std::array<bool, SIZE>, SIZE> fixed_;
    
    // Counts unique non-zero values in an array (used for scoring)
    static int count_unique(const std::array<int, SIZE>& values);
};

}  // namespace sudoku_ga
