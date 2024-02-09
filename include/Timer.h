#pragma once
#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <iostream>

class Timer {
public:
  Timer();
  ~Timer();

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTimepoint;

  void Stop();
};

#endif //TIMER_H
