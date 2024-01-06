#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#define TIMESLOTCOUNT 108
#define TIMESLOTDURATION 5
inline bool day7 = false;
inline unsigned int dimensionCount = 1;

#include "Course.h"
#include "Timeslot.h"
#include "Classroom.h"

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

  void checkValidity();
  inline std::vector<Classroom> getAvailableClassrooms(int day, int start, int end);
  void randomizedSuccesor();
  void initializeSchedule();
  double cooling();
  int cost();

  explicit Solution(vector<Course> list, vector<Classroom> classrooms);
  void Solve();
};


#endif //SOLUTION_H