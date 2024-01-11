#include "Classroom.h"

Classroom::Classroom(int capacity, std::string id) {
    this->capacity = capacity;
    this->id = id;
}
Classroom::Classroom() = default;
Classroom::~Classroom() = default;
