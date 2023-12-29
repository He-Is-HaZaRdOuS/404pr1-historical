#include "Solution.h"
#include <cmath>
#include <iostream>

// constructor
Solution::Solution(vector<Course> list) {
    this->courseList = list;
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
    std::vector<bool> placed = std::vector<bool>(courseList.size());
    for (int i = 0; i < placed.size(); i++) {
        placed.at(i) = false;
    }
    // for each slot...
    for (int day = 0; day < 7; day++) {
        for (int slot = 0; slot < 108; slot++) {
            // ... find a course to place contiguously
            for (int c = 0; c < courseList.size(); c++) {
                if (placed.at(c) == true) continue;
                bool canFit = true;
                // check if exam can fit ...
                for (int i = 0; i < courseList.at(c).examDuration / TIMESLOTDURATION; ++i) {
                    if (slot + i >= 108 || timeTable[day][slot + i].status != AVAILABLE) {
                        canFit = false;
                        break;
                    }
                }
                // ... if so, place it
                if (canFit) {
                    placed.at(c) = true;
                    for (int i = 0; i < courseList.at(c).examDuration / TIMESLOTDURATION; ++i) {
                        timeTable[day][slot + i].status = OCCUPIED;
                        timeTable[day][slot + i].currentCourse = courseList.at(c);
                    }
                }
            }
        }
    }

    // print schedule (WIP)
    // TODO: merge contiguous slots
    for (int i = 0; i < 108; i++)
        printf("%02d:%02d %10s %10s %10s %10s %10s %10s %10s\n",
               9 + (i * TIMESLOTDURATION) / 60, (i * TIMESLOTDURATION) % 60,
               timeTable[0][i].currentCourse.code.c_str(),
               timeTable[1][i].currentCourse.code.c_str(),
               timeTable[2][i].currentCourse.code.c_str(),
               timeTable[3][i].currentCourse.code.c_str(),
               timeTable[4][i].currentCourse.code.c_str(),
               timeTable[5][i].currentCourse.code.c_str(),
               timeTable[6][i].currentCourse.code.c_str()
        );

    int placedCount = 0;
    for (int i = 0; i < placed.size(); i++) {
        if (placed.at(i)) placedCount++;
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

