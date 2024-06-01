#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct geClock geClock;
geClock* geClockNew();
geClock* geClockFree(geClock* c);
void geClockStart(geClock* c);
void geClockUpdate(geClock* c);
int geClockTick(geClock* c);
double geClockGetTps();
int geClockGetTicksThisFrame();
int geClockGetTicksPerSecond();
double geClockGetUpdateTimeMilliseconds();
double geClockGetUpdateTimeSeconds();
#ifdef __cplusplus
}
#endif
