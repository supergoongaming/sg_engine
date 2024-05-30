
#pragma once
typedef struct geText geText;
typedef struct geColor geColor;
#include <GoonEngine/point.h>
#include <GoonEngine/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

// Should be ran during engine initialization so that the font content can work
// properly
void geInitializeTextContentType();
/**
 * @brief Generates new text.  If bounds is not null word wrapping will happen.
 *
 * @param text The text to generate
 * @param fontName The name of the font to use, will manage this data
 * @param fontSize The size of the font
 * @param fontBounds If passed, will perform word wrapping
 * @return geText*
 */
geText* geTextNew(const char* text, const char* fontName, int fontSize,
				  Point* fontBounds);
// Loads the text
void geTextLoad(geText* t);
// Frees or reduces ref count on text
void geTextFree(geText* t);
// Draw the text to the screen, in screen coordinates
void geTextDraw(geText* t);
void geTextSetColor(geText* t, geColor* color);
void geTextSetDrawRect(geText* t, geRectangle* drawRect);

#ifdef __cplusplus
}
#endif