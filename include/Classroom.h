#pragma once
#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <string>

class Classroom {
public:
    explicit Classroom(int capacity, const std::string& id);
    ~Classroom() = default;
    int32_t m_capacity;
    std::string m_id;

private:
    Classroom() = default;
};



#endif //CLASSROOM_H
