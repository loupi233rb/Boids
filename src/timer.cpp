#include "timer.h"


HighPrecisionTimer::HighPrecisionTimer() {
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startCount);
};

double HighPrecisionTimer::getElapsedSeconds() {
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount);
    return (double)(currentCount.QuadPart - m_startCount.QuadPart) / m_frequency.QuadPart;
};

double HighPrecisionTimer::getElapsedMicroseconds() {
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount);
    return (currentCount.QuadPart - m_startCount.QuadPart) * 1000000.0 / m_frequency.QuadPart;
};

void HighPrecisionTimer::reset() {
    QueryPerformanceCounter(&m_startCount);
};