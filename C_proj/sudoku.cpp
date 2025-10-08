#include "decl.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>

// Use add_clause from cnfparser.cpp
extern void add_clause(Formula &formula, Clause *clause);

// Helper: Add clause to formula (direct, for CNF encoding)
static void add_clause_lits(Formula &formula, int lits[], int num_lits)
{
    Clause *clause = new Clause();
    clause->num_lits = num_lits;
    std::memcpy(clause->lits, lits, num_lits * sizeof(int));
    clause->next = nullptr;
    if (!formula.clauses)
    {
        formula.clauses = clause;
    }
    else
    {
        Clause *cur = formula.clauses;
        while (cur->next)
            cur = cur->next;
        cur->next = clause;
    }
    formula.num_clauses++;
}

// Fisher-Yates shuffle for randomized puzzle generation
static void shuffle_array(int arr[], int n)
{
    for (int i = n - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

// Check if placing value v at (row, col) is valid for given sudoku type
static bool is_valid_placement(int grid[9][9], int row, int col, int v, bool percent)
{
    // Classic sudoku rules: row, column, and 3x3 block uniqueness
    for (int i = 0; i < 9; ++i)
        if (grid[row][i] == v || grid[i][col] == v)
            return false;

    int br = row / 3 * 3, bc = col / 3 * 3;
    for (int i = br; i < br + 3; ++i)
        for (int j = bc; j < bc + 3; ++j)
            if (grid[i][j] == v)
                return false;

    // Percent sudoku additional rules:
    // diagonal constraints
    if (percent)
    {
        // Main diagonal (top-left to bottom-right)
        if (row == col)
        {
            for (int i = 0; i < 9; ++i)
                if (grid[i][i] == v)
                    return false;
        }

        // Anti-diagonal (top-right to bottom-left)
        if (row + col == 8)
        {
            for (int i = 0; i < 9; ++i)
                if (grid[i][8 - i] == v)
                    return false;
        }

        // specific % window constraints (1,1)->(3,3) and (5,5)->(7,7) squares
        if (row >= 1 && row <= 3 && col >= 1 && col <= 3)
        {
            for (int i = 1; i <= 3; ++i)
                for (int j = 1; j <= 3; ++j)
                    if (grid[i][j] == v)
                        return false;
        }
        if (row >= 5 && row <= 7 && col >= 5 && col <= 7)
        {
            for (int i = 5; i <= 7; ++i)
                for (int j = 5; j <= 7; ++j)
                    if (grid[i][j] == v)
                        return false;
        }
    }

    return true;
}

// Backtracking Sudoku solver (for complete grid generation)
static bool fill_grid(int grid[9][9], bool percent, int row = 0, int col = 0)
{
    if (row == 9)
        return true; // All cells filled successfully
    if (col == 9)
        return fill_grid(grid, percent, row + 1, 0); // Move to next row
    if (grid[row][col] != 0)
        return fill_grid(grid, percent, row, col + 1); // Skip filled cells

    // Try values 1-9 in random order for variety
    int nums[9];
    for (int i = 0; i < 9; ++i)
        nums[i] = i + 1;
    shuffle_array(nums, 9);

    for (int idx = 0; idx < 9; ++idx)
    {
        int v = nums[idx];

        if (is_valid_placement(grid, row, col, v, percent))
        {
            grid[row][col] = v;
            if (fill_grid(grid, percent, row, col + 1))
                return true;
            grid[row][col] = 0; // Backtrack
        }
    }
    return false;
}

// Count solutions for uniqueness verification (stops at limit for efficiency)
static int count_solutions(int grid[9][9], bool percent, int row = 0, int col = 0, int limit = 2)
{
    if (row == 9)
        return 1; // Found one complete solution
    if (col == 9)
        return count_solutions(grid, percent, row + 1, 0, limit);
    if (grid[row][col] != 0)
        return count_solutions(grid, percent, row, col + 1, limit);

    int count = 0;
    for (int v = 1; v <= 9; ++v)
    {
        if (is_valid_placement(grid, row, col, v, percent))
        {
            grid[row][col] = v;
            count += count_solutions(grid, percent, row, col + 1, limit);
            grid[row][col] = 0;
            if (count >= limit)
                return count; // Early termination for efficiency
        }
    }
    return count;
}

// Encapsulated uniqueness checker with optional verification
static bool check_uniqueness(int grid[9][9], bool percent, bool do_check)
{
    if (!do_check)
    {
        // Skip uniqueness check for faster generation
        return true;
    }
    // Perform actual uniqueness check with correct sudoku type
    return count_solutions(grid, percent) == 1;
}

// Remove clues randomly while maintaining uniqueness and minimum count
static void dig_holes(int grid[9][9], bool percent, int min_clues, bool checkuni = false)
{
    // Create array of all cell positions for random removal
    int cells[81][2];
    int idx = 0;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            cells[idx][0] = i;
            cells[idx][1] = j;
            ++idx;
        }

    // Shuffle cells for random hole digging order
    for (int i = 80; i > 0; --i)
    {
        int j = rand() % (i + 1);
        int tmp0 = cells[i][0], tmp1 = cells[i][1];
        cells[i][0] = cells[j][0];
        cells[i][1] = cells[j][1];
        cells[j][0] = tmp0;
        cells[j][1] = tmp1;
    }

    int clues = 81;
    for (int k = 0; k < 81 && clues > min_clues; ++k)
    {
        int r = cells[k][0], c = cells[k][1];
        int backup = grid[r][c];
        grid[r][c] = 0; // Temporarily remove clue

        // Restore if removal breaks uniqueness or violates minimum clue count
        if (!check_uniqueness(grid, percent, checkuni) || clues - 1 < min_clues)
        {
            grid[r][c] = backup; // Restore if not unique or too few clues
        }
        else
        {
            clues--; // Successfully removed clue
        }
    }
}

// Convert Sudoku grid to CNF formula (supports both classic and percent variants)
static void encode_sudoku_cnf(const int grid[9][9], Formula &formula, bool percent)
{
    formula.num_vars = 729; // 9x9x9 variables: x(r,c,v) = cell (r,c) has value v
    formula.num_clauses = 0;
    formula.clauses = nullptr;

    // Cell constraints: each cell must have at least one value (1-9)
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
        {
            int lits[9];
            for (int v = 0; v < 9; ++v)
                lits[v] = r * 81 + c * 9 + v + 1; // Variable encoding: x(r,c,v)
            add_clause_lits(formula, lits, 9);
        }

    // Row constraints: each value appears at most once per row
    for (int r = 0; r < 9; ++r)
        for (int v = 0; v < 9; ++v)
            for (int c1 = 0; c1 < 9; ++c1)
                for (int c2 = c1 + 1; c2 < 9; ++c2)
                {
                    int lits[2] = {-(r * 81 + c1 * 9 + v + 1), -(r * 81 + c2 * 9 + v + 1)};
                    add_clause_lits(formula, lits, 2);
                }

    // Column constraints: each value appears at most once per column
    for (int c = 0; c < 9; ++c)
        for (int v = 0; v < 9; ++v)
            for (int r1 = 0; r1 < 9; ++r1)
                for (int r2 = r1 + 1; r2 < 9; ++r2)
                {
                    int lits[2] = {-(r1 * 81 + c * 9 + v + 1), -(r2 * 81 + c * 9 + v + 1)};
                    add_clause_lits(formula, lits, 2);
                }

    // Block constraints: each value appears at most once per 3x3 block
    for (int br = 0; br < 3; ++br)
        for (int bc = 0; bc < 3; ++bc)
            for (int v = 0; v < 9; ++v)
                for (int i1 = 0; i1 < 9; ++i1)
                    for (int i2 = i1 + 1; i2 < 9; ++i2)
                    {
                        int r1 = br * 3 + i1 / 3, c1 = bc * 3 + i1 % 3;
                        int r2 = br * 3 + i2 / 3, c2 = bc * 3 + i2 % 3;
                        int lits[2] = {-(r1 * 81 + c1 * 9 + v + 1), -(r2 * 81 + c2 * 9 + v + 1)};
                        add_clause_lits(formula, lits, 2);
                    }

    // Cell uniqueness: each cell has at most one value
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            for (int v1 = 0; v1 < 9; ++v1)
                for (int v2 = v1 + 1; v2 < 9; ++v2)
                {
                    int lits[2] = {-(r * 81 + c * 9 + v1 + 1), -(r * 81 + c * 9 + v2 + 1)};
                    add_clause_lits(formula, lits, 2);
                }

    // Given clues: fix known values from the puzzle
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c)
            if (grid[r][c] != 0)
            {
                int lit = r * 81 + c * 9 + (grid[r][c] - 1) + 1;
                add_clause_lits(formula, &lit, 1);
            }

    // Percent sudoku: add diagonal constraints & window constraints
    if (percent)
    {
        // Main diagonal constraint: each value appears at most once
        for (int v = 0; v < 9; ++v)
            for (int i1 = 0; i1 < 9; ++i1)
                for (int i2 = i1 + 1; i2 < 9; ++i2)
                {
                    int lits[2] = {-(i1 * 81 + i1 * 9 + v + 1), -(i2 * 81 + i2 * 9 + v + 1)};
                    add_clause_lits(formula, lits, 2);
                }

        // Anti-diagonal constraint: each value appears at most once
        for (int v = 0; v < 9; ++v)
            for (int i1 = 0; i1 < 9; ++i1)
                for (int i2 = i1 + 1; i2 < 9; ++i2)
                {
                    int lits[2] = {-(i1 * 81 + (8 - i1) * 9 + v + 1), -(i2 * 81 + (8 - i2) * 9 + v + 1)};
                    add_clause_lits(formula, lits, 2);
                }

        // Window constraints: each value appears at most once in (1,1)->(3,3) and (5,5)->(7,7)
        for (int v = 0; v < 9; ++v)
        {
            // First window (1,1) to (3,3)
            for (int i1 = 1; i1 <= 3; ++i1)
                for (int j1 = 1; j1 <= 3; ++j1)
                    for (int i2 = i1; i2 <= 3; ++i2)
                        for (int j2 = (i2 == i1 ? j1 + 1 : 1); j2 <= 3; ++j2)
                        {
                            int lits[2] = {-(i1 * 81 + j1 * 9 + v + 1), -(i2 * 81 + j2 * 9 + v + 1)};
                            add_clause_lits(formula, lits, 2);
                        }

            // Second window (5,5) to (7,7)
            for (int i1 = 5; i1 <= 7; ++i1)
                for (int j1 = 5; j1 <= 7; ++j1)
                    for (int i2 = i1; i2 <= 7; ++i2)
                        for (int j2 = (i2 == i1 ? j1 + 1 : 5); j2 <= 7; ++j2)
                        {
                            int lits[2] = {-(i1 * 81 + j1 * 9 + v + 1), -(i2 * 81 + j2 * 9 + v + 1)};
                            add_clause_lits(formula, lits, 2);
                        }
        }
    }
}

