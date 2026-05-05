#include "mutations.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <set>
#include <utility>

struct RoundEntry {
    int opponent = -1;
    bool is_home = false;
};

std::pair<int, int> pair_key(int a, int b) {
    return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
}

Matchup find_matchup_between(const Round& round, int a, int b) {
    for (const auto& game : round.games) {
        if ((game.home == a && game.away == b) ||
            (game.home == b && game.away == a)) {
            return game;
        }
    }

    return Matchup{-1, -1};
}

std::pair<int, int> ordered_pair(int home, int away) {
    return {home, away};
}

std::pair<int, int> unordered_pair_key(int a, int b) {
    if (a < b) return {a, b};
    return {b, a};
}

void add_affected_pair(std::set<std::pair<int, int>>& affected_pairs, int a, int b) {
    affected_pairs.insert(unordered_pair_key(a, b));
}

bool repair_directions_for_pair(Schedule& schedule, int a, int b) {
    std::vector<std::pair<int, int>> occurrences;

    int a_home_count = 0;
    int b_home_count = 0;

    for (int r = 0; r < static_cast<int>(schedule.rounds.size()); r++) {
        for (int g = 0; g < static_cast<int>(schedule.rounds[r].games.size()); g++) {
            Matchup& game = schedule.rounds[r].games[g];

            if (game.home == a && game.away == b) {
                occurrences.push_back({r, g});
                a_home_count++;
            }
            else if (game.home == b && game.away == a) {
                occurrences.push_back({r, g});
                b_home_count++;
            }
        }
    }

    if (occurrences.size() != 2) {
        std::cerr << "Direction repair failed for pair "
                  << a << "-" << b
                  << ": found " << occurrences.size()
                  << " occurrences, expected 2.\n";
        return false;
    }

    // Already correct.
    if (a_home_count == 1 && b_home_count == 1) {
        return true;
    }

    // Same direction twice. Flip one occurrence.
    if (a_home_count == 2 || b_home_count == 2) {
        int r = occurrences[1].first;
        int g = occurrences[1].second;

        std::swap(
            schedule.rounds[r].games[g].home,
            schedule.rounds[r].games[g].away
        );

        return true;
    }

    return false;
}

bool repair_directions_for_affected_pairs(
    Schedule& schedule,
    const std::set<std::pair<int, int>>& affected_pairs
) {
    for (const auto& p : affected_pairs) {
        if (!repair_directions_for_pair(schedule, p.first, p.second)) {
            return false;
        }
    }

    return true;
}

