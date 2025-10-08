#ifndef DECL_H
#define DECL_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// ============ CONSTANTS ============
constexpr int MAX_VARS = 5000;           // Maximum variables in SAT formula
constexpr int MAX_CLAUSES = 200000;      // Maximum clauses in SAT formula
constexpr int MAX_LITS_PER_CLAUSE = 100; // Maximum literals per clause

// ============ CORE SAT DATA STRUCTURES ============

// Clause: disjunction of literals (OR operation)
struct Clause
{
    int num_lits;                  // Number of literals in this clause
    int lits[MAX_LITS_PER_CLAUSE]; // Literal array: positive = variable, negative = negated variable
    Clause *next;                  // Linked list pointer for formula

    Clause() : num_lits(0), next(nullptr)
    {
        std::memset(lits, 0, sizeof(lits));
    }
};

// Formula: conjunction of clauses (AND operation)
struct Formula
{
    int num_vars;    // Total number of variables (1-indexed)
    int num_clauses; // Total number of clauses
    Clause *clauses; // Linked list of clauses
    Formula() : num_vars(0), num_clauses(0), clauses(nullptr) {}
};

// Assignment: variable truth value assignments
struct Assignment
{
    int values[MAX_VARS + 1]; // 1=true, 0=false, -1=unassigned (1-indexed)

    Assignment()
    {
        std::memset(values, -1, sizeof(values));
    }
};

// ============ CNF PARSER MODULE ============

int parse_cnf(const char *filename, Formula &formula); // Parse DIMACS CNF file
void add_clause(Formula &formula, Clause *clause);     // Add clause to formula (needed by sudoku.cpp)
void print_assignment(const Assignment &assignment);   // Debug: print variable assignments
void free_formula(Formula &formula);                   // Memory cleanup for formula
void print_formula(const Formula &formula);            // Debug: print entire formula

// ============ SAT SOLVER MODULE ============

int dpll(Formula &formula, Assignment &assignment);               // Basic DPLL algorithm
int dpll_optimized(Formula &formula, Assignment &assignment);     // Optimized DPLL with heuristics
int dpll_opt_recursive(Formula &formula, Assignment &assignment); // Internal recursive helper

// ============ SUDOKU MODULE ============

int generate_sudoku_cnf(int (&sudoku)[9][9], Formula &formula);         // Generate classic Sudoku CNF
int generate_percent_sudoku_cnf(int (&sudoku)[9][9], Formula &formula); // Generate % Sudoku CNF (with diagonals)
void assignment_to_grid(const Assignment &assignment, int grid[9][9]);  // Convert SAT solution to Sudoku grid

// ============ OPTIMIZATION STRUCTURES ============

// VSIDS: Variable State Independent Decaying Sum for smart variable selection
struct VSIDSData
{
    double activity[MAX_VARS + 1]; // Activity scores for each variable
    double decay;                  // Decay factor (0.95)
    double increment;              // Current increment value
};

// Undo trail: enables backtracking without expensive deep copying
struct UndoAction
{
    enum Type
    {
        ASSIGN_VAR,
        REMOVE_CLAUSE,
        REMOVE_LITERAL
    } type;
    int var, lit;                      // Variable/literal involved
    Clause *clause, *prev_clause;      // Clause pointers for restoration
    int old_lit_count;                 // Original literal count
    int old_lits[MAX_LITS_PER_CLAUSE]; // Original literals backup
};

// Watched literals: optimize unit propagation by tracking only 2 literals per clause
struct WatchedClause
{
    Clause *clause;     // Pointer to the clause
    int watch1, watch2; // Indices of the two watched literals
};

// Conflict learning: store learned clauses to avoid repeating conflicts
struct LearnedClause
{
    int lits[10]; // Learned clause literals (kept short)
    int num_lits; // Number of literals in learned clause
};

#endif // DECL_H