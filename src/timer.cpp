#include "timer.h"

#include <stdio.h>

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

#include <thread>

void FrameRateController::wait(){
	auto current_time = clock::now();
	auto elapsed_time = current_time - this->last_frame_time;
	auto sleep_time = this->target_frame_time - elapsed_time;
	// std::this_thread::sleep_until(last_frame_time + target_frame_time);
	do{
		sleep_time = last_frame_time + this->target_frame_time - clock::now();
	} while(sleep_time > frame_duration::zero());
	last_frame_time = clock::now();
	UpgrateFrameCount();
	return;
}

void FrameRateController::UpgrateFrameCount(){
	total_frame++;
	frame_count++;
	if((last_frame_time - start_count_time) >= target_output_time){
		average_fps = static_cast<double>(frame_count / std::chrono::duration_cast<std::chrono::duration<double>>(last_frame_time-start_count_time).count());
		frame_count = 0;

		// outputFps();

		start_count_time = last_frame_time;
	}
}

double FrameRateController::getFps(){return average_fps;}

void FrameRateController::outputFps(){
	printf("%.4f\n",average_fps);
	return;
}

void FrameRateController::init()
{
    this->last_frame_time = clock::now();
}

double FrameRateController::getLERP(){
	return std::chrono::duration_cast<std::chrono::duration<double>>(clock::now() - last_frame_time).count() / target_frame_time.count();
}

bool FrameRateController::readyToUpdate(){
	auto gap = clock::now() - last_frame_time;

}

GTimer::GTimer(double rate, int maxstep){
	this->deltaTime = duration(1.0 / rate);
	this->maxStep = maxstep;
	this->curStep = 0;
	this->accumulator = duration::zero();
}

void GTimer::init(){
	this->startTime = clock::now();
	this->accumulator = duration::zero();
	this->curStep = 0;
}

void GTimer::addTask(const std::function<void()>& task){
	this->tasks.push_back(task);
}

void GTimer::step(){
	clock::time_point current = clock::now();
	this->accumulator += std::chrono::duration_cast<duration>(current - this->startTime);
	startTime = current;
	while(this->accumulator >= this->deltaTime){
		this->accumulator -= this->deltaTime;
		this->curStep++;
		if(this->curStep > this->maxStep){
			this->accumulator = duration::zero();
			break;
		}
		for(auto &task:tasks){
			task();
		}
	}
	this->curStep = 0;
	return;
}

double GTimer::getLERP(){
	return this->accumulator.count() / this->deltaTime.count();
}