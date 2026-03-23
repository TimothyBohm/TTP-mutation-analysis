#include "read_schedules.hpp"
#include "schedule.hpp"
#include "mutations.hpp"
#include <iostream>
#include <random>

std::mt19937 rng(42); //reproducibility
//std::mt19937 rng(std::random_device{}()); //complete randomness

//g++ -std=c++17 *.cpp -o run
//./run

int main() {
    ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-8.csv");

    if (data.schedules.empty()) {
        std::cerr << "No schedules loaded.\n";
        return 0;
    }

    Schedule original = data.schedules[0];

    auto round_neighbors = generate_round_swap_neighbors(original);

    std::cout << "Round-swap neighbors: " << round_neighbors.size() << "\n";

    int total_neighbors = 0;

    for (const auto& round_neighbor : round_neighbors) {
        auto home_neighbors = generate_home_away_neighbors(round_neighbor.schedule);
        total_neighbors += static_cast<int>(home_neighbors.size());
        for(int i = 0; i < home_neighbors.size(); i++) {
            ViolationCounts v = evaluate_schedule(home_neighbors[i].schedule);
            std::cout << "Round swap: (" << round_neighbor.round1 << ", " << round_neighbor.round2 << ")"
                      << " Home/Away swap: (" << home_neighbors[i].teamA << ", " << home_neighbors[i].teamB << ")"
                      << " Violations - NoRepeat: " << v.noRepeat
                      << ", MaxStreak: " << v.maxStreak
                      << ", Total: " << v.total
                      << "\n";
        }
    }

    std::cout << "Total second-level neighbors: " << total_neighbors << "\n";

    return 0;
}