#include "main.h"

#include <algorithm>
#include <Classroom.h>
#include <Course.h>
#include <unordered_map>
#include <fstream>
#include <cstring>
#include <iostream>
#include <array>

#include "CSV.h"
#include "Solution.h"
#include "Typedefs.h"

#define idColumn 0

#define capacityColumn 1

#define professorColumn 1
#define codeColumn 2
#define durationColumn 3

#define dayColumn 0
#define timeColumn 1
#define commonCourseColumn 2

const std::vector<std::string> __DAYS = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};

std::vector<Course> loadCourses(const char* path);

std::vector<Classroom> loadClassrooms(const char* path);

Vector2D<std::string> loadBlockedHours(const char* path);

std::vector<Course> findConflictingCourses(std::vector<Course> courseList);

int main() {
  std::vector<Classroom> classrooms = loadClassrooms(RESOURCES_PATH "classroom.csv");
  std::vector<Course> coursesDepartment = loadCourses(RESOURCES_PATH "course-list6.csv");
  Vector2D<std::string> blockedHours = loadBlockedHours(RESOURCES_PATH "bloc7676ked.csv");

  coursesDepartment = findConflictingCourses(coursesDepartment);

  std::sort(coursesDepartment.begin(), coursesDepartment.end(), [](const Course&c1, const Course&c2) {
    return (strcmp(c1.code.c_str(), c2.code.c_str()) > 0) ? true : false;
  });

  //Solution solution{coursesCENG, classrooms, blockedHours};
  Solution solution{coursesDepartment, classrooms, blockedHours};
  solution.initializeSchedule();
  solution.cost(solution.timeTable);

  return 0;
}

std::vector<Course> loadCourses(const char* path) {
  const CSV courseList{path};
  std::vector<Course> courses;
  const Vector2D<std::string> uniqueCourses = courseList.filter(codeColumn, [](const std::string&value) {
    static std::unordered_map<std::string, bool> seen;
    if (seen.find(value) == seen.end()) {
      seen.emplace(value, true);
      return true;
    }
    return false;
  });

  for (std::vector<std::string> row: uniqueCourses) {
    const unsigned long studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
    courses.emplace_back(row.at(professorColumn), row.at(codeColumn),
                         std::stoi(row.at(durationColumn)), studentCount);
  }


  for (auto&course : courses) {
    Vector2D<std::string> rows = courseList.filter(codeColumn, course.code);
    for (std::vector<std::string> row: rows) {
      int s = std::stoi(row.at(idColumn));
      course.studentList.push_back(s);
    }
    //std::sort(course.studentList.begin(), course.studentList.end());
  }

  return courses;
}

