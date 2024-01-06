#pragma once
#ifndef TIMESLOT_H
#define TIMESLOT_H

#include "Course.h"

typedef enum TimeSlotStatus {AVAILABLE,OCCUPIED,BLOCKED} TimeSlotStatus;

class Timeslot {
public:
    Timeslot();
    ~Timeslot();
    Course currentCourse;
    TimeSlotStatus status;
};



#endif //TIMESLOT_H
