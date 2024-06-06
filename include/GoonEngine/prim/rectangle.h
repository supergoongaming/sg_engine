#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct geRectangle {
	int x, y, w, h;
} geRectangle;

geRectangle geRectangleZero();
int geRectangleIsZero(geRectangle* lhs);
int geRectangleIsOverlap(geRectangle* lhs, geRectangle* rhs);
geRectangle geRectangleGetOverlapRect(geRectangle* lhs, geRectangle* rhs);

#ifdef __cplusplus
}
#endif
