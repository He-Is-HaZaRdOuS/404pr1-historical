#include "Solution.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_map>
#include <limits>

#include "Typedefs.h"

/* function prototypes */
inline void printTimeTable(const std::vector<Week>&timeTable, const Vector2D<std::string>&blockedList);

inline std::string getRegularTime(const std::string&str);

inline std::string getRegularTime(int startTimeH, int startTimeM, int endTimeH, int endTimeM);

/* constructor */
Solution::Solution(vector<Course> list, vector<Classroom> classrooms, Vector2D<std::string> blockedHours) {
  this->courseList = list;
  this->classList = classrooms;
  this->blockedHours = blockedHours;
}

/* Generates N-many schedules and picks the best one out of the bunch */
void Solution::Solve(const int nValue) {
  vector<Week> newSchedule;
  int N = nValue;
  int F = 0;
  const int LOWER_LIMIT = 10;
  std::vector<scheduleStatus> schedules;
  schedules.reserve(N);

  for(int n = 0; n < N; ++n){
      schedules.push_back(initializeSchedule()); // make new random schedule
      if(schedules.at(n).cost == -2){ // failed to make schedule
        ++F;
      }
      if(N-F < LOWER_LIMIT){ // less than 10 successful schedules out of N
        N=N+LOWER_LIMIT; // increase N to iterate more
        std::cout << "Most schedules so far have failed, trying " << LOWER_LIMIT << " more times." << std::endl;
      }
      schedules.at(n).cost = cost(schedules.at(n).schedule, schedules.at(n).dim);
      std::cout << "\rIteration Count: " << n+1 << "/" << N << std::flush;
  }
  std::cout << std::endl;

  bool day6exists = false;
  scheduleStatus min = schedules.at(0);
  for(int n = 0; n < N; ++n){
    if(!schedules.at(n).day) {
      day6exists = true;
      min = schedules.at(n);
    }
  }

  /* find best schedule with minimum cost and days needed */
  for(int n = 0; n < N; ++n){
    if(day6exists) {
      if(schedules.at(n).cost < min.cost && !schedules.at(n).day)
        min = schedules.at(n);
    }
    else {
      if(schedules.at(n).cost < min.cost)
        min = schedules.at(n);
    }
  }

  /* update global variables to those of the best schedule */
  __DimensionCount = min.dim;
  __Day7Needed = min.day;
  __placed = min.placed;
  timeTable = min.schedule;

    /* assign classrooms to exams */
    classroomStatus returnVal{true, 0};
    do{
      int newCapacity = 0;
      std::string id;
      if(!returnVal.returnStatus){
        resetAssignedClassrooms();
        if (returnVal.wantedCapacity != -1) {
          returnVal.wantedCapacity *= 2; // Leave empty seats between students
          AskForCapacityAgain:
          // enter classroom with size wantedCapacity
          std::cout << "PLEASE ENTER A NEW CLASSROOM WITH ID AND SIZE OF AT LEAST: " << returnVal.wantedCapacity << std::endl;
          std::cin >> id >> newCapacity;
          while (!std::cin.good()){
            std::cout << "PLEASE ENTER THE ID FIRST AND THE CAPACITY SECOND!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> id >> newCapacity;
          }
          if(newCapacity < returnVal.wantedCapacity){
            std::cout << "NEW CAPACITY MUST BE BIGGER THAN OR EQUAL TO WANTED CAPACITY" << std::endl;
            goto AskForCapacityAgain;
          }
          else{
            for(const Classroom&room : classList){
              if(room.id == id){
                std::cout << "NEW ID MUST BE DIFFERENT FROM ALREADY EXISTING CLASSROOMS" << std::endl;
                goto AskForCapacityAgain;
              }
            }
            classList.emplace_back(newCapacity, id);
          }
        } else {
          // enter any sized classroom
        AskForIDAgain:
          // enter classroom with size wantedCapacity
          std::cout << "PLEASE ENTER A NEW CLASSROOM WITH ID AND ANY SIZE" << std::endl;
          std::cin >> id >> newCapacity;
          while (!std::cin.good()){
            std::cout << "PLEASE ENTER THE ID FIRST AND THE CAPACITY SECOND!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> id >> newCapacity;
          }
            for(const Classroom&room : classList){
              if(room.id == id){
                std::cout << "NEW ID MUST BE DIFFERENT FROM ALREADY EXISTING CLASSROOMS" << std::endl;
                goto AskForIDAgain;
              }
            }
            classList.emplace_back(newCapacity, id);
        }
      }
      returnVal = assignClassrooms();
    }while(!returnVal.returnStatus);



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

    const unsigned long courseCount = courseList.size();
    int placedCount = 0;
    for (unsigned long i = 0; i < courseCount; ++i) {
        if (__placed.at(i)) {
            ++placedCount;
        }
        else {
            std::cout << "Couldn't place " << courseList.at(i).code << std::endl;
        }

    }
    std::cout << "Dimensions:  " << __DimensionCount << std::endl;

    std::cout << "Number of days: " << (__Day7Needed ? 7 : 6) << std::endl;

    std::cout << "Placed course count: " << placedCount << std::endl;

    std::cout << "Cost: " << min.cost << std::endl;

    if(checkValidityPrint(timeTable))
      std::cout << "Finished" << std::endl;

}

