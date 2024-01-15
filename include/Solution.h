#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#include "Course.h"
#include "Timeslot.h"
#include "Classroom.h"

#define TIMESLOTCOUNT 108
#define TIMESLOTDURATION 5
#define ITERATIONCOUNT 100

inline bool __Day7Needed = false;
inline int __DimensionCount = 1;
inline std::vector<bool> __placed;
inline const std::vector __DAYS = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
};

/* struct to wrap static 2D array into an "object" to be used in vectors */
typedef struct Week {
    Timeslot t[7][TIMESLOTCOUNT];
} Week;

/* simple struct to return more than one value from a function */
typedef struct classroomStatus {
    bool returnStatus;
    int wantedCapacity;
} classroomStatus;

/* simple struct to return more than one value from a function */
typedef struct scheduleStatus {
    std::vector<Week> schedule;
    std::vector<bool> placed;
    int dim;
    int cost;
    bool day;
} scheduleStatus;

using namespace std;

class Solution{
public:
    std::vector<Week> timeTable;
    vector<Classroom> classList;
    vector<Course> courseList;
    Vector2D<std::string> blockedHours;

    void Solve(int nValue);
    inline scheduleStatus initializeSchedule();
    int cost(vector<Week> &table, int &dim);
    inline bool fillTable(std::vector<Week> &schedule, unsigned long courseCount, std::vector<bool> &placed, int &dimensionCount, bool day7Needed);
    static inline bool checkValidityPrint(vector<Week> &table);
    inline void setBlockedHours(std::vector<Week> &schedule, Vector2D<std::string> blockedHours, int dim);
    inline std::vector<Classroom> getAvailableClassrooms(int day, int start, int end);
    inline classroomStatus assignClassrooms();
    inline void resetAssignedClassrooms();
    static inline vector<Course> extractCoursesFromDay(std::vector<Week> &table, int &day, int &dim);

    explicit Solution(vector<Course> list, vector<Classroom> classrooms, Vector2D<std::string> blockedHours);
};

#endif //SOLUTION_H
