 #include "read_schedules.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>


ScheduleSet read_schedules_from_file(const std::string& filename) {

    ScheduleSet result;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return result;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;

        std::vector<Matchup> flat_matches;

        //Split by space to get individual matchups
        while (ss >> token) {
            std::stringstream match_stream(token);
            std::string home_str, away_str;

            if (std::getline(match_stream, home_str, ',') &&
                std::getline(match_stream, away_str, ',')) {

                Matchup m;
                m.home = std::stoi(home_str);
                m.away = std::stoi(away_str);

                flat_matches.push_back(m);
            }
        }

        //get number of teams
        int n = get_num_teams(flat_matches);
        int matches_per_round = n / 2;

        Schedule schedule;
        Round current_round;

        for (size_t i = 0; i < flat_matches.size(); i++) {
            current_round.games.push_back(flat_matches[i]);

            //When round is complete, push it
            if ((i + 1) % matches_per_round == 0) {
                schedule.rounds.push_back(current_round);
                current_round.games.clear();
            }
        }

        result.schedules.push_back(schedule);
    }

    file.close();

    std::cout << "Loaded " << result.schedules.size()
              << " schedules from " << filename << std::endl;

    return result;
}

void test_file_validity(ScheduleSet data) {
    if (data.schedules.empty()) {
        std::cerr << "No schedules loaded.\n";
    }
}