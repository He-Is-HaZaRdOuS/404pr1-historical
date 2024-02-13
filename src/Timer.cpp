#include "Timer.h"

Timer::Timer() {
  m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
  Stop();
}

void Timer::Stop() {
  m_EndTimepoint = std::chrono::high_resolution_clock::now();

  const uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
  const uint64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTimepoint).time_since_epoch().count();

  const std::chrono::duration<uint64_t, std::ratio<1, 1000000>>::rep duration = (end - start);
  const auto ms = static_cast<double>(duration) * 0.001;

  std::cout << duration << "us (" << ms << "ms)" << std::endl;
}