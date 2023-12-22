#ifndef COURSE_H
#define COURSE_H

#include "Student.h"
#include <vector>
#include <string>
typedef std::string String;


class Course {
public:
    Course(String code, String studentCount, String professorName, int examDuration);
    Course();
    ~Course();
    int studentCount;
    int examDuration;
    std::vector<Student> studentList;
    String professorName;

};



#endif //COURSE_H
