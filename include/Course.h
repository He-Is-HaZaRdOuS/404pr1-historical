#pragma once
#ifndef COURSE_H
#define COURSE_H

#include <Classroom.h>
#include <Student.h>


#include "Typedefs.h"


class Course {
public:
    explicit Course(String professorName, String code, int examDuration, int studentCount = 0);
    ~Course();
    int studentCount;
    int examDuration;
    std::vector<Student> studentList;
    String professorName;
    String code;
    std::vector<Course> conflictingCourses;
    std::vector<Classroom> classrooms;
};



#endif //COURSE_H
