#include "Timer.h"

Timer::Timer() {
  m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
  Stop();
}

void Timer::Stop() {
  m_EndTimepoint = std::chrono::high_resolution_clock::now();

  const u_int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
  const u_int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTimepoint).time_since_epoch().count();

  const std::chrono::duration<long, std::ratio<1, 1000000>>::rep duration = end - start;
  const double ms = duration * 0.001;

  std::cout << duration << "us (" << ms << "ms)" << std::endl;
}