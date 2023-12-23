#ifndef STUDENT_H
#define STUDENT_H
#include <vector>
#include "Course.h"


class Student {
public:
    explicit Student(int id);
    ~Student();
    int id;
    std::vector<Course*> courseList;

};



#endif //STUDENT_H
