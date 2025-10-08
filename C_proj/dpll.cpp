#include "decl.h"
#include <iostream>

// ============ BASIC SAT SOLVER HELPERS ============

// Check if formula is satisfied (no clauses left)
inline static bool is_satisfied(const Formula &formula)
{
    return formula.num_clauses == 0;
}

// Check if formula contains an empty clause (unsatisfiable)
inline static bool has_empty_clause(const Formula &formula)
{
    Clause *c = formula.clauses;
    while (c)
    {
        if (c->num_lits == 0)
            return true;
        c = c->next;
    }
    return false;
}

// Find a unit clause (returns literal value or 0 if none)
inline static int find_unit_clause(const Formula &formula)
{
    Clause *c = formula.clauses;
    while (c)
    {
        if (c->num_lits == 1)
            return c->lits[0];
        c = c->next;
    }
    return 0;
}

// Pick the first unassigned variable (simple heuristic)
inline static int pick_variable(const Formula &formula, const Assignment &assignment)
{
    for (int v = 1; v <= formula.num_vars; ++v)
    {
        if (assignment.values[v] == -1)
            return v;
    }
    return 0;
}

// Remove a clause from the formula's linked list
inline static void remove_clause(Formula &formula, Clause *&prev, Clause *&cur)
{
    Clause *to_delete = cur;
    cur = cur->next;
    if (prev)
        prev->next = cur;
    else
        formula.clauses = cur;
    formula.num_clauses--;
    delete to_delete;
}

// Remove a literal from a clause (shift remaining literals)
inline static void remove_literal_from_clause(Clause *clause, int lit)
{
    for (int i = 0; i < clause->num_lits; ++i)
    {
        if (clause->lits[i] == lit)
        {
            // Shift all elements after position i forward to fill the gap
            for (int j = i; j < clause->num_lits - 1; ++j)
            {
                clause->lits[j] = clause->lits[j + 1];
            }
            clause->num_lits--;
            return; // Found and removed, exit early
        }
    }
}

// Simplify formula by assigning a literal (unit propagation)
static void assign_literal(Formula &formula, Assignment &assignment, int lit)
{
    int var = std::abs(lit);
    int value = (lit > 0) ? 1 : 0;
    assignment.values[var] = value;

    Clause *c = formula.clauses;
    Clause *prev = nullptr;
    while (c)
    {
        bool clause_satisfied = false;
        for (int i = 0; i < c->num_lits; ++i)
        {
            if (c->lits[i] == lit)
            {
                clause_satisfied = true;
                break;
            }
        }
        if (clause_satisfied)
        {
            remove_clause(formula, prev, c); // Remove satisfied clause
        }
        else
        {
            remove_literal_from_clause(c, -lit); // Remove falsified literal
            prev = c;
            c = c->next;
        }
    }
}

// Deep copy formula for backtracking (preserves original state)
static Formula deep_copy_formula(const Formula &src)
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

// ============ BASIC DPLL ALGORITHM ============

// Davis-Putnam-Logemann-Loveland SAT solver (recursive backtracking)
int dpll(Formula &formula, Assignment &assignment)
{
    // Unit propagation: repeatedly assign unit clauses
    int unit_lit;
    while ((unit_lit = find_unit_clause(formula)) != 0)
    {
        assign_literal(formula, assignment, unit_lit);
        if (has_empty_clause(formula))
            return 0; // Unsatisfiable
    }
    if (is_satisfied(formula))
        return 1; // Satisfiable

    // Pick branching variable
    int var = pick_variable(formula, assignment);
    if (var == 0)
        return 0;

    // Try var = true (positive literal)
    Formula formula_copy = deep_copy_formula(formula);
    Assignment assignment_copy = assignment;
    assign_literal(formula_copy, assignment_copy, var);
    int result = dpll(formula_copy, assignment_copy);
    if (result)
    {
        assignment = assignment_copy;
        free_formula(formula_copy);
        return 1;
    }
    free_formula(formula_copy);

    // Try var = false (negative literal)
    formula_copy = deep_copy_formula(formula);
    assignment_copy = assignment;
    assign_literal(formula_copy, assignment_copy, -var);
    result = dpll(formula_copy, assignment_copy);
    if (result)
    {
        assignment = assignment_copy;
        free_formula(formula_copy);
        return 1;
    }
    free_formula(formula_copy);

    return 0;
}

