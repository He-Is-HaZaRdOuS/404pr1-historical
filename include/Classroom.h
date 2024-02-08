#pragma once
#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <string>

class Classroom {
public:
    explicit Classroom(int capacity, const std::string& id);
    Classroom();
    ~Classroom();
    int capacity;
    std::string id;
};



#endif //CLASSROOM_H
