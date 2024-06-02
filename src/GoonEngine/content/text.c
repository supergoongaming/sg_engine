
#include <GoonEngine/content/content.h>
#include <GoonEngine/content/font.h>
#include <GoonEngine/content/image.h>
#include <GoonEngine/content/text.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/primitives/color.h>
#include <GoonEngine/primitives/rectangle.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include <stdbool.h>
#include FT_FREETYPE_H

typedef struct geText {
	const char *Text;
	const char *FontName;
	int FontSize;
	int WordWrap;
	int LettersToDraw;
	geFont *Font;
	geColor Color;
	geImage *Texture;
	gePoint TextLocation;
	gePoint TextDrawSize;
	gePoint TextSize;
} geText;

SDL_Surface *geCreateSurfaceForCharacter(FT_Face face, int r, int g, int b) {
	// Check if this is a space, return null if so
	if (face->glyph->bitmap.width == 0 && face->glyph->bitmap.rows == 0)
		return NULL;
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
		face->glyph->bitmap.buffer, face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		8 * face->glyph->bitmap.pitch / face->glyph->bitmap.width,
		face->glyph->bitmap.pitch, SDL_PIXELFORMAT_INDEX8);
	SDL_Palette palette;
	palette.colors = (SDL_Color *)alloca(256 * sizeof(SDL_Color));
	int numColors = 256;
	for (int i = 0; i < numColors; ++i) {
		palette.colors[i].r = r;
		palette.colors[i].g = g;
		palette.colors[i].b = b;
		palette.colors[i].a = (Uint8)(i);
	}

	palette.ncolors = numColors;
	SDL_SetPaletteColors(surface->format->palette, palette.colors, 0,
						 palette.ncolors);
	SDL_SetColorKey(surface, SDL_TRUE, 0);
	return surface;
}

geImage *createEmptyTexture(int width, int height) {
	return geImageNewRenderTarget("thing", width, height);
}

// Word is the entire string here.
// geImage *createTextureForText(const char *word, geFont *font, gePoint *textureDimensions, geColor color) {
geImage *createTextureForText(geText *t) {
	int maxWidth = 0;
	int maxHeight = 0;
	FT_Face f = geFontGetFont(t->Font);
	// Set the bounds of the text if set.
	if (gePointIsZero(&t->TextDrawSize)) {
		maxWidth = t->TextDrawSize.x;
		maxHeight = t->TextDrawSize.y;
	}
	// Set the size based on the size of the text.
	else {
		for (size_t i = 0; i < strlen(t->Text); i++) {
			char letter = t->Text[i];
			int result = FT_Load_Char(f, letter, FT_LOAD_RENDER);
			if (result) {
				LogError("Failed to load character %c with error code %d \n", letter, result);
				continue;
			}
			maxWidth += (f->glyph->advance.x >> 6);
			int letterHeight = (f->ascender - f->descender) >> 6;
			maxHeight = maxHeight > letterHeight ? maxHeight : letterHeight;
		}
	}
	geImage *paper = createEmptyTexture(maxWidth * 2, maxHeight * 2);
	int x = 0;
	int baseline = 0;
	for (size_t i = 0; i < strlen(t->Text); i++) {
		char letter = t->Text[i];
		int result = FT_Load_Char(f, letter, FT_LOAD_RENDER);
		if (result) {
			printf("Failed to load character %c with error code %d \n", letter,
				   result);
			continue;
		}
		// remember everything is in 64 per one pixel, so you must always use
		// the bitshifter
		if (baseline == 0) {
			baseline = (f->ascender - f->descender) >> 6;
		}
		int y = baseline - (f->glyph->metrics.horiBearingY >> 6);
		SDL_Surface *letterSurface = geCreateSurfaceForCharacter(f, t->Color.R, t->Color.G, t->Color.B);
		// Don't blit if this was a space, but still advance.
		if (!letterSurface) {
			x += (f->glyph->advance.x >> 6);
			SDL_FreeSurface(letterSurface);
			continue;
		}
		geRectangle dst = {x, y, letterSurface->w, letterSurface->h};
		// Convert to a texture
		// SDL_Texture *letterTexture =
		char letterString[2];
		letterString[0] = letter;
		letterString[1] = '\0';
		geImage *letterTexture =
			geImageNewFromSurface(letterString, letterSurface);
		// Draw this onto the paper
		geImageDrawImageToImage(letterTexture, paper, NULL, &dst);
		x += (f->glyph->advance.x >> 6);
	}
	t->TextSize.x = maxWidth;
	t->TextSize.y = maxHeight;
	return paper;
}

static void textFree(geText *t) {
	LogWarn("Freeing text %s", t->Text);
	if (t->Font) geFontFree(t->Font);
	t->Font = NULL;
	geImageFree(t->Texture);
	t->Texture = NULL;
	free(t->Font);
}

static void textNewContent(geContent *content, void *data) {
	geText *t = (geText *)data;
	content->Data.Text = t;
}

static void textDeleteContent(geContent *content) {
	if (!content) return;
	textFree(content->Data.Text);
}

static void textLoadContent(geContent *content) {
	geText *t = content->Data.Text;
	if (t) {
		geTextLoad(t);
	}
}

static int textFindContent(const char *path, geContent *content) {
	geText *t = (geText *)content->Data.Text;
	if (!t) {
		return 0;
	}
	return strcmp(path, t->Text) == 0;
}

void geInitializeTextContentType() {
	geAddContentTypeFunctions(geContentTypeText, textNewContent,
							  textDeleteContent, textLoadContent,
							  textFindContent);
}

geText *geTextNew(const char *text, const char *fontName, int fontSize) {
	geText *t = malloc(sizeof(*t));
	t->Text = text;
	t->Texture = NULL;
	t->Font = NULL;
	t->FontSize = fontSize;
	t->FontName = fontName;
	t->TextLocation = gePointZero();
	t->TextSize = gePointZero();
	t->TextDrawSize = gePointZero();
	t->WordWrap = true;
	t->Color.R = t->Color.G = t->Color.B = t->Color.A = 255;
	geAddContent(geContentTypeText, t);
	return t;
}

void geTextLoad(geText *t) {
	t->Font = geFontNew(t->FontName, t->FontSize);
	if (!t->Font) {
		LogWarn("Could not load font name %s for text, %s", t->FontName,
				t->Text);
	}
	geFontLoad(t->Font);
	// t->Texture = createTextureForText(t->Text, t->Font, &t->TextSize, t->Color);
	t->Texture = createTextureForText(t);
}

void geTextSetColor(geText *t, geColor *color) {
	t->Color.R = color->R;
	t->Color.G = color->G;
	t->Color.B = color->B;
	t->Color.A = color->A;
}
void geTextDraw(geText *t) {
	geRectangle r;
	r.x = t->TextLocation.x;
	r.y = t->TextLocation.y;
	r.w = t->TextDrawSize.x;
	r.h = t->TextDrawSize.y;
	geImageDraw(t->Texture, NULL, &r);
}

void geTextFree(geText *f) {
	geUnloadContent(geContentTypeText, f->Text);
}

void geTextSetLocation(geText *t, int x, int y) {
	t->TextLocation.x = x;
	t->TextLocation.y = y;
}

void geTextSetDrawSize(geText *t, int x, int y) {
	t->TextDrawSize.x = x;
	t->TextDrawSize.y = y;
}

void geTextSetDrawRect(geText *t, geRectangle *r) {
	t->TextLocation.x = r->x;
	t->TextLocation.y = r->y;
	t->TextDrawSize.x = r->w;
	t->TextDrawSize.y = r->y;
}