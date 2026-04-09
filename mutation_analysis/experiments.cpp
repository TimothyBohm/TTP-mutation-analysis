#include "experiments.hpp"
#include "mutations.hpp"
#include "schedule.hpp"
#include "read_schedules.hpp"
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <string>

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
    int max_schedules,
    std::vector<int> team_sizes
) {
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

void run_one_step_home_away_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules
) {
    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    out << "schedule_id,team_size,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];
        int num_teams = get_num_teams(original);

        long long total_neighbors = 0;
        long long feasible_neighbors = 0;

        long long sum_noRepeat = 0;
        long long sum_maxStreak = 0;
        long long sum_doubleRoundRobin = 0;
        long long sum_total = 0;

        int min_total = std::numeric_limits<int>::max();
        int max_total = std::numeric_limits<int>::min();

        for (int teamA = 0; teamA < num_teams; teamA++) {
            for (int teamB = teamA + 1; teamB < num_teams; teamB++) {
                Schedule mutated = original;
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

        out << schedule_id << ","
            << num_teams << ","
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
            std::cout << "[Home/Away] Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "One-step home/away experiment finished. Results written to "
              << output_file << std::endl;
}

void run_one_step_round_swap_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules
) {
    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    out << "schedule_id,team_size,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];
        int num_teams = get_num_teams(original);
        int num_rounds = get_num_rounds(original);

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
                Schedule mutated = original;
                round_swap(mutated, r1, r2);

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

        out << schedule_id << ","
            << num_teams << ","
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
            std::cout << "[Round Swap] Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "One-step round-swap experiment finished. Results written to "
              << output_file << std::endl;
}

void run_one_step_experiments_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    int max_schedules,
    const std::vector<int>& team_sizes
) {
    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";

        std::string home_away_output =
            output_folder + "/results_one_step_home_away_" + std::to_string(n) + ".csv";

        std::string round_swap_output =
            output_folder + "/results_one_step_round_swap_" + std::to_string(n) + ".csv";

        std::cout << "Processing team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);

        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_one_step_home_away_experiment(data, home_away_output, max_schedules);
        run_one_step_round_swap_experiment(data, round_swap_output, max_schedules);

        std::cout << "Finished team size " << n << ".\n";
    }

    std::cout << "All selected team sizes processed.\n";
}

