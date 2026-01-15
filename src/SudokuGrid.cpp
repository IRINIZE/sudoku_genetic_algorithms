#include "SudokuGrid.hpp"

#include <algorithm>
#include <bitset>
#include <sstream>
#include <stdexcept>

namespace sudoku_ga {

// Default constructor: all cells empty, none fixed
SudokuGrid::SudokuGrid() {
    for (auto& row : grid_) {
        row.fill(0);
    }
    for (auto& row : fixed_) {
        row.fill(false);
    }
}

// Build grid from a string like "003020600900305001..."
SudokuGrid::SudokuGrid(const std::string& puzzle) : SudokuGrid() {
    if (puzzle.length() < SIZE * SIZE) {
        throw std::invalid_argument("Puzzle string must have at least 81 characters");
    }
    
    for (int i = 0; i < SIZE * SIZE; ++i) {
        int row = i / SIZE;
        int col = i % SIZE;
        char c = puzzle[i];
        
        if (c >= '1' && c <= '9') {
            // This is a given number - mark it as fixed
            grid_[row][col] = c - '0';
            fixed_[row][col] = true;
        } else {
            // Anything else (0, ., space, etc.) means empty
            grid_[row][col] = 0;
            fixed_[row][col] = false;
        }
    }
}

// Simple getters and setters
int SudokuGrid::get(int row, int col) const {
    return grid_[row][col];
}

void SudokuGrid::set(int row, int col, int value) {
    grid_[row][col] = value;
}

bool SudokuGrid::is_fixed(int row, int col) const {
    return fixed_[row][col];
}

// Count how many unique digits (1-9) are in an array
// We use a bitset as a fast way to track which digits we've seen
int SudokuGrid::count_unique(const std::array<int, SIZE>& values) {
    std::bitset<10> seen;  // Index 0 unused, 1-9 for digits
    int count = 0;
    
    for (int v : values) {
        if (v >= 1 && v <= 9 && !seen[v]) {
            seen[v] = true;
            ++count;
        }
    }
    return count;
}

// Row score = how many unique digits in that row (max 9)
int SudokuGrid::get_row_score(int row) const {
    return count_unique(grid_[row]);
}

// Column score = how many unique digits in that column (max 9)
int SudokuGrid::get_column_score(int col) const {
    std::array<int, SIZE> column;
    for (int row = 0; row < SIZE; ++row) {
        column[row] = grid_[row][col];
    }
    return count_unique(column);
}

// Total fitness = sum of all row scores + all column scores (max 162)
int SudokuGrid::get_total_score() const {
    int score = 0;
    for (int i = 0; i < SIZE; ++i) {
        score += get_row_score(i);
        score += get_column_score(i);
    }
    return score;
}

// Score for 3 consecutive rows (a "band")
// band_index 0 = rows 0-2, band_index 1 = rows 3-5, band_index 2 = rows 6-8
int SudokuGrid::get_row_band_score(int band_index) const {
    int score = 0;
    int start_row = band_index * SUBBLOCK_SIZE;
    
    for (int r = start_row; r < start_row + SUBBLOCK_SIZE; ++r) {
        score += get_row_score(r);
    }
    return score;
}

// Score for 3 consecutive columns (a "stack")
int SudokuGrid::get_column_stack_score(int stack_index) const {
    int score = 0;
    int start_col = stack_index * SUBBLOCK_SIZE;
    
    for (int c = start_col; c < start_col + SUBBLOCK_SIZE; ++c) {
        score += get_column_score(c);
    }
    return score;
}

// Convert sub-block index (0-8) to the top-left corner coordinates
// Sub-blocks are numbered left-to-right, top-to-bottom:
//   0 1 2
//   3 4 5
//   6 7 8
std::pair<int, int> SudokuGrid::subblock_top_left(int subblock_index) {
    int block_row = subblock_index / SUBBLOCK_SIZE;  // Which row of blocks (0, 1, or 2)
    int block_col = subblock_index % SUBBLOCK_SIZE;  // Which column of blocks (0, 1, or 2)
    return {block_row * SUBBLOCK_SIZE, block_col * SUBBLOCK_SIZE};
}

// Get positions of cells we're allowed to change (not fixed)
// Returns a list of (row, col) pairs
std::vector<std::pair<int, int>> SudokuGrid::get_subblock_non_fixed_positions(int subblock_index) const {
    auto [top, left] = subblock_top_left(subblock_index);
    std::vector<std::pair<int, int>> positions;
    
    for (int r = top; r < top + SUBBLOCK_SIZE; ++r) {
        for (int c = left; c < left + SUBBLOCK_SIZE; ++c) {
            if (!fixed_[r][c]) {
                positions.emplace_back(r, c);
            }
        }
    }
    return positions;
}

// Copy 3 rows from another grid (used in crossover)
void SudokuGrid::copy_row_band_from(const SudokuGrid& other, int band_index) {
    int start_row = band_index * SUBBLOCK_SIZE;
    
    for (int r = start_row; r < start_row + SUBBLOCK_SIZE; ++r) {
        grid_[r] = other.grid_[r];
        fixed_[r] = other.fixed_[r];
    }
}

// Copy 3 columns from another grid (used in crossover)
void SudokuGrid::copy_column_stack_from(const SudokuGrid& other, int stack_index) {
    int start_col = stack_index * SUBBLOCK_SIZE;
    
    for (int row = 0; row < SIZE; ++row) {
        for (int c = start_col; c < start_col + SUBBLOCK_SIZE; ++c) {
            grid_[row][c] = other.grid_[row][c];
            fixed_[row][c] = other.fixed_[row][c];
        }
    }
}

bool SudokuGrid::is_solved() const {
    return get_total_score() == MAX_SCORE;
}

// Pretty-print the grid with separators between sub-blocks
std::ostream& operator<<(std::ostream& os, const SudokuGrid& grid) {
    for (int row = 0; row < SudokuGrid::SIZE; ++row) {
        // Print horizontal separator every 3 rows
        if (row > 0 && row % SudokuGrid::SUBBLOCK_SIZE == 0) {
            os << "------+-------+------\n";
        }
        
        for (int col = 0; col < SudokuGrid::SIZE; ++col) {
            // Print vertical separator every 3 columns
            if (col > 0 && col % SudokuGrid::SUBBLOCK_SIZE == 0) {
                os << " |";
            }
            
            int val = grid.get(row, col);
            if (val == 0) {
                os << " .";
            } else {
                os << ' ' << val;
            }
        }
        os << '\n';
    }
    return os;
}

}  // namespace sudoku_ga
