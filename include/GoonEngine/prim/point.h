#pragma once
#ifdef _cplusplus
extern "C" {
#endif

typedef struct gePoint {
	int x, y;
} gePoint;

gePoint gePointZero();
int gePointIsZero(gePoint* p);
int gePointCompare(gePoint* lhs, gePoint* rhs);
#ifdef _cplusplus
}
#endif