// Generate classic Sudoku puzzle and encode as CNF
int generate_sudoku_cnf(int (&sudoku)[9][9], Formula &formula)
{
    std::memset(sudoku, 0, sizeof(sudoku));
    fill_grid(sudoku, false);                         // false = classic sudoku rules
    dig_holes(sudoku, false, 17 + rand() % 25, true); // Remove clues while ensuring uniqueness
    encode_sudoku_cnf(sudoku, formula, false);        // Encode without diagonal constraints
    return 1;
}

// Generate percent Sudoku puzzle (with diagonal constraints) and encode as CNF
int generate_percent_sudoku_cnf(int (&sudoku)[9][9], Formula &formula)
{
    std::memset(sudoku, 0, sizeof(sudoku));
    fill_grid(sudoku, true);                         // true = percent sudoku rules (with diagonals)
    dig_holes(sudoku, true, 17 + rand() % 45, true); // More aggressive hole digging for harder puzzles
    encode_sudoku_cnf(sudoku, formula, true);        // Encode with diagonal constraints
    return 1;
}

// Convert SAT solution back to solved Sudoku grid
void assignment_to_grid(const Assignment &assignment, int grid[9][9])
{
    std::memset(grid, 0, sizeof(int) * 81);
    for (int var = 1; var <= 729; ++var)
    {
        if (assignment.values[var] == 1) // If variable is assigned true
        {
            // Decode variable: x(r,c,v) -> grid[r][c] = v
            int idx = var - 1;
            int row = idx / 81;
            int col = (idx % 81) / 9;
            int val = (idx % 9) + 1;
            grid[row][col] = val;
        }
    }
}