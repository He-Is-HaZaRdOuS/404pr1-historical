#include "main.h"

#include "Classroom.h"
#include "Course.h"
#include "CSV.h"
#include "Solution.h"
#include "Timer.h"
#include "Typedefs.h"

/* function prototypes */
std::vector<Course> loadCourses(const char* path);

std::vector<Classroom> loadClassrooms(const char* path);

Vector2D<std::string> loadBlockedHours(const char* path);

std::vector<Course> findConflictingCourses(std::vector<Course> courseList);

int main(const int argc, char **argv) {
  /* inner-scope begin */
  {
    /* start timer */
    Timer timer;
    if (argc == 2 && 0 == strncmp(argv[1],"-h",2)) {
      std::cout << "N-value: Number of iterations to find optimal schedule." << std::endl;
      std::cout << "course-list.csv: CSV file containing course lists. " <<
      "StudentID,Professor Name,CourseID,ExamDuration(in mins)" << std::endl;
      std::cout << "classrooms.csv: CSV file containing classrooms. (RoomID,Capacity)" << std::endl;
      std::cout << "blocked.csv: (Optional) CSV file containing blocked hours. " <<
      "(Day,TimeInterval,Course)" << std::endl;
      return 0;
    }
    if (argc != 4 && argc != 5) {
      std::cout << "Usage: (blocked hours are optional)\n"
                << "./303pr1 <N-value> <course-list.csv> <classrooms.csv> "
                   "<?blocked.csv>" << std::endl;
      std::cout << "Run './303pr1 -h' for help" << std::endl;
      return 1;
    }
    const int nValue = std::stoi(argv[1]);
    std::string f_courseList = argv[2];
    f_courseList = RESOURCES_PATH + f_courseList;
    std::string f_classrooms = argv[3];
    f_classrooms = RESOURCES_PATH + f_classrooms;
    std::string f_blockedHours;
    if (argc == 5)
      f_blockedHours = argv[4];
    f_blockedHours = RESOURCES_PATH + f_blockedHours;

    const std::vector<Classroom> classrooms = loadClassrooms(f_classrooms.c_str());
    std::vector<Course> coursesDepartment = loadCourses(f_courseList.c_str());
    const Vector2D<std::string> blockedHours = loadBlockedHours(f_blockedHours.c_str());

    coursesDepartment = findConflictingCourses(coursesDepartment);

    /* solve the problem */
    Solution solution{coursesDepartment, classrooms, blockedHours};
    if (nValue <= 0) {
      std::cout << "Invalid N-value: " << nValue << ", will use 100 instead" << std::endl;
      solution.Solve(100);
    } else {
      solution.Solve(nValue);
    }
  } /* inner-scope end */
  return 0;
}

std::vector<Course> loadCourses(const char* path) {
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

  std::vector<Course> courses;
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
    const unsigned long studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
    courses.emplace_back(n_Main::uniqueProfessors.find(row.at(professorColumn))->first, row.at(codeColumn),
                         std::stoi(row.at(durationColumn)), studentCount);
    courses.back().m_studentList.reserve(studentCount);
  }


  /* add students of each course to a vector */
  for (auto&course : courses) {
    Vector2D<std::string> rows = courseList.filter(codeColumn, course.m_code);
    for (std::vector<std::string> row: rows) {
      course.m_studentList.emplace_back(std::stoi(row.at(idColumn)));
    }
    std::sort(course.m_studentList.begin(), course.m_studentList.end());
  }

  return courses;
}

std::vector<Classroom> loadClassrooms(const char* path) {
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
  std::vector<Classroom> classrooms;
  for (std::vector<std::string> row: classroomList.getData()) {
    classrooms.emplace_back(std::stoi(row.at(capacityColumn)), row.at(idColumn));
  }

  return classrooms;
}

