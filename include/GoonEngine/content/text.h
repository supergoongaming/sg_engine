
#pragma once
typedef struct geText geText;
typedef struct geColor geColor;
typedef struct geImage geImage;
#include <GoonEngine/prim/point.h>
#include <GoonEngine/prim/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

void geInitializeTextContentType();
geText* geTextNew(const char* text, const char* fontName, int fontSize);
void geTextLoad(geText* t);
void geTextFree(geText* t);
void geTextDraw(geText* t);

// gePoint geTextMeasureDebug(geText* text);
gePoint geTextGetTextSize(geText* text);
int geTextLength(geText* text);
gePoint geTextLocation(geText* t);
geRectangle geTextBoundingBox(geText* t);
geRectangle geTextBoundingBoxWithBackground(geText* t);

// Update Text
void geTextSetColor(geText* t, geColor* color);
void geTextSetBounds(geText* t, int x, int y);
void geTextSetNumDrawCharacters(geText* t, int num);
void geTextSetImage(geText* t, geImage* i);
void geTextSetTextureBackgroundColor(geColor* c);

// Draw data
void geTextSetLocation(geText* t, int x, int y);

#ifdef __cplusplus
}
#endif