#ifndef SCHEDULE_GENERATOR_HPP
#define SCHEDULE_GENERATOR_HPP

#include <algorithm>
#include <optional>
#include <random>
#include <vector>

#include "../mutation_analysis/schedule.hpp"

//to compile: g++ -std=c++17 main.cpp ../mutation_analysis/schedule.cpp -o run -O3 -Wall -Wextra

enum class GenerationMethod {
    DFS,
    RandDFS
};

struct StreakState {
    int home_left;
    int away_left;
    int streak_length;
    bool last_was_home;
};

inline bool same_matchup(const Matchup& a, const Matchup& b) {
    return a.home == b.home && a.away == b.away;
}

inline std::vector<Matchup> generate_matchups(int n, std::mt19937& rng) {
    std::vector<Matchup> matchups;

    for (int home = 0; home < n; ++home) {
        for (int away = 0; away < n; ++away) {
            if (home != away) {
                matchups.push_back({home, away});
            }
        }
    }

    std::shuffle(matchups.begin(), matchups.end(), rng);
    return matchups;
}

inline std::vector<StreakState> generate_streak_count(int n) {
    std::vector<StreakState> streaks(n);

    for (int team = 0; team < n; ++team) {
        streaks[team] = {n - 1, n - 1, 0, true};
    }

    return streaks;
}

inline Schedule matchups_to_schedule(const std::vector<Matchup>& scheduled_matchups, int n) {
    Schedule schedule;
    int games_per_round = n / 2;

    for (int i = 0; i < static_cast<int>(scheduled_matchups.size()); i += games_per_round) {
        Round round;

        for (int j = 0; j < games_per_round; ++j) {
            round.games.push_back(scheduled_matchups[i + j]);
        }

        schedule.rounds.push_back(round);
    }

    return schedule;
}

inline bool prevent_back_to_back(
    const Matchup& matchup,
    const std::vector<Matchup>& previous_round
) {
    for (const Matchup& previous : previous_round) {
        if (previous.home == matchup.away && previous.away == matchup.home) {
            return true;
        }
    }

    return false;
}

inline bool prevent_four_in_a_row(
    const Matchup& matchup,
    const std::vector<StreakState>& streaks
) {
    const StreakState& home_team = streaks[matchup.home];
    const StreakState& away_team = streaks[matchup.away];

    bool home_team_has_three_home =
        home_team.streak_length == MAX_STREAK && home_team.last_was_home;

    bool away_team_has_three_away =
        away_team.streak_length == MAX_STREAK && !away_team.last_was_home;

    return home_team_has_three_home || away_team_has_three_away;
}

inline bool check_future_streak_violation(
    const Matchup& matchup,
    const std::vector<StreakState>& streaks
) {
    const StreakState& home_team = streaks[matchup.home];

    int home_left_after = home_team.home_left - 1;
    int away_left_after = home_team.away_left;

    int x = std::max(home_left_after, away_left_after);
    int y = std::min(home_left_after, away_left_after);

    int s = 0;
    if (home_team.last_was_home && home_left_after > away_left_after) {
        s = home_team.streak_length;
    }

    if ((x + s) / static_cast<double>(MAX_STREAK) > y + 1) {
        return true;
    }

    const StreakState& away_team = streaks[matchup.away];

    home_left_after = away_team.home_left;
    away_left_after = away_team.away_left - 1;

    x = std::max(home_left_after, away_left_after);
    y = std::min(home_left_after, away_left_after);

    s = 0;
    if (!away_team.last_was_home && away_left_after > home_left_after) {
        s = away_team.streak_length;
    }

    if ((x + s) / static_cast<double>(MAX_STREAK) > y + 1) {
        return true;
    }

    return false;
}

inline void update_streaks(
    const Matchup& matchup,
    std::vector<StreakState>& streaks
) {
    StreakState& home_team = streaks[matchup.home];

    home_team.home_left--;

    if (home_team.last_was_home) {
        home_team.streak_length++;
    } else {
        home_team.streak_length = 1;
        home_team.last_was_home = true;
    }

    StreakState& away_team = streaks[matchup.away];

    away_team.away_left--;

    if (!away_team.last_was_home) {
        away_team.streak_length++;
    } else {
        away_team.streak_length = 1;
        away_team.last_was_home = false;
    }
}

