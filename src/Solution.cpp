#include "Solution.h"
#include <cmath>
#include <iostream>

// constructor
Solution::Solution(vector<Course> list) {
    this->courseList = list;
    /* hold one dimension initially */
    this->timeTable.push_back(Week{});
}

// pseudocode of simulated annealing
void Solution::Solve() {
    int newScore;
    int oldScore;
    int delta;

    //fill the sudoku table random
    initializeSchedule();
    oldScore = cost();

    while (tempmax > tempmin) {
        for (int i = 0; i < iterationTimes; i++) {
            if (oldScore == 0) {
                tempmax = tempmin;
                break;
            }

            //get new sudoku board according to a logic
            //newSchedule = randomizeSuccesor(board);
            //newScore = cost(newboard);
            delta = newScore - oldScore;
            if (delta > 0) {
                //probability of accepting bad moves
                if ((rand() % 1000) / 1000.0 < exp(-delta / (K * tempmax))) {
                    //Schedule = newSchedule;
                    oldScore = newScore;
                }
                //accept good moves always
            }
            else {
                //Schedule = newSchedule;
                oldScore = newScore;
            }
            //keep number of all iterations
            count++;
        }
        // decrase the temperature
        tempmax = cooling();
    }
}

// make a small change in schedule
void Solution::randomizedSuccesor() {
}

// make first schedule according to a logic
void Solution::initializeSchedule() {
    // mark all courses as "not placed"
    const unsigned long courseCount = courseList.size();
    std::vector<bool> placed = std::vector<bool>(courseCount);
    for (int i = 0; i < courseCount; i++) {
        placed.at(i) = false;
    }
    // for each slot...
    for (int day = 0; day < 7; day++) {
        for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
            // ... find a course to place contiguously
            for (int c = 0; c < courseCount; ++c) {
                if (placed.at(c) == true) continue;
                const unsigned long courseTimePartition = courseList.at(c).examDuration / TIMESLOTDURATION;
                bool canFit = true;
                // check if exam can fit ...
                for (int i = 0; i < courseTimePartition; ++i) {
                    if (slot + i >= TIMESLOTCOUNT || timeTable.at(0).t[day][slot + i].status != AVAILABLE) {
                        canFit = false;
                        break;
                    }
                }
                // ... if so, place it
                if (canFit) {
                    placed.at(c) = true;
                    for (int i = 0; i < courseTimePartition; ++i) {
                        timeTable.at(0).t[day][slot+i].status = OCCUPIED;
                        timeTable.at(0).t[day][slot+i].currentCourse = courseList.at(c);
                        //timeTable[day][slot + i].status = OCCUPIED;
                        //timeTable[day][slot + i].currentCourse = courseList.at(c);
                    }
                }
            }
        }
    }

    // print schedule (WIP)
    // TODO: merge contiguous slots
    for (int i = 0; i < TIMESLOTCOUNT; ++i)
        printf("%02d:%02d %10s %10s %10s %10s %10s %10s %10s\n",
               9 + (i * TIMESLOTDURATION) / 60, (i * TIMESLOTDURATION) % 60,
               timeTable.at(0).t[0][i].currentCourse.code.c_str(),
               timeTable.at(0).t[1][i].currentCourse.code.c_str(),
               timeTable.at(0).t[2][i].currentCourse.code.c_str(),
               timeTable.at(0).t[3][i].currentCourse.code.c_str(),
               timeTable.at(0).t[4][i].currentCourse.code.c_str(),
               timeTable.at(0).t[5][i].currentCourse.code.c_str(),
               timeTable.at(0).t[6][i].currentCourse.code.c_str()
        );

    int placedCount = 0;
    for (int i = 0; i < courseCount; ++i) {
        if (placed.at(i)) ++placedCount;
    }
    std::cout << "Placed course count: " << placedCount << std::endl;

    std::cout << "Finished";
}

// calculate cost
int Solution::cost() {
    return 0;
}

// for annealing process
double Solution::cooling() {
    return tempmax * alpha;
}

