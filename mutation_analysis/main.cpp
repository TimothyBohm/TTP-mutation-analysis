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
    
    /* // Driver tests
    run_one_step_experiments_driver(
        SCHEDULE_FOLDER,
        "../Results/1_step",
        100,
        {6,8,10,12}
    );

    run_two_step_experiments_driver(
        SCHEDULE_FOLDER,
        "../Results/2_step",
        {6,8,10,12},
        20
    );

    run_global_summary_one_step_driver(
        SCHEDULE_FOLDER,
        "../Results/1_step",
        {6,8,10,12},
        100,
        false
    );

    run_global_summary_two_step_driver(
        SCHEDULE_FOLDER,
        "../Results/2_step",
        {6,8,10,12},
        20,
        true
    );  */


    random_walk_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {16},
        20000,
        50,
        42
    );

    /* run_random_walk_global_summary_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {6,8,10,12},
        20000,
        20,
        42
    ); */


    /* run_random_walk_single_op_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {14, 16},
        MutationType::HOME_AWAY_SWAP_MUTATION,
        100000,
        1,
        42
    );

    run_random_walk_single_op_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {14, 16},
        MutationType::ROUND_SWAP_MUTATION,
        100000,
        1,
        42
    ); */

    return 0;
}