// make first schedule according to a logic
inline scheduleStatus Solution::initializeSchedule() {

     bool day7Needed = false;
     int dimensionCount = 1;
     int returnValue = -1;
     std::vector<bool> placed;
     std::vector<Week> schedule;

  const unsigned long courseCount = courseList.size();
  placed = std::vector<bool>(courseCount);
  bool isFilled = false;

do{
  int iterationCount = 0;
  do {
    if(!isFilled) {
      /* initialize everything necessary */
      //std::cout << iterationCount << std::endl;
      ++iterationCount;
      dimensionCount = 1;
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
    isFilled = fillTable(schedule, courseCount, placed, dimensionCount, day7Needed);
  }while(!isFilled && iterationCount < ITERATIONCOUNT);

  /* Unlock Day 7 and try again */
  if(!day7Needed && !isFilled) {
    day7Needed = true;
    //std::cout << "DAY 7 UNLOCKED!" << std::endl;
    /* invoke algorithm again after unlocking 7th day */
  }
  else
    break;
}while(true);

  /* FAILED */
  if(day7Needed && !isFilled) {
    returnValue = -2;
    //std::cout << "COULDN'T FILL TABLE AT ALL, TERMINATING!";
    //exit(1);
  }
  /*
   * returnValue = -1 : SUCCESS
   * returnValue = -2 : FAILED (increment F)
   * */
  return scheduleStatus{schedule, placed, dimensionCount, returnValue, day7Needed};

}

/* fill table linearly without causing conflicts */
inline bool Solution::fillTable(std::vector<Week> &schedule, const unsigned long courseCount, std::vector<bool> &placed, int &dimensionCount, bool day7Needed) {
  // for each slot...
  for (int dim = 0; dim < (int)dimensionCount; ++dim) {
    if(schedule.size() < (unsigned long)dimensionCount)
      schedule.push_back(Week{});
    setBlockedHours(schedule, blockedHours, dim);
    bool currentDimensionIsEmpty = true;
    for (int day = 0; day < (day7Needed ? 7 : 6); day++) {
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
            if (slot + i >= TIMESLOTCOUNT || schedule.at(dim).t[day][slot + i].status != AVAILABLE || schedule.at(dim).t[day][slot + i].status == BLOCKED) {

              canFit = false;
              break;
            }

            for(int otherDim = 0; otherDim < (int)dimensionCount; ++otherDim) {
              if(dim != otherDim) {
                for(std::string& conflict: courseList.at(c).conflictingCourses) {
                  if(schedule.at(otherDim).t[day][slot+i].currentCourse.code == conflict) {
                    //std::cout << schedule.at(otherDim).t[day][slot].currentCourse.code << std::endl;
                    conflicts = true;
                    break;
                  }
                }
                  if(schedule.at(otherDim).t[day][slot+i].currentCourse.professorName == schedule.at(dim).t[day][slot+i].currentCourse.professorName){
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
              schedule.at(dim).t[day][slot+i].status = OCCUPIED;
              schedule.at(dim).t[day][slot+i].currentCourse = courseList.at(c);
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
      ++dimensionCount;
    }
  }
  /* get rid of empty dimension (dirty fix) */
  dimensionCount--;
  schedule.pop_back();

  unsigned long placedCount = 0;
  for (unsigned long i = 0; i < courseCount; ++i) {
    if (placed.at(i)) {
      ++placedCount;
    }
  }

  return placedCount == courseCount;
}

/* fill time slots of blocked hours and block them from being used */
void Solution::setBlockedHours(std::vector<Week> &schedule, Vector2D<std::string> blockedList, const int dim) {
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
        schedule[dim].t[day][timeslot].status = BLOCKED;
        schedule[dim].t[day][timeslot].currentCourse = Course{"UNSPECIFIED", blockedList.at(day).at(d+1), 5*(1+endTimeSlot-startTimeSlot)};
      }
    }
  }
}


/* check the entire schedule for conflicting courses (same student or same professor at the same time) */
inline bool Solution::checkValidityPrint(vector<Week> &table) {
    bool t = true;
    for (int dim = 0; dim < __DimensionCount; ++dim) {
        for (int day = 0; day < (__Day7Needed ? 7 : 6); day++) {
            for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
                const unsigned long conflictSize = table.at(dim).t[day][slot].currentCourse.conflictingCourses.size();
                for(unsigned long cSize = 0; cSize < conflictSize; ++cSize) {
                    for(int otherDim = 0; otherDim < __DimensionCount; ++otherDim) {
                        if(dim != otherDim && table.at(otherDim).t[day][slot].status == OCCUPIED && table.at(dim).t[day][slot].status == OCCUPIED)
                        if(table.at(otherDim).t[day][slot].currentCourse.code == table.at(dim).t[day][slot].currentCourse.conflictingCourses.at(cSize)) {
                            std::cout << "ILLEGAL " << table.at(otherDim).t[day][slot].currentCourse.code << " conflicts with " << table.at(dim).t[day][slot].currentCourse.conflictingCourses.at(cSize) << std::endl;
                            t = false;
                        }
                    }
                }
            }
        }
    }

    return t;
}

