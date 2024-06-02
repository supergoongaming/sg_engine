#include <GoonEngine/internal/clock.h>
#include <GoonEngine/gnpch.h>

static double _deltaTimeInSeconds;


void geClockStart(geClock* c) {
	c->Previous = SDL_GetPerformanceCounter();
}

void geClockUpdate(geClock* c) {
	uint64_t current = SDL_GetPerformanceCounter();
	_deltaTimeInSeconds = (double)(current - c->Previous) / (double)SDL_GetPerformanceFrequency();
	c->Previous = current;
}

double geClockGetUpdateTimeMilliseconds() {
	return _deltaTimeInSeconds * 1000;
}

double geClockGetUpdateTimeSeconds() {
	return _deltaTimeInSeconds;
}
