// {{{ MIT License
//
// Copyright 2020 Roland Kaminski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// }}}

#ifndef CLINGCON_BASE_H
#define CLINGCON_BASE_H

#include <clingo.hh>
#include <optional>

//! @file clingcon/base.hh
//! Basic data types.
//!
//! @author Roland Kaminski

namespace Clingcon {

// defaults for thread config
constexpr bool DEFAULT_PROPAGATE_CHAIN{true};
constexpr bool DEFAULT_REFINE_REASONS{true};
constexpr bool DEFAULT_REFINE_INTRODUCE{true};

// defaults for global config
constexpr int DEFAULT_MAX_INT{1 << 30};
constexpr int DEFAULT_MIN_INT{-DEFAULT_MAX_INT};
constexpr bool DEFAULT_SORT_CONSTRAINTS{true};
constexpr uint32_t DEFAULT_CLAUSE_LIMIT{1000};
constexpr bool DEFAULT_LITERALS_ONLY{false};
constexpr uint32_t DEFAULT_WEIGHT_CONSTRAINT_LIMIT{0};
constexpr uint32_t DEFAULT_DISTINCT_LIMIT{1000};
constexpr bool DEFAULT_CHECK_SOLUTION{true};
constexpr bool DEFAULT_CHECK_STATE{false};
constexpr bool DEFAULT_TRANSLATE_MINIMIZE{false};

using literal_t = Clingo::literal_t;
using weight_t = Clingo::weight_t;
constexpr literal_t TRUE_LIT{1};


//! Thread specific statistics.
struct ThreadStatistics {
    //! Reset all statistics to their starting values.
    void reset() {
        *this = ThreadStatistics();
    }

    //! Accumulate statistics in `stats`.
    void accu(ThreadStatistics const &stats) {
        time_propagate += stats.time_propagate;
        time_check += stats.time_check;
        time_undo += stats.time_undo;
        refined_reason += stats.refined_reason;
        introduced_reason += stats.introduced_reason;
        literals += stats.literals;
    }

    double time_propagate{0};
    double time_check{0};
    double time_undo{0};
    uint64_t refined_reason{0};
    uint64_t introduced_reason{0};
    uint64_t literals{0};
};


//! Propagator specific statistics.
struct Statistics {
    //! Reset all statistics to their starting values.
    void reset() {
        time_init = 0;
        time_translate = 0;
        time_simplify = 0;
        num_variables = 0;
        num_constraints = 0;
        num_clauses = 0;
        num_literals = 0;
        translate_removed = 0;
        translate_added = 0;
        translate_clauses = 0;
        translate_wcs = 0;
        translate_literals = 0;
        cost.reset();
        for (auto &s : tstats) {
            s.reset();
        }
    }

    //! Accumulate statistics in `stat`.
    void accu(Statistics &stat) {
        time_init += stat.time_init;
        time_translate += stat.time_translate;
        time_simplify += stat.time_simplify;
        num_variables += stat.num_variables;
        num_constraints += stat.num_constraints;
        num_clauses += stat.num_clauses;
        num_literals += stat.num_literals;
        translate_removed += stat.translate_removed;
        translate_added += stat.translate_added;
        translate_clauses += stat.translate_clauses;
        translate_wcs += stat.translate_wcs;
        translate_literals += stat.translate_literals;
        cost = stat.cost;

        tstats.resize(stat.tstats.size());
        auto it = stat.tstats.begin();
        for (auto &tstat : tstats) {
            tstat.accu(*it++);
        }
    }

    double time_init = 0;
    double time_translate = 0;
    double time_simplify = 0;
    uint64_t num_variables = 0;
    uint64_t num_constraints = 0;
    uint64_t num_clauses = 0;
    uint64_t num_literals = 0;
    uint64_t translate_removed = 0;
    uint64_t translate_added = 0;
    uint64_t translate_clauses = 0;
    uint64_t translate_wcs = 0;
    uint64_t translate_literals = 0;
    std::optional<uint64_t> cost;
    std::vector<ThreadStatistics> tstats;
};


//! Per state configuration.
struct StateConfig {
    bool propagate_chain{DEFAULT_PROPAGATE_CHAIN};
    bool refine_reasons{DEFAULT_REFINE_REASONS};
    bool refine_introduce{DEFAULT_REFINE_INTRODUCE};
};


//! Global configuration.
struct Config {
    //! Get state specific configuration.
    StateConfig &state_config(uint32_t thread_id) {
        if (states.size() <= thread_id) {
            states.resize(thread_id, default_state_config);
        }
        return states[thread_id];
    }

    int32_t min_int{DEFAULT_MIN_INT};
    int32_t max_int{DEFAULT_MAX_INT};
    bool sort_constraints{DEFAULT_SORT_CONSTRAINTS};
    uint32_t clause_limit{DEFAULT_CLAUSE_LIMIT};
    bool literals_only{DEFAULT_LITERALS_ONLY};
    uint32_t weight_constraint_limit{DEFAULT_WEIGHT_CONSTRAINT_LIMIT};
    uint32_t distinct_limit{DEFAULT_DISTINCT_LIMIT};
    bool check_solution{DEFAULT_CHECK_SOLUTION};
    bool check_state{DEFAULT_CHECK_STATE};
    bool translate_minimize{DEFAULT_TRANSLATE_MINIMIZE};
    StateConfig default_state_config;
    std::vector<StateConfig> states;
};


//! Class to add solver literals, create clauses, and access the current
//! assignment.
class AbstractClauseCreator {
public:
    AbstractClauseCreator() = default;

