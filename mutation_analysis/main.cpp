#include "read_schedules.hpp"
#include "schedule.hpp"
#include "mutations.hpp"
#include "experiments.hpp"
#include <iostream>
#include <random>

#define SCHEDULE_FOLDER "../Schedules/Schedules_ALL"

std::mt19937 rng(42); //reproducibility
//std::mt19937 rng(std::random_device{}()); //complete randomness

//g++ -std=c++17 *.cpp -o run
//g++ -std=c++17 *.cpp -o run -Wall -Wextra
//./run

int main() {
    //ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-8.csv");
    //test_file_validity(data);

    //run_nested_neighborhood_experiment(data, "../Results/results_two_steps_8.csv", 10);
    //run_nested_neighborhood_experiment_aggregate(data, "../Results/results_two_steps_aggregate_8.csv", 10);
    //run_global_summary_by_team_size(data, "../Results/two_steps_global_summary_8.csv", 10, false);


    run_global_summary_driver(SCHEDULE_FOLDER, "../Results/two_steps_global_summary.csv", 10, {6, 8, 10, 12, 14});

    return 0;
}