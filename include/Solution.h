#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#define TIMESLOTCOUNT 108
#define TIMESLOTDURATION 5
inline bool day7 = false;

#include "Course.h"
#include "Timeslot.h"
#include "Classroom.h"

#define K 0.9

using namespace std;

class Solution{
public:
    Timeslot timeTable [7][TIMESLOTCOUNT];
    double tempmax = 1;
    double tempmin = 0.001;
    double alpha = 0.99998;
    int iterationTimes = 100;
    int count = 0;
    vector<Classroom> classList;
    vector<Course> courseList;

    void randomizedSuccesor();
    void initializeSchedule();
    double cooling();
    int cost();

    explicit Solution(vector<Course> list);
    void Solve();
};


#endif //SOLUTION_H
