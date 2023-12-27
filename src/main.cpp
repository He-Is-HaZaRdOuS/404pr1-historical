#include "main.h"

#include <Classroom.h>
#include <Course.h>
#include <iostream>
#include <unordered_map>

#include "CSV.h"
#include "Timeslot.h"
#include "Typedefs.h"
#include <iostream>
#include <fstream>

#define classroomColumnSize 2
#define courseColumnSize 4
#define idColumn 0
#define capacityColumn 1
#define professorColumn 1
#define codeColumn 2
#define durationColumn 3

std::vector<Course> loadCourses(const char*path);

std::vector<Classroom> loadClassrooms(const char*path);

int main(){
    std::vector<Classroom> classrooms = loadClassrooms(RESOURCES_PATH "classroom.csv");
    std::vector<Course> courses = loadCourses(RESOURCES_PATH "course-list1.csv");

    Timeslot timetable[7][108];

    for (int i = 0; i < courses.size(); ++i) {
        for (int j = 0; j < classrooms.size(); ++j) {
            courses[i].classrooms.push_back(classrooms[j]);
        }
    }
/*
    std::cout << courses.at(0).code << " " << courses.at(0).studentList.size() << std::endl;
    for (const auto &item: courses.at(0).studentList) {
        std::cout << item.id << std::endl;
    }

    std::cout << courses.at(0).code << "*" << std::endl;
    for (int i = 0; i < courses.at(0).conflictingCourses.size(); ++i) {
        std::cout << courses.at(0).conflictingCourses.at(i).code << std::endl;
    }
*/
    return 0;
}

std::vector<Course> loadCourses(const char*path) {
    const CSV courseList{path};
    std::vector<Course> courses;
    const Vector2<String> uniqueCourses = courseList.filter(codeColumn, [](const std::string& value) {
        static std::unordered_map<String, bool> seen;
        if (seen.find(value) == seen.end()) {
            seen.emplace(value,true);
            return true;
        }
        return false;
    });

    for (std::vector<String>row : uniqueCourses) {
        const int studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
        courses.emplace_back(row.at(professorColumn), row.at(codeColumn),
std::stoi(row.at(durationColumn)), studentCount);
    }

    for (auto&course: courses) {
        Vector2<String> rows = courseList.filter(codeColumn, course.code);
        for (std::vector<String> row : rows) {
            Student s{std::stoi(row.at(idColumn))};
            course.studentList.push_back(s);
        }
    }

    unsigned long courseSize = courses.size();

    for (int i = 0; i < courseSize; ++i) {
        bool skip = false;
        unsigned long L1S = courses.at(i).studentList.size();
        for (int j = 0; j < courseSize && !skip; ++j) {
            unsigned L2S = courses.at(j).studentList.size();
            if (i != j) {
                for (int k = 0; k < L1S && !skip; k++) {
                    for (int l = 0; l < L2S && !skip; l++) {
                        if (courses.at(i).studentList.at(k).id == courses.at(j).studentList.at(l).id) {

                            unsigned long L1C = courses.at(j).conflictingCourses.size();
                            for (int o = 0; o < L1C; ++o) {
                                if (courses.at(j).conflictingCourses.at(o).code == courses.at(i).code) {
                                    std::cout << courses.at(j).conflictingCourses.at(o).code << " already conflicts with " << courses.at(i).code << std::endl;
                                    skip = true;
                                    break;
                                }

                            }
                                if(!skip && courses.at(i).code != courses.at(j).code){
                                    courses.at(i).conflictingCourses.push_back(courses.at(j));
                                    courses.at(j).conflictingCourses.push_back(courses.at(i));
                                    std::cout << courses.at(i).code << " conflicts with " << courses.at(j).code << std::endl;
                                }
                        }
                    }
                }
            }
        }
    }

    return courses;
}

std::vector<Classroom> loadClassrooms(const char*path) {
    CSV classroomList{path};
    std::vector<Classroom> classrooms;
    for (std::vector<String> row : classroomList.getData()) {
        classrooms.emplace_back(std::stoi(row.at(capacityColumn)), row.at(idColumn));
    }

    return classrooms;
}
