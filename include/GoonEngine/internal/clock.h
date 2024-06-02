#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clock is used for measuring the gametime of the game, is a simple wrapper to keep track of timings.
 *
 */
typedef struct geClock {
	uint64_t Previous;
} geClock;
void geClockStart(geClock* c);
void geClockUpdate(geClock* c);
double geClockGetUpdateTimeMilliseconds();
double geClockGetUpdateTimeSeconds();
#ifdef __cplusplus
}
#endif
