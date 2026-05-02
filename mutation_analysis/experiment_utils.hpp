#ifndef EXPERIMENT_UTILS_HPP
#define EXPERIMENT_UTILS_HPP

#include "schedule.hpp"
#include "mutations.hpp"
#include <fstream>
#include <limits>
#include <string>
#include <random>

struct ExperimentStats {
    long long total = 0;
    long long feasible = 0;

    long long sum_noRepeat = 0;
    long long sum_maxStreak = 0;
    long long sum_doubleRoundRobin = 0;
    long long sum_total = 0;

    int min_total = std::numeric_limits<int>::max();
    int max_total = std::numeric_limits<int>::min();

    void add(const ViolationCounts& v) {
        total++;

        if (is_feasible(v)) feasible++;

        sum_noRepeat += v.noRepeat;
        sum_maxStreak += v.maxStreak;
        sum_doubleRoundRobin += v.doubleRoundRobin;
        sum_total += v.total;

        if (v.total < min_total) min_total = v.total;
        if (v.total > max_total) max_total = v.total;
    }

    double ratio() const {
        return total > 0 ? static_cast<double>(feasible) / total : 0.0;
    }

    double avg_noRepeat() const {
        return total > 0 ? static_cast<double>(sum_noRepeat) / total : 0.0;
    }

    double avg_maxStreak() const {
        return total > 0 ? static_cast<double>(sum_maxStreak) / total : 0.0;
    }

    double avg_doubleRoundRobin() const {
        return total > 0 ? static_cast<double>(sum_doubleRoundRobin) / total : 0.0;
    }

    double avg_total() const {
        return total > 0 ? static_cast<double>(sum_total) / total : 0.0;
    }

    int safe_min() const {
        return total > 0 ? min_total : 0;
    }

    int safe_max() const {
        return total > 0 ? max_total : 0;
    }
};

inline int get_schedule_limit(const ScheduleSet& data, int max_schedules) {
    if (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size())) {
        return static_cast<int>(data.schedules.size());
    }

    return max_schedules;
}

inline void write_summary_header(std::ofstream& out) {
    out << "schedule_id,team_size,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";
}

inline void write_global_summary_header(std::ofstream& out) {
    out << "team_size,num_original_schedules,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";
}

inline void write_summary_row(
    std::ofstream& out,
    int schedule_id,
    int team_size,
    const ExperimentStats& stats
) {
    out << schedule_id << ","
        << team_size << ","
        << stats.total << ","
        << stats.feasible << ","
        << stats.ratio() << ","
        << stats.avg_noRepeat() << ","
        << stats.avg_maxStreak() << ","
        << stats.avg_doubleRoundRobin() << ","
        << stats.avg_total() << ","
        << stats.safe_min() << ","
        << stats.safe_max() << "\n";
}

inline void write_global_summary_row(
    std::ofstream& out,
    int team_size,
    int num_original_schedules,
    const ExperimentStats& stats
) {
    out << team_size << ","
        << num_original_schedules << ","
        << stats.total << ","
        << stats.feasible << ","
        << stats.ratio() << ","
        << stats.avg_noRepeat() << ","
        << stats.avg_maxStreak() << ","
        << stats.avg_doubleRoundRobin() << ","
        << stats.avg_total() << ","
        << stats.safe_min() << ","
        << stats.safe_max() << "\n";
}

template <typename Func>
void for_each_home_away_neighbor(const Schedule& original, Func func) {
    int n = get_num_teams(original);

    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            Schedule mutated = original;
            home_away_swap(mutated, a, b);
            func(mutated);
        }
    }
}

template <typename Func>
void for_each_round_swap_neighbor(const Schedule& original, Func func) {
    int r = get_num_rounds(original);

    for (int r1 = 0; r1 < r; r1++) {
        for (int r2 = r1 + 1; r2 < r; r2++) {
            Schedule mutated = original;
            round_swap(mutated, r1, r2);
            func(mutated);
        }
    }
}

template <typename Func>
void for_each_two_step_home_away_neighbor(const Schedule& original, Func func) {
    int n = get_num_teams(original);

    for (int a1 = 0; a1 < n; a1++) {
        for (int b1 = a1 + 1; b1 < n; b1++) {
            Schedule first = original;
            home_away_swap(first, a1, b1);

            for (int a2 = 0; a2 < n; a2++) {
                for (int b2 = a2 + 1; b2 < n; b2++) {
                    Schedule mutated = first;
                    home_away_swap(mutated, a2, b2);
                    func(mutated);
                }
            }
        }
    }
}

template <typename Func>
void for_each_two_step_round_swap_neighbor(const Schedule& original, Func func) {
    int r = get_num_rounds(original);

    for (int r1a = 0; r1a < r; r1a++) {
        for (int r1b = r1a + 1; r1b < r; r1b++) {
            Schedule first = original;
            round_swap(first, r1a, r1b);

            for (int r2a = 0; r2a < r; r2a++) {
                for (int r2b = r2a + 1; r2b < r; r2b++) {
                    Schedule mutated = first;
                    round_swap(mutated, r2a, r2b);
                    func(mutated);
                }
            }
        }
    }
}

template <typename Func>
ExperimentStats collect_stats(const Schedule& original, Func neighbor_loop) {
    ExperimentStats stats;

    neighbor_loop(original, [&](const Schedule& mutated) {
        stats.add(evaluate_schedule(mutated));
    });

    return stats;
}

struct RandomWalkStats {
    int feasible_count = 0;
    int current_feasible_streak = 0;
    int longest_feasible_streak = 0;

    long long total_sum = 0;
    int max_total = 0;
    int steps = 0;

    void add(const ViolationCounts& v) {
        bool feasible = is_feasible(v);

        if (feasible) {
            feasible_count++;
            current_feasible_streak++;
            if (current_feasible_streak > longest_feasible_streak) {
                longest_feasible_streak = current_feasible_streak;
            }
        } else {
            current_feasible_streak = 0;
        }

        total_sum += v.total;

        if (steps == 0 || v.total > max_total) {
            max_total = v.total;
        }

        steps++;
    }

    double feasible_ratio() const {
        return steps > 0 ? static_cast<double>(feasible_count) / steps : 0.0;
    }

    double running_avg_total() const {
        return steps > 0 ? static_cast<double>(total_sum) / steps : 0.0;
    }
};

inline MutationType choose_random_mutation(std::mt19937& rng) {
    static std::uniform_int_distribution<int> dist(0, 1);

    return dist(rng) == 0
        ? HOME_AWAY_SWAP_MUTATION
        : ROUND_SWAP_MUTATION;
}

inline ViolationCounts apply_random_mutation_and_evaluate(
    Schedule& current,
    std::mt19937& rng,
    MutationType& chosen_operator
) {
    chosen_operator = choose_random_mutation(rng);

    if (chosen_operator == HOME_AWAY_SWAP_MUTATION) {
        home_away_swap_random(current, rng);
    } else {
        round_swap_random(current, rng);
    }

    return evaluate_schedule(current);
}

inline ViolationCounts apply_mutation_and_evaluate(
    Schedule& current,
    std::mt19937& rng,
    MutationType op
) {
    if (op == HOME_AWAY_SWAP_MUTATION) {
        home_away_swap_random(current, rng);
    } else if (op == ROUND_SWAP_MUTATION) {
        round_swap_random(current, rng);
    }

    return evaluate_schedule(current);
}

#endif