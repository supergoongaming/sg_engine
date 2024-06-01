#include <GoonEngine/clock.h>
#include <GoonEngine/window.h>
#include <GoonEngine/gnpch.h>

#define _framesPerSecond 60
#define _fpsRoundAmount 100
static const double _updateTimeSeconds = 1.0 / _framesPerSecond;
static int _framesCounted = 0;
static double _tps = 0;
static int _frameTicks[_framesPerSecond];
static double _timeBetweenFrame = 0;
static int _ticksThisFrame = 0;
static uint64_t _timeHolder;

typedef struct geClock {
	uint64_t Previous;
	double Lag;

} geClock;

geClock* geClockNew() {
	geClock* c = malloc(sizeof(*c));
	c->Previous = SDL_GetPerformanceCounter();
	return c;
}

geClock* geClockFree(geClock* c) {
	free(c);
}

void geClockStart(geClock* c) {
	c->Previous = SDL_GetPerformanceCounter();
}

void geClockUpdate(geClock* c) {
	_timeHolder = SDL_GetPerformanceCounter();
	_timeBetweenFrame = (double)(_timeHolder - c->Previous) / (double)SDL_GetPerformanceFrequency();
	_ticksThisFrame = 0;
	LogWarn("Starting update, frames counted is %d", _framesCounted);
	c->Lag += _timeBetweenFrame;
	c->Previous = _timeHolder;
}

double geClockGetUpdateTimeMilliseconds() {
	return _updateTimeSeconds * 1000;
}

double geClockGetUpdateTimeSeconds() {
	return _updateTimeSeconds;
}

int geClockTick(geClock* c) {
	// Do not try and catchup more than 3 frames.  Game handles up to 180 Vsync.
	// if(_ticksThisFrame > 3) return false;
	if (c->Lag >= _updateTimeSeconds) {
		++_ticksThisFrame;
		c->Lag -= _updateTimeSeconds;
		return true;
	}
	// update the ticks for this frame
	_frameTicks[_framesCounted] = _ticksThisFrame;
	LogWarn("Frames counted is %d", _framesCounted);
	if (_framesCounted < _framesPerSecond) {
		++_framesCounted;
	} else {
		LogWarn("Calculating average..");
		double average = 0;
		for (size_t i = 0; i < _framesPerSecond; i++) {
			average += _frameTicks[i];
		}
		_tps = _framesPerSecond * (average / (double)_framesPerSecond);
		LogWarn("TPS is %f", _tps);
		LogWarn("Setting Frames counted to 0");
		_framesCounted = 0;
	}
	return false;
}

double geClockGetTps() {
	// Calculate the average if we up the speed
	return _tps * ((double)geWindowGetRefreshRate() / (double)60) ;
	// double thing = _tps * _fpsRoundAmount;
	// double thing2 = thing/_fpsRoundAmount;
	// return thing2;
}

int geClockGetTicksThisFrame() {
}
int geClockGetTicksPerSecond() {
}