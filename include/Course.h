#pragma once
#ifndef COURSE_H
#define COURSE_H

#include <Classroom.h>
#include "Typedefs.h"


class Course {
public:
    explicit Course(String professorName, String code, int examDuration, int studentCount = 0);
    ~Course();
    int studentCount;
    int examDuration;
    String professorName;
    String code;
    std::vector<int> studentList;
    std::vector<String> conflictingCourses;
    std::vector<Classroom> classrooms;
};



#endif //COURSE_H
