#include "Course.h"

Course::Course(std::string professorName, std::string code, int examDuration, int studentCount) {
    this->examDuration = examDuration;
    this->studentCount = studentCount;
    this->code = code;
    this->professorName = professorName;
}

Course::Course() = default;

Course::~Course() {

}




