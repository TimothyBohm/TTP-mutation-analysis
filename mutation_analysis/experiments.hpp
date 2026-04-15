#ifndef EXPERIMENTS_HPP
#define EXPERIMENTS_HPP

#include "schedule.hpp"
#include <string>
#include <vector>

struct ExperimentStats {
    long long total_neighbors = 0;
    long long feasible_neighbors = 0;

    long long sum_noRepeat = 0;
    long long sum_maxStreak = 0;
    long long sum_doubleRoundRobin = 0;
    long long sum_total = 0;

    int min_total = std::numeric_limits<int>::max();
    int max_total = std::numeric_limits<int>::min();

    void add(const ViolationCounts& v) {
        total_neighbors++;

        if (is_feasible(v)) {
            feasible_neighbors++;
        }

        sum_noRepeat += v.noRepeat;
        sum_maxStreak += v.maxStreak;
        sum_doubleRoundRobin += v.doubleRoundRobin;
        sum_total += v.total;

        if (v.total < min_total) min_total = v.total;
        if (v.total > max_total) max_total = v.total;
    }

    double feasible_ratio() const {
        return total_neighbors > 0
            ? static_cast<double>(feasible_neighbors) / total_neighbors
            : 0.0;
    }

    double avg_noRepeat() const {
        return total_neighbors > 0
            ? static_cast<double>(sum_noRepeat) / total_neighbors
            : 0.0;
    }

    double avg_maxStreak() const {
        return total_neighbors > 0
            ? static_cast<double>(sum_maxStreak) / total_neighbors
            : 0.0;
    }

    double avg_doubleRoundRobin() const {
        return total_neighbors > 0
            ? static_cast<double>(sum_doubleRoundRobin) / total_neighbors
            : 0.0;
    }

    double avg_total() const {
        return total_neighbors > 0
            ? static_cast<double>(sum_total) / total_neighbors
            : 0.0;
    }

    int safe_min_total() const {
        return total_neighbors > 0 ? min_total : 0;
    }

    int safe_max_total() const {
        return total_neighbors > 0 ? max_total : 0;
    }
};

inline int get_schedule_limit(const ScheduleSet& data, int max_schedules) {
    if (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size())) {
        return static_cast<int>(data.schedules.size());
    }
    return max_schedules;
}

//one line per neighbor schedule, with details about the mutation and its violations
void run_nested_neighborhood_experiment(
    const ScheduleSet& data, 
    const std::string& output_file, 
    int max_schedules = -1
);

//one line per original schedule, with aggregated stats about its neighborhood
void run_nested_neighborhood_experiment_aggregate(
    const ScheduleSet& data, 
    const std::string& output_file, 
    int max_schedule = -1
);

//one line per team size, with aggregated stats about all schedules of that size
void run_global_summary_by_team_size(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1,
    bool append = false
);

//driver loop for multiple team sizes
void run_global_summary_driver(
    const std::string& data_folder,
    const std::string& output_file,
    int max_schedules = -1,
    std::vector<int> team_sizes = {6, 8, 10}
);

void run_one_step_home_away_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1
);

void run_one_step_round_swap_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1
);

// Driver for selected team sizes
void run_one_step_experiments_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    int max_schedules = -1,
    const std::vector<int>& team_sizes = {8}
);

// New: 2-step home/away -> home/away
void run_two_step_home_away_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1
);

// New: 2-step round swap -> round swap
void run_two_step_round_swap_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1
);

// New: driver for selected team sizes
void run_two_step_experiments_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules = -1
);

// Global summaries: 1-step
void run_global_summary_one_step_home_away(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1,
    bool append = false
);

void run_global_summary_one_step_round_swap(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1,
    bool append = false
);

// Global summaries: 2-step
void run_global_summary_two_step_home_away(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1,
    bool append = false
);

void run_global_summary_two_step_round_swap(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules = -1,
    bool append = false
);

// Drivers
void run_global_summary_one_step_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules = -1,
    bool append = false
);

void run_global_summary_two_step_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules = -1,
    bool append = false
);

void run_random_walk_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

void run_random_walk_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

void run_random_walk_global_summary(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42,
    bool append = false
);

void run_random_walk_global_summary_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

#endif
