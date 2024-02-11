#include "CSV.h"

#include <iostream>
#include <algorithm>
#include <cstring>

CSV::CSV(const char* path, const char separator) : m_rows(0), m_cols(-1) {
    FILE* fp = fopen(path, "r");
    if (fp == nullptr)
        throw errno;

    char in;
    std::string val;
    int col = 0;
    m_data.emplace_back();
    while (fscanf(fp, "%c", &in) != EOF) {
        if (in == separator) {
            transform(val.begin(), val.end(), val.begin(), static_cast<int (*)(int)>(&std::toupper));
            m_data.at(m_rows).push_back(val);
            val = "";
            col++;
        }
        else if (in == '\n') {
            m_data.at(m_rows).push_back(val);
            m_rows++;
            col++;
            if (m_cols == -1)
                m_cols = col;
            if (col != m_cols) {
                throw errno;
            }

            col = 0;
            m_data.emplace_back();
            val = "";
        }
        else {
            val += in;
        }
    }
    m_cols = static_cast<long>(m_data.at(0).size());
    m_data.pop_back();
}

void CSV::printRows(const std::string&separator) const {
    for (const auto& row: m_data) {
        const uint32_t rowSize = row.size();
        for (uint32_t col = 0; col < rowSize; ++col) {
            std::cout << row.at(col);
            if (col != rowSize - 1)
                std::cout << separator;
        }
        std::cout << std::endl;
    }
}

Vector2D<std::string> CSV::filter(const long column, bool (*filter)(const std::string&)) const {
    auto matches = Vector2D<std::string>(0);
    if (column < 0 || column > m_cols) return matches;
    for (auto&row : m_data) {
        if (filter(row.at(column))) {
            matches.push_back(row);
        }
    }
    return matches;
}


Vector2D<std::string> CSV::filter(int column, const std::string&value) const {
    auto matches = Vector2D<std::string>(0);
    if (column < 0 || column > m_cols) return matches;
    for (auto&row : m_data) {
        if (row.at(column) == value) {
            matches.push_back(row);
        }
    }
    return matches;
}

Vector2D<std::string> CSV::getData() {
    return m_data;
}

void CSV::removeHeader() {
    --m_rows;
    m_data.erase(m_data.begin(), m_data.begin() + 1);
}

void loadCourses(const char* path, std::vector<Course>* courses) {
  CSV courseList;
  try{
    const CSV c{path}; // check if specified file exists
    courseList = c;
  }
  catch(...){ // throw error if not
    std::cerr << "Could not load data from " << path << " because it does not exist." << std::endl;
    exit(1);
  }

  /* check if header exists and trim it out */
  if (courseList.getData().at(0).at(0) == "STUDENTID") {
    courseList.removeHeader();
  }

  /* put unique courses into a 2D vector */
  const Vector2D<std::string> uniqueCourses = courseList.filter(codeColumn, [](const std::string& value) {
    static std::unordered_map<std::string, bool> seen;
    if (seen.find(value) == seen.end()) {
      seen.emplace(value, true);
      return true;
    }
    return false;
  });


  /* construct course objects from provided data */
  for (std::vector<std::string> row: uniqueCourses) {
    if(!n_Main::uniqueCourses.count(row.at(codeColumn)))
      n_Main::uniqueCourses.emplace(row.at(codeColumn), true);
    if(!n_Main::uniqueProfessors.count(row.at(professorColumn)))
      n_Main::uniqueProfessors.emplace(row.at(professorColumn), true);
    const uint64_t  studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
    courses->emplace_back(n_Main::uniqueProfessors.find(row.at(professorColumn))->first, row.at(codeColumn),
                         std::stoi(row.at(durationColumn)), studentCount);
    courses->back().m_studentList.reserve(studentCount);
  }

  /* add students of each course to a vector */
  uint64_t size = courses->size();
  for (uint64_t index = 0; index < size; ++index) {
    Vector2D<std::string> rows = courseList.filter(codeColumn, courses->at(index).m_code);
    for (std::vector<std::string> row: rows) {
      courses->at(index).m_studentList.emplace_back(std::stoi(row.at(idColumn)));
    }
    std::sort(courses->at(index).m_studentList.begin(), courses->at(index).m_studentList.end());
  }
  return;
}

