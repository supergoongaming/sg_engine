
#include <GoonEngine/SdlSurface.h>
#include <GoonEngine/color.h>
#include <GoonEngine/content/content.h>
#include <GoonEngine/content/font.h>
#include <GoonEngine/content/text.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/rectangle.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include <stdbool.h>
#include FT_FREETYPE_H

typedef struct geText {
	const char *Text;
	const char *FontName;
	int FontSize;
	geFont *Font;
	geColor Color;
	SDL_Texture *Texture;
	geRectangle DrawRect;
	Point TextBounds;
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

SDL_Surface *createEmptySurface(int width, int height) {
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
		0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
	if (surface == NULL) {
		printf("Failed to create empty surface: %s\n", SDL_GetError());
		return NULL;
	}
	SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
	return surface;
}

SDL_Texture *geCreateTextureForString(const char *word, geFont *font,
									  Point *textureDimensions, geColor color) {
	int totalWidth = 0;
	int maxHeight = 0;
	FT_Face f = geFontGetFont(font);
	for (size_t i = 0; i < strlen(word); i++) {
		char letter = word[i];
		int result = FT_Load_Char(f, letter, FT_LOAD_RENDER);
		if (result) {
			printf("Failed to load character %c with error code %d \n", letter,
				   result);
			continue;
		}
		totalWidth += (f->glyph->advance.x >> 6);
		int letterHeight = (f->ascender - f->descender) >> 6;
		maxHeight = maxHeight > letterHeight ? maxHeight : letterHeight;
	}
	SDL_Surface *paper = createEmptySurface(totalWidth * 2, maxHeight * 2);
	int x = 0;
	int baseline = 0;
	for (size_t i = 0; i < strlen(word); i++) {
		char letter = word[i];
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
		SDL_Surface *letterSurface =
			geCreateSurfaceForCharacter(f, color.R, color.G, color.B);
		// Don't blit if this was a space, but still advance.
		if (!letterSurface) {
			x += (f->glyph->advance.x >> 6);
			SDL_FreeSurface(letterSurface);
			continue;
		}
		SDL_Rect dst = {x, y, letterSurface->w, letterSurface->h};
		SDL_BlitSurface(letterSurface, NULL, paper, &dst);
		x += (f->glyph->advance.x >> 6);
		SDL_FreeSurface(letterSurface);
	}
	textureDimensions->x = totalWidth;
	textureDimensions->y = maxHeight;
	SDL_Texture *charTexture = geCreateTextureFromSurface(paper);
	return charTexture;
}

static void textFree(geText *t) {
	LogWarn("Freeing text %s", t->Text);
	if (t->Font) geFontFree(t->Font);
	t->Font = NULL;
	geDestroyTexture(t->Texture);
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

geText *geTextNew(const char *text, const char *fontName, int fontSize,
				  Point *fontBounds) {
	geText *t = malloc(sizeof(*t));
	t->Text = text;
	t->Texture = NULL;
	t->Font = NULL;
	t->FontSize = fontSize;
	t->FontName = fontName;
	t->DrawRect.x = t->DrawRect.y = t->DrawRect.w = t->DrawRect.h = 0;
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
	t->Texture =
		geCreateTextureForString(t->Text, t->Font, &t->TextBounds, t->Color);
}

void geTextSetColor(geText *t, geColor *color) {
	t->Color.R = color->R;
	t->Color.G = color->G;
	t->Color.B = color->B;
	t->Color.A = color->A;
}
void geTextDraw(geText *t) {
	geRectangle r;
	r.x = t->DrawRect.x;
	r.y = t->DrawRect.y;
	r.w = t->DrawRect.w != 0 ? t->DrawRect.w : t->TextBounds.x;
	r.h = t->DrawRect.h != 0 ? t->DrawRect.h : t->TextBounds.y;
	geDrawTexture(t->Texture, NULL, &r, false);
}

void geTextFree(geText *f) { geUnloadContent(geContentTypeText, f->Text); }
void geTextSetDrawRect(geText *t, geRectangle *r) {
	t->DrawRect.x = r->x;
	t->DrawRect.y = r->y;
	t->DrawRect.w = r->w;
	t->DrawRect.h = r->h;
}