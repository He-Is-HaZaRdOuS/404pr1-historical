#include "Main.h"

#include "CSV.h"
#include "Classroom.h"
#include "Course.h"
#include "Solution.h"
#include "Timer.h"
#include "Typedefs.h"

#include <future>

void findConflictingCourses(std::vector<Course>* courseList);

int32_t main(const int argc, char **argv) {
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
    const int32_t nValue = std::stoi(argv[1]);
    std::string f_courseList = argv[2];
    f_courseList = RESOURCES_PATH + f_courseList;
    std::string f_classrooms = argv[3];
    f_classrooms = RESOURCES_PATH + f_classrooms;
    std::string f_blockedHours;
    if (argc == 5)
      f_blockedHours = argv[4];
    f_blockedHours = RESOURCES_PATH + f_blockedHours;

    std::vector<Classroom> classrooms;
    std::vector<Course> coursesDepartment;
    Vector2D<std::string> blockedHours;

    n_Main::future_course = std::async(std::launch::async, loadCourses, f_courseList.c_str(), &coursesDepartment);
    n_Main::future_classroom = std::async(std::launch::async, loadClassrooms, f_classrooms.c_str(), &classrooms);
    n_Main::future_blocked = std::async(std::launch::async, loadBlockedHours, f_blockedHours.c_str(), &blockedHours);
    n_Main::future_course.wait();
    n_Main::future_conflict = std::async(std::launch::async, findConflictingCourses, &coursesDepartment);
    n_Main::future_conflict.wait();
    n_Main::future_classroom.wait();
    n_Main::future_blocked.wait();


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

void findConflictingCourses(std::vector<Course>* courseList) {
  const uint64_t courseSize = courseList->size();
  for (uint64_t c1Index = 0; c1Index < courseSize; ++c1Index) {
    std::string_view c1Code = n_Main::uniqueCourses.find(courseList->at(c1Index).m_code)->first;
    for (uint64_t c2Index = c1Index + 1; c2Index < courseSize; ++c2Index) {
      const uint64_t c2StudentCnt = courseList->at(c2Index).m_studentList.size();
      std::string_view c2Code = n_Main::uniqueCourses.find(courseList->at(c2Index).m_code)->first;
      if (courseList->at(c1Index).m_professorName.compare(courseList->at(c2Index).m_professorName) == 0) {
        courseList->at(c1Index).m_conflictingCourses[c2Code] = true;
        courseList->at(c2Index).m_conflictingCourses[c1Code] = true;
        continue;
      }
      if (courseList->at(c2Index).m_conflictingCourses.count(courseList->at(c1Index).m_code)) {
        break;
      }
      for (uint64_t c2StudentIndex = 0; c2StudentIndex < c2StudentCnt; c2StudentIndex++) {
        if (std::binary_search(courseList->at(c1Index).m_studentList.begin(),
                                   courseList->at(c1Index).m_studentList.end(),
                                   courseList->at(c2Index).m_studentList.at(c2StudentIndex))) {
          if (strcmp(courseList->at(c1Index).m_code.c_str(), courseList->at(c2Index).m_code.c_str()) != 0) {
            courseList->at(c1Index).m_conflictingCourses[c2Code] = true;
            courseList->at(c2Index).m_conflictingCourses[c1Code] = true;
          }
                                   }
      }

    }
  }
  return;
}
