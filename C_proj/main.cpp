#include "decl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// Display Sudoku grid in 9x9 format
inline static void print_sudoku_grid(const int grid[9][9])
{
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
            cout << grid[i][j] << " ";
        cout << endl;
    }
}

// Write SAT solver results to .res file (DIMACS format)
static void write_result_file(const string &filename, int result, const Assignment &assignment, int elapsed_ms)
{
    string outname = filename.substr(0, filename.find_last_of('.')) + ".res";
    ofstream fout(outname);
    fout << "s " << result << endl; // Solution status
    if (result == 1)
    {
        fout << "v "; // Variable assignments
        for (int i = 1; i < MAX_VARS; ++i)
        {
            if (assignment.values[i] != -1)
                fout << (assignment.values[i] ? i : -i) << " ";
        }
        fout << endl;
    }
    fout << "t " << elapsed_ms << endl; // Execution time
    fout.close();
    cout << "Result written to: " << outname << endl;
}

// Deep copy formula for performance comparison (preserves original)
static Formula deep_copy_formula_main(const Formula &src)
{
    Formula dst;
    dst.num_vars = src.num_vars;
    dst.num_clauses = src.num_clauses;
    dst.clauses = nullptr;
    Clause *cur_src = src.clauses;
    Clause **cur_dst = &dst.clauses;
    while (cur_src)
    {
        Clause *new_clause = new Clause();
        new_clause->num_lits = cur_src->num_lits;
        for (int i = 0; i < cur_src->num_lits; ++i)
            new_clause->lits[i] = cur_src->lits[i];
        new_clause->next = nullptr;
        *cur_dst = new_clause;
        cur_dst = &new_clause->next;
        cur_src = cur_src->next;
    }
    return dst;
}

