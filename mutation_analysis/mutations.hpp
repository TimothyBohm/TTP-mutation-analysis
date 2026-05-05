#ifndef MUTATIONS_HPP
#define MUTATIONS_HPP

#include "schedule.hpp"
//#include "experiments.hpp"
#include "schedule.hpp"
#include <random>
#include <string>

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

enum MutationType {
    HOME_AWAY_SWAP_MUTATION,
    ROUND_SWAP_MUTATION,
    TEAM_SWAP_MUTATION,
    MATCH_SWAP_MUTATION,
    MATCH_ROUND_SWAP_MUTATION
    // Add more mutation types here if needed
};

void apply_mutation(Schedule& schedule, MutationType op);

// Swap two rounds
void round_swap(Schedule& schedule, int r1, int r2);
void round_swap_consecutive(Schedule& schedule, int r);
void round_swap_random(Schedule& schedule, std::mt19937& rng);

// Swap home/away between two teams across the whole schedule
void home_away_swap(Schedule& schedule, int teamA, int teamB);
void home_away_swap_random(Schedule& schedule, std::mt19937& rng);

std::vector<HomeAwayNeighbor> generate_home_away_neighbors(const Schedule& schedule);
std::vector<RoundSwapNeighbor> generate_round_swap_neighbors(const Schedule& schedule);

// Swap two teams across the whole schedule
void team_swap(Schedule& schedule, int teamA, int teamB);
void team_swap_random(Schedule& schedule, std::mt19937& rng);

//match swap mutation
void match_swap(Schedule& schedule, int round_id, int team1, int team2);
void match_swap_random(Schedule& schedule, std::mt19937& rng);

void match_round_swap(Schedule& schedule, int team, int r1, int r2);
void match_round_swap_random(Schedule& schedule, std::mt19937& rng);

std::string mutation_type_to_string(MutationType type);

#endif