    AbstractClauseCreator(AbstractClauseCreator &&) = delete;
    AbstractClauseCreator(AbstractClauseCreator const &) = delete;
    AbstractClauseCreator &operator=(AbstractClauseCreator &&) = delete;
    AbstractClauseCreator &operator=(AbstractClauseCreator const &) = delete;

    virtual ~AbstractClauseCreator() = default;

    //! Add a new solver literal.
    virtual literal_t add_literal() = 0;

    //! Watch the given solver literal.
    virtual void add_watch(literal_t lit) = 0;

    //! Call unit propagation on the solver.
    virtual bool propagate() = 0;

    //! Add the given clause to the sovler.
    virtual bool add_clause(Clingo::LiteralSpan clause, Clingo::ClauseType type = Clingo::ClauseType::Learnt) = 0;

    //! Get the assignment.
    virtual Clingo::Assignment assignment() = 0;
};


//! Implement an `AbstractClauseCreator` using a `Clingo::PropagateInit`
//! object and extra functions.
class InitClauseCreator final : public AbstractClauseCreator {
public:
    enum State {
        StateInit = 0,
        StateTranslate = 1
    };
    using Clause = std::vector<literal_t>;
    using WeightConstraint = std::tuple<literal_t, std::vector<Clingo::WeightedLiteral>, weight_t, Clingo::WeightConstraintType>;
    using MinimizeLiteral = std::tuple<literal_t, weight_t, int>;

    InitClauseCreator(Clingo::PropagateInit &init, Statistics &stats)
    : init_{init}
    , stats_{stats} {
    }

    InitClauseCreator(InitClauseCreator &&) = delete;
    InitClauseCreator(InitClauseCreator const &) = delete;
    InitClauseCreator &operator=(InitClauseCreator &&) = delete;
    InitClauseCreator &operator=(InitClauseCreator const &) = delete;

    ~InitClauseCreator() override = default;

    literal_t add_literal() override {
        auto lit = init_.add_literal();
        ++stats_.num_literals;
        if (state_ == StateTranslate) {
            ++stats_.translate_literals;
        }
        return lit;
    }

    void add_watch(literal_t lit) override {
        init_.add_watch(lit);
    }

    bool propagate() override {
        return commit() && init_.propagate();
    }

    bool add_clause(Clingo::LiteralSpan clause, Clingo::ClauseType type = Clingo::ClauseType::Learnt) override {
        assert(type != Clingo::ClauseType::Volatile && type != Clingo::ClauseType::VolatileStatic);
        static_cast<void>(type);

        ++stats_.num_clauses;
        if (state_ == StateTranslate) {
            ++stats_.translate_clauses;
        }

        clauses_.emplace_back(clause.begin(), clause.end());
        return true;
    }

    Clingo::Assignment assignment() override {
        return init_.assignment();
    }

    //! Set the state to log either init literals or additionally translation
    //! literals.
    void set_state(State state) {
        state_ = state;
    }

    //! Map the literal to a solver literal.
    literal_t solver_literal(literal_t literal) {
        return init_.solver_literal(literal);
    }

    //! Add a weight constraint of form `lit == (wlits <= bound)`.
    bool add_weight_constraint(literal_t lit, Clingo::WeightedLiteralSpan wlits, weight_t bound, Clingo::WeightConstraintType type) {
        auto ass = assignment();
        if (ass.is_true(lit)) {
            if (type < 0) {
                return true;
            }
        }
        else if (ass.is_false(lit)) {
            if (type > 0) {
                return true;
            }
        }

        if (state_ == StateTranslate) {
            ++stats_.translate_wcs;
        }
        weight_constraints_.emplace_back(lit, std::vector<Clingo::WeightedLiteral>{wlits.begin(), wlits.end()}, bound, type);
        return true;
    }

    //! Add a literal to the objective function.
    void add_minimize(literal_t lit, weight_t weight, int level) {
        minimize_.emplace_back(lit, weight, level);
    }

    //! Commit accumulated constraints.
    bool commit() {
        for (auto &clause : clauses_) {
            if (!init_.add_clause(clause)) {
                return false;
            }
        }
        clauses_.clear();

        for (auto const &[lit, wlits, bound, type] : weight_constraints_) {
            auto inv = static_cast<Clingo::WeightConstraintType>(-type);
            if (!init_.add_weight_constraint(-lit, wlits, bound+1, inv)) {
                return false;
            }
        }
        weight_constraints_.clear();

        for (auto const &[lit, weight, level] : minimize_) {
            init_.add_minimize(lit, weight, level);
        }
        minimize_.clear();

        return true;
    }

private:
    State state_{StateInit};
    Clingo::PropagateInit &init_;
    Statistics &stats_;
    std::vector<Clause> clauses_;
    std::vector<WeightConstraint> weight_constraints_;
    std::vector<MinimizeLiteral> minimize_;
};


//! Implement an `AbstractClauseCreator` using a `Clingo::PropagateControl`
//! object.
class ControlClauseCreator final : public AbstractClauseCreator {
public:
    ControlClauseCreator(Clingo::PropagateControl &control, ThreadStatistics &stats)
    : control_{control}
    , stats_{stats} {
    }

    literal_t add_literal() override {
        ++stats_.literals;
        return control_.add_literal();
    }

    void add_watch(literal_t lit) override {
        control_.add_watch(lit);
    }

    bool propagate() override {
        return control_.propagate();
    }

    bool add_clause(Clingo::LiteralSpan clause, Clingo::ClauseType type = Clingo::ClauseType::Learnt) override {
        return control_.add_clause(clause, type) && propagate();
    }

    Clingo::Assignment assignment() override {
        return control_.assignment();
    }

private:
    Clingo::PropagateControl &control_;
    ThreadStatistics &stats_;
};

} // namespace Clingcon


#endif // CLINGCON_BASE_H
