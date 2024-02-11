#pragma once
#ifndef COURSE_H
#define COURSE_H

#include "Classroom.h"
#include "Typedefs.h"

#include <unordered_map>

class Course {
public:
    explicit Course(const std::string& professorName, const std::string& code, uint32_t examDuration, uint64_t studentCount = 0);
    ~Course() = default;
    uint64_t m_studentCount;
    uint32_t m_examDuration;
    std::string_view m_professorName;
    std::string m_code;
    std::vector<uint32_t> m_studentList;
    std::unordered_map<std::string_view, bool> m_conflictingCourses;
    std::vector<Classroom> m_classrooms;
};



#endif //COURSE_H
