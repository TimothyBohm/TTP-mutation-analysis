#ifndef MUTATIONS_HPP
#define MUTATIONS_HPP

#include "schedule.hpp"
#include "random"

struct HomeAwayNeighbor {
    int teamA;
    int teamB;
    Schedule schedule;
};

struct RoundSwapNeighbor {
    int round1;
    int round2;
    Schedule schedule;
};

// Swap two rounds
void round_swap(Schedule& schedule, int r1, int r2);
void round_swap_consecutive(Schedule& schedule, int r);
void round_swap_random(Schedule& schedule, std::mt19937& rng);

// Swap home/away between two teams across the whole schedule
void home_away_swap(Schedule& schedule, int teamA, int teamB);
void home_away_swap_random(Schedule& schedule, std::mt19937& rng);

std::vector<HomeAwayNeighbor> generate_home_away_neighbors(const Schedule& schedule);
std::vector<RoundSwapNeighbor> generate_round_swap_neighbors(const Schedule& schedule);

#endif