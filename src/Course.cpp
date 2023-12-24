#include "Course.h"

Course::Course(String professorName, String code, int examDuration, int studentCount) {
    this->examDuration = examDuration;
    this->studentCount = studentCount;
    this->code = code;
    this->professorName = professorName;
}

Course::~Course() {

}