void run_two_step_home_away_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules
) {
    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    out << "schedule_id,team_size,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];
        int num_teams = get_num_teams(original);

        long long total_neighbors = 0;
        long long feasible_neighbors = 0;

        long long sum_noRepeat = 0;
        long long sum_maxStreak = 0;
        long long sum_doubleRoundRobin = 0;
        long long sum_total = 0;

        int min_total = std::numeric_limits<int>::max();
        int max_total = std::numeric_limits<int>::min();

        // First home/away step
        for (int teamA1 = 0; teamA1 < num_teams; teamA1++) {
            for (int teamB1 = teamA1 + 1; teamB1 < num_teams; teamB1++) {
                Schedule first_neighbor = original;
                home_away_swap(first_neighbor, teamA1, teamB1);

                // Second home/away step
                for (int teamA2 = 0; teamA2 < num_teams; teamA2++) {
                    for (int teamB2 = teamA2 + 1; teamB2 < num_teams; teamB2++) {
                        Schedule mutated = first_neighbor;
                        home_away_swap(mutated, teamA2, teamB2);

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

        out << schedule_id << ","
            << num_teams << ","
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
            std::cout << "[2-step Home/Away] Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "Two-step home/away experiment finished. Results written to "
              << output_file << std::endl;
}

void run_two_step_round_swap_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules
) {
    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    out << "schedule_id,team_size,total_neighbors,feasible_neighbors,feasible_ratio,"
           "avg_noRepeat,avg_maxStreak,avg_doubleRoundRobin,avg_total,min_total,max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        const Schedule& original = data.schedules[schedule_id];
        int num_teams = get_num_teams(original);
        int num_rounds = get_num_rounds(original);

        long long total_neighbors = 0;
        long long feasible_neighbors = 0;

        long long sum_noRepeat = 0;
        long long sum_maxStreak = 0;
        long long sum_doubleRoundRobin = 0;
        long long sum_total = 0;

        int min_total = std::numeric_limits<int>::max();
        int max_total = std::numeric_limits<int>::min();

        // First round-swap step
        for (int r1a = 0; r1a < num_rounds; r1a++) {
            for (int r1b = r1a + 1; r1b < num_rounds; r1b++) {
                Schedule first_neighbor = original;
                round_swap(first_neighbor, r1a, r1b);

                // Second round-swap step
                for (int r2a = 0; r2a < num_rounds; r2a++) {
                    for (int r2b = r2a + 1; r2b < num_rounds; r2b++) {
                        Schedule mutated = first_neighbor;
                        round_swap(mutated, r2a, r2b);

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

        out << schedule_id << ","
            << num_teams << ","
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
            std::cout << "[2-step Round Swap] Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "Two-step round-swap experiment finished. Results written to "
              << output_file << std::endl;
}

void run_two_step_experiments_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules
) {
    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";

        std::string home_away_output =
            output_folder + "/results_two_step_home_away_" + std::to_string(n) + ".csv";

        std::string round_swap_output =
            output_folder + "/results_two_step_round_swap_" + std::to_string(n) + ".csv";

        std::cout << "Processing 2-step experiments for team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);

        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_two_step_home_away_experiment(data, home_away_output, max_schedules);
        run_two_step_round_swap_experiment(data, round_swap_output, max_schedules);

        std::cout << "Finished 2-step experiments for team size " << n << ".\n";
    }

    std::cout << "All selected 2-step experiments completed.\n";
}

void run_global_summary_one_step_home_away(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules,
    bool append
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for 1-step home/away global summary.\n";
        return;
    }

    std::ofstream out;
    if (append) out.open(output_file, std::ios::app);
    else out.open(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
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
        int num_teams = get_num_teams(original);

        for (int teamA = 0; teamA < num_teams; teamA++) {
            for (int teamB = teamA + 1; teamB < num_teams; teamB++) {
                Schedule mutated = original;
                home_away_swap(mutated, teamA, teamB);

                ViolationCounts v = evaluate_schedule(mutated);

                total_neighbors++;
                if (is_feasible(v)) feasible_neighbors++;

                sum_noRepeat += v.noRepeat;
                sum_maxStreak += v.maxStreak;
                sum_doubleRoundRobin += v.doubleRoundRobin;
                sum_total += v.total;

                if (v.total < min_total) min_total = v.total;
                if (v.total > max_total) max_total = v.total;
            }
        }
    }

    double feasible_ratio = total_neighbors > 0 ? static_cast<double>(feasible_neighbors) / total_neighbors : 0.0;
    double avg_noRepeat = total_neighbors > 0 ? static_cast<double>(sum_noRepeat) / total_neighbors : 0.0;
    double avg_maxStreak = total_neighbors > 0 ? static_cast<double>(sum_maxStreak) / total_neighbors : 0.0;
    double avg_doubleRoundRobin = total_neighbors > 0 ? static_cast<double>(sum_doubleRoundRobin) / total_neighbors : 0.0;
    double avg_total = total_neighbors > 0 ? static_cast<double>(sum_total) / total_neighbors : 0.0;

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
}

void run_global_summary_one_step_round_swap(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules,
    bool append
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for 1-step round-swap global summary.\n";
        return;
    }

    std::ofstream out;
    if (append) out.open(output_file, std::ios::app);
    else out.open(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
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

        for (int r1 = 0; r1 < num_rounds; r1++) {
            for (int r2 = r1 + 1; r2 < num_rounds; r2++) {
                Schedule mutated = original;
                round_swap(mutated, r1, r2);

                ViolationCounts v = evaluate_schedule(mutated);

                total_neighbors++;
                if (is_feasible(v)) feasible_neighbors++;

                sum_noRepeat += v.noRepeat;
                sum_maxStreak += v.maxStreak;
                sum_doubleRoundRobin += v.doubleRoundRobin;
                sum_total += v.total;

                if (v.total < min_total) min_total = v.total;
                if (v.total > max_total) max_total = v.total;
            }
        }
    }

    double feasible_ratio = total_neighbors > 0 ? static_cast<double>(feasible_neighbors) / total_neighbors : 0.0;
    double avg_noRepeat = total_neighbors > 0 ? static_cast<double>(sum_noRepeat) / total_neighbors : 0.0;
    double avg_maxStreak = total_neighbors > 0 ? static_cast<double>(sum_maxStreak) / total_neighbors : 0.0;
    double avg_doubleRoundRobin = total_neighbors > 0 ? static_cast<double>(sum_doubleRoundRobin) / total_neighbors : 0.0;
    double avg_total = total_neighbors > 0 ? static_cast<double>(sum_total) / total_neighbors : 0.0;

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
}

void run_global_summary_two_step_home_away(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules,
    bool append
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for 2-step home/away global summary.\n";
        return;
    }

    std::ofstream out;
    if (append) out.open(output_file, std::ios::app);
    else out.open(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
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
        int num_teams = get_num_teams(original);

        for (int teamA1 = 0; teamA1 < num_teams; teamA1++) {
            for (int teamB1 = teamA1 + 1; teamB1 < num_teams; teamB1++) {
                Schedule first_neighbor = original;
                home_away_swap(first_neighbor, teamA1, teamB1);

                for (int teamA2 = 0; teamA2 < num_teams; teamA2++) {
                    for (int teamB2 = teamA2 + 1; teamB2 < num_teams; teamB2++) {
                        Schedule mutated = first_neighbor;
                        home_away_swap(mutated, teamA2, teamB2);

                        ViolationCounts v = evaluate_schedule(mutated);

                        total_neighbors++;
                        if (is_feasible(v)) feasible_neighbors++;

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
    }

    double feasible_ratio = total_neighbors > 0 ? static_cast<double>(feasible_neighbors) / total_neighbors : 0.0;
    double avg_noRepeat = total_neighbors > 0 ? static_cast<double>(sum_noRepeat) / total_neighbors : 0.0;
    double avg_maxStreak = total_neighbors > 0 ? static_cast<double>(sum_maxStreak) / total_neighbors : 0.0;
    double avg_doubleRoundRobin = total_neighbors > 0 ? static_cast<double>(sum_doubleRoundRobin) / total_neighbors : 0.0;
    double avg_total = total_neighbors > 0 ? static_cast<double>(sum_total) / total_neighbors : 0.0;

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
}

void run_global_summary_two_step_round_swap(
    const ScheduleSet& data,
    const std::string& output_file,
    int max_schedules,
    bool append
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for 2-step round-swap global summary.\n";
        return;
    }

    std::ofstream out;
    if (append) out.open(output_file, std::ios::app);
    else out.open(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
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

        for (int r1a = 0; r1a < num_rounds; r1a++) {
            for (int r1b = r1a + 1; r1b < num_rounds; r1b++) {
                Schedule first_neighbor = original;
                round_swap(first_neighbor, r1a, r1b);

                for (int r2a = 0; r2a < num_rounds; r2a++) {
                    for (int r2b = r2a + 1; r2b < num_rounds; r2b++) {
                        Schedule mutated = first_neighbor;
                        round_swap(mutated, r2a, r2b);

                        ViolationCounts v = evaluate_schedule(mutated);

                        total_neighbors++;
                        if (is_feasible(v)) feasible_neighbors++;

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
    }

    double feasible_ratio = total_neighbors > 0 ? static_cast<double>(feasible_neighbors) / total_neighbors : 0.0;
    double avg_noRepeat = total_neighbors > 0 ? static_cast<double>(sum_noRepeat) / total_neighbors : 0.0;
    double avg_maxStreak = total_neighbors > 0 ? static_cast<double>(sum_maxStreak) / total_neighbors : 0.0;
    double avg_doubleRoundRobin = total_neighbors > 0 ? static_cast<double>(sum_doubleRoundRobin) / total_neighbors : 0.0;
    double avg_total = total_neighbors > 0 ? static_cast<double>(sum_total) / total_neighbors : 0.0;

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
}

void run_global_summary_one_step_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules,
    bool append
) {
    bool first_home = true;
    bool first_round = true;
    if(append){
        first_home = false;
        first_round = false;
    }
    

    std::string home_file = output_folder + "/global_summary_one_step_home_away.csv";
    std::string round_file = output_folder + "/global_summary_one_step_round_swap.csv";

    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";
        std::cout << "Processing 1-step global summaries for team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);
        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_global_summary_one_step_home_away(data, home_file, max_schedules, !first_home);
        run_global_summary_one_step_round_swap(data, round_file, max_schedules, !first_round);

        first_home = false;
        first_round = false;
    }

    std::cout << "Finished 1-step global summaries.\n";
}

void run_global_summary_two_step_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int max_schedules,
    bool append
) {
    bool first_home = true;
    bool first_round = true;
    if(append){
        first_home = false;
        first_round = false;
    }

    std::string home_file = output_folder + "/global_summary_two_step_home_away.csv";
    std::string round_file = output_folder + "/global_summary_two_step_round_swap.csv";

    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";
        std::cout << "Processing 2-step global summaries for team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);
        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_global_summary_two_step_home_away(data, home_file, max_schedules, !first_home);
        run_global_summary_two_step_round_swap(data, round_file, max_schedules, !first_round);

        first_home = false;
        first_round = false;
    }

    std::cout << "Finished 2-step global summaries.\n";
}

void run_random_walk_experiment(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules,
    unsigned int seed
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for random walk experiment.\n";
        return;
    }

    std::ofstream out(output_file);

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    out << "schedule_id,team_size,step,mutation_id,operator,"
           "noRepeat,maxStreak,doubleRoundRobin,total,"
           "feasible,current_feasible_streak,longest_feasible_streak,"
           "running_feasible_ratio,running_avg_total,running_max_total\n";

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> operator_dist(0, 1);

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        Schedule current = data.schedules[schedule_id];
        int team_size = get_num_teams(current);

        int current_feasible_streak = 0;
        int longest_feasible_streak = 0;

        // Running aggregates
        int feasible_count_so_far = 0;
        long long total_violations_sum_so_far = 0;
        int max_total_so_far = 0;

        for (int step = 0; step < walk_length; step++) {
            MutationType chosen_operator =
                (operator_dist(rng) == 0) ? HOME_AWAY_SWAP_MUTATION : ROUND_SWAP_MUTATION;

            if (chosen_operator == HOME_AWAY_SWAP_MUTATION) {
                home_away_swap_random(current, rng);
            } else {
                round_swap_random(current, rng);
            }

            ViolationCounts v = evaluate_schedule(current);

            bool feasible = is_feasible(v);

            if (feasible) {
                feasible_count_so_far++;
                current_feasible_streak++;
                if (current_feasible_streak > longest_feasible_streak) {
                    longest_feasible_streak = current_feasible_streak;
                }
            } else {
                current_feasible_streak = 0;
            }

            total_violations_sum_so_far += v.total;
            if (step == 0 || v.total > max_total_so_far) {
                max_total_so_far = v.total;
            }

            double running_feasible_ratio =
                static_cast<double>(feasible_count_so_far) / static_cast<double>(step + 1);

            double running_avg_total =
                static_cast<double>(total_violations_sum_so_far) / static_cast<double>(step + 1);

            long long mutation_id =
                static_cast<long long>(schedule_id) * walk_length + step;

            out << schedule_id << ","
                << team_size << ","
                << step << ","
                << mutation_id << ","
                << mutation_type_to_string(chosen_operator) << ","
                << v.noRepeat << ","
                << v.maxStreak << ","
                << v.doubleRoundRobin << ","
                << v.total << ","
                << (feasible ? 1 : 0) << ","
                << current_feasible_streak << ","
                << longest_feasible_streak << ","
                << running_feasible_ratio << ","
                << running_avg_total << ","
                << max_total_so_far << "\n";
        }

        if ((schedule_id + 1) % 10 == 0) {
            std::cout << "[Random Walk] Processed " << (schedule_id + 1) << " schedules\n";
        }
    }

    out.close();
    std::cout << "Random walk experiment finished. Results written to "
              << output_file << std::endl;
}

void run_random_walk_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules,
    unsigned int seed
) {
    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";
        std::string output_file = output_folder + "/random_walk_2_" + std::to_string(n) + ".csv";

        std::cout << "Processing random walk for team size " << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);

        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_random_walk_experiment(data, output_file, walk_length, max_schedules, seed);

        std::cout << "Finished random walk for team size " << n << ".\n";
    }

    std::cout << "All random walk experiments completed.\n";
}