inline bool check_repeat_in_current_round(
    const Matchup& matchup,
    const std::vector<Matchup>& current_round
) {
    for (const Matchup& previous : current_round) {
        bool team_already_used =
            matchup.home == previous.home ||
            matchup.home == previous.away ||
            matchup.away == previous.home ||
            matchup.away == previous.away;

        bool breaks_round_order = matchup.home < previous.home;

        if (team_already_used || breaks_round_order) {
            return true;
        }
    }

    return false;
}

inline std::vector<Matchup> get_current_round(
    const std::vector<Matchup>& scheduled_matchups,
    int n
) {
    int games_per_round = n / 2;
    int index = static_cast<int>(scheduled_matchups.size()) % games_per_round;

    if (index == 0) {
        return {};
    }

    return std::vector<Matchup>(
        scheduled_matchups.end() - index,
        scheduled_matchups.end()
    );
}

inline std::vector<Matchup> get_previous_round(
    const std::vector<Matchup>& scheduled_matchups,
    int n
) {
    int games_per_round = n / 2;

    if (static_cast<int>(scheduled_matchups.size()) < games_per_round) {
        return {};
    }

    int index = static_cast<int>(scheduled_matchups.size()) % games_per_round;

    int end_index;
    if (index == 0) {
        end_index = static_cast<int>(scheduled_matchups.size());
    } else {
        end_index = static_cast<int>(scheduled_matchups.size()) - index;
    }

    int start_index = end_index - games_per_round;

    return std::vector<Matchup>(
        scheduled_matchups.begin() + start_index,
        scheduled_matchups.begin() + end_index
    );
}

inline bool check_constraints(
    const std::vector<Matchup>& scheduled_matchups,
    const std::vector<StreakState>& streaks,
    int n,
    const Matchup& matchup
) {
    std::vector<Matchup> current_round = get_current_round(scheduled_matchups, n);
    std::vector<Matchup> previous_round = get_previous_round(scheduled_matchups, n);

    if (check_repeat_in_current_round(matchup, current_round)) {
        return true;
    }

    if (!previous_round.empty() && prevent_back_to_back(matchup, previous_round)) {
        return true;
    }

    if (prevent_four_in_a_row(matchup, streaks)) {
        return true;
    }

    if (check_future_streak_violation(matchup, streaks)) {
        return true;
    }

    return false;
}

inline std::vector<Matchup> remove_matchup(
    const std::vector<Matchup>& matchups,
    const Matchup& chosen
) {
    std::vector<Matchup> result;
    result.reserve(matchups.size() - 1);

    for (const Matchup& matchup : matchups) {
        if (!same_matchup(matchup, chosen)) {
            result.push_back(matchup);
        }
    }

    return result;
}

inline std::vector<Matchup> get_legal_moves(
    const std::vector<Matchup>& scheduled_matchups,
    const std::vector<Matchup>& remaining_matchups,
    const std::vector<StreakState>& streaks,
    int n
) {
    std::vector<Matchup> legal_moves;

    for (const Matchup& matchup : remaining_matchups) {
        if (!check_constraints(scheduled_matchups, streaks, n, matchup)) {
            legal_moves.push_back(matchup);
        }
    }

    return legal_moves;
}

inline void apply_normalization(
    int n,
    std::vector<Matchup>& remaining_matchups,
    std::vector<StreakState>& streaks,
    std::vector<Matchup>& scheduled_matchups
) {
    for (int team = 0; team < n; team += 2) {
        Matchup fixed_matchup{team, team + 1};

        scheduled_matchups.push_back(fixed_matchup);

        remaining_matchups = remove_matchup(remaining_matchups, fixed_matchup);

        update_streaks(fixed_matchup, streaks);
    }
}

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

inline Schedule generate_one_schedule(
    int n,
    GenerationMethod method,
    std::mt19937& rng
) {
    if (method == GenerationMethod::DFS) {
        std::vector<Matchup> matchups = generate_matchups(n, rng);
        std::vector<StreakState> streaks = generate_streak_count(n);
        std::vector<Matchup> scheduled_matchups;


        apply_normalization(n, matchups, streaks, scheduled_matchups);
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
        std::vector<Matchup> matchups = generate_matchups(n, rng);
        std::vector<StreakState> streaks = generate_streak_count(n);
        std::vector<Matchup> scheduled_matchups;

        apply_normalization(n, matchups, streaks, scheduled_matchups);
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

    throw std::runtime_error("Unknown generation method");
}

#endif
