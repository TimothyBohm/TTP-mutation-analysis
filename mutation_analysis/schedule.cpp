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

// Build team-based representation from round-based schedule
TeamScheduleTable build_team_schedule_table(const Schedule& schedule) {
    TeamScheduleTable table;

    if (schedule.rounds.empty()) {
        return table;
    }

    int num_teams = static_cast<int>(schedule.rounds[0].games.size()) * 2;
    table.teams.resize(num_teams);

    for (const auto& round : schedule.rounds) {
        for (const auto& game : round.games) {
            //home team stores positive away opponent
            table.teams[game.home].opponents.push_back(game.away + 1);
            //away team stores negative home opponent
            table.teams[game.away].opponents.push_back(-(game.home + 1));
        }
    }

    return table;
}

// Print team-based table
void print_team_schedule_table(const TeamScheduleTable& table) {
    std::cout << "========================\n";
    std::cout << "Team-based schedule table\n";
    std::cout << "========================\n";

    for (size_t t = 0; t < table.teams.size(); t++) {
        std::cout << "Team " << std::setw(2) << t << ": ";

        for (int opp : table.teams[t].opponents) {
            std::cout << std::setw(4) << opp << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

//additional helper functions
int get_opponent_id(int value) {
    return std::abs(value) - 1;
}

bool is_home_game(int value) {
    return value > 0;
}

bool is_away_game(int value) {
    return value < 0;
}

int count_noRepeat_violations(const TeamScheduleTable& table) {
    int violations = 0;

    for (const auto& team : table.teams) {
        for (size_t i = 0; i + 1 < team.opponents.size(); i++) {
            int op1 = get_opponent_id(team.opponents[i]);
            int op2 = get_opponent_id(team.opponents[i + 1]);

            if (op1 == op2) {
                violations++;
            }
        }
    }

    return violations;
}

int count_maxStreak_violations(const TeamScheduleTable& table) {
    int violations = 0;

    for (const auto& team : table.teams) {
        int current_streak = 1;

        for (size_t i = 1; i < team.opponents.size(); i++) {
            bool same_location =
                (is_home_game(team.opponents[i]) && is_home_game(team.opponents[i - 1])) ||
                (is_away_game(team.opponents[i]) && is_away_game(team.opponents[i - 1]));

            if (same_location) {
                current_streak++;

                if (current_streak > 3) {
                    violations++;
                }
            } else {
                current_streak = 1;
            }
        }
    }

    return violations;
}