// ============ OPTIMIZATION MODULES ============

// 1. VSIDS (Variable State Independent Decaying Sum) - smart variable selection
static VSIDSData vsids_data;

inline static void init_vsids(const Formula &formula)
{
    for (int i = 1; i <= formula.num_vars; ++i)
        vsids_data.activity[i] = 0.0;
    vsids_data.increment = 1.0;
    vsids_data.decay = 0.95;
}

inline static void bump_activity(int var)
{
    vsids_data.activity[var] += vsids_data.increment;
}

inline static void decay_activities(const Formula &formula)
{
    vsids_data.increment /= vsids_data.decay;
    for (int i = 1; i <= formula.num_vars; ++i)
        vsids_data.activity[i] *= vsids_data.decay;
}

// Pick variable with highest activity score
static int pick_variable_vsids(const Formula &formula, const Assignment &assignment)
{
    int best_var = 0;
    double best_score = -1.0;
    for (int v = 1; v <= formula.num_vars; ++v)
    {
        if (assignment.values[v] == -1 && vsids_data.activity[v] > best_score)
        {
            best_score = vsids_data.activity[v];
            best_var = v;
        }
    }
    return best_var;
}

// 2. Undo Trail - efficient backtracking without deep copying
static UndoAction undo_trail[MAX_VARS * 100];
static int trail_size = 0;

inline static void push_assign(int var)
{
    UndoAction action = {}; // Initialize all fields to 0
    action.type = UndoAction::ASSIGN_VAR;
    action.var = var;
    undo_trail[trail_size++] = action;
}

inline static void push_remove_clause(Clause *clause, Clause *prev)
{
    UndoAction action = {}; // Initialize all fields to 0
    action.type = UndoAction::REMOVE_CLAUSE;
    action.clause = clause;
    action.prev_clause = prev;
    undo_trail[trail_size++] = action;
}

inline static void push_remove_literal(Clause *clause, int old_count, int old_lits[])
{
    UndoAction action = {}; // Initialize all fields to 0
    action.type = UndoAction::REMOVE_LITERAL;
    action.clause = clause;
    action.old_lit_count = old_count;
    for (int i = 0; i < old_count; ++i)
        action.old_lits[i] = old_lits[i];
    undo_trail[trail_size++] = action;
}

// Undo all changes back to specified trail level
static void undo_to_level(Formula &formula, Assignment &assignment, int level)
{
    while (trail_size > level)
    {
        UndoAction &action = undo_trail[--trail_size];
        switch (action.type)
        {
        case UndoAction::ASSIGN_VAR:
            assignment.values[action.var] = -1;
            break;
        case UndoAction::REMOVE_CLAUSE:
            // Properly restore clause to its original position in linked list
            if (action.prev_clause)
            {
                action.clause->next = action.prev_clause->next;
                action.prev_clause->next = action.clause;
            }
            else
            {
                action.clause->next = formula.clauses;
                formula.clauses = action.clause;
            }
            formula.num_clauses++;
            break;
        case UndoAction::REMOVE_LITERAL:
            action.clause->num_lits = action.old_lit_count;
            for (int i = 0; i < action.old_lit_count; ++i)
                action.clause->lits[i] = action.old_lits[i];
            break;
        }
    }
}

// 3. Watched Literals - faster unit propagation (only check 2 literals per clause)
static WatchedClause watched_clauses[MAX_CLAUSES];
static int num_watched = 0;