int main()
{
    std::ios::sync_with_stdio(false);

    while (true)
    {
        // Main menu interface
        cout << "==== SAT & Sudoku Solver ====" << endl;
        cout << "1. Solve SAT (Optimized)" << endl;
        cout << "2. Solve Sudoku (Optimized)" << endl;
        cout << "3. Compare SAT solvers (current vs. optimized)" << endl;
        cout << "0. Exit" << endl;
        cout << "Select option: ";
        int top_choice;
        cin >> top_choice;
        if (top_choice == 0)
            break;

        if (top_choice == 1) // SAT solving mode
        {
            cout << "Enter CNF file path: ";
            string cnf_file;
            cin >> cnf_file;

            Formula formula;
            if (!parse_cnf(cnf_file.c_str(), formula))
            {
                cerr << "Failed to parse CNF file." << endl;
                continue;
            }

            cout << "Parsed CNF with " << formula.num_vars << " variables and " << formula.num_clauses << " clauses." << endl;

            // Run optimized DPLL solver
            Assignment assignment;
            clock_t start = clock();
            cout << "Timer started." << endl;
            cout << "Starting optimized DPLL algorithm..." << endl;

            int result = dpll_optimized(formula, assignment);
            clock_t end = clock();
            int elapsed_ms = (end - start) * 1000 / CLOCKS_PER_SEC;
            cout << "Timer stopped." << endl
                 << endl;

            cout << "SAT result: " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << endl;
            cout << "Time: " << elapsed_ms << " ms" << endl;
            if (result)
                print_assignment(assignment);

            write_result_file(cnf_file, result ? 1 : 0, assignment, elapsed_ms);
            free_formula(formula);
        }
        else if (top_choice == 2) // Sudoku solving mode
        {
            cout << "Sudoku category: 1. Classic  2. Percent Sudoku" << endl;
            int cat_choice;
            cin >> cat_choice;
            bool percent = (cat_choice == 2);

            while (true) // Generate until solvable puzzle found
            {
                int sudoku[9][9] = {0};
                Formula formula;
                int gen_ok = percent
                                 ? generate_percent_sudoku_cnf(sudoku, formula)
                                 : generate_sudoku_cnf(sudoku, formula);

                if (!gen_ok)
                {
                    cerr << "Failed to generate CNF from Sudoku." << endl;
                    continue;
                }

                // Solve generated Sudoku
                Assignment assignment;
                clock_t start = clock();

                int result = dpll_optimized(formula, assignment);
                clock_t end = clock();
                int elapsed_ms = (end - start) * 1000 / CLOCKS_PER_SEC;

                if (result)
                {
                    cout << "Original Sudoku Grid:" << endl;
                    print_sudoku_grid(sudoku);
                    cout << "Sudoku Solution:" << endl;
                    int solved_grid[9][9];
                    assignment_to_grid(assignment, solved_grid);
                    print_sudoku_grid(solved_grid);

                    cout << "Sudoku result: SOLVED" << endl;
                    cout << "Time: " << elapsed_ms << " ms" << endl;
                    write_result_file("generated_sudoku.txt", 1, assignment, elapsed_ms);
                    free_formula(formula);
                    break;
                }
                else
                {
                    cout << "Sudoku result: NO SOLUTION. Generating another sudoku..." << endl;
                    free_formula(formula);
                    // Continue loop to generate new puzzle
                }
            }
        }
        else if (top_choice == 3) // Performance comparison mode
        {
            cout << "==== SAT Solver Comparison ====" << endl;
            cout << "Enter CNF file path: ";
            string cnf_file;
            cin >> cnf_file;

            Formula formula;
            if (!parse_cnf(cnf_file.c_str(), formula))
            {
                cerr << "Failed to parse CNF file." << endl;
                continue;
            }

            cout << "Parsed CNF with " << formula.num_vars << " variables and " << formula.num_clauses << " clauses." << endl;
            cout << "Running comparison tests..." << endl
                 << endl;

            // Test 1: Optimized DPLL (with heuristics)
            Formula formula_opt = deep_copy_formula_main(formula);
            Assignment assignment_opt;

            cout << "Testing optimized DPLL..." << endl;
            clock_t start_opt = clock();
            int result_opt = dpll_optimized(formula_opt, assignment_opt);
            clock_t end_opt = clock();
            int time_opt = (end_opt - start_opt) * 1000 / CLOCKS_PER_SEC;

            cout << "Optimized DPLL result: " << (result_opt ? "SATISFIABLE" : "UNSATISFIABLE") << endl;
            cout << "Optimized DPLL time: " << time_opt << " ms" << endl
                 << endl;

            // Test 2: Basic DPLL (naive approach)
            Formula formula_basic = deep_copy_formula_main(formula);
            Assignment assignment_basic;

            cout << "Testing basic DPLL..." << endl;
            clock_t start_basic = clock();
            int result_basic = dpll(formula_basic, assignment_basic);
            clock_t end_basic = clock();
            int time_basic = (end_basic - start_basic) * 1000 / CLOCKS_PER_SEC;

            cout << "Basic DPLL result: " << (result_basic ? "SATISFIABLE" : "UNSATISFIABLE") << endl;
            cout << "Basic DPLL time: " << time_basic << " ms" << endl
                 << endl;

            // Performance analysis
            cout << "==== COMPARISON RESULTS ====" << endl;
            cout << "Optimized DPLL: " << time_opt << " ms" << endl;
            cout << "Basic DPLL:     " << time_basic << " ms" << endl;

            // Calculate performance improvement metrics
            if (time_basic > 0)
            {
                double improvement_rate = (double)abs(time_basic - time_opt) / time_basic;
                cout << "Optimization ratio (relative improvement): " << improvement_rate << " (" << (improvement_rate * 100) << "%)" << endl;

                if (time_opt < time_basic)
                {
                    double speedup = (double)time_basic / time_opt;
                    cout << "Optimized version is " << speedup << "x faster!" << endl;
                }
                else if (time_opt > time_basic)
                {
                    double slowdown = (double)time_opt / time_basic;
                    cout << "Basic version is " << slowdown << "x faster (optimization overhead)" << endl;
                }
                else
                {
                    cout << "Both versions performed identically" << endl;
                }
            }
            else
            {
                cout << "Basic DPLL time is 0ms - cannot calculate relative improvement" << endl;
            }

            // Verify correctness: both solvers should agree on satisfiability
            if (result_basic == result_opt)
            {
                cout << "Results match: Both solvers agree on satisfiability" << endl;
            }
            else
            {
                cout << "WARNING: Results differ! Optimized=" << result_opt << ", Basic=" << result_basic << endl;
            }

            // Memory cleanup
            free_formula(formula);
            free_formula(formula_basic);
            free_formula(formula_opt);
        }
        else
        {
            cout << "Invalid option." << endl;
        }
    }
    cout << "Goodbye!" << endl;
    return 0;
}