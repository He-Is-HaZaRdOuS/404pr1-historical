#pragma once
#ifndef COURSE_H
#define COURSE_H

#include "Classroom.h"
#include "Typedefs.h"


class Course {
public:
    explicit Course(const std::string& professorName, const std::string& code, int examDuration, int studentCount = 0);
    ~Course();
    int studentCount;
    int examDuration;
    std::string professorName;
    std::string code;
    std::vector<int> studentList;
    std::vector<std::string_view> conflictingCourses;
    std::vector<Classroom> classrooms;
};



#endif //COURSE_H
