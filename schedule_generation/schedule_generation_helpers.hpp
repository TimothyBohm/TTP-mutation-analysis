#ifndef SCHEDULE_GENERATION_HELPERS_HPP
#define SCHEDULE_GENERATION_HELPERS_HPP

#include <algorithm>
#include <random>
#include <vector>

#include "../mutation_analysis/schedule.hpp"

struct StreakState {
    int home_left;
    int away_left;
    int streak_length;
    bool last_was_home;
};

inline void print_partial_schedule(
    const Schedule& schedule,
    int depth,
    const std::string& action
) {
    std::cout << "\n[" << action << "] depth = " << depth << "\n";

    for (int r = 0; r < static_cast<int>(schedule.rounds.size()); ++r) {
        std::cout << "Round " << (r + 1) << ": ";

        if (schedule.rounds[r].games.empty()) {
            std::cout << "(empty)";
        }
        else {
            for (const Matchup& matchup : schedule.rounds[r].games) {
                std::cout << matchup.home << "," << matchup.away << " ";
            }
        }

        std::cout << "\n";
    }

    std::cout << "----------------------------------------\n";
}

inline bool same_matchup(const Matchup& a, const Matchup& b) {
    return a.home == b.home && a.away == b.away;
}

inline std::vector<Matchup> generate_matchups_after_normalization(
    int n,
    std::mt19937& rng
) {
    std::vector<Matchup> matchups;

    for (int home = 0; home < n; ++home) {
        for (int away = 0; away < n; ++away) {
            if (home == away) {
                continue;
            }

            bool is_fixed_first_round =
                home % 2 == 0 &&
                away == home + 1;

            if (is_fixed_first_round) {
                continue;
            }

            matchups.push_back({home, away});
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

inline Schedule create_normalized_empty_schedule(int n) {
    Schedule schedule;

    int number_of_rounds = 2 * (n - 1);

    schedule.rounds.resize(number_of_rounds);

    // Fill normalized first round: (0,1), (2,3), (4,5), ...
    for (int team = 0; team < n; team += 2) {
        schedule.rounds[0].games.push_back({team, team + 1});
    }

    return schedule;
}

inline std::vector<int> generate_round_insertion_order(
    int n,
    std::mt19937& rng
) {
    std::vector<int> insertion_order;

    int number_of_rounds = 2 * (n - 1);
    int games_per_round = n / 2;

    // Round 0 is normalized/fixed, so we only fill rounds 1 to number_of_rounds - 1
    for (int round_index = 1; round_index < number_of_rounds; ++round_index) {
        for (int game = 0; game < games_per_round; ++game) {
            insertion_order.push_back(round_index);
        }
    }

    std::shuffle(
        insertion_order.begin(),
        insertion_order.end(),
        rng
    );

    return insertion_order;
}

inline bool team_already_in_round(
    const Round& round,
    const Matchup& matchup
) {
    for (const Matchup& existing : round.games) {
        if (
            matchup.home == existing.home ||
            matchup.home == existing.away ||
            matchup.away == existing.home ||
            matchup.away == existing.away
        ) {
            return true;
        }
    }

    return false;
}

inline std::vector<Matchup> get_legal_moves_for_round(
    const Schedule& partial_schedule,
    const std::vector<Matchup>& remaining_matchups,
    int round_index,
    int n
) {
    std::vector<Matchup> legal_moves;

    const Round& round = partial_schedule.rounds[round_index];

    int games_per_round = n / 2;

    if (static_cast<int>(round.games.size()) >= games_per_round) {
        return legal_moves;
    }

    for (const Matchup& matchup : remaining_matchups) {
        if (!team_already_in_round(round, matchup)) {
            legal_moves.push_back(matchup);
        }
    }

    return legal_moves;
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
    std::vector<StreakState>& streaks,
    std::vector<Matchup>& scheduled_matchups
) {
    for (int team = 0; team < n; team += 2) {
        Matchup fixed_matchup{team, team + 1};

        scheduled_matchups.push_back(fixed_matchup);
        update_streaks(fixed_matchup, streaks);
    }
}

#endif