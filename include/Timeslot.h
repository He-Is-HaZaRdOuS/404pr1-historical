#pragma once
#ifndef TIMESLOT_H
#define TIMESLOT_H

#include "Course.h"

namespace n_Timeslot {
    typedef enum TimeSlotStatus {AVAILABLE,OCCUPIED,BLOCKED} TimeSlotStatus;
}


class Timeslot {
public:
    Timeslot();
    ~Timeslot();
    Course m_currentCourse;
    n_Timeslot::TimeSlotStatus m_status;
};



#endif //TIMESLOT_H
