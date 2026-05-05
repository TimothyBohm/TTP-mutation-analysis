#ifndef EXPERIMENTS_HPP
#define EXPERIMENTS_HPP

#include "mutations.hpp"
#include "schedule.hpp"
#include <string>
#include <vector>



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
void one_step_experiments_driver(
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
void two_step_experiments_driver(
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
void global_summary_one_step_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules = -1,
    bool append = false
);

void global_summary_two_step_driver(
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

void run_random_walk_single_operator(
    const ScheduleSet& data,
    const std::string& output_file,
    MutationType op,
    int walk_length,
    int max_schedules,
    unsigned int seed
);

void random_walk_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

void random_walk_single_op_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    MutationType op,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

void random_walk_global_summary(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42,
    bool append = false
);

void random_walk_global_summary_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules = -1,
    unsigned int seed = 42
);

void run_random_walk_save_feasible_hits(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules = 20,
    unsigned int seed = 42
);

#endif