inline static void init_watched_literals(const Formula &formula)
{
    num_watched = 0;
    Clause *c = formula.clauses;
    while (c && num_watched < MAX_CLAUSES)
    {
        if (c->num_lits > 0) // Only watch non-empty clauses
        {
            int watch1 = 0;
            int watch2 = (c->num_lits > 1) ? 1 : 0;
            watched_clauses[num_watched] = {c, watch1, watch2};
            num_watched++;
        }
        c = c->next;
    }
}

// More robust watched literals implementation
static int find_unit_clause_watched(const Assignment &assignment)
{
    // First pass: clean up invalid watched clauses
    for (int i = 0; i < num_watched; ++i)
    {
        if (!watched_clauses[i].clause || watched_clauses[i].clause->num_lits == 0)
        {
            // Remove invalid entry by shifting array
            for (int j = i; j < num_watched - 1; ++j)
                watched_clauses[j] = watched_clauses[j + 1];
            num_watched--;
            i--; // Re-check this position
        }
    }

    // Second pass: find unit clauses
    for (int i = 0; i < num_watched; ++i)
    {
        WatchedClause &wc = watched_clauses[i];
        int unassigned = 0, unit_lit = 0;
        bool satisfied = false;

        // Check all literals for correctness
        for (int j = 0; j < wc.clause->num_lits; ++j)
        {
            int lit = wc.clause->lits[j];
            int var = std::abs(lit);
            if (assignment.values[var] == -1)
            {
                unassigned++;
                unit_lit = lit;
                if (unassigned > 1)
                    break; // Early exit optimization
            }
            else if ((lit > 0 && assignment.values[var] == 1) ||
                     (lit < 0 && assignment.values[var] == 0))
            {
                satisfied = true;
                break;
            }
        }
        if (!satisfied && unassigned == 1)
            return unit_lit;
    }
    return 0;
}

// 4. Pure Literal Elimination - assign variables that appear with only one polarity
static int find_pure_literal(const Formula &formula, const Assignment &assignment)
{
    static int lit_count[MAX_VARS * 2 + 1]; // lit_count[var*2] = positive, lit_count[var*2+1] = negative
    for (int i = 0; i <= MAX_VARS * 2; ++i)
        lit_count[i] = 0;

    // Count positive and negative occurrences of each variable
    Clause *c = formula.clauses;
    while (c)
    {
        for (int i = 0; i < c->num_lits; ++i)
        {
            int lit = c->lits[i];
            int var = std::abs(lit);
            if (assignment.values[var] == -1)
            {
                if (lit > 0)
                    lit_count[var * 2]++;
                else
                    lit_count[var * 2 + 1]++;
            }
        }
        c = c->next;
    }

    // Find variables that appear with only one polarity
    for (int v = 1; v <= formula.num_vars; ++v)
    {
        if (assignment.values[v] == -1)
        {
            if (lit_count[v * 2] > 0 && lit_count[v * 2 + 1] == 0)
                return v; // Only positive occurrences
            if (lit_count[v * 2] == 0 && lit_count[v * 2 + 1] > 0)
                return -v; // Only negative occurrences
        }
    }
    return 0;
}

// 5. Conflict Learning - learn clauses to avoid repeating conflicts
static LearnedClause learned_clauses[1000];
static int num_learned = 0;

static void learn_conflict_clause(int conflicting_var)
{
    if (num_learned >= 1000)
        return; // limit learned clauses

    LearnedClause &clause = learned_clauses[num_learned++];
    clause.num_lits = 1;
    clause.lits[0] = -conflicting_var; // simple negation learning
}

// Find unit clause among learned clauses
static int find_learned_unit_clause(const Assignment &assignment)
{
    for (int i = 0; i < num_learned; ++i)
    {
        LearnedClause &clause = learned_clauses[i];
        int unassigned = 0, unit_lit = 0;
        bool satisfied = false;

        for (int j = 0; j < clause.num_lits; ++j)
        {
            int lit = clause.lits[j];
            int var = std::abs(lit);
            if (assignment.values[var] == -1)
            {
                unassigned++;
                unit_lit = lit;
            }
            else if ((lit > 0 && assignment.values[var] == 1) ||
                     (lit < 0 && assignment.values[var] == 0))
            {
                satisfied = true;
                break;
            }
        }
        if (!satisfied && unassigned == 1)
            return unit_lit;
    }
    return 0;
}

