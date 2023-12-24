#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H


#include "Course.h"
#define K 0.9

using namespace std;

class Solution{
private:
    int *timeSlots[108];
    double tempmax = 1;
    double tempmin = 0.001;
    double alpha = 0.99998;
    int iterationTimes = 100;
    int count = 0;
    int dayNum;
    vector<Course> courseList;

    void randomizedSuccesor();
    void initializeSchedule();
    double cooling();
    int cost();

public:
    explicit Solution(int dayNum, vector<Course> list);
    void Solve();
};


#endif //SOLUTION_H
