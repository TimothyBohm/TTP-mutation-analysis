#include "experiments.hpp"
#include "mutations.hpp"
#include "schedule.hpp"
#include "read_schedules.hpp"
#include <fstream>
#include <iostream>
#include <limits>

void run_nested_neighborhood_experiment(const ScheduleSet& data, const std::string& output_file, int max_schedules) {
    std::ofstream out(output_file);
    int feasible_counter = 0;
    int total_counter = 0;
    test_file_validity(data);

    if (!out.is_open()) {
        std::cerr << "Could not open output file!!" << output_file << std::endl;
        return;
    }

    //CSV header
    out << "schedule_id,round1,round2,teamA,teamB,noRepeat,maxStreak,doubleRoundRobin,total,feasible\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];

        int num_rounds = get_num_rounds(original);
        int num_teams = get_num_teams(original);

        //first level: every round-swap neighbor
        for (int r1 = 0; r1 < num_rounds; r1++) {
            for (int r2 = r1 + 1; r2 < num_rounds; r2++) {

                //create one round-swap neighbor
                Schedule round_neighbor = original;
                round_swap(round_neighbor, r1, r2);

                //second level: all home/away neighbors of this round
                for (int teamA = 0; teamA < num_teams; teamA++) {
                    for (int teamB = teamA + 1; teamB < num_teams; teamB++) {

                        //create a second-level neighbor
                        Schedule mutated = round_neighbor;
                        home_away_swap(mutated, teamA, teamB);

                        //evaluate
                        ViolationCounts v = evaluate_schedule(mutated);

                        //update counters
                        total_counter++;
                        if (is_feasible(v)) {
                            feasible_counter++;
                        }

                        //write one CSV row
                        out << schedule_id << ","
                            << r1 << ","
                            << r2 << ","
                            << teamA << ","
                            << teamB << ","
                            << v.noRepeat << ","
                            << v.maxStreak << ","
                            << v.doubleRoundRobin << ","
                            << v.total << ","
                            << (is_feasible(v) ? 1 : 0)
                            << "\n";

                        //mutated schedule goes out of scope and gets overwritten next iteration
                    }
                }

                //round_neighbor also gets overwritten next iteration
            }
        }

        if ((schedule_id + 1) % 10 == 0) {
            std::cout << "Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    std::cout << "Feasible schedules found: " << feasible_counter << " out of " << total_counter << "\n";

    out.close();
    std::cout << "Experiment finished. Results written to " << output_file << std::endl;
}

void run_nested_neighborhood_experiment_aggregate(const ScheduleSet& data, const std::string& output_file, int max_schedules) 
{
    test_file_validity(data);
    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Could not open output file!!" << output_file << std::endl;
        return;
    }

    out << "schedule_id,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,"
           "min_total,max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];

        int num_rounds = get_num_rounds(original);
        int num_teams = get_num_teams(original);

        long long total_neighbors = 0;
        long long feasible_neighbors = 0;

        long long sum_noRepeat = 0;
        long long sum_maxStreak = 0;
        long long sum_doubleRoundRobin = 0;
        long long sum_total = 0;

        int min_total = std::numeric_limits<int>::max();
        int max_total = std::numeric_limits<int>::min();

        for (int r1 = 0; r1 < num_rounds; r1++) {
            for (int r2 = r1 + 1; r2 < num_rounds; r2++) {
                Schedule round_neighbor = original;
                round_swap(round_neighbor, r1, r2);

                for (int teamA = 0; teamA < num_teams; teamA++) {
                    for (int teamB = teamA + 1; teamB < num_teams; teamB++) {
                        Schedule mutated = round_neighbor;
                        home_away_swap(mutated, teamA, teamB);

                        //evaluate the mutated schedule
                        ViolationCounts v = evaluate_schedule(mutated);

                        total_neighbors++;
                        if (is_feasible(v)) {
                            feasible_neighbors++;
                        }

                        //update violations
                        sum_noRepeat += v.noRepeat;
                        sum_maxStreak += v.maxStreak;
                        sum_doubleRoundRobin += v.doubleRoundRobin;
                        sum_total += v.total;

                        if (v.total < min_total) min_total = v.total;
                        if (v.total > max_total) max_total = v.total;
                    }
                }
            }
        }

        double feasible_ratio = (total_neighbors > 0)
            ? static_cast<double>(feasible_neighbors) / static_cast<double>(total_neighbors)
            : 0.0;

        double avg_noRepeat = (total_neighbors > 0)
            ? static_cast<double>(sum_noRepeat) / static_cast<double>(total_neighbors)
            : 0.0;

        double avg_maxStreak = (total_neighbors > 0)
            ? static_cast<double>(sum_maxStreak) / static_cast<double>(total_neighbors)
            : 0.0;

        double avg_doubleRoundRobin = (total_neighbors > 0)
            ? static_cast<double>(sum_doubleRoundRobin) / static_cast<double>(total_neighbors)
            : 0.0;

        double avg_total = (total_neighbors > 0)
            ? static_cast<double>(sum_total) / static_cast<double>(total_neighbors)
            : 0.0;

        if (total_neighbors == 0) {
            min_total = 0;
            max_total = 0;
        }

        //write a line to CSV
        out << schedule_id << ","
            << total_neighbors << ","
            << feasible_neighbors << ","
            << feasible_ratio << ","
            << avg_noRepeat << ","
            << avg_maxStreak << ","
            << avg_doubleRoundRobin << ","
            << avg_total << ","
            << min_total << ","
            << max_total << "\n";

        if ((schedule_id + 1) % 10 == 0) {
            std::cout << "Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "Aggregate experiment finished. Results written to " << output_file << std::endl;
}


void run_global_summary_by_team_size(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules,
    bool append
) {
    test_file_validity(data);

    std::ofstream out;
    if (append) {
        out.open(output_file, std::ios::app);
    } else {
        out.open(output_file);
    }

    if (!out.is_open()) {
        std::cerr << "Could not open output file!!" << output_file << std::endl;
        return;
    }

    if (!append) {
        out << "team_size,num_original_schedules,total_neighbors,feasible_neighbors,feasible_ratio,"
               "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";
    }

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    int team_size = get_num_teams(data.schedules[0]);

    long long total_neighbors = 0;
    long long feasible_neighbors = 0;

    long long sum_noRepeat = 0;
    long long sum_maxStreak = 0;
    long long sum_doubleRoundRobin = 0;
    long long sum_total = 0;

    int min_total = std::numeric_limits<int>::max();
    int max_total = std::numeric_limits<int>::min();

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];

        int num_rounds = get_num_rounds(original);
        int num_teams = get_num_teams(original);

        for (int r1 = 0; r1 < num_rounds; r1++) {
            for (int r2 = r1 + 1; r2 < num_rounds; r2++) {
                Schedule round_neighbor = original;
                round_swap(round_neighbor, r1, r2);

                for (int teamA = 0; teamA < num_teams; teamA++) {
                    for (int teamB = teamA + 1; teamB < num_teams; teamB++) {
                        Schedule mutated = round_neighbor;
                        home_away_swap(mutated, teamA, teamB);

                        ViolationCounts v = evaluate_schedule(mutated);

                        total_neighbors++;
                        if (is_feasible(v)) {
                            feasible_neighbors++;
                        }

                        sum_noRepeat += v.noRepeat;
                        sum_maxStreak += v.maxStreak;
                        sum_doubleRoundRobin += v.doubleRoundRobin;
                        sum_total += v.total;

                        if (v.total < min_total) min_total = v.total;
                        if (v.total > max_total) max_total = v.total;
                    }
                }
            }
        }

        if ((schedule_id + 1) % 10 == 0) {
            std::cout << "Processed " << (schedule_id + 1)
                      << " schedules for team size " << team_size << "\n";
        }
    }

    double feasible_ratio = (total_neighbors > 0)
        ? static_cast<double>(feasible_neighbors) / static_cast<double>(total_neighbors)
        : 0.0;

    double avg_noRepeat = (total_neighbors > 0)
        ? static_cast<double>(sum_noRepeat) / static_cast<double>(total_neighbors)
        : 0.0;

    double avg_maxStreak = (total_neighbors > 0)
        ? static_cast<double>(sum_maxStreak) / static_cast<double>(total_neighbors)
        : 0.0;

    double avg_doubleRoundRobin = (total_neighbors > 0)
        ? static_cast<double>(sum_doubleRoundRobin) / static_cast<double>(total_neighbors)
        : 0.0;

    double avg_total = (total_neighbors > 0)
        ? static_cast<double>(sum_total) / static_cast<double>(total_neighbors)
        : 0.0;

    if (total_neighbors == 0) {
        min_total = 0;
        max_total = 0;
    }

    out << team_size << ","
        << limit << ","
        << total_neighbors << ","
        << feasible_neighbors << ","
        << feasible_ratio << ","
        << avg_noRepeat << ","
        << avg_maxStreak << ","
        << avg_doubleRoundRobin << ","
        << avg_total << ","
        << min_total << ","
        << max_total << "\n";

    out.close();

    std::cout << "Global summary written for team size " << team_size
              << " to " << output_file << std::endl;
}

void run_global_summary_driver(
    const std::string& data_folder,
    const std::string& output_file,
    int max_schedules
) {
    std::vector<int> team_sizes = {4, 6, 8, 10, 12, 14, 16};

    bool first = true;

    for (int n : team_sizes) {
        std::string file_path = data_folder + "/ALL-" + std::to_string(n) + ".csv";

        std::cout << "Processing team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(file_path);

        test_file_validity(data);

        run_global_summary_by_team_size(
            data,
            output_file,
            max_schedules,
            !first
        );

        first = false;
    }

    std::cout << "All team sizes processed.\n";
}