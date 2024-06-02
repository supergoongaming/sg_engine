
#pragma once
typedef struct geText geText;
typedef struct geColor geColor;
#include <GoonEngine/primitives/point.h>
#include <GoonEngine/primitives/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

void geInitializeTextContentType();
geText* geTextNew(const char* text, const char* fontName, int fontSize);
void geTextLoad(geText* t);
void geTextFree(geText* t);
void geTextDraw(geText* t);
void geTextSetColor(geText* t, geColor* color);
void geTextSetLocation(geText* t, int x, int y);
void geTextSetDrawSize(geText* t, int x, int y);
void geTextSetDrawRect(geText* t, geRectangle* drawRect);

#ifdef __cplusplus
}
#endif