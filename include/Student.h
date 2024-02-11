#pragma once
#ifndef STUDENT_H
#define STUDENT_H
#include <cstdint>

class Student {
public:
    explicit Student(uint32_t id);
    ~Student();

    uint32_t m_id;

};



#endif //STUDENT_H