/* return an integer representing how bad this schedule is for certain students */
/* cost is incremented when a student has more than one exam on the same day */
int Solution::cost(vector<Week> &table, int &dim) {
    int cost = 0;

    for (int day = 0; day < (__Day7Needed ? 7 : 6); ++day) {
        std::vector<Course> dayList = extractCoursesFromDay(table, day, dim);
        const unsigned long dayListSize = dayList.size();
        for (unsigned long c1Index = 0; c1Index < dayListSize; ++c1Index) {
            const unsigned long c1SSize = dayList.at(c1Index).studentCount;
            for (unsigned long s1Index = 0; s1Index < c1SSize; ++s1Index) {
                for (unsigned long c2Index = c1Index + 1; c2Index < dayListSize; ++c2Index) {
                    if (std::binary_search(dayList.at(c2Index).studentList.begin(),
                                           dayList.at(c2Index).studentList.end(),
                                           dayList.at(c1Index).studentList.at(s1Index))) {
                        ++cost;
                    }
                }

            }

        }

    }

  return cost;
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

/* print the information in a neat fashion */
inline void printTimeTable(const std::vector<Week>&timeTable, const Vector2D<std::string>&blockedList) {
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
      std::cout << "\n" << __DAYS[day] << "\n\n";
    }
    std::cout << info << std::endl;
  }

  std::cout << "\nBlocked Hours: ";

  int blockedDay = 0;
  bool blockedExists = false;
  for(const auto &row : blockedList) {
    if(row.size() > 1){
      if(!blockedExists){
        blockedExists = true;
        std::cout << std::endl;
      }
      std::cout << __DAYS[blockedDay] << std::endl;
      for(unsigned long i = 1; i < row.size(); i=i+2){
        std::cout << getRegularTime(row.at(i)) << "(" << row.at(i+1) << ")" << std::endl;
      }
      std::cout << std::endl;
    }
    ++blockedDay;
  }
  if(!blockedExists){
    std::cout << "None" << std::endl;
  }

}

