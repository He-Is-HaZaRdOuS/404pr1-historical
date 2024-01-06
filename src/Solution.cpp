#include "Solution.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <unordered_map>

// constructor
Solution::Solution(vector<Course> list, vector<Classroom> classrooms) {
    this->courseList = list;
    this->classList = classrooms;
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
    for (int dim = 0; dim < dimensionCount; ++dim) {
        this->timeTable.push_back(Week{});
        bool currentDimensionIsEmpty = true;
        for (int day = 0; day < 7; day++) {
            for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
                // ... find a course to place contiguously
                for (int c = 0; c < courseCount; ++c) {
                    if (placed.at(c) == true) continue;
                    const unsigned long courseTimePartition = courseList.at(c).examDuration / TIMESLOTDURATION;
                    bool canFit = true;
                    // check if exam can fit ...
                    for (int i = 0; i < courseTimePartition; ++i) {
                        if (slot + i >= TIMESLOTCOUNT || timeTable.at(dim).t[day][slot + i].status != AVAILABLE) {
                            canFit = false;
                            break;
                        }
                    }
                    // ... if so, place it
                    if (canFit) {
                        placed.at(c) = true;
                        for (int i = 0; i < courseTimePartition; ++i) {
                            currentDimensionIsEmpty = false;
                            timeTable.at(dim).t[day][slot+i].status = OCCUPIED;
                            timeTable.at(dim).t[day][slot+i].currentCourse = courseList.at(c);
                        }
                    }
                }
            }
        }
        if(currentDimensionIsEmpty) {
            std::cout << "CANT MAKE SCHEDULE" << std::endl;
        }
        else {
            /* increment dimension and assign attempt to assign non-intersecting courses */
            ++dimensionCount;
        }
    }
    /* get rid of empty dimension (dirty fix) */
    dimensionCount--;
    timeTable.pop_back();

    /* TODO
     * Simulated Annealing may have to be done before assignment of classrooms xo
     */

    /* assign classrooms to exams */
    unsigned int classListSize = courseList.size();
    for(int day = 0; day < 7; ++day){
        for(int slot = 0; slot < TIMESLOTCOUNT; ++slot){
            for(int dim = 0; dim < dimensionCount; ++dim) {
                for(int cSize = 0; cSize < classListSize; ++cSize) {
                    /* check if a course does not have any classrooms assigned yet */
                    if(timeTable.at(dim).t[day][slot].currentCourse.classrooms.empty()) {
                        /* find empty classrooms in given time interval */
                        const int end = slot + (timeTable.at(dim).t[day][slot].currentCourse.examDuration / TIMESLOTDURATION);
                        std::vector<Classroom> availableClassrooms = getAvailableClassrooms(day, slot, end);
                        /* sort classrooms from in ascending order according to their capacity */
                        std::sort(availableClassrooms.begin(), availableClassrooms.end(), [](const Classroom&c1, const Classroom&c2) {
                            return c1.capacity < c2.capacity;
                        });

                        const unsigned int classSize = availableClassrooms.size();
                        const unsigned int courseStudentCount = timeTable.at(dim).t[day][slot].currentCourse.studentCount;
                        bool courseCanFitInOneClassroom = false;
                        /* Assign a single classroom if half of its capacity fits the number of students */
                        for(int cIndex = 0; cIndex < classSize; ++cIndex) {
                            if(availableClassrooms.at(cIndex).capacity/2 >= courseStudentCount) {
                                for(int i = slot; i < end; ++i) {
                                    timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(cIndex));
                                }
                                courseCanFitInOneClassroom = true;
                                break;
                            }
                        }
                        /* Assign multiple classrooms to course */
                        if(!courseCanFitInOneClassroom) {
                            unsigned int assignedCapacity = 0;
                            for(int i = classSize - 1; assignedCapacity < courseStudentCount && i >= 0; --i) {
                                assignedCapacity = assignedCapacity + (availableClassrooms.at(i).capacity/2);
                                timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(i));
                                for(int j = 0; j < i; ++j) {
                                    if((availableClassrooms.at(j).capacity/2) + assignedCapacity >= courseStudentCount) {
                                        assignedCapacity = assignedCapacity + (availableClassrooms.at(j).capacity/2);
                                        timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(j));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    // print schedule (WIP)
    // TODO: merge contiguous slots
    for (int d=0; d<dimensionCount; ++d) {
    printf("Dimendion: %d\n", d);
    for (int i = 0; i < TIMESLOTCOUNT; ++i)
        printf("%02d:%02d %10s %10s %10s %10s %10s %10s %10s\n",
               9 + (i * TIMESLOTDURATION) / 60, (i * TIMESLOTDURATION) % 60,
               timeTable.at(d).t[0][i].currentCourse.code.c_str(),
               timeTable.at(d).t[1][i].currentCourse.code.c_str(),
               timeTable.at(d).t[2][i].currentCourse.code.c_str(),
               timeTable.at(d).t[3][i].currentCourse.code.c_str(),
               timeTable.at(d).t[4][i].currentCourse.code.c_str(),
               timeTable.at(d).t[5][i].currentCourse.code.c_str(),
               timeTable.at(d).t[6][i].currentCourse.code.c_str()
        );
    }

    int placedCount = 0;
    for (int i = 0; i < courseCount; ++i) {
        if (placed.at(i)) ++placedCount;
    }
    std::cout << "Placed course count: " << placedCount << std::endl;

    std::cout << "Finished";
}

// calculate cost
int Solution::cost() {
    int cost = 0;

  for(int day = 0; day < 7; ++day) {
    for(int i = 0; i < dimensionCount; ++i)
      for(int j = 0; j < TIMESLOTCOUNT; ++j) {

        if(timeTable.at(i).t[day][j].status == OCCUPIED)
          for(int l = 0; l < dimensionCount; ++l)
            for(int k = 0; k < TIMESLOTCOUNT; ++k) {

              if(timeTable.at(i).t[day][j].currentCourse.code != timeTable.at(l).t[day][k].currentCourse.code
                  && timeTable.at(l).t[day][k].status == OCCUPIED) {
                if(timeTable.at(i).t[day][j].currentCourse.professorName
                    == timeTable.at(l).t[day][k].currentCourse.professorName) {
                  ++cost;
                }
                for(auto &a : timeTable.at(i).t[day][j].currentCourse.conflictingCourses) {
                  if(a == timeTable.at(l).t[day][k].currentCourse.code)
                    ++cost;
                }
              }
            }
      }
  }

  cout << "\ncost: " << cost << endl;
  return 0;
}

// for annealing process
double Solution::cooling() {
    return tempmax * alpha;
}

/* return a vector of non-intersecting classrooms for the given time interval */
inline std::vector<Classroom> Solution::getAvailableClassrooms(const int day, const int start, const int end) {
    const unsigned int cSize = classList.size();
    std::vector<Classroom> result;
    std::unordered_map<String, bool> availableClassrooms;
    for(Classroom& c : classList) {
        availableClassrooms.emplace(c.id, true);
    }

    for(int dim = 0; dim < dimensionCount; ++dim) {
        for(int i = start; i < end; ++i) {
            if(timeTable.at(dim).t[day][i].status == OCCUPIED) {
                availableClassrooms[timeTable.at(dim).t[day][i].currentCourse.code] = false;
            }
        }
    }

    for (const pair<const string, bool>& available : availableClassrooms) {
        if(available.second) {
            for(int i = 0; i < cSize; ++i) {
                if(classList.at(i).id == available.first) {
                    result.push_back(classList.at(i));
                }
            }
        }
    }
    return result;
}

