#pragma once
#ifndef SOLUTION_H
#define SOLUTION_H

#include "Course.h"
#include "Timeslot.h"
#include "Classroom.h"
#include "Typedefs.h"

namespace n_Solution {
    inline constexpr uint8_t TIMESLOTCOUNT = 108;
    inline constexpr uint8_t TIMESLOTDURATION = 5;
    inline constexpr uint8_t ITERATIONCOUNT = 100;
    inline bool sundayUnlocked = false;
    inline uint32_t dimensionCount = 1;
    inline std::vector<bool> placed;
    inline const std::vector DAYS = {
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
        Timeslot week[7][TIMESLOTCOUNT];
    } Week;

    /* simple struct to return more than one value from a function */
    typedef struct classroomStatus {
        int64_t wantedCapacity;
        bool returnStatus;
    } classroomStatus;

    /* simple struct to return more than one value from a function */
    typedef struct scheduleStatus {
        std::vector<Week> schedule;
        uint32_t dim;
        int64_t cost;
        std::vector<bool> placed;
        bool day;
    } scheduleStatus;
}

class Solution{
private:
    std::vector<n_Solution::Week> timeTable;
    std::vector<Classroom> classList;
    std::vector<Course> courseList;
    Vector2D<std::string> blockedHours;

    [[nodiscard]] inline n_Solution::scheduleStatus initializeSchedule() const;
    [[nodiscard]] inline std::vector<Classroom> getAvailableClassrooms(uint8_t day, uint8_t start, uint8_t end) const;
    static int cost(const std::vector<n_Solution::Week> &table, uint32_t dim);
    static inline std::vector<Course> extractCoursesFromDay(const std::vector<n_Solution::Week> &table, uint8_t day, uint32_t dimensionCount);
    static inline bool checkValidityPrint(const std::vector<n_Solution::Week> &table);
    static inline void setBlockedHours(std::vector<n_Solution::Week> &schedule, const Vector2D<std::string> &blockedList, uint32_t dim);
    inline bool fillTable(std::vector<n_Solution::Week> &schedule, uint64_t courseCount, std::vector<bool> &placed, uint32_t &dimensionCount, bool sundayUnlocked) const;
    inline n_Solution::classroomStatus assignClassrooms();
    inline void resetAssignedClassrooms();

public:
    explicit Solution(const std::vector<Course>& list, const std::vector<Classroom>& classrooms, const Vector2D<std::string>& blockedHours);
    void Solve(int nValue);
};

#endif //SOLUTION_H
