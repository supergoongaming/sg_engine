#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct geRectangle {
	int x, y, w, h;
} geRectangle;

typedef struct geRectangleF {
	float x, y, w, h;
} geRectangleF;

geRectangle geRectangleZero();
int geRectangleIsZero(geRectangle* lhs);
int geRectangleIsOverlap(geRectangle* lhs, geRectangle* rhs);
geRectangle geRectangleGetOverlapRect(geRectangle* lhs, geRectangle* rhs);

#ifdef __cplusplus
}
#endif