Vector2D<std::string> loadBlockedHours(const char* path) {
  CSV blockedList;
  try {
    CSV b{path};
    blockedList = b;
  }catch(...){
    Vector2D<std::string> emptyList;
    for(int i = 0; i < 7; ++i){
      std::vector<std::string> temp;
      temp.push_back(__DAYS.at(i));
      emptyList.push_back(temp);
    }
    return emptyList;
  }
  Vector2D<std::string> blockedDays = blockedList.filter(dayColumn, [](const std::string&value) {
    static std::unordered_map<std::string, bool> seen;
    if (seen.find(value) == seen.end()) {
      seen.emplace(value, true);
      return true;
    }
    return false;
  });

  const unsigned long dayCount = blockedDays.size();
  for(unsigned long i = 0; i <  dayCount; ++i) {
    //std::string t = blockedDays.at(i).at(blockedDays.at(i).size()-1);
    blockedDays.at(i).pop_back();
    blockedDays.at(i).pop_back();
    //blockedDays.at(i).push_back(t);
  }

  Vector2D<std::string> copy = blockedDays;

  for(auto&day : blockedDays) {
    transform(day.at(0).begin(), day.at(0).end(), day.at(0).begin(), static_cast<int (*)(int)>(&std::toupper));
    Vector2D<std::string> rows = blockedList.filter(dayColumn, day.at(0));
    for(std::vector<std::string> row : rows) {
      day.push_back(row.at(timeColumn));
      day.push_back(row.at(commonCourseColumn));
    }
  }

  const unsigned long size = blockedDays.size();
  std::vector<bool> valid(size, true);

  for(unsigned long i = 0; i < size; ++i) {
    transform(blockedDays.at(i).at(0).begin(), blockedDays.at(i).at(0).end(), blockedDays.at(i).at(0).begin(), static_cast<int (*)(int)>(&std::toupper));
    if (!(std::find(__DAYS.begin(), __DAYS.end(), blockedDays.at(i).at(0)) != __DAYS.end())){
      valid.at(i) = false;
    }
  }

  bool shouldExit = false;
  for(unsigned long i = 0; i < size; ++i){
    if(!valid.at(i)){
      shouldExit = true;
      std::cerr << "Input error: " << copy.at(i).at(0) << " is not a day of the week" << std::endl;
    }
  }

  if(shouldExit)
    exit(1);

  /* Error checking:
   * check if the given time interval is formatted correctly
   * check if given time intervals don't intersect with each other
   * */
  for(auto&day : blockedDays) {
    const unsigned long daySize = day.size();
    if(daySize > 3){
      for(unsigned long i = 1; i < daySize; i=i+2){
        std::string startTimeH1, endTimeH1, startTimeM1, endTimeM1;
        int startTimeSlot1, endTimeSlot1;
        try{
          startTimeH1 = day.at(i).substr(0, 2);
          startTimeM1 = day.at(i).substr(3, 2);
          endTimeH1 = day.at(i).substr(6, 2);
          endTimeM1 = day.at(i).substr(9, 2);

          if(startTimeH1.length() != 2 || startTimeM1.length() != 2 || endTimeH1.length() != 2 || endTimeM1.length() != 2){
            std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(i) << std::endl;
            std::cerr << "Time interval should be formatted as HH:MM-HH:MM" << std::endl;
            exit(1);
          }

          startTimeSlot1 = ((stoi(startTimeH1) - 9) * 60 + stoi(startTimeM1)) / TIMESLOTDURATION;
          endTimeSlot1 = (((stoi(endTimeH1) - 9) * 60 + stoi(endTimeM1))/ TIMESLOTDURATION) - 1;

          if(endTimeSlot1 < startTimeSlot1){
            std::cerr << std::endl << "Input error in: " << day.at(i) << "; End time cannot be equal or less than Start time." << std::endl;
            exit(1);
          }
          if(startTimeSlot1 < 0){
            std::cerr << std::endl << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
            exit(1);
          }
          if(endTimeSlot1 > 107){
            std::cerr << std::endl << "Timing error in " << day.at(0) << "; End time should not be later than 18:00." << std::endl;
            exit(1);
          }
        }
        catch(...){
          std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(i) << std::endl;
          exit(1);
        }

        for(unsigned long j = i + 2; j < daySize; j=j+2){
          if(i != j){
            std::string startTimeH2, endTimeH2, startTimeM2, endTimeM2;
            int  startTimeSlot2, endTimeSlot2;
            try{
              startTimeH2 = day.at(j).substr(0, 2);
              startTimeM2 = day.at(j).substr(3, 2);
              endTimeH2 = day.at(j).substr(6, 2);
              endTimeM2 = day.at(j).substr(9, 2);

              if(startTimeH2.length() == 1 || startTimeM2.length() == 1 || endTimeH2.length() == 1 || endTimeM2.length() == 1){
                std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(j) << std::endl;
                std::cerr << "Time interval should be formatted as HH:MM-HH:MM" << std::endl;
                exit(1);
              }

              startTimeSlot2 = ((stoi(startTimeH2) - 9) * 60 + stoi(startTimeM2)) / TIMESLOTDURATION;
              endTimeSlot2 = (((stoi(endTimeH2) - 9) * 60 + stoi(endTimeM2))/ TIMESLOTDURATION) - 1;

              if(endTimeSlot2 < startTimeSlot2){
                std::cerr << std::endl << "Input error in: " << day.at(j) << "; End time cannot be equal or less than Start time." << std::endl;
                exit(1);
              }
              if(startTimeSlot2 < 0){
                std::cerr << std::endl << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
                exit(1);
              }
              if(endTimeSlot2 > 107){
                std::cerr << std::endl << "Timing error in " << day.at(0) << "; End time should not be later than 18:00." << std::endl;
                exit(1);
              }
            }
            catch(...){
              std::cerr << std::endl << "Formatting error in: " << day.at(j) << std::endl;
              exit(1);
            }
            if(endTimeSlot1 >= startTimeSlot2 && startTimeSlot2 >= startTimeSlot1){
              std::cerr << std::endl << "Timing error in " << day.at(0) << "; Start time of an additional blocked interval must not intersect with End Time of previous intervals." << std::endl;
              exit(1);
            }
          }
        }
      }
    }
    else{
      std::string startTimeH, endTimeH, startTimeM, endTimeM;
      int startTimeSlot, endTimeSlot;
      try{
        startTimeH = day.at(1).substr(0, 2);
        startTimeM = day.at(1).substr(3, 2);
        endTimeH = day.at(1).substr(6, 2);
        endTimeM = day.at(1).substr(9, 2);

        if(startTimeH.length() == 1 || startTimeM.length() == 1 || endTimeH.length() == 1 || endTimeM.length() == 1){
          std::cerr << "Formatting error in " << day.at(0) << " at " << day.at(0) << std::endl;
          std::cerr << "Time interval should be formatted as HH:MM-HH:MM" << std::endl;
          exit(1);
        }

        startTimeSlot = ((stoi(startTimeH) - 9) * 60 + stoi(startTimeM)) / TIMESLOTDURATION;
        endTimeSlot = (((stoi(endTimeH) - 9) * 60 + stoi(endTimeM))/ TIMESLOTDURATION) - 1;
        //std::cout << startTimeSlot1 << " " << endTimeSlot1;

        if(endTimeSlot < startTimeSlot){
          std::cerr << "Input error in: " << day.at(0) << "; End time cannot be equal or less than Start time." << std::endl;
          exit(1);
        }
        if(startTimeSlot < 0){
          std::cerr << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
          exit(1);
        }
        if(endTimeSlot > 107){
          std::cerr << "Timing error in " << day.at(0) << "; End time should not be later than 18:00." << std::endl;
          exit(1);
        }
      }
      catch(...){
        std::cerr << "Formatting error in " << day.at(0) << " at " << day.at(1) << std::endl;
        exit(1);
      }
    }
  }

  Vector2D<std::string> sortedBlockedDays;

  /* Sort days of the week starting from monday and fill in the missing days (if any)*/
  for(unsigned long i = 0; i < 7; ++i){
    for(unsigned long j = 0; j < dayCount; ++j){
      if(blockedDays.at(j).at(0) == __DAYS.at(i)){
        sortedBlockedDays.push_back(blockedDays.at(j));
        break;
      }
      else if(j+1 == dayCount){
        std::vector<std::string> temp;
        temp.push_back(__DAYS.at(i));
        sortedBlockedDays.push_back(temp);
        break;
      }
    }
  }

/*
  for(const auto &row : blockedDays) {
    for(const auto &col : row)
      std::cout << col << " ";
    std::cout << std::endl;
  }

  std::cout << std::endl;
*/
  for(const auto &row : sortedBlockedDays) {
    for(const auto &col : row)
      std::cout << col << " ";
    std::cout << std::endl;
  }

  return sortedBlockedDays;
}

