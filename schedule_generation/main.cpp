#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <unordered_map>

#include "schedule_generator.hpp"

/*
===============================================================================
Random Schedule Generator

Compile:
    g++ -std=c++17 main.cpp ../mutation_analysis/schedule.cpp -o run -O3 -Wall -Wextra

Command line arguments:
    ./run [teams] [num_schedules] [output_file] [method] [seed] [--append/--frequency]

Arguments:
    teams           Number of teams (must be even and >= 4)
    num_schedules   Number of schedules to generate
    output_file     CSV file to save schedules
    method          methods: dfs, randdfs, or rowsfirst
    seed            Random seed (optional, default = 42)
    --append        Append schedules to existing file instead of overwriting
    --frequency     Instead of saving schedules, save frequency counts of unique schedules (only supported for 4 teams)

Examples:
    Generate 1000 schedules for 6 teams using DFS:
        ./run 6 1000 dfs/schedules_6.csv dfs

    Append 1,000 additional schedules to an existing file:
        ./run 8 1000 rand_dfs/schedules_8.csv randdfs 42 --append
===============================================================================
*/

GenerationMethod parse_method(const std::string& method_name) {
    if (method_name == "dfs") return GenerationMethod::DFS;
    if (method_name == "randdfs") return GenerationMethod::RandDFS;
    if (method_name == "rowsfirst") return GenerationMethod::RowsFirst;

    std::cerr << "[ERROR] Unknown generation method: " << method_name << "\n";
    std::cerr << "Available methods: dfs, randdfs, rowsfirst\n";
    std::exit(1);
}

void save_schedule(std::ofstream& file, const Schedule& schedule) {
    bool first = true;

    for (const Round& round : schedule.rounds) {
        for (const Matchup& matchup : round.games) {
            if (!first) file << " ";

            file << matchup.home << "," << matchup.away;
            first = false;
        }
    }

    file << "\n";
}

std::string schedule_to_key(const Schedule& schedule) {
    std::string key;

    bool first = true;

    for (const Round& round : schedule.rounds) {
        for (const Matchup& matchup : round.games) {
            if (!first) {
                key += " ";
            }

            key += std::to_string(matchup.home);
            key += ",";
            key += std::to_string(matchup.away);

            first = false;
        }
    }

    return key;
}

void save_frequency_table(
    std::ofstream& file,
    const std::unordered_map<std::string, long long>& frequencies
) {
    for (const auto& entry : frequencies) {
        file << entry.second << ";" << entry.first << "\n";
    }
}

int main(int argc, char* argv[]) {
    // default values if they are not parsed from command line arguments
    int teams = argc > 1 ? std::stoi(argv[1]) : 8;
    int number_of_schedules = argc > 2 ? std::stoi(argv[2]) : 100;
    std::string output_path = argc > 3 ? argv[3] : "schedules.csv";
    std::string method_name = argc > 4 ? argv[4] : "dfs";
    unsigned int seed = argc > 5 ? static_cast<unsigned int>(std::stoul(argv[5])) : 42;

    bool append = false;
    bool frequency = false;

    for (int i = 6; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--append") {
            append = true;
        }
        else if (arg == "--frequency") {
            frequency = true;
        }
        else {
            std::cerr << "[ERROR] Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    //correct number of teams check
    if (teams < 4 || teams % 2 != 0) {
        std::cerr << "[ERROR] Number of teams must be even and at least 4.\n";
        return 1;
    }

    if (number_of_schedules <= 0) {
        std::cerr << "[ERROR] Number of schedules must be greater than 0.\n";
        return 1;
    }
    if (frequency && teams != 4) {
        std::cerr << "[ERROR] --frequency is currently only supported for 4 teams.\n";
        return 1;
    }

    // parse method and initialize random number generator
    GenerationMethod method = parse_method(method_name);
    std::mt19937 rng(seed);

    // open output file for writing schedules
    std::ofstream output_file;

    if (append) {
        output_file.open(output_path, std::ios::app);
    }
    else {
        output_file.open(output_path, std::ios::trunc);
    }

    if (!output_file.is_open()) {
        std::cerr << "[ERROR] Could not open output file: " << output_path << "\n";
        return 1;
    }

    std::cout << "Generating " << number_of_schedules << " schedules for " << teams << " teams using method '"
              << method_name << "saved in the location: " << output_path << (append ? " (append)\n" : " (overwrite)\n");


    //time the generation process
    auto start_time = std::chrono::high_resolution_clock::now();

    int generated = 0;
    std::unordered_map<std::string, long long> schedule_frequencies;

    //rows first generation statistics
    GenerationStats stats;

    // main generation loop
    while (generated < number_of_schedules) {
        Schedule schedule = generate_one_schedule(teams, method, rng, stats);
        ViolationCounts violations = evaluate_schedule(schedule);

        if (!is_feasible(violations)) {
            std::cerr << "[ERROR] Generated infeasible schedule.\n";
            print_violations("Invalid generated schedule", schedule);
            return 1;
        }

        if (frequency) {
            std::string key = schedule_to_key(schedule);
            schedule_frequencies[key]++;
        }
        else {
            save_schedule(output_file, schedule);
        }
        generated++;

        if (generated % 1000 == 0) {
            std::cout << "Generated " << generated << " schedules\n";
        }
    }

    if (frequency) {
        save_frequency_table(output_file, schedule_frequencies);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed_seconds = std::chrono::duration<double>(end_time - start_time).count();

    std::cout << "\nDone.\n";
    std::cout << "Generated: " << generated << "\n";
    if (method == GenerationMethod::RowsFirst) {
        std::cout << "Attempts: " << stats.attempts << "\n";
        std::cout << "Successful attempts: " << stats.successful_attempts << "\n";
        std::cout << "Acceptance ratio: "<< static_cast<double>(stats.successful_attempts)/ stats.attempts<< "\n";
    }
    std::cout << "Elapsed time: " << elapsed_seconds << " seconds\n";
    std::cout << "Schedules per second: " << generated / elapsed_seconds << "\n";

    return 0;
}