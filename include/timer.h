#pragma once

#include <windows.h>
#include <chrono>

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


class FrameRateController{
private:
	using clock = std::chrono::high_resolution_clock;
	using frame_duration = std::chrono::duration<double, std::ratio<1>>;
	
	clock::time_point last_frame_time;
	clock::time_point start_count_time;
	frame_duration target_frame_time;
	frame_duration target_output_time;
	int frame_count = 0;
	int total_frame = 0;
	double average_fps = 0.;
	
public:
	FrameRateController(double target_frame_rate, double target_gap)
		:last_frame_time(clock::now())
		,start_count_time(clock::now())
		,target_frame_time(1.0 / target_frame_rate)
		,target_output_time(target_gap){}
    void init();
	void wait();
	void UpgrateFrameCount();
	double getFps();
	void outputFps();
};