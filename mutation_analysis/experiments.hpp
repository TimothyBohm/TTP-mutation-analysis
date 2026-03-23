#ifndef EXPERIMENTS_HPP
#define EXPERIMENTS_HPP

#include "schedule.hpp"
#include <string>

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
    int max_schedules = -1
);

#endif