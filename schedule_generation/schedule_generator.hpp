#ifndef SCHEDULE_GENERATOR_HPP
#define SCHEDULE_GENERATOR_HPP

#include <algorithm>
#include <optional>
#include <random>
#include <string>
#include <vector>

struct Match {
    int home;
    int away;

    bool operator==(const Match& other) const {
        return home == other.home && away == other.away;
    }
};

using Schedule = std::vector<Match>;

struct StreakState {
    int home_left;
    int away_left;
    int streak_length;
    bool last_was_home;
};

enum class GenerationMethod {
    DFS,
    RandDFS,
    RandRestart
};

inline std::vector<Match> generate_matchups(int n, std::mt19937& rng) {
    std::vector<Match> matchups;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                matchups.push_back({i, j});
            }
        }
    }

    std::shuffle(matchups.begin(), matchups.end(), rng);
    return matchups;
}

inline std::vector<StreakState> generate_streak_count(int n) {
    std::vector<StreakState> streaks(n);

    for (int i = 0; i < n; ++i) {
        streaks[i] = {
            n - 1,  // home games left
            n - 1,  // away games left
            0,      // current streak length
            true    // same as Python's initial "home"
        };
    }

    return streaks;
}

inline bool prevent_back_to_back(
    const Match& m,
    const std::vector<Match>& previous_round
) {
    for (const Match& p : previous_round) {
        if (p.home == m.away && p.away == m.home) {
            return true;
        }
    }
    return false;
}

inline bool prevent_four_in_a_row(
    const Match& m,
    const std::vector<StreakState>& streaks
) {
    const StreakState& home_team = streaks[m.home];
    const StreakState& away_team = streaks[m.away];

    bool home_team_has_three_home =
        home_team.streak_length == 3 && home_team.last_was_home;

    bool away_team_has_three_away =
        away_team.streak_length == 3 && !away_team.last_was_home;

    return home_team_has_three_home || away_team_has_three_away;
}

inline bool check_future_streak_violation(
    const Match& m,
    const std::vector<StreakState>& streaks
) {
    const StreakState& home_team = streaks[m.home];

    int home_left_after = home_team.home_left - 1;
    int away_left_after = home_team.away_left;

    int x = std::max(home_left_after, away_left_after);
    int y = std::min(home_left_after, away_left_after);

    int s = 0;
    if (home_team.last_was_home && home_left_after > away_left_after) {
        s = home_team.streak_length;
    }

    if ((x + s) / 3.0 > y + 1) {
        return true;
    }

    const StreakState& away_team = streaks[m.away];

    home_left_after = away_team.home_left;
    away_left_after = away_team.away_left - 1;

    x = std::max(home_left_after, away_left_after);
    y = std::min(home_left_after, away_left_after);

    s = 0;
    if (!away_team.last_was_home && away_left_after > home_left_after) {
        s = away_team.streak_length;
    }

    if ((x + s) / 3.0 > y + 1) {
        return true;
    }

    return false;
}

inline void update_streaks(
    const Match& m,
    std::vector<StreakState>& streaks
) {
    StreakState& home_team = streaks[m.home];

    home_team.home_left--;

    if (home_team.last_was_home) {
        home_team.streak_length++;
    } else {
        home_team.streak_length = 1;
        home_team.last_was_home = true;
    }

    StreakState& away_team = streaks[m.away];

    away_team.away_left--;

    if (!away_team.last_was_home) {
        away_team.streak_length++;
    } else {
        away_team.streak_length = 1;
        away_team.last_was_home = false;
    }
}

inline bool check_repeat_in_current_round(
    const Match& m,
    const std::vector<Match>& current_round
) {
    for (const Match& p : current_round) {
        if (
            m.home == p.home ||
            m.home == p.away ||
            m.away == p.home ||
            m.away == p.away ||
            m.home < p.home
        ) {
            return true;
        }
    }

    return false;
}

inline bool check_constraints(
    const Schedule& schedule,
    const std::vector<StreakState>& streaks,
    int n,
    const Match& m
) {
    int matches_per_round = n / 2;
    int index = static_cast<int>(schedule.size()) % matches_per_round;

    std::vector<Match> current_round;
    if (index > 0) {
        current_round.insert(
            current_round.end(),
            schedule.end() - index,
            schedule.end()
        );
    }

    std::vector<Match> previous_round;
    if (static_cast<int>(schedule.size()) >= matches_per_round) {
        int end_index = static_cast<int>(schedule.size()) - index;
        int start_index = end_index - matches_per_round;

        if (index == 0) {
            start_index = static_cast<int>(schedule.size()) - matches_per_round;
            end_index = static_cast<int>(schedule.size());
        }

        previous_round.insert(
            previous_round.end(),
            schedule.begin() + start_index,
            schedule.begin() + end_index
        );
    }

    if (check_repeat_in_current_round(m, current_round)) {
        return true;
    }

    if (!previous_round.empty() && prevent_back_to_back(m, previous_round)) {
        return true;
    }

    if (prevent_four_in_a_row(m, streaks)) {
        return true;
    }

    if (check_future_streak_violation(m, streaks)) {
        return true;
    }

    return false;
}

inline std::vector<Match> remove_matchup(
    const std::vector<Match>& matchups,
    const Match& chosen
) {
    std::vector<Match> result;
    result.reserve(matchups.size() - 1);

    for (const Match& m : matchups) {
        if (!(m == chosen)) {
            result.push_back(m);
        }
    }

    return result;
}

inline std::vector<Match> get_legal_moves(
    const Schedule& schedule,
    const std::vector<Match>& remaining_matchups,
    const std::vector<StreakState>& streaks,
    int n
) {
    std::vector<Match> legal;

    for (const Match& m : remaining_matchups) {
        if (!check_constraints(schedule, streaks, n, m)) {
            legal.push_back(m);
        }
    }

    return legal;
}

inline bool dfs_one_schedule(
    int n,
    const std::vector<Match>& remaining_matchups,
    const std::vector<StreakState>& streaks,
    const Schedule& schedule,
    Schedule& result
) {
    if (remaining_matchups.empty()) {
        result = schedule;
        return true;
    }

    for (const Match& m : remaining_matchups) {
        if (check_constraints(schedule, streaks, n, m)) {
            continue;
        }

        Schedule new_schedule = schedule;
        new_schedule.push_back(m);

        std::vector<Match> new_remaining =
            remove_matchup(remaining_matchups, m);

        std::vector<StreakState> new_streaks = streaks;
        update_streaks(m, new_streaks);

        if (dfs_one_schedule(
                n,
                new_remaining,
                new_streaks,
                new_schedule,
                result
            )) {
            return true;
        }
    }

    return false;
}