/* convert army time to regular time (00:00-24:00 -> 12:00 AM/PM) */
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

/* convert army time to regular time (00:00-24:00 -> 12:00 AM/PM) */
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

/* assign unused classrooms to timeslots */
inline classroomStatus Solution::assignClassrooms(){
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
            // return false if no classrooms are available
            if (availableClassrooms.empty()) {
              return classroomStatus{false, -1};
            }

            /* sort classrooms from in ascending order according to their capacity */
            std::sort(availableClassrooms.begin(), availableClassrooms.end(),
                      [](const Classroom &c1, const Classroom &c2)
                      {
                        return c1.capacity < c2.capacity;
                      });

            const unsigned long classSize = availableClassrooms.size();
            const unsigned long courseStudentCount = timeTable.at(dim).t[day][slot].currentCourse.studentCount;
            bool courseCanFitInOneClassroom = false;
            /* Assign a single classroom if half of its capacity fits the number of students */
            for (unsigned long cIndex = 0; cIndex < classSize; ++cIndex) {
              // const unsigned long classRoomCount = timeTable.at(dim).t[day][slot].currentCourse.classrooms.size();
              if (availableClassrooms.at(cIndex).capacity / 2 >= (int)courseStudentCount) {
                timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(cIndex));
                courseCanFitInOneClassroom = true;
                break;
              }
            }
            /* Assign multiple classrooms to course */
            if (!courseCanFitInOneClassroom){
              unsigned int assignedCapacity = 0;
              for (int i = (int)classSize - 1; assignedCapacity < courseStudentCount && i >= 0; --i) {
                // assign biggest available classroom and update variable
                assignedCapacity = assignedCapacity + (availableClassrooms.at(i).capacity / 2);
                timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(i));
                for (int j = 0; j < i; ++j) {
                  if ((availableClassrooms.at(j).capacity / 2) + assignedCapacity >= courseStudentCount) {
                    assignedCapacity = assignedCapacity + (availableClassrooms.at(j).capacity / 2);
                    timeTable.at(dim).t[day][slot].currentCourse.classrooms.push_back(availableClassrooms.at(j));
                    break;
                  }
                }
              }
              // başaramadık abi
              if (assignedCapacity < courseStudentCount) {
                int r = courseStudentCount - assignedCapacity;
                return classroomStatus{false, r};
              }
            }
          }
        }
      }
    }
  }
  return classroomStatus{true, 0};
}

/* erase assigned classrooms from timeslots */
void Solution::resetAssignedClassrooms() {
  for (int dim = 0; dim < __DimensionCount; ++dim){
    for(int day = 0; day < (__Day7Needed ? 7 : 6); ++day){
      for (int slot = 0; slot < TIMESLOTCOUNT; ++slot) {
        timeTable.at(dim).t[day][slot].currentCourse.classrooms.clear();
      }
    }
  }
}

/* return a list of courses from a specific day */
inline vector<Course> Solution::extractCoursesFromDay(std::vector<Week> &table, int &day, int &dimensionCount){
    std::vector<Course> list;
    for(int dim = 0; dim < dimensionCount; ++dim){
        for(int slot = 0; slot < TIMESLOTCOUNT; ++slot){
            // fetch course and skip ahead
            list.push_back(table.at(dim).t[day][slot].currentCourse);
            slot = slot + table.at(dim).t[day][slot].currentCourse.examDuration / TIMESLOTDURATION;
            //std::cout << "SKIPPED SLOT: " << slot << std::endl;
        }
    }
    return list;
}
