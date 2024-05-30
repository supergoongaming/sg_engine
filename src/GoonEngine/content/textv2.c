
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

typedef struct geRichText {
	const char *Text;
	const char *FontName;
	int FontSize;
	geFont *Font;
	geColor Color;
	geImage *Texture;
	geRectangle DrawRect;
	Point TextBounds;
} geRichText;

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

geImage *geCreateTextureForString(const char *word, geFont *font,
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
	geImage *paper = createEmptyTexture(totalWidth * 2, maxHeight * 2);
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
	textureDimensions->x = totalWidth;
	textureDimensions->y = maxHeight;
	return paper;
}

static void textFree(geRichText *t) {
	LogWarn("Freeing text %s", t->Text);
	if (t->Font) geFontFree(t->Font);
	t->Font = NULL;
	geImageFree(t->Texture);
	t->Texture = NULL;
	free(t->Font);
}

static void textNewContent(geContent *content, void *data) {
	geRichText *t = (geRichText *)data;
	content->Data.Text = t;
}

static void textDeleteContent(geContent *content) {
	if (!content) return;
	textFree(content->Data.Text);
}

static void textLoadContent(geContent *content) {
	geRichText *t = content->Data.Text;
	if (t) {
		geRichTextLoad(t);
	}
}

static int textFindContent(const char *path, geContent *content) {
	geRichText *t = (geRichText *)content->Data.Text;
	if (!t) {
		return 0;
	}
	return strcmp(path, t->Text) == 0;
}

void geInitializeTextv2ContentType() {
	geAddContentTypeFunctions(geContentTypeText, textNewContent,
							  textDeleteContent, textLoadContent,
							  textFindContent);
}

geRichText *geRichTextNew(const char *text, const char *fontName, int fontSize,
						  Point *fontBounds) {
	geRichText *t = malloc(sizeof(*t));
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

void geRichTextLoad(geRichText *t) {
	t->Font = geFontNew(t->FontName, t->FontSize);
	if (!t->Font) {
		LogWarn("Could not load font name %s for text, %s", t->FontName,
				t->Text);
	}
	geFontLoad(t->Font);
	t->Texture =
		geCreateTextureForString(t->Text, t->Font, &t->TextBounds, t->Color);
}

void geRichTextSetColor(geRichText *t, geColor *color) {
	t->Color.R = color->R;
	t->Color.G = color->G;
	t->Color.B = color->B;
	t->Color.A = color->A;
}
void geRichTextDraw(geRichText *t) {
	geRectangle r;
	r.x = t->DrawRect.x;
	r.y = t->DrawRect.y;
	r.w = t->DrawRect.w != 0 ? t->DrawRect.w : t->TextBounds.x;
	r.h = t->DrawRect.h != 0 ? t->DrawRect.h : t->TextBounds.y;
	geImageDraw(t->Texture, NULL, &r);
}

void geRichTextFree(geRichText *f) {
	geUnloadContent(geContentTypeText, f->Text);
}
void geRichTextSetDrawRect(geRichText *t, geRectangle *r) {
	t->DrawRect.x = r->x;
	t->DrawRect.y = r->y;
	t->DrawRect.w = r->w;
	t->DrawRect.h = r->h;
}