Vector2D<std::string> loadBlockedHours(const char* path) {
  CSV blockedList;
  try {
    const CSV b{path}; // check if specified file exists
    blockedList = b;
  }catch(...){ // if not, assume there are no blocked hours and make an empty vector
    Vector2D<std::string> emptyList;
    for(int dayIndex = 0; dayIndex < 7; ++dayIndex){
      std::vector<std::string> temp;
      temp.emplace_back(n_Main::DAYS_UPPERCASE.at(dayIndex));
      emptyList.emplace_back(temp);
    }
    return emptyList;
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
  const unsigned long dayCount = blockedDays.size();
  for(unsigned long dayIndex = 0; dayIndex <  dayCount; ++dayIndex) {
    blockedDays.at(dayIndex).pop_back();
    blockedDays.at(dayIndex).pop_back();
  }

  Vector2D<std::string> copy = blockedDays;

  /* convert strings to uppercase effectively making them case-insensitive */
  for(auto&day : blockedDays) {
    transform(day.at(0).begin(), day.at(0).end(), day.at(0).begin(), static_cast<int (*)(int)>(&std::toupper));
    Vector2D<std::string> rows = blockedList.filter(dayColumn, day.at(0));
    for(std::vector<std::string> row : rows) {
      day.emplace_back(row.at(timeColumn));
      day.emplace_back(row.at(commonCourseColumn));
    }
  }

  const unsigned long size = blockedDays.size();
  std::vector<bool> valid;
  valid.reserve(7);

  /* check if any invalid days exist */
  for(unsigned long dayIndex = 0; dayIndex < size; ++dayIndex) {
    transform(blockedDays.at(dayIndex).at(0).begin(), blockedDays.at(dayIndex).at(0).end(), blockedDays.at(dayIndex).at(0).begin(), static_cast<int (*)(int)>(&std::toupper));
    if (std::find(n_Main::DAYS_UPPERCASE.begin(), n_Main::DAYS_UPPERCASE.end(),
                  blockedDays.at(dayIndex).at(0)) == n_Main::DAYS_UPPERCASE.end()){
      valid.emplace_back() = false;
    }
    valid.emplace_back(true);
  }

  bool shouldExit = false;
  for(unsigned long dayIndex = 0; dayIndex < size; ++dayIndex){
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
    const unsigned long daySize = day.size();
    if(daySize > 3){
      for(unsigned long interval0 = 1; interval0 < daySize; interval0=interval0+2){
        std::string startTimeH1, endTimeH1, startTimeM1, endTimeM1;
        int startTimeSlot1, endTimeSlot1;
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

        for(unsigned long interval1 = interval0 + 2; interval1 < daySize; interval1=interval1+2){
          if(interval0 != interval1){
            std::string startTimeH2, endTimeH2, startTimeM2, endTimeM2;
            int  startTimeSlot2, endTimeSlot2;
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

  Vector2D<std::string> sortedBlockedDays;

  /* Sort days of the week starting from monday and fill in the missing days (if any)*/
  for(unsigned long day = 0; day < 7; ++day){
    for(unsigned long dayIndex = 0; dayIndex < dayCount; ++dayIndex){
      if(blockedDays.at(dayIndex).at(0) == n_Main::DAYS_UPPERCASE.at(day)){
        sortedBlockedDays.emplace_back(blockedDays.at(dayIndex));
        break;
      }
      if(dayIndex + 1 == dayCount){
        std::vector<std::string> temp;
        temp.emplace_back(n_Main::DAYS_UPPERCASE.at(day));
        sortedBlockedDays.emplace_back(temp);
        break;
      }
    }
  }

  return sortedBlockedDays;
}

std::vector<Course> findConflictingCourses(std::vector<Course> courseList) {
  const unsigned long courseSize = courseList.size();
  for (unsigned long c1Index = 0; c1Index < courseSize; ++c1Index) {
    std::string_view c1Code = n_Main::uniqueCourses.find(courseList.at(c1Index).m_code)->first;
    for (unsigned long c2Index = c1Index + 1; c2Index < courseSize; ++c2Index) {
      const unsigned long c2StudentCnt = courseList.at(c2Index).m_studentList.size();
      std::string_view c2Code = n_Main::uniqueCourses.find(courseList.at(c2Index).m_code)->first;
      if (courseList.at(c1Index).m_professorName.compare(courseList.at(c2Index).m_professorName) == 0) {
        courseList.at(c1Index).m_conflictingCourses[c2Code] = true;
        courseList.at(c2Index).m_conflictingCourses[c1Code] = true;
        continue;
      }
      if (courseList.at(c2Index).m_conflictingCourses.count(courseList.at(c1Index).m_code)) {
        break;
      }
      for (unsigned long c2StudentIndex = 0; c2StudentIndex < c2StudentCnt; c2StudentIndex++) {
        if (std::binary_search(courseList.at(c1Index).m_studentList.begin(),
                                   courseList.at(c1Index).m_studentList.end(),
                                   courseList.at(c2Index).m_studentList.at(c2StudentIndex))) {
          if (strcmp(courseList.at(c1Index).m_code.c_str(), courseList.at(c2Index).m_code.c_str()) != 0) {
            courseList.at(c1Index).m_conflictingCourses[c2Code] = true;
            courseList.at(c2Index).m_conflictingCourses[c1Code] = true;
          }
        }
      }

    }
  }
  return courseList;
}
