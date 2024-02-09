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
    if(!Main::uniqueCourses.count(row.at(codeColumn)))
      Main::uniqueCourses.emplace(row.at(codeColumn), true);
    if(!Main::uniqueProfessors.count(row.at(professorColumn)))
      Main::uniqueProfessors.emplace(row.at(professorColumn), true);
    const unsigned long studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
    courses.emplace_back(Main::uniqueProfessors.find(row.at(professorColumn))->first, row.at(codeColumn),
                         std::stoi(row.at(durationColumn)), studentCount);
    courses.back().studentList.reserve(studentCount);
  }


  /* add students of each course to a vector */
  for (auto&course : courses) {
    Vector2D<std::string> rows = courseList.filter(codeColumn, course.code);
    for (std::vector<std::string> row: rows) {
      course.studentList.emplace_back(std::stoi(row.at(idColumn)));
    }
    std::sort(course.studentList.begin(), course.studentList.end());
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
    for(int i = 0; i < 7; ++i){
      std::vector<std::string> temp;
      temp.emplace_back(Main::DAYS_UPPERCASE.at(i));
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
  for(unsigned long i = 0; i <  dayCount; ++i) {
    blockedDays.at(i).pop_back();
    blockedDays.at(i).pop_back();
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
  for(unsigned long i = 0; i < size; ++i) {
    transform(blockedDays.at(i).at(0).begin(), blockedDays.at(i).at(0).end(), blockedDays.at(i).at(0).begin(), static_cast<int (*)(int)>(&std::toupper));
    if (std::find(Main::DAYS_UPPERCASE.begin(), Main::DAYS_UPPERCASE.end(),
                  blockedDays.at(i).at(0)) == Main::DAYS_UPPERCASE.end()){
      valid.emplace_back() = false;
    }
    valid.emplace_back(true);
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
              if(endTimeSlot2 > TIMESLOTCOUNT - 1){
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
      if(blockedDays.at(j).at(0) == Main::DAYS_UPPERCASE.at(i)){
        sortedBlockedDays.emplace_back(blockedDays.at(j));
        break;
      }
      if(j+1 == dayCount){
        std::vector<std::string> temp;
        temp.emplace_back(Main::DAYS_UPPERCASE.at(i));
        sortedBlockedDays.emplace_back(temp);
        break;
      }
    }
  }

  return sortedBlockedDays;
}

std::vector<Course> findConflictingCourses(std::vector<Course> courseList) {
  const unsigned long courseSize = courseList.size();
  for (unsigned long i = 0; i < courseSize; ++i) {
    for (unsigned long j = i + 1; j < courseSize; ++j) {
      const unsigned long L2S = courseList.at(j).studentList.size();
      std::string_view sj = Main::uniqueCourses.find(courseList.at(j).code)->first;
      std::string_view si = Main::uniqueCourses.find(courseList.at(i).code)->first;
      if (courseList.at(i).professorName.compare(courseList.at(j).professorName) == 0) {
        courseList.at(i).conflictingCourses[sj] = true;
        courseList.at(j).conflictingCourses[si] = true;
        continue;
      }
      if (courseList.at(j).conflictingCourses.count(courseList.at(i).code)) {
        break;
      }
      for (unsigned long l = 0; l < L2S; l++) {
        if (std::binary_search(courseList.at(i).studentList.begin(),
                                   courseList.at(i).studentList.end(),
                                   courseList.at(j).studentList.at(l))) {
          if (courseList.at(i).code != courseList.at(j).code) {
            courseList.at(i).conflictingCourses[sj] = true;
            courseList.at(j).conflictingCourses[si] = true;
          }
        }
      }

    }
  }
  return courseList;
}
