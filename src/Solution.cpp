#include "Solution.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <unordered_map>

inline void printTimeTable(const std::vector<Week>&timeTable);

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
  const unsigned long courseCount = courseList.size();
  std::vector<bool> placed = std::vector<bool>(courseCount);;
  bool isFilled = false;

  TryAgain: {
    int iterationCount = 0;
    do {
      if(!isFilled) {
        /* initialize everything necessary */
        std::cout << iterationCount << std::endl;
        ++iterationCount;
        __DimensionCount = 1;
        timeTable.clear();
        // mark all courses as "not placed"
        for (int i = 0; i < courseCount; i++) {
          placed.at(i) = false;
        }
        /* shuffle vectors and try again */
        auto rd = std::random_device {};
        auto rng = std::default_random_engine { rd() };
        std::shuffle(std::begin(courseList), std::end(courseList), rng);
        std::shuffle(std::begin(placed), std::end(placed), rng);
      }
      /* invoke algorithm */
      isFilled = fillTable(courseCount, placed);
    }while(!isFilled && iterationCount < 10);

    /* Unlock Day 7 and try again */
    if(!__Day7Needed && !isFilled && iterationCount >= 10) {
      __Day7Needed = true;
      std::cout << "DAY 7 UNLOCKED!" << std::endl;
      /* invoke algorithm again after unlocking 7th day */
      goto TryAgain;
    }
  }

  /* FAILED */
  if(__Day7Needed && !isFilled) {
    std::cout << "COULDNT FILL TABLE AT ALL, TERMINATING!";
    exit(1);
  }

  /* TODO
     * Simulated Annealing may have to be done before assignment of classrooms xo
   */

  /* assign classrooms to exams */
  const unsigned int classListSize = courseList.size();
  for(int day = 0; day < (__Day7Needed ? 7 : 6); ++day){
    for(int slot = 0; slot < TIMESLOTCOUNT; ++slot){
      for(int dim = 0; dim < __DimensionCount; ++dim) {
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
  for (int d=0; d<__DimensionCount; ++d) {
    printf("Dimension: %d\n", d);
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

  printTimeTable(timeTable);

  int placedCount = 0;
  for (int i = 0; i < courseCount; ++i) {
    if (placed.at(i)) {
      ++placedCount;
    }
    else {
      std::cout << "Couldn't place " << courseList.at(i).code << std::endl;
    }

  }
  std::cout << "Dimensions:  " << __DimensionCount << std::endl;

  std::cout << "Number of days: " << (__Day7Needed ? 7 : 6) << std::endl;

  std::cout << "Placed course count: " << placedCount << std::endl;

  std::cout << "Finished";

  checkValidity();
}

inline bool Solution::fillTable(const unsigned long courseCount, std::vector<bool> &placed) {
  // for each slot...
  for (int dim = 0; dim < __DimensionCount; ++dim) {
    if(timeTable.size() < __DimensionCount)
      this->timeTable.push_back(Week{});
    bool currentDimensionIsEmpty = true;
    for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
      for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
        // ... find a course to place contiguously
        for (int c = 0; c < courseCount; ++c) {
          if (placed.at(c) == true) continue;
          const unsigned long courseTimePartition = courseList.at(c).examDuration / TIMESLOTDURATION;
          bool canFit = true;
          bool conflicts = false;

          // check if exam can fit ...
          for (int i = 0; i < courseTimePartition; ++i) {
            if (slot + i >= TIMESLOTCOUNT || timeTable.at(dim).t[day][slot + i].status != AVAILABLE) {

              canFit = false;
              break;
            }

            for(int otherDim = 0; otherDim < __DimensionCount; ++otherDim) {
              if(dim != otherDim) {
                for(String& conflict: courseList.at(c).conflictingCourses) {
                  if(timeTable.at(otherDim).t[day][slot+i].currentCourse.code == conflict) {
                    //std::cout << timeTable.at(otherDim).t[day][slot].currentCourse.code << std::endl;
                    conflicts = true;
                    break;
                  }
                }
              }
            }

          }
          // ... if so, place it
          if (canFit && !conflicts) {
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
      //std::cout << "CANT MAKE SCHEDULE" << std::endl;
    }
    else {
      /* increment dimension and assign attempt to assign non-intersecting courses */
      ++__DimensionCount;
    }
  }
  /* get rid of empty dimension (dirty fix) */
  __DimensionCount--;
  timeTable.pop_back();

  int placedCount = 0;
  for (int i = 0; i < courseCount; ++i) {
    if (placed.at(i)) {
      ++placedCount;
    }
  }

  return placedCount == courseCount;
}

inline void Solution::checkValidity() {
  for (int dim = 0; dim < __DimensionCount; ++dim) {
    for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
      for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
        const unsigned long conflictSize = timeTable.at(dim).t[day][slot].currentCourse.conflictingCourses.size();
        for(int cSize = 0; cSize < conflictSize; ++cSize) {
          for(int otherDim = 0; otherDim < __DimensionCount; ++otherDim) {
            if(timeTable.at(otherDim).t[day][slot].currentCourse.code == timeTable.at(dim).t[day][slot].currentCourse.conflictingCourses.at(cSize)) {
              std::cout << "ILLEGAL " << timeTable.at(dim).t[day][slot].currentCourse.code << " conflicts with " << timeTable.at(dim).t[day][slot].currentCourse.conflictingCourses.at(cSize) << std::endl;
            }
          }
        }
      }
    }
  }
}

// calculate cost
int Solution::cost() {
    int cost = 0;

  for(int day = 0; day < (__Day7Needed ? 7 : 6); ++day) {
    for(int i = 0; i < __DimensionCount; ++i)
      for(int j = 0; j < TIMESLOTCOUNT; ++j) {

        if(timeTable.at(i).t[day][j].status == OCCUPIED)
          for(int l = 0; l < __DimensionCount; ++l)
            for(int k = 0; k < TIMESLOTCOUNT; ++k) {

              if(timeTable.at(l).t[day][k].status == OCCUPIED && timeTable.at(i).t[day][j].currentCourse.code != timeTable.at(l).t[day][k].currentCourse.code) {
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

    for(int dim = 0; dim < __DimensionCount; ++dim) {
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

inline void printTimeTable(const std::vector<Week>&timeTable) {
  const std::vector days = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
  };
  constexpr int dayOffset = 100000;

  std::vector<std::pair<int,std::string>> exams;

  for (int dim = 0; dim < __DimensionCount; dim++) {
    for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
      for (int t = 0; t < TIMESLOTCOUNT; t++) {
        if (timeTable.at(dim).t[day][t].status == OCCUPIED) {
          const int begin = t * TIMESLOTDURATION;
          const int end = begin + timeTable.at(dim).t[day][t].currentCourse.examDuration;
          t += timeTable.at(dim).t[day][t].currentCourse.examDuration / TIMESLOTDURATION - 1;
          const std::vector<Classroom>& classrooms = timeTable.at(dim).t[day][t].currentCourse.classrooms;
          char buf[512] = {0};
          std::string info;
          sprintf(buf, "%02d:%02d %s - ", 9 + begin / 60, begin % 60,
                  begin / 60 >= 3 ? "PM" : "AM");
          info += buf;
          sprintf(buf, "%02d:%02d %s: ", 9 + end / 60, end % 60,
                  end / 60 >= 3 ? "PM" : "AM");
          info += buf;
          sprintf(buf, "%-7s - ",
                  timeTable.at(dim).t[day][t].currentCourse.code.c_str());
          info += buf;
          sprintf(buf, "%-5s: ", classrooms.size() > 1 ? "Rooms" : "Room");
          info += buf;
          for (const auto &classroom : classrooms) {
            sprintf(buf, "%s ", classroom.id.c_str());
            info += buf;
          }
          info.shrink_to_fit();
          exams.emplace_back(day*dayOffset+t,info);
        }
      }
    }
  }

  std::sort(exams.begin(), exams.end(), [](const auto& exam1, const auto& exam2) {
      if (exam1.first < exam2.first) return true;
    return false;
  });

  int day = -1;
  for (const auto &[when, info] : exams) {
    if (when / dayOffset > day) {
      day = when / dayOffset;
      std::cout << "\n" << days[day] << "\n\n";
    }
    std::cout << info << std::endl;
  }

  std::cout << "\nBlocked Hours:\n   (missing feature)\n\n" << std::endl;
}