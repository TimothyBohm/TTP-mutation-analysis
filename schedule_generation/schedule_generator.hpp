#ifndef SCHEDULE_GENERATOR_HPP
#define SCHEDULE_GENERATOR_HPP

#include <algorithm>
#include <random>
#include <vector>

#include "../mutation_analysis/schedule.hpp"
#include "schedule_generation_helpers.hpp"

//to compile: g++ -std=c++17 main.cpp ../mutation_analysis/schedule.cpp -o run -O3 -Wall -Wextra

enum class GenerationMethod {
    DFS,
    RandDFS,
    RowsFirst,
    RoundRandDFS
};

struct GenerationStats {
    long long attempts = 0;
    long long successful_attempts = 0;
    double total_time_seconds = 0.0;
};

inline bool dfs_one_schedule(
    int n,
    const std::vector<Matchup>& remaining_matchups,
    const std::vector<StreakState>& streaks,
    const std::vector<Matchup>& scheduled_matchups,
    Schedule& result
) {
    if (remaining_matchups.empty()) {
        Schedule completed = matchups_to_schedule(scheduled_matchups, n);
        ViolationCounts violations = evaluate_schedule(completed);

        if (is_feasible(violations)) {
            result = completed;
            return true;
        }

        return false;
    }

    for (const Matchup& matchup : remaining_matchups) {
        if (check_constraints(scheduled_matchups, streaks, n, matchup)) {
            continue;
        }

        std::vector<Matchup> new_scheduled_matchups = scheduled_matchups;
        new_scheduled_matchups.push_back(matchup);

        std::vector<Matchup> new_remaining_matchups =
            remove_matchup(remaining_matchups, matchup);

        std::vector<StreakState> new_streaks = streaks;
        update_streaks(matchup, new_streaks);

        if (dfs_one_schedule(
                n,
                new_remaining_matchups,
                new_streaks,
                new_scheduled_matchups,
                result
            )) {
            return true;
        }
    }

    return false;
}

inline bool rand_dfs_one_schedule(
    int n,
    const std::vector<Matchup>& remaining_matchups,
    const std::vector<StreakState>& streaks,
    const std::vector<Matchup>& scheduled_matchups,
    Schedule& result,
    std::mt19937& rng
) {
    if (remaining_matchups.empty()) {
        Schedule completed = matchups_to_schedule(scheduled_matchups, n);
        ViolationCounts violations = evaluate_schedule(completed);

        if (is_feasible(violations)) {
            result = completed;
            return true;
        }

        return false;
    }

    std::vector<Matchup> legal_moves =
        get_legal_moves(scheduled_matchups, remaining_matchups, streaks, n);

    std::shuffle(legal_moves.begin(), legal_moves.end(), rng);

    for (const Matchup& matchup : legal_moves) {
        std::vector<Matchup> new_scheduled_matchups = scheduled_matchups;
        new_scheduled_matchups.push_back(matchup);

        std::vector<Matchup> new_remaining_matchups =
            remove_matchup(remaining_matchups, matchup);

        std::vector<StreakState> new_streaks = streaks;
        update_streaks(matchup, new_streaks);

        if (rand_dfs_one_schedule(
                n,
                new_remaining_matchups,
                new_streaks,
                new_scheduled_matchups,
                result,
                rng
            )) {
            return true;
        }
    }

    return false;
}

inline Round rows_first_one_round(
    int n,
    std::mt19937& rng
) {
    std::vector<int> teams;

    for (int team = 0; team < n; ++team) {
        teams.push_back(team);
    }

    std::shuffle(teams.begin(), teams.end(), rng);

    Round round;

    std::uniform_int_distribution<int> home_away_choice(0, 1);

    for (int i = 0; i < n; i += 2) {
        int team_a = teams[i];
        int team_b = teams[i + 1];

        if (home_away_choice(rng) == 0) {
            round.games.push_back({team_a, team_b});
        }
        else {
            round.games.push_back({team_b, team_a});
        }
    }

    std::sort(
        round.games.begin(),
        round.games.end(),
        [](const Matchup& a, const Matchup& b) {
            return a.home < b.home;
        }
    );

    return round;
}

inline Schedule rows_first_attempt_schedule(
    int n,
    std::mt19937& rng
) {
    Schedule schedule;

    // Fixed normalized first round: (0,1), (2,3), (4,5), ...
    Round first_round;

    for (int team = 0; team < n; team += 2) {
        first_round.games.push_back({team, team + 1});
    }

    schedule.rounds.push_back(first_round);

    // Generate the remaining rounds randomly using rows-first generation
    int number_of_rounds = 2 * (n - 1);

    for (int round_index = 1; round_index < number_of_rounds; ++round_index) {
        Round round = rows_first_one_round(n, rng);
        schedule.rounds.push_back(round);
    }

    return schedule;
}

