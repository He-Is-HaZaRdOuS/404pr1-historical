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

    std::cout << "Classrooms" << std::endl;
    for (const auto& classroom: classrooms) {
        std::cout << classroom.id << " " << classroom.capacity << std::endl;
    }

    int totalDur = 0;
    std::cout << "Courses" << std::endl;
    for (const auto& course: courses) {
        totalDur += course.examDuration;
        std::cout << course.code << " "
            << course.professorName << " "
            << course.examDuration << " "
            << course.studentCount << std::endl;
    }
    std::cout << courses.size() << " " << totalDur;


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

    for (std::vector<String> row : uniqueCourses) {
        const int studentCount = courseList.filter(codeColumn, row.at(codeColumn)).size();
        courses.emplace_back(row.at(professorColumn), row.at(codeColumn),
std::stoi(row.at(durationColumn)), studentCount);
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
