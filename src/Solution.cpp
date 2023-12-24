#include "Solution.h"
#include <iostream>
#include <cmath>

// constructor
Solution::Solution(int dayNum, vector<Course> list) {
    this->dayNum = dayNum;
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

}

// calculate cost
int Solution::cost() {
    return 0;
}

// for annealing process
double Solution::cooling() {
    return tempmax*alpha;
}

