#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <vector>

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


//print helper functions
void print_matchup(const Matchup& m);

void print_round(const Round& r, int round_index);

void print_schedule(const Schedule& s);

void print_schedule_set(const ScheduleSet& set, int limit);

#endif