#ifndef TIMER_CPP
#define TIMER_CPP

#include <windows.h>

class HighPrecisionTimer {
private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startCount;
    
public:
    HighPrecisionTimer();
    double getElapsedSeconds();
    double getElapsedMicroseconds();
    void reset();
};

#endif