#include "schedule.hpp"
#include <iomanip>
#include <iostream>

//print a single matchup
void print_matchup(const Matchup& m) {
    std::cout << "| " << m.home << "-" << m.away << " |";
}

//print a round
void print_round(const Round& r, int round_index) {
    std::cout << "Round " << std::setw(2) << round_index << ": ";

    for (const auto& game : r.games) {
        std::cout << std::setw(5);
        print_matchup(game);
        std::cout << " ";
    }

    std::cout << std::endl;
}

//print a full schedule
void print_schedule(const Schedule& s) {
    std::cout << "========================\n";
    std::cout << "Schedule (" << s.rounds.size() << " rounds)\n";
    std::cout << "========================\n";

    for (size_t i = 0; i < s.rounds.size(); i++) {
        print_round(s.rounds[i], i);
    }

    std::cout << std::endl;
}

//print multiple schedules
void print_schedule_set(const ScheduleSet& set, int limit) {
    int count = 0;

    for (const auto& schedule : set.schedules) {
        if (count >= limit) break;

        std::cout << "Schedule :" << count << "\n";
        print_schedule(schedule);

        count++;
    }

    if (set.schedules.empty()) {
        std::cout << "No schedules to display.\n";
    }
}