#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "schedule_generator.hpp"



GenerationMethod parse_method(const std::string& method_name) {
    if (method_name == "dfs") return GenerationMethod::DFS;
    if (method_name == "randdfs") return GenerationMethod::RandDFS;

    std::cerr << "[ERROR] Unknown generation method: " << method_name << "\n";
    std::cerr << "Available methods: dfs, randdfs\n";
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

int main(int argc, char* argv[]) {
    // default values if they are not parsed from command line arguments
    int teams = argc > 1 ? std::stoi(argv[1]) : 8;
    int number_of_schedules = argc > 2 ? std::stoi(argv[2]) : 100;
    std::string output_path = argc > 3 ? argv[3] : "schedules.csv";
    std::string method_name = argc > 4 ? argv[4] : "dfs";
    unsigned int seed = argc > 5 ? static_cast<unsigned int>(std::stoul(argv[5])) : 42;

    bool append = false;

    for (int i = 6; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--append") {
            append = true;
        }
        else {
            std::cerr << "[ERROR] Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    if (teams < 4 || teams % 2 != 0) {
        std::cerr << "[ERROR] Number of teams must be even and at least 4.\n";
        return 1;
    }

    if (number_of_schedules <= 0) {
        std::cerr << "[ERROR] Number of schedules must be greater than 0.\n";
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

    int generated = 0;

    // main generation loop
    while (generated < number_of_schedules) {
        Schedule schedule = generate_one_schedule(teams, method, rng);
        ViolationCounts violations = evaluate_schedule(schedule);

        if (!is_feasible(violations)) {
            std::cerr << "[ERROR] Generated infeasible schedule.\n";
            print_violations("Invalid generated schedule", schedule);
            return 1;
        }

        save_schedule(output_file, schedule);
        generated++;

        if (generated % 100 == 0) {
            std::cout << "Generated " << generated << " schedules\n";
        }
    }

    std::cout << "\nDone.\n";
    std::cout << "Generated: " << generated << "\n";

    return 0;
}