void run_random_walk_global_summary(
    const ScheduleSet& data,
    const std::string& output_file,
    int walk_length,
    int max_schedules,
    unsigned int seed,
    bool append
) {
    if (data.schedules.empty()) {
        std::cerr << "Error: no schedules available for random walk global summary.\n";
        return;
    }

    std::ofstream out;
    if (append) {
        out.open(output_file, std::ios::app);
    } else {
        out.open(output_file);
    }

    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << output_file << std::endl;
        return;
    }

    if (!append) {
        out << "team_size,num_original_schedules,walk_length,total_mutations,"
               "avg_feasible_ratio,avg_total_violations,avg_max_total_violations,"
               "avg_longest_feasible_streak\n";
    }

    int limit = (max_schedules < 0 || max_schedules > static_cast<int>(data.schedules.size()))
              ? static_cast<int>(data.schedules.size())
              : max_schedules;

    int team_size = get_num_teams(data.schedules[0]);

    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> operator_dist(0, 1);

    long long total_mutations = 0;
    long long total_violations_sum = 0;

    double sum_feasible_ratio_per_walk = 0.0;
    double sum_max_total_per_walk = 0.0;
    double sum_longest_feasible_streak_per_walk = 0.0;

    for (int schedule_id = 0; schedule_id < limit; schedule_id++) {
        Schedule current = data.schedules[schedule_id];

        int feasible_count = 0;
        int longest_feasible_streak = 0;
        int current_feasible_streak = 0;
        int max_total_in_walk = 0;

        for (int step = 0; step < walk_length; step++) {
            MutationType chosen_operator =
                (operator_dist(rng) == 0) ? HOME_AWAY_SWAP_MUTATION : ROUND_SWAP_MUTATION;

            if (chosen_operator == HOME_AWAY_SWAP_MUTATION) {
                home_away_swap_random(current, rng);
            } else {
                round_swap_random(current, rng);
            }

            ViolationCounts v = evaluate_schedule(current);

            total_mutations++;
            total_violations_sum += v.total;

            if (v.total > max_total_in_walk) {
                max_total_in_walk = v.total;
            }

            if (is_feasible(v)) {
                feasible_count++;
                current_feasible_streak++;
                if (current_feasible_streak > longest_feasible_streak) {
                    longest_feasible_streak = current_feasible_streak;
                }
            } else {
                current_feasible_streak = 0;
            }
        }

        double feasible_ratio_this_walk =
            (walk_length > 0) ? static_cast<double>(feasible_count) / walk_length : 0.0;

        sum_feasible_ratio_per_walk += feasible_ratio_this_walk;
        sum_max_total_per_walk += static_cast<double>(max_total_in_walk);
        sum_longest_feasible_streak_per_walk += static_cast<double>(longest_feasible_streak);

        if ((schedule_id + 1) % 10 == 0) {
            std::cout << "[Random Walk Global Summary] Processed "
                      << (schedule_id + 1) << " schedules for team size "
                      << team_size << "\n";
        }
    }

    double avg_feasible_ratio =
        (limit > 0) ? sum_feasible_ratio_per_walk / limit : 0.0;

    double avg_total_violations =
        (total_mutations > 0) ? static_cast<double>(total_violations_sum) / total_mutations : 0.0;

    double avg_max_total_violations =
        (limit > 0) ? sum_max_total_per_walk / limit : 0.0;

    double avg_longest_feasible_streak =
        (limit > 0) ? sum_longest_feasible_streak_per_walk / limit : 0.0;

    out << team_size << ","
        << limit << ","
        << walk_length << ","
        << total_mutations << ","
        << avg_feasible_ratio << ","
        << avg_total_violations << ","
        << avg_max_total_violations << ","
        << avg_longest_feasible_streak << "\n";

    out.close();

    std::cout << "Random walk global summary written for team size "
              << team_size << " to " << output_file << std::endl;
}

void run_random_walk_global_summary_driver(
    const std::string& data_folder,
    const std::string& output_folder,
    const std::vector<int>& team_sizes,
    int walk_length,
    int max_schedules,
    unsigned int seed
) {
    std::string output_file = output_folder + "/random_walk_global_summary.csv";
    bool first = true;

    for (int n : team_sizes) {
        std::string input_file = data_folder + "/ALL-" + std::to_string(n) + ".csv";

        std::cout << "Processing random walk global summary for team size "
                  << n << "...\n";

        ScheduleSet data = read_schedules_from_file(input_file);

        if (data.schedules.empty()) {
            std::cout << "Warning: no schedules found for " << n << ", skipping.\n";
            continue;
        }

        run_random_walk_global_summary(
            data,
            output_file,
            walk_length,
            max_schedules,
            seed,
            !first
        );

        first = false;
    }

    std::cout << "All random walk global summaries completed.\n";
}