inline Schedule rows_first_one_schedule(
    int n,
    std::mt19937& rng,
    GenerationStats& stats
) {
    while (true) {
    stats.attempts++;

    Schedule schedule = rows_first_attempt_schedule(n, rng);
    ViolationCounts violations = evaluate_schedule(schedule);

    if (is_feasible(violations)) {
        stats.successful_attempts++;
        return schedule;
    }
}
}

inline bool round_rand_dfs_one_schedule(
    int n,
    const std::vector<int>& insertion_order,
    int depth,
    const std::vector<Matchup>& remaining_matchups,
    const Schedule& partial_schedule,
    Schedule& result,
    std::mt19937& rng
) {
    if (depth == static_cast<int>(insertion_order.size())) {
        ViolationCounts violations = evaluate_schedule(partial_schedule);

        if (is_feasible(violations)) {
            result = partial_schedule;
            return true;
        }

        return false;
    }

    int round_index = insertion_order[depth];

    std::vector<Matchup> legal_moves =
        get_legal_moves_for_round(
            partial_schedule,
            remaining_matchups,
            round_index,
            n
        );

    std::shuffle(
        legal_moves.begin(),
        legal_moves.end(),
        rng
    );

    for (const Matchup& matchup : legal_moves) {
        Schedule new_partial_schedule = partial_schedule;
        new_partial_schedule.rounds[round_index].games.push_back(matchup);

        std::sort(
            new_partial_schedule.rounds[round_index].games.begin(),
            new_partial_schedule.rounds[round_index].games.end(),
            [](const Matchup& a, const Matchup& b) {
                return a.home < b.home;
            }
        );

        print_partial_schedule(
            new_partial_schedule,
            depth,
            "INSERT matchup " + std::to_string(matchup.home) + "," +
            std::to_string(matchup.away) + " into round " +
            std::to_string(round_index + 1)
        );

        std::vector<Matchup> new_remaining_matchups =
            remove_matchup(remaining_matchups, matchup);

        if (round_rand_dfs_one_schedule(
                n,
                insertion_order,
                depth + 1,
                new_remaining_matchups,
                new_partial_schedule,
                result,
                rng
            )) {
            return true;
        }
        print_partial_schedule(
            partial_schedule,
            depth,
            "BACKTRACK from matchup " + std::to_string(matchup.home) + "," +
            std::to_string(matchup.away) + " in round " +
            std::to_string(round_index + 1)
        );
    }

    return false;
}

inline Schedule generate_one_schedule_round_rand_dfs(
    int n,
    std::mt19937& rng
) {
    Schedule partial_schedule =
        create_normalized_empty_schedule(n);

    std::vector<int> insertion_order =
        generate_round_insertion_order(n, rng);

    std::vector<Matchup> remaining_matchups =
        generate_matchups_after_normalization(n, rng);

    Schedule result;

    bool success = round_rand_dfs_one_schedule(
        n,
        insertion_order,
        0,
        remaining_matchups,
        partial_schedule,
        result,
        rng
    );

    if (!success) {
        throw std::runtime_error(
            "Round-randomized DFS failed to generate a schedule"
        );
    }

    return result;
}

inline Schedule generate_one_schedule(
    int n,
    GenerationMethod method,
    std::mt19937& rng,
    GenerationStats& stats
) {
    if (method == GenerationMethod::DFS) {
        std::vector<StreakState> streaks = generate_streak_count(n);
        std::vector<Matchup> scheduled_matchups;

        apply_normalization(n, streaks, scheduled_matchups);

        std::vector<Matchup> matchups = generate_matchups_after_normalization(n, rng);
        Schedule result;

        bool success = dfs_one_schedule(n, matchups, streaks, scheduled_matchups, result);

        if (!success) {
            throw std::runtime_error(
                "DFS failed to generate a schedule"
            );
        }

        return result;
    }

    if (method == GenerationMethod::RandDFS) {
        std::vector<StreakState> streaks = generate_streak_count(n);
        std::vector<Matchup> scheduled_matchups;

        apply_normalization(n, streaks, scheduled_matchups);

        std::vector<Matchup> matchups = generate_matchups_after_normalization(n, rng);
        Schedule result;

        bool success = rand_dfs_one_schedule(
            n,
            matchups,
            streaks,
            scheduled_matchups,
            result,
            rng
        );

        if (!success) {
            throw std::runtime_error(
                "Randomized DFS failed to generate a schedule"
            );
        }
        return result;
    }

    if (method == GenerationMethod::RoundRandDFS) {
        return generate_one_schedule_round_rand_dfs(n, rng);
    }

    if (method == GenerationMethod::RowsFirst) {
    return rows_first_one_schedule(
        n,
        rng,
        stats
    );
}

    throw std::runtime_error("Unknown generation method");
}

#endif
