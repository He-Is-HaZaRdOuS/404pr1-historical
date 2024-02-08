#include "Course.h"

Course::Course(const std::string& professorName, const std::string& code, const int examDuration, const int studentCount)
    : studentCount(studentCount), examDuration(examDuration), professorName(professorName), code(code) {
}



Course::~Course() {

}
