#include "read_schedules.hpp"
#include "schedule.hpp"
#include "mutations.hpp"
#include "experiments.hpp"
#include <iostream>
#include <random>

#define SCHEDULE_FOLDER "../Schedules/Schedules_ALL"

std::mt19937 rng(42); //reproducibility
//std::mt19937 rng(std::random_device{}()); //complete randomness

/* 
to compile, run:
g++ -std=c++17 *.cpp -o run -Wall -Wextra
to run: 
./run 
*/


/* 
Format of the arguments for the experiment drivers
1. data_folder: The folder where the input schedule files are located.
2. output_folder: The folder where the output results will be saved.
3. team_sizes: A vector of integers representing the different team sizes to be tested (e.g., {4, 6, 8, 10, 12, 14, 16}).
4. walk_length: The number of steps to perform in the random walk.
5. max_schedules: The maximum number of schedules to process for each team size (default is -1, which means no limit).
6. seed: The random seed for reproducibility (default is 42).

*/

int main() {


    //Experiment 1: Random walks
    random_walk_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {4,8,12},
        1000,
        10,
        42
    );

    random_walk_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {4,8,12},
        10000,
        1,
        42
    );

    random_walk_single_op_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {8,12,16},
        MutationType::MATCH_SWAP_MUTATION,
        1000,
        10,
        42
    );

    random_walk_global_summary_driver(
        SCHEDULE_FOLDER,
        "../Results/random_walk",
        {4,8,12},
        1000,
        10,
        42
    );



    //experiment 2: Frequency count of revisited feasible schedules
    run_random_walk_save_feasible_hits(
        read_schedules_from_file("../Schedules/Schedules_ALL/All-4.csv"),
        "../Results/random_walk/feasible_hits/feasible_hits_4.csv",
        1000,
        10,
        42
    );

    run_random_walk_save_feasible_hits(
        read_schedules_from_file("../Schedules/Schedules_ALL/All-8.csv"),
        "../Results/random_walk/feasible_hits/feasible_hits_8.csv",
        1000,
        10,
        42
    );
    


    //experiment 3: Transition matrix of violations
    run_random_walk_violation_transition_matrix(
        read_schedules_from_file("../Schedules/Schedules_ALL/All-8.csv"),
        "../Results/random_walk/transition_matrix/transition_matrix_8.csv",
        10000,
        0,
        42
    );

    run_random_walk_violation_transition_matrix(
        read_schedules_from_file("../Schedules/Schedules_ALL/All-12.csv"),
        "../Results/random_walk/transition_matrix/transition_matrix_12.csv",
        10000,
        0,
        42
    );

    return 0;
}
