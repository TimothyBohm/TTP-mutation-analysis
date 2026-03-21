#include "read_schedules.hpp"
#include <iostream>

//g++ -std=c++17 *.cpp -o run
//./run

int main() {
    ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-8.csv");

    if (!data.schedules.empty()) {
        //Schedule s = data.schedules[0];
        //TeamScheduleTable table = build_team_schedule_table(s);
        //print_team_schedule_table(table);
        //ViolationCounts v = evaluate_schedule(table);

        Schedule s = make_noRepeat_test_schedule();
        print_schedule(s);
        TeamScheduleTable table = build_team_schedule_table(s);
        print_team_schedule_table(table);
        ViolationCounts v = evaluate_schedule(table);

        std::cout << "Violations:\n";
        std::cout << "NoRepeat: " << v.noRepeat << std::endl;
        std::cout << "MaxStreak: " << v.maxStreak << std::endl;
        std::cout << "DoubleRoundRobin: " << v.doubleRoundRobin << std::endl;
        std::cout << "Total: " << v.total << std::endl;
    }

    return 0;
}