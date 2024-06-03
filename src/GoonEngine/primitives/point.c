#include <GoonEngine/prim/point.h>

gePoint gePointZero() {
	gePoint point = {0, 0};
	return point;
}
int gePointIsZero(gePoint* p) {
	gePoint p2 = gePointZero();
	return gePointCompare(p, &p2);
}

int gePointCompare(gePoint* lhs, gePoint* rhs) {
	return lhs->x == rhs->x && lhs->y == rhs->y;
}