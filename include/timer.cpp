#ifndef TIMER_CPP
#define TIMER_CPP
#endif

#include <windows.h>

class HighPrecisionTimer {
private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCount;
    
public:
    HighPrecisionTimer() {
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_startCount);
    }
    
    double getElapsedSeconds() {
        LARGE_INTEGER currentCount;
        QueryPerformanceCounter(&currentCount);
        return (double)(currentCount.QuadPart - m_startCount.QuadPart) / m_frequency.QuadPart;
    }
    
    double getElapsedMicroseconds() {
        LARGE_INTEGER currentCount;
        QueryPerformanceCounter(&currentCount);
        return (currentCount.QuadPart - m_startCount.QuadPart) * 1000000.0 / m_frequency.QuadPart;
    }
    
    void reset() {
        QueryPerformanceCounter(&m_startCount);
    }
};