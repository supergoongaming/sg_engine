#pragma once
#include <GoonEngine/prim/color.h>

typedef struct gePoint gePoint;
typedef struct geRectangle geRectangle;

#ifdef __cplusplus
extern "C" {
#endif
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
void geUtilsDrawDebugText(const char *text);
void geUtilsDrawRect(geRectangle *dstRect, geColor *color);
int geUtilsIsPointInRect(geRectangle *rect, gePoint *point);

#ifdef __cplusplus
}
#endif
