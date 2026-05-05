#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <string>
#include <vector>
#define MAX_STREAK 3

//single game: home vs away
struct Matchup {
    int home;
    int away;
};

//round = multiple matchups (n/2 games)
struct Round {
    std::vector<Matchup> games;
};

//full schedule = sequence of rounds
struct Schedule {
    std::vector<Round> rounds;
};

//collection of schedules
struct ScheduleSet {
    std::vector<Schedule> schedules;
};

//team-based representation:
//positive opponent = home
//negative opponent = away
struct TeamSchedule {
    std::vector<int> opponents;
};

struct TeamScheduleTable {
    std::vector<TeamSchedule> teams;
};


//print helper functions
void print_violations(const std::string& label, const Schedule& schedule);

void print_matchup(const Matchup& m);

void print_round(const Round& r, int round_index);

void print_schedule(const Schedule& s);

void print_schedule_set(const ScheduleSet& set, int limit);

//team table representation functions
TeamScheduleTable build_team_schedule_table(const Schedule& schedule);

void print_table(const TeamScheduleTable& table);

void print_table_from_schedule(const Schedule& schedule);

//small helper functions
int get_opponent_id(int value);
int get_num_teams(const Schedule& schedule);
int get_num_teams(const std::vector<Matchup>& matchups);
int get_num_rounds(const Schedule& schedule);
bool is_home_game(int value);
bool is_away_game(int value);

//constraint counters
int count_noRepeat_violations(const TeamScheduleTable& table);
int count_maxStreak_violations(const TeamScheduleTable& table);
int count_double_round_robin_violations(const TeamScheduleTable& table);

struct ViolationCounts {
    int noRepeat = 0;
    int maxStreak = 0;
    int doubleRoundRobin = 0;
    int total = 0;
};

bool is_feasible(const ViolationCounts& v);

ViolationCounts evaluate_table(const TeamScheduleTable& table);

ViolationCounts evaluate_schedule(const Schedule& schedule);


//helper function for violation counting
Schedule make_test_schedule();

ViolationCounts evaluate_schedule(const Schedule& schedule);


#endif