#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct geRectangle {
	int x, y, w, h;
} geRectangle;

geRectangle geRectangleZero();
int geRectangleIsZero(geRectangle *lhs);

#ifdef __cplusplus
}
#endif
