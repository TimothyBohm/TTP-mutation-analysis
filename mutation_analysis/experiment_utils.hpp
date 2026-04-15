#ifndef EXPERIMENT_UTILS_HPP
#define EXPERIMENT_UTILS_HPP

#include "schedule.hpp"
#include <fstream>
#include <limits>

struct ExperimentStats {
    long long total_neighbors = 0;
    long long feasible_neighbors = 0;
    long long sum_noRepeat = 0;
    long long sum_maxStreak = 0;
    long long sum_doubleRoundRobin = 0;
    long long sum_total = 0;
    int min_total = std::numeric_limits<int>::max();
    int max_total = std::numeric_limits<int>::min();

    void add(const ViolationCounts& v);

    double feasible_ratio() const;
    double avg_noRepeat() const;
    double avg_maxStreak() const;
    double avg_doubleRoundRobin() const;
    double avg_total() const;
    int safe_min_total() const;
    int safe_max_total() const;
};

int get_schedule_limit(const ScheduleSet& data, int max_schedules);

void write_summary_row(std::ofstream& out, int id, int team_size, const ExperimentStats& stats);

template <typename Func>
void for_each_home_away_neighbor(const Schedule& original, Func func);

template <typename Func>
void for_each_round_swap_neighbor(const Schedule& original, Func func);

template <typename Func>
void for_each_nested_round_then_home_away(const Schedule& original, Func func);

template <typename Func>
void for_each_two_step_home_away(const Schedule& original, Func func);

template <typename Func>
void for_each_two_step_round_swap(const Schedule& original, Func func);

//#include "experiment_utils.tpp"

#endif