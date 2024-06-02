#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
void geUtilsDrawDebugText(const char *text);
void geUtilsDrawRect(geRectangle *dstRect, geColor *color);

#ifdef __cplusplus
}
#endif
