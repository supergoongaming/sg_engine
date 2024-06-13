#pragma once
#include <stdlib.h>
#include <GoonEngine/prim/color.h>

typedef struct gePoint gePoint;
typedef struct geRectangle geRectangle;
typedef struct geRectangleF geRectangleF;

#ifdef __cplusplus
extern "C" {
#endif
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
void geUtilsDrawDebugText(const char *text);
void geUtilsDrawRect(geRectangle *dstRect, geColor *color);
void geUtilsDrawRectF(geRectangleF *dstRect, geColor *color);
int geUtilsIsPointInRect(geRectangle *rect, gePoint *point);
int GetLoadFilename(char* buffer, size_t bufferSize, const char* filename);
int geGetFileFilepath(char* buffer, size_t bufferSize, const char* filename);

#ifdef __cplusplus
}
#endif
