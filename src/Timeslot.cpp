#include "Timeslot.h"

Timeslot::Timeslot(): m_currentCourse("---","---",0,0), m_status(n_Timeslot::TimeSlotStatus::AVAILABLE) {
}

Timeslot::~Timeslot() = default;