//swap two rounds
void round_swap(Schedule& schedule, int r1, int r2) {
    //check bounds
    if (r1 < 0 || r2 < 0 || r1 >= static_cast<int>(schedule.rounds.size()) || r2 >= static_cast<int>(schedule.rounds.size())) {
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

void team_swap(Schedule& schedule, int teamA, int teamB) {
    if (teamA == teamB) {
        std::cerr << "Cannot swap the same team: " << teamA << std::endl;
        return;
    }

    for (auto& round : schedule.rounds) {
        for (auto& game : round.games) {

            // Replace teamA with teamB
            if (game.home == teamA) game.home = teamB;
            else if (game.home == teamB) game.home = teamA;

            if (game.away == teamA) game.away = teamB;
            else if (game.away == teamB) game.away = teamA;
        }
    }
}

void team_swap_random(Schedule& schedule, std::mt19937& rng) {
    if (schedule.rounds.empty() || schedule.rounds[0].games.empty()) return;

    int num_teams = get_num_teams(schedule);

    std::uniform_int_distribution<int> dist(0, num_teams - 1);

    int teamA = dist(rng);
    int teamB;
    do {
        teamB = dist(rng);
    } while (teamB == teamA);

    team_swap(schedule, teamA, teamB);
}

int get_opponent_in_round(const Round& round, int team) {
    for (const auto& game : round.games) {
        if (game.home == team) return game.away;
        if (game.away == team) return game.home;
    }
    return -1;
}

bool team_is_home_in_round(const Round& round, int team) {
    for (const auto& game : round.games) {
        if (game.home == team) return true;
        if (game.away == team) return false;
    }

    return false;
}

bool teams_play_together_in_round(const Round& round, int team1, int team2) {
    for (const auto& game : round.games) {
        if ((game.home == team1 && game.away == team2) ||
            (game.home == team2 && game.away == team1)) {
            return true;
        }
    }

    return false;
}

int find_round_where_teams_play(
    const Schedule& schedule,
    int team1,
    int team2,
    int excluded_round
) {
    for (int r = 0; r < static_cast<int>(schedule.rounds.size()); r++) {
        if (r == excluded_round) continue;

        if (teams_play_together_in_round(schedule.rounds[r], team1, team2)) {
            return r;
        }
    }

    return -1;
}

void set_game_for_team_preserve_location(
    Round& round,
    int team,
    int new_opponent
) {
    for (auto& game : round.games) {
        if (game.home == team || game.away == team) {
            bool team_was_home = (game.home == team);

            if (team_was_home) {
                game.home = team;
                game.away = new_opponent;
            } else {
                game.home = new_opponent;
                game.away = team;
            }

            return;
        }
    }
}

void match_swap(Schedule& schedule, int round_id, int team1, int team2) {
    int num_rounds = get_num_rounds(schedule);

    if (team1 == team2) {
        std::cerr << "match_swap failed: same team selected.\n";
        return;
    }

    if (round_id < 0 || round_id >= num_rounds) {
        std::cerr << "match_swap failed: invalid round id.\n";
        return;
    }

    if (teams_play_together_in_round(schedule.rounds[round_id], team1, team2)) {
        std::cerr << "match_swap invalid: selected teams play each other in this round.\n";
        return;
    }

    std::set<std::pair<int, int>> affected_pairs;

    int a = get_opponent_in_round(schedule.rounds[round_id], team1);
    int b = get_opponent_in_round(schedule.rounds[round_id], team2);

    if (a == -1 || b == -1) {
        std::cerr << "match_swap failed: team missing from selected round.\n";
        return;
    }

    add_affected_pair(affected_pairs, team1, a);
    add_affected_pair(affected_pairs, team2, b);
    add_affected_pair(affected_pairs, team1, b);
    add_affected_pair(affected_pairs, team2, a);

    // Initial move:
    // team1-a and team2-b become team1-b and team2-a
    set_game_for_team_preserve_location(schedule.rounds[round_id], team1, b);
    set_game_for_team_preserve_location(schedule.rounds[round_id], team2, a);

    int current_duplicate = b;
    int previous_round = round_id;

    for (int chain_step = 0; chain_step < (num_rounds * num_rounds); chain_step++) {
        int repair_round = find_round_where_teams_play(
            schedule,
            team1,
            current_duplicate,
            previous_round
        );

        if (repair_round == -1) {
            std::cerr << "match_swap repair failed: could not find duplicate "
                      << team1 << "-" << current_duplicate << ".\n";
            return;
        }

        int next_opponent = get_opponent_in_round(schedule.rounds[repair_round], team2);

        if (next_opponent == -1) {
            std::cerr << "match_swap repair failed: team2 missing from repair round.\n";
            return;
        }

        add_affected_pair(affected_pairs, team1, current_duplicate);
        add_affected_pair(affected_pairs, team2, next_opponent);
        add_affected_pair(affected_pairs, team1, next_opponent);
        add_affected_pair(affected_pairs, team2, current_duplicate);

        // Repair step:
        // team1-current_duplicate and team2-next_opponent
        // become team1-next_opponent and team2-current_duplicate
        set_game_for_team_preserve_location(
            schedule.rounds[repair_round],
            team1,
            next_opponent
        );

        set_game_for_team_preserve_location(
            schedule.rounds[repair_round],
            team2,
            current_duplicate
        );

        // The chain closes when team1 gets back the opponent it lost in the start round.
        if (next_opponent == a) {
            repair_directions_for_affected_pairs(schedule, affected_pairs);
            return;
        }

        current_duplicate = next_opponent;
        previous_round = repair_round;
    }

    //std::cerr << "match_swap repair failed: chain too long.\n";
}

void match_swap_random(Schedule& schedule, std::mt19937& rng) {
    if (schedule.rounds.empty() || schedule.rounds[0].games.empty()) return;

    int num_rounds = get_num_rounds(schedule);
    int num_teams = get_num_teams(schedule);

    std::uniform_int_distribution<int> round_dist(0, num_rounds - 1);
    std::uniform_int_distribution<int> team_dist(0, num_teams - 1);

    for (int attempt = 0; attempt < 100; attempt++) {
        Schedule backup = schedule;

        int r = round_dist(rng);

        int team1 = team_dist(rng);
        int team2;

        do {
            team2 = team_dist(rng);
        } while (team2 == team1);

        if (teams_play_together_in_round(schedule.rounds[r], team1, team2)) {
            continue;
        }

        match_swap(schedule, r, team1, team2);

        ViolationCounts v = evaluate_schedule(schedule);

        // Accept only if double round robin was preserved.
        if (v.doubleRoundRobin == 0) {
            return;
        }

        schedule = backup;
    }

    std::cerr << "match_swap_random failed after 100 attempts.\n";
}

RoundEntry get_round_entry(const Round& round, int team) {
    for (const auto& game : round.games) {
        if (game.home == team) {
            return RoundEntry{game.away, true};
        }

        if (game.away == team) {
            return RoundEntry{game.home, false};
        }
    }

    return RoundEntry{-1, false};
}

void rebuild_round_from_entries(
    Round& round,
    const std::vector<RoundEntry>& entries,
    int preferred_team = -1
) {
    int n = static_cast<int>(entries.size());

    std::vector<bool> used(n, false);
    std::vector<Matchup> new_games;

    auto add_game_for_team = [&](int team) {
        if (team < 0 || team >= n || used[team]) return;

        int opponent = entries[team].opponent;

        if (opponent < 0 || opponent >= n || used[opponent]) return;

        bool team_home = entries[team].is_home;
        bool opponent_home = entries[opponent].is_home;

        int home;
        int away;

        if (team_home != opponent_home) {
            // Both teams agree about home/away.
            if (team_home) {
                home = team;
                away = opponent;
            } else {
                home = opponent;
                away = team;
            }
        } else {
            // Conflict. Preserve this team's location.
            if (team_home) {
                home = team;
                away = opponent;
            } else {
                home = opponent;
                away = team;
            }
        }

        new_games.push_back(Matchup{home, away});
        used[team] = true;
        used[opponent] = true;
    };

    // Preserve the chosen team's location first.
    if (preferred_team >= 0 && preferred_team < n) {
        add_game_for_team(preferred_team);
    }

    for (int team = 0; team < n; team++) {
        add_game_for_team(team);
    }

    round.games = new_games;
}

void match_round_swap(Schedule& schedule, int team, int r1, int r2) {
    int num_rounds = get_num_rounds(schedule);
    int num_teams = get_num_teams(schedule);

    if (r1 < 0 || r1 >= num_rounds || r2 < 0 || r2 >= num_rounds || r1 == r2) {
        std::cerr << "match_round_swap failed: invalid rounds.\n";
        return;
    }

    if (team < 0 || team >= num_teams) {
        std::cerr << "match_round_swap failed: invalid team.\n";
        return;
    }

    std::vector<int> opp_r1(num_teams, -1);
    std::vector<int> opp_r2(num_teams, -1);

    for (int t = 0; t < num_teams; t++) {
        opp_r1[t] = get_opponent_in_round(schedule.rounds[r1], t);
        opp_r2[t] = get_opponent_in_round(schedule.rounds[r2], t);

        if (opp_r1[t] == -1 || opp_r2[t] == -1) {
            std::cerr << "match_round_swap failed: team missing from round.\n";
            return;
        }
    }

    if (opp_r1[team] == opp_r2[team]) {
        std::cerr << "match_round_swap failed: same opponent in both rounds.\n";
        return;
    }

    // Build alternating repair cycle:
    // team --r1--> a --r2--> b --r1--> c --r2--> ... --> team
    std::vector<int> cycle;
    std::vector<bool> seen(num_teams, false);

    int current = team;
    bool use_r1 = true;

    for (int step = 0; step < num_teams + 2; step++) {
        if (current < 0 || current >= num_teams) {
            std::cerr << "match_round_swap failed: invalid chain team.\n";
            return;
        }

        if (seen[current]) {
            if (current == team) {
                break;
            }

            std::cerr << "match_round_swap failed: chain closed at wrong team.\n";
            return;
        }

        seen[current] = true;
        cycle.push_back(current);

        current = use_r1 ? opp_r1[current] : opp_r2[current];
        use_r1 = !use_r1;

        if (current == team) {
            break;
        }
    }

    if (current != team) {
        std::cerr << "match_round_swap failed: repair chain did not close.\n";
        return;
    }

    if (cycle.size() < 2 || cycle.size() % 2 != 0) {
        std::cerr << "match_round_swap failed: invalid repair cycle size.\n";
        return;
    }

    std::set<std::pair<int, int>> move_from_r1;
    std::set<std::pair<int, int>> move_from_r2;
    std::set<std::pair<int, int>> affected_pairs;

    for (int i = 0; i < static_cast<int>(cycle.size()); i++) {
        int a = cycle[i];
        int b = cycle[(i + 1) % cycle.size()];

        std::pair<int, int> key = pair_key(a, b);
        affected_pairs.insert(key);

        if (i % 2 == 0) {
            move_from_r1.insert(key);
        } else {
            move_from_r2.insert(key);
        }
    }

    std::vector<Matchup> new_r1_games;
    std::vector<Matchup> new_r2_games;

    // Keep unaffected games.
    for (const auto& game : schedule.rounds[r1].games) {
        if (move_from_r1.count(pair_key(game.home, game.away)) == 0) {
            new_r1_games.push_back(game);
        }
    }

    for (const auto& game : schedule.rounds[r2].games) {
        if (move_from_r2.count(pair_key(game.home, game.away)) == 0) {
            new_r2_games.push_back(game);
        }
    }

    // Move odd cycle edges from r2 into r1.
    for (const auto& key : move_from_r2) {
        Matchup game = find_matchup_between(schedule.rounds[r2], key.first, key.second);

        if (game.home == -1) {
            std::cerr << "match_round_swap failed: could not find moved r2 match.\n";
            return;
        }

        new_r1_games.push_back(game);
    }

    // Move even cycle edges from r1 into r2.
    for (const auto& key : move_from_r1) {
        Matchup game = find_matchup_between(schedule.rounds[r1], key.first, key.second);

        if (game.home == -1) {
            std::cerr << "match_round_swap failed: could not find moved r1 match.\n";
            return;
        }

        new_r2_games.push_back(game);
    }

    if (new_r1_games.size() != schedule.rounds[r1].games.size() ||
        new_r2_games.size() != schedule.rounds[r2].games.size()) {
        std::cerr << "match_round_swap failed: rebuilt round has wrong size.\n";
        return;
    }

    schedule.rounds[r1].games = new_r1_games;
    schedule.rounds[r2].games = new_r2_games;

    // Optional but recommended, same helper from match_swap.
    repair_directions_for_affected_pairs(schedule, affected_pairs);
}

void match_round_swap_random(Schedule& schedule, std::mt19937& rng) {
    if (schedule.rounds.empty() || schedule.rounds[0].games.empty()) return;

    int num_rounds = get_num_rounds(schedule);
    int num_teams = get_num_teams(schedule);

    std::uniform_int_distribution<int> team_dist(0, num_teams - 1);
    std::uniform_int_distribution<int> round_dist(0, num_rounds - 1);

    for (int attempt = 0; attempt < 100; attempt++) {
        Schedule backup = schedule;

        int team = team_dist(rng);

        int r1 = round_dist(rng);
        int r2;

        do {
            r2 = round_dist(rng);
        } while (r2 == r1);

        int opp1 = get_opponent_in_round(schedule.rounds[r1], team);
        int opp2 = get_opponent_in_round(schedule.rounds[r2], team);

        if (opp1 == -1 || opp2 == -1 || opp1 == opp2) {
            continue;
        }

        match_round_swap(schedule, team, r1, r2);

        ViolationCounts v = evaluate_schedule(schedule);

        if (v.doubleRoundRobin == 0) {
            return;
        }

        schedule = backup;
    }

    std::cerr << "match_round_swap_random failed after 100 attempts.\n";
}

std::string mutation_type_to_string(MutationType type) {
    switch (type) {
        case HOME_AWAY_SWAP_MUTATION:
            return "home_away_swap";
        case ROUND_SWAP_MUTATION:
            return "round_swap";
        case TEAM_SWAP_MUTATION:
            return "team_swap";
        case MATCH_SWAP_MUTATION:
            return "match_swap";
        case MATCH_ROUND_SWAP_MUTATION:
            return "match_round_swap";
        default:
            return "unknown";
    }
}
