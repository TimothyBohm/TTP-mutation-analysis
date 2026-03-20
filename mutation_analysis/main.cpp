#include "read_schedules.hpp"
#include <iostream>

//g++ -std=c++17 *.cpp -o run
//./run

int main() {
    std::cout << "Hello, Mutation Analysis!" << std::endl;

    ScheduleSet data = read_schedules_from_file("../Schedules/Schedules_ALL/ALL-8.csv");

    print_schedule_set(data, 3);

    return 0;
}