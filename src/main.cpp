#include "main.h"

#include <algorithm>
#include <Classroom.h>
#include <Course.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "CSV.h"
#include "Timeslot.h"
#include "Typedefs.h"
#include "Solution.h"

#define classroomColumnSize 2
#define courseColumnSize 4
#define idColumn 0
#define capacityColumn 1
#define professorColumn 1
#define codeColumn 2
#define durationColumn 3

std::vector<Course> loadCourses(const char* path);

std::vector<Classroom> loadClassrooms(const char* path);

int main() {
    std::vector<Classroom> classrooms = loadClassrooms(RESOURCES_PATH "classroom.csv");
    std::vector<Course> courses = loadCourses(RESOURCES_PATH "t1.csv");
    std::vector<Course> coursesS = loadCourses(RESOURCES_PATH "course-list3.csv");

    unsigned int cs = coursesS.size();
    for(int i = 0; i < cs; ++i) {
        if(coursesS.at(i).code.at(0) == 'S') {
            courses.push_back(coursesS.at(i));
        }
    }

/*
    unsigned int cc = coursesC.size();
    unsigned int cs = coursesS.size();
    for(int i=0; i < cc; i++) {
        for(int j=0; j < cs; j++) {
            Course n;
            if(coursesC.at(i).code != coursesS.at(j).code) {
                n = coursesC.at(i);
                bool exists = false;
                for(int k=0; k < courses.size(); k++) {
                    if(n.code == courses.at(k).code)
                        exists = true;
                }
                if(!exists) {
                    courses.push_back(n);
                }
            }
        }
    }
    */

    std::sort(courses.begin(), courses.end(), [](const Course&c1, const Course&c2) {
        return (strcmp(c1.code.c_str(), c2.code.c_str()) > 0) ? true : false;
    });

    for(Course& c : courses) {
        std::cout << c.code << std::endl;
    }



    Solution solution{courses, classrooms};
    solution.initializeSchedule();
    solution.cost();



    //    Timeslot timetable[7][108];

    /*
    for (auto & course : courses) {
        for (const auto & classroom : classrooms) {
            course.classrooms.push_back(classroom);
        }
    }
     */

    /*
    std::cout << courses.at(0).code << " " << courses.at(0).studentList.size() << std::endl;
    for (const auto &item: courses.at(0).studentList) {
        std::cout << item.id << std::endl;
    }
    */

    /*print conflicting courses of each course*/

    /*
    for(const Course& c : courses) {
        std::cout << "*Courses that conflict with " << c.code << "*" << std::endl;
        for (auto & conflictingCourse : c.conflictingCourses) {
            //std::cout << conflictingCourse << std::endl;
        }
    }
    */


    return 0;
}

std::vector<Course> loadCourses(const char* path) {
    const CSV courseList{path};
    std::vector<Course> courses;
    const Vector2<String> uniqueCourses = courseList.filter(codeColumn, [](const std::string&value) {
        static std::unordered_map<String, bool> seen;
        if (seen.find(value) == seen.end()) {
            seen.emplace(value, true);
            return true;
        }
        return false;
    });

    for (std::vector<String> row: uniqueCourses) {
        const unsigned long studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
        courses.emplace_back(row.at(professorColumn), row.at(codeColumn),
                             std::stoi(row.at(durationColumn)), studentCount);
    }


    for (auto&course: courses) {
        Vector2<String> rows = courseList.filter(codeColumn, course.code);
        for (std::vector<String> row: rows) {
            int s = std::stoi(row.at(idColumn));
            course.studentList.push_back(s);
        }
        //std::sort(course.studentList.begin(), course.studentList.end());
    }

    const unsigned long courseSize = courses.size();
    for (int i = 0; i < courseSize; ++i) {
        const unsigned long L1S = courses.at(i).studentList.size();
        for (int j = 0; j < courseSize; ++j) {
            bool skip = false;
            const unsigned long L2S = courses.at(j).studentList.size();
            if (i != j) {
                for (int k = 0; !skip && k < L1S; k++) {
                    for (int l = 0; !skip && l < L2S; l++) {
                        if (courses.at(i).studentList.at(k) == courses.at(j).studentList.at(l)) {
                            const unsigned long L1C = courses.at(j).conflictingCourses.size();
                            for (int o = 0; o < L1C; ++o) {
                                if (courses.at(j).conflictingCourses.at(o) == courses.at(i).code) {
                                    //std::cout << courses.at(j).conflictingCourses.at(o) << " already conflicts with " << courses.at(i).code << std::endl;
                                    skip = true;
                                    break;
                                }
                            }
                            if (!skip && courses.at(i).code != courses.at(j).code) {
                                courses.at(i).conflictingCourses.push_back(courses.at(j).code);
                                courses.at(j).conflictingCourses.push_back(courses.at(i).code);
                                //std::cout << courses.at(i).code << " conflicts with " << courses.at(j).code << std::endl;
                            }
                        }
                    }
                }
            }
        }
        std::sort(courses.at(i).conflictingCourses.begin(), courses.at(i).conflictingCourses.end());
    }

    return courses;
}

std::vector<Classroom> loadClassrooms(const char* path) {
    CSV classroomList{path};
    std::vector<Classroom> classrooms;
    for (std::vector<String> row: classroomList.getData()) {
        classrooms.emplace_back(std::stoi(row.at(capacityColumn)), row.at(idColumn));
    }

    return classrooms;
}
