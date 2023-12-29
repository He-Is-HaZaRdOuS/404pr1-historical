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
            } else {
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
    int dayCount = 5;
    int i, day, start = 0;
    bool contiguous = false;
    for(auto & course : courseList){
        bool placed = false;
        int timePartition = course.examDuration / TIMESLOTDURATION;
        //std::cout << course.code << " " << timePartition <<  std::endl;

        for(day = 0; !placed && day < dayCount; ++day){
            for(i = 0; !placed && i < TIMESLOTCOUNT; ++i) {
                contiguous = false;
                if(timeTable[day][i].status == AVAILABLE){
                    int timeLim = i + timePartition;
                    for(int j = i; j < timeLim; ++j){
                        if(timeTable[day][j].status != AVAILABLE){
                            contiguous = true;
                            break;
                        }
                    }
                    if(!contiguous){
                        //std::cout << ((contiguous) ? "true" : "false") << " " << i << std::endl;
                        start = i;
                        int timeDur = start + timePartition;
                        for(i = start; i < timeDur; ++i){
                            timeTable[day][i].status = OCCUPIED;
                            timeTable[day][i].currentCourse = course;
                        }
                        placed = true;
                        break;
                }
            }
        }
        /*
         * Implement going to 3rd dimension
         * if current dimension's timetable is full
         */


        }
    }

std::cout << "Finished";
}

// calculate cost
int Solution::cost() {
    return 0;
}

// for annealing process
double Solution::cooling() {
    return tempmax*alpha;
}

