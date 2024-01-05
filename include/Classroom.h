#pragma once
#ifndef CLASSROOM_H
#define CLASSROOM_H

#include "Typedefs.h"

class Classroom {
public:
    explicit Classroom(int capacity, String id);
    Classroom();
    ~Classroom();
    int capacity;
    String id;
};



#endif //CLASSROOM_H
