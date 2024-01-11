#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#define TIMESLOTCOUNT 108
#define TIMESLOTDURATION 5
#define ITERATIONCOUNT 100
inline bool __Day7Needed = false;
inline unsigned int __DimensionCount = 1;

#include "Course.h"
#include "Timeslot.h"
#include "Classroom.h"

#include <array>


#define K 0.9

typedef struct Week {
  Timeslot t[7][TIMESLOTCOUNT];
} Week;

using namespace std;

class Solution{
public:
  std::vector<Week> timeTable;
  //Timeslot timeTable [7][TIMESLOTCOUNT];
  double tempmax = 1;
  double tempmin = 0.001;
  double alpha = 0.99998;
  int iterationTimes = 100;
  int count = 0;
  vector<Classroom> classList;
  vector<Course> courseList;
  Vector2D<std::string> blockedHours;

  void randomizedSuccesor();
  void initializeSchedule();
  double cooling();
  int cost(vector<Week> table);
  inline bool fillTable(unsigned long courseCount, std::vector<bool> &placed);
  inline void checkValidity();
  void setBlockedHours(Vector2D<std::string> blockedHours, int dim);
  inline std::vector<Classroom> getAvailableClassrooms(int day, int start, int end);

  explicit Solution(vector<Course> list, vector<Classroom> classrooms, Vector2D<std::string> blockedHours);
  void Solve();
};


#endif //SOLUTION_H