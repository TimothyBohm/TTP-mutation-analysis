#include "mutations.hpp"
#include <iostream>
#include <algorithm>
#include <random>

//swap two rounds
void round_swap(Schedule& schedule, int r1, int r2) {
    //check bounds
    if (r1 < 0 || r2 < 0 || r1 >= schedule.rounds.size() || r2 >= schedule.rounds.size()) {
        std::cerr << "Round swap indices out of bounds: " << r1 << ", " << r2 << std::endl;
        return; 
    }

    std::swap(schedule.rounds[r1], schedule.rounds[r2]);
}

void round_swap_consecutive(Schedule& schedule, int r) {
    round_swap(schedule, r, r + 1);
}

void round_swap_random(Schedule& schedule, std::mt19937& rng) {
    int num_rounds = get_num_rounds(schedule);
    if (num_rounds < 2) return;

    std::uniform_int_distribution<int> dist(0, num_rounds - 1);

    int r1 = dist(rng);
    int r2;
    do {
        r2 = dist(rng);
    } while (r2 == r1);

    round_swap(schedule, r1, r2);
}

//swap home/away for matches between 2 teams
void home_away_swap(Schedule& schedule, int teamA, int teamB) {
    if(teamA == teamB) {
        std::cerr << "Cannot swap home/away for the same team: " << teamA << std::endl;
        return;
    }
    for (auto& round : schedule.rounds) {
        for (auto& game : round.games) {

            // Check if this matchup is between the two teams
            if ((game.home == teamA && game.away == teamB) ||
                (game.home == teamB && game.away == teamA)) {

                // Swap home and away
                std::swap(game.home, game.away);
            }
        }
    }
}

void home_away_swap_random(Schedule& schedule, std::mt19937& rng) {
    if (schedule.rounds.empty() || schedule.rounds[0].games.empty()) return;

    int num_teams = get_num_teams(schedule);

    std::uniform_int_distribution<int> dist(0, num_teams - 1);

    int teamA = dist(rng);
    int teamB;
    do {
        teamB = dist(rng);
    } while (teamB == teamA);

    home_away_swap(schedule, teamA, teamB);
}

std::vector<HomeAwayNeighbor> generate_home_away_neighbors(const Schedule& schedule) {
    std::vector<HomeAwayNeighbor> neighbors;

    int num_teams = get_num_teams(schedule);
    if (num_teams < 2) {
        return neighbors;
    }

    for (int teamA = 0; teamA < num_teams; teamA++) {
        for (int teamB = teamA + 1; teamB < num_teams; teamB++) {
            Schedule neighbor = schedule;
            home_away_swap(neighbor, teamA, teamB);
            neighbors.push_back({teamA, teamB, neighbor});
        }
    }

    return neighbors;
}

std::vector<RoundSwapNeighbor> generate_round_swap_neighbors(const Schedule& schedule) {
    std::vector<RoundSwapNeighbor> neighbors;

    int num_rounds = get_num_rounds(schedule);
    if (num_rounds < 2) {
        return neighbors;
    }

    for (int r1 = 0; r1 < num_rounds; r1++) {
        for (int r2 = r1 + 1; r2 < num_rounds; r2++) {
            Schedule neighbor = schedule;
            round_swap(neighbor, r1, r2);
            neighbors.push_back({r1, r2, neighbor});
        }
    }

    return neighbors;
}