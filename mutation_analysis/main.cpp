#include "read_schedules.hpp"
#include <iostream>

//g++ -std=c++17 *.cpp -o run
//./run

int main() {
    std::cout << "Hello, Mutation Analysis!" << std::endl;

    ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-8.csv");

    //print_schedule_set(data, 3);

    if (!data.schedules.empty()) {
        TeamScheduleTable table = build_team_schedule_table(data.schedules[0]);
        print_team_schedule_table(table);
    }

    return 0;
}