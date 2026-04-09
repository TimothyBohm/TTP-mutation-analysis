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
    ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-16.csv");
    test_file_validity(data);

    //run_one_step_home_away_experiment(data, "../Results/one_step/results_one_step_home_away_16.csv", 10000);
    //run_one_step_round_swap_experiment(data, "../Results/one_step/results_one_step_round_swap_16.csv", 200);

    //run_two_step_home_away_experiment(data, "../Results/two_step/results_two_step_home_away_16.csv", 200);

    /* run_one_step_experiments_driver(
        SCHEDULE_FOLDER,
        "../Results",
        1000,
        {8}
    );

    run_two_step_experiments_driver(
        SCHEDULE_FOLDER,
        "../Results",
        {8},
        1000
    ); */

    /* run_global_summary_one_step_driver(
        SCHEDULE_FOLDER,
        "../Results/one_step",
        {6,8,10,12,14,16},
        1000,
        false
    ); */

    /* run_global_summary_two_step_driver(
        SCHEDULE_FOLDER,
        "../Results/two_step",
        {14,16},
        25,
        true
    ); */


    /* run_random_walk_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {16},
        1000000,   // walk length
        1,    // number of original schedules
        42     // seed
    ); */

    run_random_walk_global_summary_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {6,8,10,12,14,16},
        20000,   // walk length
        50,    // number of starting schedules per team size
        42     // seed
    );


    //run_nested_neighborhood_experiment(data, "../Results/results_two_steps_8.csv", 10);
    //run_nested_neighborhood_experiment_aggregate(data, "../Results/results_two_steps_aggregate_8.csv", 10000);
    //run_global_summary_by_team_size(data, "../Results/two_steps_global_summary_8.csv", 10, false);


    //run_global_summary_driver(SCHEDULE_FOLDER, "../Results/two_steps_global_summary.csv", 10, {6, 8, 10, 12, 14});

    return 0;
}
