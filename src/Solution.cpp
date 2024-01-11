#include "Solution.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>

#include "Typedefs.h"

inline void printTimeTable(const std::vector<Week>&timeTable, const Vector2D<std::string>&blockedList);

inline std::string getRegularTime(const std::string&str);

inline std::string getRegularTime(int startTimeH, int startTimeM, int endTimeH, int endTimeM);

// constructor
Solution::Solution(vector<Course> list, vector<Classroom> classrooms, Vector2D<std::string> blockedHours) {
  this->courseList = list;
  this->classList = classrooms;
  this->blockedHours = blockedHours;
}

// pseudocode of simulated annealing
void Solution::Solve() {
  int newScore;
  int oldScore;
  int delta;

  //fill the sudoku table random
  initializeSchedule();
  oldScore = cost(timeTable);

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

do{
  int iterationCount = 0;
  do {
    if(!isFilled) {
      /* initialize everything necessary */
      std::cout << iterationCount << std::endl;
      ++iterationCount;
      __DimensionCount = 1;
      timeTable.clear();
      // mark all courses as "not placed"
      for (unsigned long i = 0; i < courseCount; ++i) {
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
  }while(!isFilled && iterationCount < ITERATIONCOUNT);

  /* Unlock Day 7 and try again */
  if(!__Day7Needed && !isFilled) {
    __Day7Needed = true;
    std::cout << "DAY 7 UNLOCKED!" << std::endl;
    /* invoke algorithm again after unlocking 7th day */
  }
  else
    break;
}while(true);

  /* FAILED */
  if(__Day7Needed && !isFilled) {
    std::cout << "COULDN'T FILL TABLE AT ALL, TERMINATING!";
    exit(1);
  }

  /* TODO
     * Simulated Annealing may have to be done before assignment of classrooms xo
   */

  /* assign classrooms to exams */
  const unsigned int classListSize = courseList.size();
  for(int day = 0; day < (__Day7Needed ? 7 : 6); ++day){
    for( int slot = 0; slot < TIMESLOTCOUNT; ++slot){
      for(int dim = 0; dim < __DimensionCount; ++dim) {
        for(unsigned long cSize = 0; cSize < classListSize; ++cSize) {
          /* check if a course does not have any classrooms assigned yet */
          if(timeTable.at(dim).t[day][slot].currentCourse.classrooms.empty()) {
            /* find empty classrooms in given time interval */
            const int end = slot + (timeTable.at(dim).t[day][slot].currentCourse.examDuration / TIMESLOTDURATION);
            std::vector<Classroom> availableClassrooms = getAvailableClassrooms(day, slot, end);
            /* sort classrooms from in ascending order according to their capacity */
            std::sort(availableClassrooms.begin(), availableClassrooms.end(), [](const Classroom&c1, const Classroom&c2) {
              return c1.capacity < c2.capacity;
            });

            const unsigned long classSize = availableClassrooms.size();
            const unsigned long courseStudentCount = timeTable.at(dim).t[day][slot].currentCourse.studentCount;
            bool courseCanFitInOneClassroom = false;
            /* Assign a single classroom if half of its capacity fits the number of students */
            for(unsigned long cIndex = 0; cIndex < classSize; ++cIndex) {
              //const unsigned long classRoomCount = timeTable.at(dim).t[day][slot].currentCourse.classrooms.size();
              if(availableClassrooms.at(cIndex).capacity/2 >= (int)courseStudentCount) {
                timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(cIndex));
                courseCanFitInOneClassroom = true;
                break;
              }
            }
            /* Assign multiple classrooms to course */
            if(!courseCanFitInOneClassroom) {
              unsigned int assignedCapacity = 0;
              for(int i = (int)classSize - 1; assignedCapacity < courseStudentCount && i >= 0; --i) {
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
  for (int d = 0; d < (int)__DimensionCount; ++d) {
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

  printTimeTable(timeTable, blockedHours);

  int placedCount = 0;
  for (unsigned long i = 0; i < courseCount; ++i) {
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
  for (int dim = 0; dim < (int)__DimensionCount; ++dim) {
    if(timeTable.size() < (unsigned long)__DimensionCount)
      this->timeTable.push_back(Week{});
    setBlockedHours(blockedHours, dim);
    bool currentDimensionIsEmpty = true;
    for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
      for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
        // ... find a course to place contiguously
        for (unsigned long c = 0; c < courseCount; ++c) {
          if (placed.at(c)) continue;
          const int courseTimePartition = courseList.at(c).examDuration / TIMESLOTDURATION;
          bool canFit = true;
          bool conflicts = false;
          bool sameProfessor = false;

          // check if exam can fit ...
          for (int i = 0; i < (int)courseTimePartition; ++i) {
            if (slot + i >= TIMESLOTCOUNT || timeTable.at(dim).t[day][slot + i].status != AVAILABLE || timeTable.at(dim).t[day][slot + i].status == BLOCKED) {

              canFit = false;
              break;
            }

            for(int otherDim = 0; otherDim < (int)__DimensionCount; ++otherDim) {
              if(dim != otherDim) {
                for(std::string& conflict: courseList.at(c).conflictingCourses) {
                  if(timeTable.at(otherDim).t[day][slot+i].currentCourse.code == conflict) {
                    //std::cout << timeTable.at(otherDim).t[day][slot].currentCourse.code << std::endl;
                    conflicts = true;
                    break;
                  }
                }
                  if(timeTable.at(otherDim).t[day][slot+i].currentCourse.professorName == timeTable.at(dim).t[day][slot+i].currentCourse.professorName){
                    sameProfessor = true;
                    break;
                }
              }
            }
          }
          // ... if so, place it
          if (canFit && !conflicts && !sameProfessor) {
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

  unsigned long placedCount = 0;
  for (unsigned long i = 0; i < courseCount; ++i) {
    if (placed.at(i)) {
      ++placedCount;
    }
  }

  return placedCount == courseCount;
}

void Solution::setBlockedHours(Vector2D<std::string> blockedList, const int dim) {
  for(unsigned long day = 0; day < blockedList.size(); ++day) {
    for(unsigned long d = 1; d < blockedList.at(day).size(); d=d+2) {
      // Parse and set blocked hours
      int startTimeH, endTimeH, startTimeM, endTimeM;
      startTimeH = std::stoi(blockedList.at(day).at(d).substr(0, 2));
      startTimeM = std::stoi(blockedList.at(day).at(d).substr(3, 2));
      endTimeH = std::stoi(blockedList.at(day).at(d).substr(6, 2));
      endTimeM = std::stoi(blockedList.at(day).at(d).substr(9, 2));

      int startTimeSlot = ((startTimeH - 9) * 60 + startTimeM) / TIMESLOTDURATION;
      int endTimeSlot = (((endTimeH - 9) * 60 + endTimeM )/ TIMESLOTDURATION) - 1;
      //std::cout << blockedList.at(day).at(d) << std::endl;
      //std::cout << "startTimeSlot: " << startTimeSlot << " endTimeSlot: " << endTimeSlot << std::endl;

      // Marking the timeslots as "Blocked"
      for (int timeslot = startTimeSlot; timeslot <= endTimeSlot; ++timeslot) {
        timeTable[dim].t[day][timeslot].status = BLOCKED;
        timeTable[dim].t[day][timeslot].currentCourse = Course{"UNSPECIFIED", blockedList.at(day).at(d+1), 5*(1+endTimeSlot-startTimeSlot)};
      }
    }
  }
}


inline void Solution::checkValidity() {
  for (int dim = 0; dim < __DimensionCount; ++dim) {
    for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
      for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
        const unsigned long conflictSize = timeTable.at(dim).t[day][slot].currentCourse.conflictingCourses.size();
        for(unsigned long cSize = 0; cSize < conflictSize; ++cSize) {
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
int Solution::cost(vector<Week> table) {
  int cost = 0;

  for(int day = 0; day < (__Day7Needed ? 7 : 6); ++day) {
    for(int i = 0; i < __DimensionCount; ++i)
      for(int j = 0; j < TIMESLOTCOUNT; ++j) {

        if(table.at(i).t[day][j].status == OCCUPIED)
          for(int l = 0; l < __DimensionCount; ++l)
            for(int k = 0; k < TIMESLOTCOUNT; ++k) {

              if(table.at(l).t[day][k].status == OCCUPIED && table.at(i).t[day][j].currentCourse.code != table.at(l).t[day][k].currentCourse.code) {

                //                check for same instructor
                if(table.at(i).t[day][j].currentCourse.professorName
                    == table.at(l).t[day][k].currentCourse.professorName) {
                  ++cost;
                }

                //                check for same students
                for(auto &a : table.at(i).t[day][j].currentCourse.conflictingCourses) {
                  if(a == table.at(l).t[day][k].currentCourse.code)
                    ++cost;
                }

                //                check for same class years (CENG101 - CENG102)
                if(table.at(i).t[day][j].currentCourse.code.at(4) == table.at(l).t[day][k].currentCourse.code.at(4))
                  ++cost;
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
  const unsigned long cSize = classList.size();
  std::vector<Classroom> result;
  std::unordered_map<std::string, bool> availableClassrooms;
  for(Classroom& c : classList) {
    availableClassrooms.emplace(c.id, true);
  }

  for(int dim = 0; dim < __DimensionCount; ++dim) {
    for(int i = start; i < end; ++i) {
      if(timeTable.at(dim).t[day][i].status == OCCUPIED) {
        const unsigned long size = timeTable.at(dim).t[day][i].currentCourse.classrooms.size();
        for(unsigned long j = 0; j < size; ++j) {
          availableClassrooms[timeTable.at(dim).t[day][i].currentCourse.classrooms.at(j).id] = false;
        }
      }
    }
  }

  for (const pair<const string, bool>& available : availableClassrooms) {
    if(available.second) {
      for(unsigned long i = 0; i < cSize; ++i) {
        if(classList.at(i).id == available.first) {
          result.push_back(classList.at(i));
        }
      }
    }
  }
  return result;
}

inline void printTimeTable(const std::vector<Week>&timeTable, const Vector2D<std::string>&blockedList) {
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
          std::string time = getRegularTime(9 + begin / 60, begin % 60, 9 + end / 60, end % 60);
          std::string info;
          sprintf(buf, "%s ", time.c_str());
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

  std::cout << "\nBlocked Hours:\n" << std::endl;

  for(const auto &row : blockedList) {
    if(row.size() > 1){
      std::cout << row.at(0) << std::endl;
      for(unsigned long i = 1; i < row.size(); i=i+2){
        std::cout << getRegularTime(row.at(i)) << "(" << row.at(i+1) << ")" << std::endl;
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

inline std::string getRegularTime(const std::string&str){
  int startTimeH, endTimeH, startTimeM, endTimeM;
  startTimeH = std::stoi(str.substr(0, 2));
  startTimeM = std::stoi(str.substr(3, 2));
  endTimeH = std::stoi(str.substr(6, 2));
  endTimeM = std::stoi(str.substr(9, 2));

  bool startAM = true;
  bool endAM = true;
  std::string startH = to_string(startTimeH);
  std::string endH = to_string(endTimeH);
  std::string startM = to_string(startTimeM);
  std::string endM = to_string(endTimeM);

  if(startTimeH > 12){
    startAM = false;
    startTimeH = startTimeH - 12;
  }
  if(endTimeH > 12){
    endAM = false;
    endTimeH = endTimeH - 12;
  }
  if(startTimeH < 10){
    startH = "";
    //startH.append("0");
    startH.append(to_string(startTimeH));
  }
  if(endTimeH < 10){
    endH = "";
    //endH.append("0");
    endH.append(to_string(endTimeH));
  }
  if(startTimeM < 10){
    startM = "";
    startM.append("0");
    startM.append(to_string(startTimeM));
  }
  if(endTimeM < 10){
    endM = "";
    endM.append("0");
    endM.append(to_string(endTimeM));
  }

  std::string result = "";
  result.append(startH);
  result.append(":");
  result.append(startM);
  result.append(((startAM) ? " AM " : " PM "));
  result.append("- ");
  result.append(endH);
  result.append(":");
  result.append(endM);
  result.append(((endAM) ? " AM: Common Course " : " PM: Common Course "));
  return result;

}

inline std::string getRegularTime(int startTimeH, int startTimeM, int endTimeH, int endTimeM){
  bool startAM = true;
  bool endAM = true;
  std::string startH = to_string(startTimeH);
  std::string endH = to_string(endTimeH);
  std::string startM = to_string(startTimeM);
  std::string endM = to_string(endTimeM);

  if(startTimeH > 12){
    startAM = false;
    startTimeH = startTimeH - 12;
  }
  if(endTimeH > 12){
    endAM = false;
    endTimeH = endTimeH - 12;
  }
  if(startTimeH < 10){
    startH = "";
    //startH.append("0");
    startH.append(to_string(startTimeH));
  }
  if(endTimeH < 10){
    endH = "";
    //endH.append("0");
    endH.append(to_string(endTimeH));
  }
  if(startTimeM < 10){
    startM = "";
    startM.append("0");
    startM.append(to_string(startTimeM));
  }
  if(endTimeM < 10){
    endM = "";
    endM.append("0");
    endM.append(to_string(endTimeM));
  }

  std::string result = "";
  result.append(startH);
  result.append(":");
  result.append(startM);
  result.append(((startAM) ? " AM " : " PM "));
  result.append("- ");
  result.append(endH);
  result.append(":");
  result.append(endM);
  result.append(((endAM) ? " AM: " : " PM:"));
  return result;
}