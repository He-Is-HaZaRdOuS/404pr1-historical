#include "Solution.h"

#include <Main.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>
#include <unordered_map>

/* function prototypes */
inline void printTimeTable(const std::vector<n_Solution::Week>&timeTable, const Vector2D<std::string>&blockedList);

inline std::string getRegularTime(const std::string&str);

inline std::string getRegularTime(uint8_t startTimeH, uint8_t startTimeM, uint8_t endTimeH, uint8_t endTimeM);

/* constructor */
Solution::Solution(const std::vector<Course>& list, const std::vector<Classroom>& classrooms, const Vector2D<std::string>& blockedHours)
  : classList(classrooms), courseList(list), blockedHours(blockedHours) {
}

/* Generates N-many schedules and picks the best one out of the bunch */
void Solution::Solve(const int32_t nValue) {
  uint64_t N = nValue;
  uint64_t F = 0;
  const uint64_t LOWER_LIMIT = N / 2;
  n_Solution::scheduleStatus minSchedule;

  minSchedule = initializeSchedule();
  minSchedule.cost = cost(minSchedule.schedule, minSchedule.dim);

  for(uint64_t n = 0; n < N; ++n){
    n_Solution::scheduleStatus newSchedule;
    newSchedule = initializeSchedule(); // make new random schedule
    if(newSchedule.cost == -2){ // failed to make schedule
      ++F;
    }
    if(N-F < LOWER_LIMIT){
      N=N+LOWER_LIMIT; // increase N to iterate more
      std::cout << "Most schedules so far have failed, trying " << LOWER_LIMIT << " more times." << std::endl;
    }
    newSchedule.cost = cost(newSchedule.schedule, newSchedule.dim);
    // both are either 6 days or 7 days
    if(!(newSchedule.day ^ minSchedule.day)) {
      if(newSchedule.cost < minSchedule.cost)
        minSchedule = newSchedule;
    }
    // if minSchedule uses 7 days, then newSchedule used only 6, so update the minimum
    else if(minSchedule.day) {
        minSchedule = newSchedule;
    }

    std::cout << "\rIteration Count: " << n+1 << "/" << N << std::flush;
  }
  std::cout << std::endl;

  /* update global variables to those of the best schedule */
  n_Solution::dimensionCount = minSchedule.dim;
  n_Solution::sundayUnlocked = minSchedule.day;
  n_Solution::placed = minSchedule.placed;
  timeTable = minSchedule.schedule;

    /* assign classrooms to exams */
    n_Solution::classroomStatus returnVal{0, true};
    do {
      if (!returnVal.returnStatus) {
        std::string id;
        int32_t newCapacity = 0;
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
          for(const Classroom&room : classList){
            if(room.m_id == id){
              std::cout << "NEW ID MUST BE DIFFERENT FROM ALREADY EXISTING CLASSROOMS" << std::endl;
              goto AskForCapacityAgain;
            }
          }
          classList.emplace_back(newCapacity, id);

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
              if(room.m_id == id){
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
  for (uint32_t d = 0; d < n_Solution::dimensionCount; ++d) {
    printf("Dimension: %d\n", d);
    for (uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot)
      printf("%02d:%02d %10s %10s %10s %10s %10s %10s %10s\n",
             9 + (slot * n_Solution::TIMESLOTDURATION) / 60, (slot * n_Solution::TIMESLOTDURATION) % 60,
             timeTable.at(d).week[0][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[1][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[2][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[3][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[4][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[5][slot].m_currentCourse.m_code.c_str(),
             timeTable.at(d).week[6][slot].m_currentCourse.m_code.c_str()
      );
  }


printTimeTable(timeTable, blockedHours);

    const uint64_t courseCount = courseList.size();
    uint64_t placedCount = 0;
    for (uint64_t index = 0; index < courseCount; ++index) {
        if (n_Solution::placed.at(index)) {
            ++placedCount;
        }
        else {
            std::cout << "Couldn't place " << courseList.at(index).m_code << std::endl;
        }

    }
    std::cout << "Dimensions:  " << n_Solution::dimensionCount << std::endl;

    std::cout << "Number of days: " << (n_Solution::sundayUnlocked ? 7 : 6) << std::endl;

    std::cout << "Placed course count: " << placedCount << std::endl;

    std::cout << "Cost: " << minSchedule.cost << std::endl;

    if(checkValidityPrint(timeTable))
      std::cout << "Finished" << std::endl;

}

// make first schedule according to a logic
inline n_Solution::scheduleStatus Solution::initializeSchedule() {

     bool sundayNeeded = false;
     uint32_t dimensionCount = 1;
     int64_t returnValue = -1;
     std::vector<bool> placed;
     std::vector<n_Solution::Week> schedule;

  const uint64_t courseCount = courseList.size();
  placed = std::vector<bool>(courseCount);
  bool isFilled = false;

do{
  uint8_t iterationCount = 0;
  do {
    if(!isFilled) {
      /* initialize everything necessary */
      //std::cout << iterationCount << std::endl;
      ++iterationCount;
      dimensionCount = 1;
      // mark all courses as "not placed"
      for (uint32_t index = 0; index < courseCount; ++index) {
        placed.at(index) = false;
      }
      /* shuffle vectors and try again */
      auto rd = std::random_device {};
      auto rng = std::default_random_engine { rd() };
      std::shuffle(std::begin(courseList), std::end(courseList), rng);
      std::shuffle(std::begin(placed), std::end(placed), rng);
    }
    /* invoke algorithm */
    isFilled = fillTable(schedule, courseCount, placed, dimensionCount, sundayNeeded);
  }while(!isFilled && iterationCount < n_Solution::ITERATIONCOUNT);

  /* Unlock Day 7 and try again */
  if(!sundayNeeded && !isFilled) {
    sundayNeeded = true;
    /* invoke algorithm again after unlocking 7th day */
  }
  else
    break;
}while(true);

  /* FAILED */
  if(sundayNeeded && !isFilled) {
    returnValue = -2;
  }
  /*
   * returnValue = -1 : SUCCESS
   * returnValue = -2 : FAILED (increment F)
   * */
  return n_Solution::scheduleStatus{schedule, dimensionCount, returnValue, placed, sundayNeeded};

}

/* fill table linearly without causing conflicts */
inline bool Solution::fillTable(std::vector<n_Solution::Week> &schedule, const uint64_t courseCount, std::vector<bool> &placed, uint32_t &dimensionCount, bool sundayUnlocked) const {
  // for each slot...
  for (uint32_t dim = 0; dim < dimensionCount; ++dim) {
    if(schedule.size() < dimensionCount)
      schedule.push_back(n_Solution::Week{});
    setBlockedHours(schedule, blockedHours, dim);
    bool currentDimensionIsEmpty = true;
    for (uint8_t day = 0; day < (sundayUnlocked ? 7 : 6); ++day) {
      for (uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot) {
        // ... find a course to place contiguously
        for (uint32_t c = 0; c < courseCount; ++c) {
          if (placed.at(c)) continue;
          const uint32_t courseTimePartition = courseList.at(c).m_examDuration / n_Solution::TIMESLOTDURATION;
          bool canFit = true;
          bool blocked = false;
          bool conflicts = false;
          uint32_t skipAmount = 0;

          // check if exam can fit ...
          for (uint32_t index = 0; index < courseTimePartition; ++index) {
            if (slot + index >= n_Solution::TIMESLOTCOUNT){
              canFit = false;
              break;
            }

            if(schedule.at(dim).week[day][slot + index].m_status != n_Timeslot::TimeSlotStatus::AVAILABLE || schedule.at(dim).week[day][slot + index].m_status == n_Timeslot::TimeSlotStatus::BLOCKED) {
              blocked = true;
              skipAmount = schedule.at(dim).week[day][slot + index].m_currentCourse.m_examDuration / n_Solution::TIMESLOTDURATION;
              break;
            }

            for(uint32_t otherDim = 0; otherDim < dimensionCount; ++otherDim) {
              if(dim != otherDim && courseList.at(c).m_conflictingCourses.count(schedule.at(otherDim).week[day][slot+index].m_currentCourse.m_code)) {
                conflicts = true;
                break;
              }
            }
            if(conflicts) {
              break;
            }
          }

          if(blocked) {
            slot = slot + skipAmount - 1;
            break;
          }
          if(!canFit || conflicts) {
            continue;
          }

          // ... if so, place it
          placed.at(c) = true;
          for (uint32_t index = 0; index < courseTimePartition; ++index) {
            currentDimensionIsEmpty = false;
            schedule.at(dim).week[day][slot+index].m_status = n_Timeslot::TimeSlotStatus::OCCUPIED;
            schedule.at(dim).week[day][slot+index].m_currentCourse = courseList.at(c);
          }
        }
      }
    }
    if(!currentDimensionIsEmpty) {
      /* increment dimension */
      ++dimensionCount;
    }
  }
  /* get rid of empty dimension (dirty fix) */
  --dimensionCount;
  schedule.pop_back();

  uint64_t placedCount = 0;
  for (uint64_t index = 0; index < courseCount; ++index) {
    if (placed.at(index)) {
      ++placedCount;
    }
  }
  /* assert all courses have been placed */
  return placedCount == courseCount;
}

/* fill time slots of blocked hours and block them from being used */
void Solution::setBlockedHours(std::vector<n_Solution::Week> &schedule, const Vector2D<std::string> &blockedList, const uint32_t dim) {
  const uint8_t blockedSize = blockedList.size();
  for(uint8_t day = 0; day < blockedSize; ++day) {
    const uint32_t blockedIntervalCnt = blockedList.at(day).size();
    for(uint32_t d = 1; d < blockedIntervalCnt; d=d+2) {
      // Parse and set blocked hours
      uint8_t startTimeH, endTimeH, startTimeM, endTimeM;
      startTimeH = std::stoi(blockedList.at(day).at(d).substr(0, 2));
      startTimeM = std::stoi(blockedList.at(day).at(d).substr(3, 2));
      endTimeH = std::stoi(blockedList.at(day).at(d).substr(6, 2));
      endTimeM = std::stoi(blockedList.at(day).at(d).substr(9, 2));

      int32_t startTimeSlot = ((startTimeH - 9) * 60 + startTimeM) / n_Solution::TIMESLOTDURATION;
      int32_t endTimeSlot = (((endTimeH - 9) * 60 + endTimeM )/ n_Solution::TIMESLOTDURATION) - 1;
      //std::cout << blockedList.at(day).at(d) << std::endl;
      //std::cout << "startTimeSlot: " << startTimeSlot << " endTimeSlot: " << endTimeSlot << std::endl;

      // Marking the timeslots as "Blocked"
      for (uint32_t timeslot = startTimeSlot; timeslot <= endTimeSlot; ++timeslot) {
        schedule[dim].week[day][timeslot].m_status = n_Timeslot::TimeSlotStatus::BLOCKED;
        schedule[dim].week[day][timeslot].m_currentCourse = Course{"UNSPECIFIED", blockedList.at(day).at(d+1), static_cast<uint32_t>(5 * (1 + endTimeSlot - startTimeSlot))};
      }
    }
  }
}


/* check the entire schedule for conflicting courses (same student or same professor at the same time) */
inline bool Solution::checkValidityPrint(const std::vector<n_Solution::Week> &table) {
    bool week = true;
    for (uint32_t dim = 0; dim < n_Solution::dimensionCount; ++dim) {
        for (uint8_t day = 0; day < (n_Solution::sundayUnlocked ? 7 : 6); day++) {
            for (uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot) {
                for(uint32_t otherDim = 0; otherDim < n_Solution::dimensionCount; ++otherDim) {
                    if(dim != otherDim && table.at(otherDim).week[day][slot].m_status == n_Timeslot::TimeSlotStatus::OCCUPIED && table.at(dim).week[day][slot].m_status == n_Timeslot::TimeSlotStatus::OCCUPIED)
                      if(table.at(dim).week[day][slot].m_currentCourse.m_conflictingCourses.count(table.at(otherDim).week[day][slot].m_currentCourse.m_code)) {
                          std::cout << "ILLEGAL " << table.at(otherDim).week[day][slot].m_currentCourse.m_code << " conflicts with " << table.at(dim).week[day][slot].m_currentCourse.m_conflictingCourses.find(table.at(otherDim).week[day][slot].m_currentCourse.m_code)->first << std::endl;
                          week = false;
                      }
                }
            }
        }
    }

    return week;
}

/* return an integer representing how bad this schedule is for certain students */
/* cost is incremented when a student has more than one exam on the same day */
int64_t Solution::cost(const std::vector<n_Solution::Week> &table, const uint32_t dim) {
  int64_t cost = 0;

  for (uint8_t day = 0; day < (n_Solution::sundayUnlocked ? 7 : 6); ++day) {
    std::vector<Course> dayList = extractCoursesFromDay(table, day, dim);
    const uint32_t dayListSize = dayList.size();
    for (uint32_t c1Index = 0; c1Index < dayListSize; ++c1Index) {
      for (uint32_t c2Index = c1Index + 1; c2Index < dayListSize; ++c2Index) {
        const uint32_t c2SSize = dayList.at(c2Index).m_studentCount;
        for (uint32_t s2Index = 0; s2Index < c2SSize; ++s2Index) {
            if (std::binary_search(dayList.at(c1Index).m_studentList.begin(),
                                   dayList.at(c1Index).m_studentList.end(),
                                   dayList.at(c2Index).m_studentList.at(s2Index))) {
              ++cost;
                                 }
        }

      }

    }

  }

  return cost;
}

/* return a vector of non-intersecting classrooms for the given time interval */
inline std::vector<Classroom> Solution::getAvailableClassrooms(const uint8_t day, const uint8_t start, const uint32_t end) const {
  const uint32_t cSize = classList.size();
  std::vector<Classroom> result;
  std::unordered_map<std::string, bool> availableClassrooms;
  for(const Classroom& c : classList) {
    availableClassrooms.emplace(c.m_id, true);
  }

  for(uint32_t dim = 0; dim < n_Solution::dimensionCount; ++dim) {
    for(uint8_t index = start; index < end && index < n_Solution::TIMESLOTCOUNT - 1; ++index) {
      if(timeTable.at(dim).week[day][index].m_status == n_Timeslot::TimeSlotStatus::OCCUPIED) {
        const uint32_t size = timeTable.at(dim).week[day][index].m_currentCourse.m_classrooms.size();
        for(uint32_t roomIndex = 0; roomIndex < size; ++roomIndex) {
          availableClassrooms[timeTable.at(dim).week[day][index].m_currentCourse.m_classrooms.at(roomIndex).m_id] = false;
        }
      }
    }
  }

  for (const auto &[fst, snd] : availableClassrooms) {
    if(snd) {
      for(uint32_t index = 0; index < cSize; ++index) {
        if(classList.at(index).m_id == fst) {
          result.push_back(classList.at(index));
        }
      }
    }
  }
  return result;
}

/* print the information in a neat fashion */
inline void printTimeTable(const std::vector<n_Solution::Week>&timeTable, const Vector2D<std::string>&blockedList) {
  constexpr int32_t dayOffset = 100000;

  std::vector<std::pair<int32_t,std::string>> exams;

  for (uint32_t dim = 0; dim < n_Solution::dimensionCount; dim++) {
    for (uint8_t day = 0; day < (n_Solution::sundayUnlocked ? 7 : 6); day++) {
      for (uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; slot++) {
        if (timeTable.at(dim).week[day][slot].m_status == n_Timeslot::TimeSlotStatus::OCCUPIED) {
          const uint32_t begin = slot * n_Solution::TIMESLOTDURATION;
          const uint32_t end = begin + timeTable.at(dim).week[day][slot].m_currentCourse.m_examDuration;
          slot += timeTable.at(dim).week[day][slot].m_currentCourse.m_examDuration / n_Solution::TIMESLOTDURATION - 1;
          const std::vector<Classroom>& classrooms = timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms;
          char buf[512] = {0};
          std::string time = getRegularTime(9 + begin / 60, begin % 60, 9 + end / 60, end % 60);
          std::string info;
          sprintf(buf, "%s ", time.c_str());
          info += buf;
          sprintf(buf, "%-7s - ",
                  timeTable.at(dim).week[day][slot].m_currentCourse.m_code.c_str());
          info += buf;
          sprintf(buf, "%-5s: ", classrooms.size() > 1 ? "Rooms" : "Room");
          info += buf;
          for (const auto &classroom : classrooms) {
            sprintf(buf, "%s ", classroom.m_id.c_str());
            info += buf;
          }
          info.shrink_to_fit();
          exams.emplace_back(day * dayOffset + slot,info);
        }
      }
    }
  }

  std::sort(exams.begin(), exams.end(), [](const auto& exam1, const auto& exam2) {
    if (exam1.first < exam2.first) return true;
    return false;
  });

  int32_t day = -1;
  for (const auto &[when, info] : exams) {
    if (when / dayOffset > day) {
      day = when / dayOffset;
      std::cout << "\n" << n_Solution::DAYS[day] << "\n\n";
    }
    std::cout << info << std::endl;
  }

  std::cout << "\nBlocked Hours: ";

  uint8_t blockedDay = 0;
  bool blockedExists = false;
  for(const auto &row : blockedList) {
    if(row.size() > 1){
      if(!blockedExists){
        blockedExists = true;
        std::cout << std::endl;
      }
      std::cout << n_Solution::DAYS[blockedDay] << std::endl;
      for(uint32_t dayIndex = 1; dayIndex < row.size(); dayIndex=dayIndex+2){
        std::cout << getRegularTime(row.at(dayIndex)) << "(" << row.at(dayIndex+1) << ")" << std::endl;
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
  uint8_t startTimeH, endTimeH, startTimeM, endTimeM;
  startTimeH = std::stoi(str.substr(0, 2));
  startTimeM = std::stoi(str.substr(3, 2));
  endTimeH = std::stoi(str.substr(6, 2));
  endTimeM = std::stoi(str.substr(9, 2));

  bool startAM = true;
  bool endAM = true;
  std::string startH = std::to_string(startTimeH);
  std::string endH = std::to_string(endTimeH);
  std::string startM = std::to_string(startTimeM);
  std::string endM = std::to_string(endTimeM);

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
    startH.append(std::to_string(startTimeH));
  }
  if(endTimeH < 10){
    endH = "";
    //endH.append("0");
    endH.append(std::to_string(endTimeH));
  }
  if(startTimeM < 10){
    startM = "";
    startM.append("0");
    startM.append(std::to_string(startTimeM));
  }
  if(endTimeM < 10){
    endM = "";
    endM.append("0");
    endM.append(std::to_string(endTimeM));
  }

  std::string result;
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
inline std::string getRegularTime(uint8_t startTimeH, const uint8_t startTimeM, uint8_t endTimeH, const uint8_t endTimeM){
  bool startAM = true;
  bool endAM = true;
  std::string startH = std::to_string(startTimeH);
  std::string endH = std::to_string(endTimeH);
  std::string startM = std::to_string(startTimeM);
  std::string endM = std::to_string(endTimeM);

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
    startH.append(std::to_string(startTimeH));
  }
  if(endTimeH < 10){
    endH = "";
    //endH.append("0");
    endH.append(std::to_string(endTimeH));
  }
  if(startTimeM < 10){
    startM = "";
    startM.append("0");
    startM.append(std::to_string(startTimeM));
  }
  if(endTimeM < 10){
    endM = "";
    endM.append("0");
    endM.append(std::to_string(endTimeM));
  }

  std::string result;
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
inline n_Solution::classroomStatus Solution::assignClassrooms(){
  const uint64_t classListSize = courseList.size();
  for(uint8_t day = 0; day < (n_Solution::sundayUnlocked ? 7 : 6); ++day){
    for(uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot){
      for(uint32_t dim = 0; dim < n_Solution::dimensionCount; ++dim) {
        for(uint64_t cSize = 0; cSize < classListSize; ++cSize) {
          /* check if a course does not have any classrooms assigned yet */
          if(timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms.empty()) {
            /* find empty classrooms in given time interval */
            const uint32_t end = slot + (timeTable.at(dim).week[day][slot].m_currentCourse.m_examDuration / n_Solution::TIMESLOTDURATION);
            std::vector<Classroom> availableClassrooms = getAvailableClassrooms(day, slot, end);
            // return false if no classrooms are available
            if (availableClassrooms.empty()) {
              return n_Solution::classroomStatus{-1, false};
            }

            /* sort classrooms from in ascending order according to their capacity */
            std::sort(availableClassrooms.begin(), availableClassrooms.end(),
                      [](const Classroom &c1, const Classroom &c2)
                      {
                        return c1.m_capacity < c2.m_capacity;
                      });

            const uint64_t classSize = availableClassrooms.size();
            const uint64_t courseStudentCount = timeTable.at(dim).week[day][slot].m_currentCourse.m_studentCount;
            bool courseCanFitInOneClassroom = false;
            /* Assign a single classroom if half of its capacity fits the number of students */
            for (uint64_t cIndex = 0; cIndex < classSize; ++cIndex) {
              if (availableClassrooms.at(cIndex).m_capacity / 2 >= (courseStudentCount)) {
                timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms.push_back(availableClassrooms.at(cIndex));
                courseCanFitInOneClassroom = true;
                break;
              }
            }
            /* Assign multiple classrooms to course */
            if (!courseCanFitInOneClassroom){
              uint32_t assignedCapacity = 0;
              for (int64_t index = static_cast<int64_t>(classSize) - 1; assignedCapacity < courseStudentCount && index >= 0; --index) {
                // assign biggest available classroom and update variable
                assignedCapacity = assignedCapacity + (availableClassrooms.at(index).m_capacity / 2);
                timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms.push_back(availableClassrooms.at(index));
                for (uint64_t roomIndex = 0; roomIndex < index; ++roomIndex) {
                  if ((availableClassrooms.at(roomIndex).m_capacity / 2) + assignedCapacity >= courseStudentCount) {
                    assignedCapacity = assignedCapacity + (availableClassrooms.at(roomIndex).m_capacity / 2);
                    timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms.push_back(availableClassrooms.at(roomIndex));
                    break;
                  }
                }
              }
              // başaramadık abi
              if (assignedCapacity < courseStudentCount) {
                const int64_t r = static_cast<int64_t>(courseStudentCount) - assignedCapacity;
                return n_Solution::classroomStatus{r, false};
              }
            }
          }
        }
      }
    }
  }
  return n_Solution::classroomStatus{0, true};
}

/* erase assigned classrooms from timeslots */
void Solution::resetAssignedClassrooms() {
  for (uint32_t dim = 0; dim < n_Solution::dimensionCount; ++dim){
    for(uint8_t day = 0; day < (n_Solution::sundayUnlocked ? 7 : 6); ++day){
      for (uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot) {
        timeTable.at(dim).week[day][slot].m_currentCourse.m_classrooms.clear();
      }
    }
  }
}

/* return a list of courses from a specific day */
inline std::vector<Course> Solution::extractCoursesFromDay(const std::vector<n_Solution::Week> &table, const uint8_t day, const uint32_t dimensionCount){
    std::vector<Course> list;
    for(uint8_t dim = 0; dim < dimensionCount; ++dim){
        for(uint8_t slot = 0; slot < n_Solution::TIMESLOTCOUNT; ++slot){
            // fetch course and skip ahead
            list.push_back(table.at(dim).week[day][slot].m_currentCourse);
            slot = slot + table.at(dim).week[day][slot].m_currentCourse.m_examDuration / n_Solution::TIMESLOTDURATION;
            //std::cout << "SKIPPED SLOT: " << slot << std::endl;
        }
    }
    return list;
}
