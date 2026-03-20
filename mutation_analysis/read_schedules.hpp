#ifndef READ_SCHEDULES_HPP
#define READ_SCHEDULES_HPP

#include "schedule.hpp"
#include <string>

ScheduleSet read_schedules_from_file(const std::string& filename);

int get_num_teams(const std::vector<Matchup>& matchups);

#endif