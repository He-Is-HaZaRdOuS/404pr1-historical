#pragma once
#ifndef COURSE_H
#define COURSE_H

#include "Classroom.h"
#include "Typedefs.h"

#include <unordered_map>

class Course {
public:
    explicit Course(const std::string& professorName, const std::string& code, int examDuration, int studentCount = 0);
    ~Course();
    int studentCount;
    int examDuration;
    std::string_view professorName;
    std::string code;
    std::vector<int> studentList;
    std::unordered_map<std::string_view, bool> conflictingCourses;
    std::vector<Classroom> classrooms;
};



#endif //COURSE_H
