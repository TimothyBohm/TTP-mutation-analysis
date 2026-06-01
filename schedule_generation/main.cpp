#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <string>

#include "schedule_generator.hpp"

GenerationMethod parse_method(const std::string& method_name) {
    if (method_name == "dfs") return GenerationMethod::DFS;
    if (method_name == "randdfs") return GenerationMethod::RandDFS;
    if (method_name == "restart") return GenerationMethod::RandomRestart;

    std::cerr << "[ERROR] Unknown generation method: " << method_name << "\n";
    std::cerr << "Available methods: dfs, randdfs, restart\n";
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
    //default values if they are not parsed from command line arguments
    int teams = argc > 1 ? std::stoi(argv[1]) : 8;
    int number_of_schedules = argc > 2 ? std::stoi(argv[2]) : 100;
    std::string output_path = argc > 3 ? argv[3] : "schedules.csv";
    std::string method_name = argc > 4 ? argv[4] : "dfs";
    unsigned int seed = argc > 5 ? static_cast<unsigned int>(std::stoul(argv[5])) : 42;

    if (teams < 4 || teams % 2 != 0) {
        std::cerr << "[ERROR] Number of teams must be even and at least 4.\n";
        return 1;
    }

    if (number_of_schedules <= 0) {
        std::cerr << "[ERROR] Number of schedules must be greater than 0.\n";
        return 1;
    }

    GenerationMethod method = parse_method(method_name);
    std::mt19937 rng(seed);

    std::ofstream output_file(output_path);
    if (!output_file.is_open()) {
        std::cerr << "[ERROR] Could not open output file: " << output_path << "\n";
        return 1;
    }

    std::cout << "Generating " << number_of_schedules
              << " schedules for " << teams
              << " teams using method '" << method_name << "'\n";

    int generated = 0;
    int failed_attempts = 0;

    while (generated < number_of_schedules) {
        std::optional<Schedule> schedule =
            generate_one_schedule(teams, method, rng);

        if (!schedule.has_value()) {
            failed_attempts++;

            if (failed_attempts > 10000) {
                std::cerr << "[ERROR] Too many failed generation attempts.\n";
                return 1;
            }

            continue;
        }

        ViolationCounts violations = evaluate_schedule(schedule.value());

        if (!is_feasible(violations)) {
            std::cerr << "[ERROR] Generated infeasible schedule.\n";
            print_violations("Invalid generated schedule", schedule.value());
            return 1;
        }

        save_schedule(output_file, schedule.value());
        generated++;

        if (generated % 100 == 0) {
            std::cout << "Generated " << generated << " schedules\n";
        }
    }

    std::cout << "\nDone.\n";
    std::cout << "Generated: " << generated << "\n";
    std::cout << "Failed attempts: " << failed_attempts << "\n";

    return 0;
}