void loadClassrooms(const char* path, std::vector<Classroom>* classrooms) {
  CSV classroomList;
  try{
    const CSV c{path}; // check if file exists
    classroomList = c;
  }
  catch(...){ // throw error if not
    std::cerr << "Could not load data from " << path << " because it does not exist." << std::endl;
    exit(1);
  }

  /* discard header line */
  if (classroomList.getData().at(0).at(0) == "ROOMID") {
    classroomList.removeHeader();
  }
  for (std::vector<std::string> row: classroomList.getData()) {
    classrooms->emplace_back(std::stoi(row.at(capacityColumn)), row.at(idColumn));
  }
  return;
}

void loadBlockedHours(const char* path, Vector2D<std::string>* blockedHours) {
  CSV blockedList;
  try {
    const CSV b{path}; // check if specified file exists
    blockedList = b;
  }catch(...){ // if not, assume there are no blocked hours and make an empty vector
    for(int32_t dayIndex = 0; dayIndex < 7; ++dayIndex){
      std::vector<std::string> temp;
      temp.emplace_back(n_Main::DAYS_UPPERCASE.at(dayIndex));
      blockedHours->emplace_back(temp);
    }
    return;
  }

  /* merge repeated days into one object */
  Vector2D<std::string> blockedDays = blockedList.filter(dayColumn, [](const std::string&value) {
    static std::unordered_map<std::string, bool> seen;
    if (seen.find(value) == seen.end()) {
      seen.emplace(value, true);
      return true;
    }
    return false;
  });

  /* discard redundant info */
  const uint64_t  dayCount = blockedDays.size();
  for(uint64_t dayIndex = 0; dayIndex <  dayCount; ++dayIndex) {
    blockedDays.at(dayIndex).pop_back();
    blockedDays.at(dayIndex).pop_back();
  }

  Vector2D<std::string> copy = blockedDays;

  /* convert strings to uppercase effectively making them case-insensitive */
  for(auto&day : blockedDays) {
    transform(day.at(0).begin(), day.at(0).end(), day.at(0).begin(), static_cast<int32_t (*)(int32_t)>(&std::toupper));
    Vector2D<std::string> rows = blockedList.filter(dayColumn, day.at(0));
    for(std::vector<std::string> row : rows) {
      day.emplace_back(row.at(timeColumn));
      day.emplace_back(row.at(commonCourseColumn));
    }
  }

  const uint64_t size = blockedDays.size();
  std::vector<bool> valid;
  valid.reserve(7);

  /* check if any invalid days exist */
  for(uint64_t dayIndex = 0; dayIndex < size; ++dayIndex) {
    transform(blockedDays.at(dayIndex).at(0).begin(), blockedDays.at(dayIndex).at(0).end(), blockedDays.at(dayIndex).at(0).begin(), static_cast<int32_t (*)(int32_t)>(&std::toupper));
    if (std::find(n_Main::DAYS_UPPERCASE.begin(), n_Main::DAYS_UPPERCASE.end(),
                  blockedDays.at(dayIndex).at(0)) == n_Main::DAYS_UPPERCASE.end()){
      valid.emplace_back() = false;
    }
    valid.emplace_back(true);
  }

  bool shouldExit = false;
  for(uint64_t dayIndex = 0; dayIndex < size; ++dayIndex){
    if(!valid.at(dayIndex)){
      shouldExit = true;
      std::cerr << "Input error: " << copy.at(dayIndex).at(0) << " is not a day of the week" << std::endl;
    }
  }

  if(shouldExit)
    exit(1);

  /* Error checking:
   * check if the given time interval is formatted correctly
   * check if given time intervals don't intersect with each other
   * */
  for(auto&day : blockedDays) {
    const uint64_t daySize = day.size();
    if(daySize > 3){
      for(uint64_t interval0 = 1; interval0 < daySize; interval0=interval0+2){
        std::string startTimeH1, endTimeH1, startTimeM1, endTimeM1;
        int32_t startTimeSlot1, endTimeSlot1;
        try{
          startTimeH1 = day.at(interval0).substr(0, 2);
          startTimeM1 = day.at(interval0).substr(3, 2);
          endTimeH1 = day.at(interval0).substr(6, 2);
          endTimeM1 = day.at(interval0).substr(9, 2);

          if(startTimeH1.length() != 2 || startTimeM1.length() != 2 || endTimeH1.length() != 2 || endTimeM1.length() != 2){
            std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(interval0) << std::endl;
            std::cerr << "Time interval should be formatted as HH:MM-HH:MM" << std::endl;
            exit(1);
          }

          startTimeSlot1 = ((stoi(startTimeH1) - 9) * 60 + stoi(startTimeM1)) / n_Solution::TIMESLOTDURATION;
          endTimeSlot1 = (((stoi(endTimeH1) - 9) * 60 + stoi(endTimeM1))/ n_Solution::TIMESLOTDURATION) - 1;

          if(endTimeSlot1 < startTimeSlot1){
            std::cerr << std::endl << "Input error in: " << day.at(interval0) << "; End time cannot be equal or less than Start time." << std::endl;
            exit(1);
          }
          if(startTimeSlot1 < 0){
            std::cerr << std::endl << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
            exit(1);
          }
          if(endTimeSlot1 > n_Solution::TIMESLOTCOUNT){
            std::cerr << std::endl << "Timing error in " << day.at(0) << "; End time should not be later than 18:00." << std::endl;
            exit(1);
          }
        }
        catch(...){
          std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(interval0) << std::endl;
          exit(1);
        }

        for(uint64_t interval1 = interval0 + 2; interval1 < daySize; interval1=interval1+2){
          if(interval0 != interval1){
            std::string startTimeH2, endTimeH2, startTimeM2, endTimeM2;
            int32_t  startTimeSlot2, endTimeSlot2;
            try{
              startTimeH2 = day.at(interval1).substr(0, 2);
              startTimeM2 = day.at(interval1).substr(3, 2);
              endTimeH2 = day.at(interval1).substr(6, 2);
              endTimeM2 = day.at(interval1).substr(9, 2);

              if(startTimeH2.length() == 1 || startTimeM2.length() == 1 || endTimeH2.length() == 1 || endTimeM2.length() == 1){
                std::cerr << std::endl << "Formatting error in " << day.at(0) << " at " << day.at(interval1) << std::endl;
                std::cerr << "Time interval should be formatted as HH:MM-HH:MM" << std::endl;
                exit(1);
              }

              startTimeSlot2 = ((stoi(startTimeH2) - 9) * 60 + stoi(startTimeM2)) / n_Solution::TIMESLOTDURATION;
              endTimeSlot2 = (((stoi(endTimeH2) - 9) * 60 + stoi(endTimeM2))/ n_Solution::TIMESLOTDURATION) - 1;

              if(endTimeSlot2 < startTimeSlot2){
                std::cerr << std::endl << "Input error in: " << day.at(interval1) << "; End time cannot be equal or less than Start time." << std::endl;
                exit(1);
              }
              if(startTimeSlot2 < 0){
                std::cerr << std::endl << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
                exit(1);
              }
              if(endTimeSlot2 > n_Solution::TIMESLOTCOUNT - 1){
                std::cerr << std::endl << "Timing error in " << day.at(0) << "; End time should not be later than 18:00." << std::endl;
                exit(1);
              }
            }
            catch(...){
              std::cerr << std::endl << "Formatting error in: " << day.at(interval1) << std::endl;
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
      int32_t startTimeSlot, endTimeSlot;
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

        startTimeSlot = ((stoi(startTimeH) - 9) * 60 + stoi(startTimeM)) / n_Solution::TIMESLOTDURATION;
        endTimeSlot = (((stoi(endTimeH) - 9) * 60 + stoi(endTimeM))/ n_Solution::TIMESLOTDURATION) - 1;

        if(endTimeSlot < startTimeSlot){
          std::cerr << "Input error in: " << day.at(0) << "; End time cannot be equal or less than Start time." << std::endl;
          exit(1);
        }
        if(startTimeSlot < 0){
          std::cerr << "Timing error in " << day.at(0) << "; Start time should not be earlier than 09:00." << std::endl;
          exit(1);
        }
        if(endTimeSlot > n_Solution::TIMESLOTCOUNT){
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

  /* Sort days of the week starting from monday and fill in the missing days (if any)*/
  for(uint64_t day = 0; day < 7; ++day){
    for(uint64_t dayIndex = 0; dayIndex < dayCount; ++dayIndex){
      if(blockedDays.at(dayIndex).at(0) == n_Main::DAYS_UPPERCASE.at(day)){
        blockedHours->emplace_back(blockedDays.at(dayIndex));
        break;
      }
      if(dayIndex + 1 == dayCount){
        std::vector<std::string> temp;
        temp.emplace_back(n_Main::DAYS_UPPERCASE.at(day));
        blockedHours->emplace_back(temp);
        break;
      }
    }
  }

  return;
}
