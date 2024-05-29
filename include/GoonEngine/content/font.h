#pragma once
typedef struct geFont geFont;
#ifdef __cplusplus
extern "C" {
#endif

// Should be ran during engine initialization so that the font content can work properly
void geInitializeFontContentType();
/**
 * @brief Initializes a new font, or gets an already loaded one from the cache
 *
 * @param fontPath path to the font
 * @param size size of the font, should be in increments of 16
 * @return geFont* Font ptr, likely used when drawing text.
 */
geFont* geFontNew(const char* fontPath, int size);
// Loads a font
void geFontLoad(geFont* f);

#ifdef __cplusplus
}
#endif