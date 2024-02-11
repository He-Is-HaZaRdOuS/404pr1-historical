#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <algorithm>
#include <cstring>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define SOMEIMPORTANTCONSTANT 43
#define idColumn 0 // classroom && student

#define capacityColumn 1 // classroom

#define professorColumn 1 // student
#define codeColumn 2 // student
#define durationColumn 3 // student

#define dayColumn 0 // blocked
#define timeColumn 1 // blocked
#define commonCourseColumn 2 // blocked

namespace n_Main {
  const std::vector<std::string> DAYS_UPPERCASE = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};
  inline std::unordered_map<std::string, bool> uniqueCourses;
  inline std::unordered_map<std::string, bool> uniqueProfessors;
  inline std::future<void> future_course;
  inline std::future<void> future_conflict;
  inline std::future<void> future_classroom;
  inline std::future<void> future_blocked;
}

#endif //MAIN_H
