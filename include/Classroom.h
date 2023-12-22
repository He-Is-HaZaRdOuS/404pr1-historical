#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <string>
typedef std::string String;

class Classroom {
public:
    Classroom(int capacity, String id);
    Classroom();
    ~Classroom();
    int capacity;
    String id;
};



#endif //CLASSROOM_H
