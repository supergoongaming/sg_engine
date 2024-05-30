
#pragma once
typedef struct geRichText geRichText;
typedef struct geColor geColor;
#include <GoonEngine/point.h>
#include <GoonEngine/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

// Should be ran during engine initialization so that the font content can work
// properly
void geInitializeTextv2ContentType();
/**
 * @brief Generates new text.  If bounds is not null word wrapping will happen.
 *
 * @param text The text to generate
 * @param fontName The name of the font to use, will manage this data
 * @param fontSize The size of the font
 * @param fontBounds If passed, will perform word wrapping
 * @return geText*
 */
geRichText* geRichTextNew(const char* text, const char* fontName, int fontSize,
				  Point* fontBounds);
// Loads the text
void geRichTextLoad(geRichText* t);
// Frees or reduces ref count on text
void geRichTextFree(geRichText* t);
// Draw the text to the screen, in screen coordinates
void geRichTextDraw(geRichText* t);
void geRichTextSetColor(geRichText* t, geColor* color);
void geRichTextSetDrawRect(geRichText* t, geRectangle* drawRect);

#ifdef __cplusplus
}
#endif