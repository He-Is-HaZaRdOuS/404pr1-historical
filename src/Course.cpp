#include "Course.h"

Course::Course(const std::string& professorName, const std::string& code, const uint32_t examDuration, const uint64_t studentCount)
    : m_studentCount(studentCount), m_examDuration(examDuration), m_professorName(professorName), m_code(code) {
}