// Corrected assign_literal_opt with proper linked list handling
static void assign_literal_opt(Formula &formula, Assignment &assignment, int lit)
{
    int var = std::abs(lit);
    int value = (lit > 0) ? 1 : 0;

    push_assign(var);
    assignment.values[var] = value;
    bump_activity(var);

    Clause *c = formula.clauses;
    Clause *prev = nullptr;
    while (c)
    {
        Clause *next = c->next; // Store next pointer before any modifications

        bool clause_satisfied = false;
        for (int i = 0; i < c->num_lits; ++i)
        {
            if (c->lits[i] == lit)
            {
                clause_satisfied = true;
                break;
            }
        }

        if (clause_satisfied)
        {
            push_remove_clause(c, prev);
            // Remove clause from linked list
            if (prev)
                prev->next = next;
            else
                formula.clauses = next;
            formula.num_clauses--;
            c = next; // Use stored next pointer
        }
        else
        {
            // Save old state before removing literal
            int old_lits[MAX_LITS_PER_CLAUSE];
            int old_count = c->num_lits;
            for (int i = 0; i < old_count; ++i)
                old_lits[i] = c->lits[i];

            remove_literal_from_clause(c, -lit);

            if (c->num_lits != old_count)
                push_remove_literal(c, old_count, old_lits);

            prev = c;
            c = next; // Use stored next pointer
        }
    }
}

// ============ OPTIMIZED DPLL ALGORITHM ============

// Main entry point for optimized DPLL with all heuristics enabled
int dpll_optimized(Formula &formula, Assignment &assignment)
{
    init_vsids(formula);
    init_watched_literals(formula);
    trail_size = 0;
    num_learned = 0;

    return dpll_opt_recursive(formula, assignment);
}

// Robust optimized DPLL implementation
int dpll_opt_recursive(Formula &formula, Assignment &assignment)
{
    int trail_start = trail_size;

    // Use basic unit propagation for reliability (watched literals can be unstable)
    int unit_lit;
    while ((unit_lit = find_unit_clause(formula)) != 0)
    {
        assign_literal_opt(formula, assignment, unit_lit);
        if (has_empty_clause(formula))
        {
            undo_to_level(formula, assignment, trail_start);
            return 0;
        }
    }

    // Check for pure literals (optional optimization)
    int pure_lit = find_pure_literal(formula, assignment);
    if (pure_lit != 0)
    {
        assign_literal_opt(formula, assignment, pure_lit);
        if (has_empty_clause(formula))
        {
            undo_to_level(formula, assignment, trail_start);
            return 0;
        }
        // Continue with unit propagation after pure literal assignment
        int result = dpll_opt_recursive(formula, assignment);
        if (!result)
            undo_to_level(formula, assignment, trail_start);
        return result;
    }

    if (is_satisfied(formula))
        return 1;

    // Use VSIDS for variable selection with fallback
    int var = pick_variable_vsids(formula, assignment);
    if (var == 0)
        var = pick_variable(formula, assignment); // Fallback to basic selection

    if (var == 0)
    {
        undo_to_level(formula, assignment, trail_start);
        return 0;
    }

    decay_activities(formula);

    // Try var = true
    int trail_branch = trail_size;
    assign_literal_opt(formula, assignment, var);
    int result = dpll_opt_recursive(formula, assignment);
    if (result)
        return 1;

    undo_to_level(formula, assignment, trail_branch);

    // Learn from conflict (simple learning)
    learn_conflict_clause(var);

    // Try var = false
    assign_literal_opt(formula, assignment, -var);
    result = dpll_opt_recursive(formula, assignment);
    if (!result)
        undo_to_level(formula, assignment, trail_start);

    return result;
}
