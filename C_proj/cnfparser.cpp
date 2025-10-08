#include "decl.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Create new empty clause for formula construction
static Clause *create_clause()
{
    return new Clause();
}

// Append clause to end of formula's linked list
void add_clause(Formula &formula, Clause *clause) // Remove 'static' to export
{
    if (!formula.clauses)
    {
        formula.clauses = clause;
    }
    else
    {
        // Traverse to end of list
        Clause *cur = formula.clauses;
        while (cur->next)
            cur = cur->next;
        cur->next = clause;
    }
    formula.num_clauses++;
}

// Parse DIMACS CNF format file into Formula structure
int parse_cnf(const char *filename, Formula &formula)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Error: Cannot open CNF file " << filename << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(fin, line))
    {
        // Skip comment lines (start with 'c')
        if (line.empty() || line[0] == 'c')
            continue;

        // Parse problem header: "p cnf <variables> <clauses>"
        if (line[0] == 'p')
        {
            std::istringstream iss(line);
            std::string tmp;
            iss >> tmp; // 'p'
            iss >> tmp; // 'cnf'
            iss >> formula.num_vars;
            // Ignore declared clause count - we'll count actual clauses
            continue;
        }

        // Parse clause: sequence of literals terminated by 0
        std::istringstream iss(line);
        int lit;
        Clause *clause = create_clause();
        while (iss >> lit)
        {
            if (lit == 0) // End of clause marker
                break;
            if (clause->num_lits < MAX_LITS_PER_CLAUSE)
            {
                clause->lits[clause->num_lits++] = lit;
            }
            else
            {
                std::cerr << "Warning: Clause too long, truncated." << std::endl;
                break;
            }
        }
        add_clause(formula, clause);
    }

    fin.close();
    return 1;
}

// Debug: display entire formula structure
void print_formula(const Formula &formula)
{
    std::cout << "Formula: " << formula.num_clauses << " clauses, " << formula.num_vars << " vars\n";
    Clause *c = formula.clauses;
    int idx = 1;
    while (c)
    {
        std::cout << "Clause " << idx++ << ": ";
        for (int i = 0; i < c->num_lits; ++i)
            std::cout << c->lits[i] << " ";
        std::cout << "\n";
        c = c->next;
    }
}

// Debug: display variable assignments in DIMACS format
void print_assignment(const Assignment &assignment)
{
    std::cout << "Assignment:\n";
    for (int i = 1; i < MAX_VARS; ++i)
    {
        if (assignment.values[i] != -1)
            std::cout << (assignment.values[i] ? i : -i) << " "; // Positive if true, negative if false
    }
    std::cout << "\n";
}

// Memory cleanup: deallocate all clauses in formula
void free_formula(Formula &formula)
{
    Clause *c = formula.clauses;
    while (c)
    {
        Clause *next = c->next;
        delete c;
        c = next;
    }
    formula.clauses = nullptr;
    formula.num_clauses = 0;
}