std::vector<Classroom> loadClassrooms(const char* path) {
  CSV classroomList{path};
  std::vector<Classroom> classrooms;
  for (std::vector<std::string> row: classroomList.getData()) {
    classrooms.emplace_back(std::stoi(row.at(capacityColumn)), row.at(idColumn));
  }

  return classrooms;
}

std::vector<Course> findConflictingCourses(std::vector<Course> courseList) {
  const unsigned long courseSize = courseList.size();
  for (unsigned long i = 0; i < courseSize; ++i) {
    const unsigned long L1S = courseList.at(i).studentList.size();
    for (unsigned long j = 0; j < courseSize; ++j) {
      bool skip = false;
      const unsigned long L2S = courseList.at(j).studentList.size();
      if (i != j) {
        for (unsigned long k = 0; !skip && k < L1S; k++) {
          for (unsigned long l = 0; !skip && l < L2S; l++) {
            if (courseList.at(i).studentList.at(k) == courseList.at(j).studentList.at(l)) {
              const unsigned long L1C = courseList.at(j).conflictingCourses.size();
              for (unsigned long o = 0; o < L1C; ++o) {
                if (courseList.at(j).conflictingCourses.at(o) == courseList.at(i).code) {
                  //std::cout << courses.at(j).conflictingCourses.at(o) << " already conflicts with " << courses.at(i).code << std::endl;
                  skip = true;
                  break;
                }
              }
              if (!skip && courseList.at(i).code != courseList.at(j).code) {
                courseList.at(i).conflictingCourses.push_back(courseList.at(j).code);
                courseList.at(j).conflictingCourses.push_back(courseList.at(i).code);
                //std::cout << courses.at(i).code << " conflicts with " << courses.at(j).code << std::endl;
              }
            }
          }
        }
      }
    }
    std::sort(courseList.at(i).conflictingCourses.begin(), courseList.at(i).conflictingCourses.end());
  }
  return courseList;
}