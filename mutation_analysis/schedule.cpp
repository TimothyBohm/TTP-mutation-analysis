#include "schedule.hpp"
#include <iomanip>
#include <iostream>
#include <unordered_map>

#define MAX_STREAK 3

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

//build team-based representation from round-based schedule
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

//print team-based table
void print_table(const TeamScheduleTable& table) {
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

void print_table_from_schedule(const Schedule& schedule) {
    TeamScheduleTable table = build_team_schedule_table(schedule);
    print_table(table);
}

//additional helper functions
int get_opponent_id(int value) {
    return std::abs(value) - 1;
}

int get_num_teams(const Schedule& schedule) {
    if (schedule.rounds.empty()) {
        return 0;
    }
    return static_cast<int>(schedule.rounds[0].games.size()) * 2;
}

int get_num_rounds(const Schedule& schedule) {
    return static_cast<int>(schedule.rounds.size());
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

    //each violation is counted twice, 1 for each team, so i divide by 2 to get the correct count
    return violations/2;
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

                if (current_streak > MAX_STREAK) {
                    violations++;
                }
            } else {
                current_streak = 1;
            }
        }
    }

    return violations; 
}

int count_double_round_robin_violations(const TeamScheduleTable& table) {
    int violations = 0;
    int n = table.teams.size();

    for (int t = 0; t < n; t++) {
        std::unordered_map<int, int> home_count;
        std::unordered_map<int, int> away_count;

        for (int val : table.teams[t].opponents) {
            int opp = get_opponent_id(val);

            if (is_home_game(val)) {
                home_count[opp]++;
            } else {
                away_count[opp]++;
            }
        }

        for (int opp = 0; opp < n; opp++) {
            if (opp == t) {
                continue;
            }
            int h = home_count[opp];
            int a = away_count[opp];

            if (h != 1){
                violations++;
            } 
            if (a != 1){
                violations++;
            }
        }
    }

    //each violation is counted twice, 1 for each team, so i divide by 2 to get the correct count
    return violations/2;
}

bool is_feasible(const ViolationCounts& v) {
    return v.noRepeat == 0 && v.maxStreak == 0 && v.doubleRoundRobin == 0;
}

ViolationCounts evaluate_table(const TeamScheduleTable& table) {
    ViolationCounts result;

    result.noRepeat = count_noRepeat_violations(table);
    result.maxStreak = count_maxStreak_violations(table);
    result.doubleRoundRobin = count_double_round_robin_violations(table);

    result.total = result.noRepeat + result.maxStreak + result.doubleRoundRobin;

    return result;
}

ViolationCounts evaluate_schedule(const Schedule& schedule){
    TeamScheduleTable table = build_team_schedule_table(schedule);
    return evaluate_table(table);
}

Schedule make_test_schedule() {
    Schedule s;

    Round r0;
    r0.games.push_back({0, 1});
    r0.games.push_back({2, 3});
    r0.games.push_back({4, 5});
    s.rounds.push_back(r0);

    Round r1;
    r1.games.push_back({0, 2});
    r1.games.push_back({1, 4});
    r1.games.push_back({3, 5});
    s.rounds.push_back(r1);

    Round r2;
    r2.games.push_back({0, 3});
    r2.games.push_back({1, 5});
    r2.games.push_back({2, 4});
    s.rounds.push_back(r2);

    Round r3;
    r3.games.push_back({0, 4});
    r3.games.push_back({1, 2});
    r3.games.push_back({3, 5});
    s.rounds.push_back(r3);

    Round r4;
    r4.games.push_back({5, 0});
    r4.games.push_back({3, 1});
    r4.games.push_back({4, 2});
    s.rounds.push_back(r4);

    Round r5;
    r5.games.push_back({1, 0});
    r5.games.push_back({3, 2});
    r5.games.push_back({5, 4});
    s.rounds.push_back(r5);

    Round r6;
    r6.games.push_back({2, 0});
    r6.games.push_back({4, 1});
    r6.games.push_back({5, 3});
    s.rounds.push_back(r6);

    Round r7;
    r7.games.push_back({3, 0});
    r7.games.push_back({2, 1});
    r7.games.push_back({4, 5});
    s.rounds.push_back(r7);

    Round r8;
    r8.games.push_back({4, 0});
    r8.games.push_back({5, 1});
    r8.games.push_back({2, 3});
    s.rounds.push_back(r8);

    Round r9;
    r9.games.push_back({0, 5});
    r9.games.push_back({1, 3});
    r9.games.push_back({2, 4});
    s.rounds.push_back(r9);

    return s;
}