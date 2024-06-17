#include <GoonEngine/content/content.h>
#include <GoonEngine/content/font.h>
#include <GoonEngine/content/image.h>
#include <GoonEngine/content/text.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/prim/color.h>
#include <GoonEngine/utils.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include FT_FREETYPE_H

#define LETTER_BUFFER_LENGTH 100
static geColor _textBackgroundColor = {0, 0, 100, 200};

typedef struct geText {
	char *Text;
	const char *FontName;
	int FontSize;
	int WordWrap;
	int LettersToDraw;
	int CurrentLettersDrawn;
	int PaddingL, PaddingR, PaddingT, PaddingB;
	int Loaded;
	geFont *Font;
	geColor Color;
	geImage *Texture;
	gePoint TextBounds;
	geRectangle BoundingBox;
	gePoint *LetterPoints;
} geText;

SDL_Surface *geCreateSurfaceForCharacter(FT_Face face, int r, int g, int b) {
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
	SDL_SetPaletteColors(surface->format->palette, palette.colors, 0, palette.ncolors);
	SDL_SetColorKey(surface, SDL_TRUE, 0);
	return surface;
}

int checkShouldWrap(int x, int wordLength, int glyphWidth, int maxX) {
	return x + wordLength + glyphWidth > maxX;
}

int getLetterWidth(geText *t, char l) {
	FT_Face f = geFontGetFont(t->Font);
	int result = FT_Load_Char(f, l, FT_LOAD_DEFAULT);
	if (result) {
		LogError("Could not measure character properly.  Char %s, error %d", l, result);
		return 0;
	}
	return f->glyph->advance.x >> 6;
}

int getLetterYBearing(geText *t, char l) {
	FT_Face f = geFontGetFont(t->Font);
	int result = FT_Load_Char(f, l, FT_LOAD_DEFAULT);
	if (result) {
		LogError("Could not measure character properly.  Char %s, error %d", l, result);
		return 0;
	}
	return f->glyph->metrics.horiBearingY >> 6;
}

int getKerning(geText *t, int i) {
	if (strlen(t->Text) <= i) {
		return 0;
	}
	FT_Face f = geFontGetFont(t->Font);
	if (!FT_HAS_KERNING(f)) {
		return 0;
	}
	unsigned int glyph_index_c = FT_Get_Char_Index(f, t->Text[i]);
	unsigned int glyph_index_n = FT_Get_Char_Index(f, t->Text[i + 1]);
	FT_Vector delta;
	int result = FT_Get_Kerning(f, glyph_index_c, glyph_index_n, FT_KERNING_DEFAULT, &delta);
	if (result) {
		LogError("Could not get kerning");
	}
	result = delta.x >> 6;
	if (delta.x != 0) {
		LogWarn("Wow theres actually kerning and this hasen't been tested");
	}
	return result;
}

// Adds a word to the text letter points.  Takes the current index, and the current words length and will write it
void addWordToLetterPoints(geText *t, int wordEndPos, int wordLength, int penX, int penY) {
	int x = penX, y = penY, wordStartPos = wordEndPos - wordLength;
	for (size_t i = 0; i < wordLength; i++) {
		int wordI = wordStartPos + i;
		if (wordI >= strlen(t->Text)) {
			LogWarn("How is this possible?");
			return;
		}
		char letter = t->Text[wordI];
		gePoint p;
		p.x = x;
		p.x -= getKerning(t, wordI);
		p.y = y - getLetterYBearing(t, letter);
		t->LetterPoints[wordI] = p;
		int width = getLetterWidth(t, letter);
		x += width;
	}
}

gePoint measure(geText *t) {
	int maxWidth = t->TextBounds.x ? t->TextBounds.x : INT_MAX;
	int maxHeight = t->TextBounds.y ? t->TextBounds.y : INT_MAX;
	maxWidth -= t->PaddingR;
	gePoint textSize = gePointZero();
	int currentWordLength = 0, currentWordLetters = 0;
	FT_Face f = geFontGetFont(t->Font);
	int ascenderInPixels = (f->ascender * t->FontSize) / f->units_per_EM;
	int descenderInPixels = (f->descender * t->FontSize) / f->units_per_EM;
	int lineSpace = (f->height * t->FontSize) / f->units_per_EM;
	int startLoc = ascenderInPixels + t->PaddingT;

	// int penX = 0, penY = startLoc;
	int penX = t->PaddingL, penY = startLoc;
	for (size_t i = 0; i < strlen(t->Text); i++) {
		char letter = t->Text[i];
		if (letter == '\n') {
			if (currentWordLength) {
				addWordToLetterPoints(t, i, currentWordLetters, penX, penY);
				penX += currentWordLength;
				if (penX > textSize.x) {
					textSize.x = penX;
				}
			}
			penX = t->PaddingL;
			penY += lineSpace;
			currentWordLength = 0;
			currentWordLetters = 0;
			continue;
		}
		int letterSize = getLetterWidth(t, letter);
		if (letter == ' ') {
			addWordToLetterPoints(t, i, currentWordLetters, penX, penY);
			penX += currentWordLength + letterSize;
			currentWordLength = 0;
			currentWordLetters = 0;
			continue;
		}
		if (checkShouldWrap(penX, currentWordLength, letterSize, maxWidth)) {
			if (penX > textSize.x) {
				textSize.x = penX;
			}
			penX = t->PaddingL;
			penY += lineSpace;
		}
		currentWordLength += letterSize;
		++currentWordLetters;
	}
	if (currentWordLength) {
		addWordToLetterPoints(t, strlen(t->Text), currentWordLetters, penX, penY);
		penX += currentWordLength;
	}
	textSize.x = MAX(textSize.x, penX);
	textSize.y = penY - descenderInPixels;
	if (textSize.y > maxHeight) {
		LogWarn("Your text overflowed through Y, please adjust your bounds else it will flow past");
	}
	return textSize;
}

void getLetterContentName(geText *t, char *buf, char letter) {
	size_t len = 1 + strlen(t->FontName) + 4 + 1;
	snprintf(buf, len, "%c%s%d", letter, t->FontName, t->FontSize);
}

void createTexturesForText(geText *t) {
	FT_Face f = geFontGetFont(t->Font);
	char letterString[LETTER_BUFFER_LENGTH];
	for (size_t i = 0; i < strlen(t->Text); i++) {
		char letter = t->Text[i];
		if (letter == ' ' || letter == '\n') {
			continue;
		}
		int result = FT_Load_Char(f, letter, FT_LOAD_RENDER);
		if (result) {
			printf("Failed to load character %c with error code %d \n", letter,
				   result);
			continue;
		}
		SDL_Surface *letterSurface = geCreateSurfaceForCharacter(f, t->Color.R, t->Color.G, t->Color.B);
		if (!letterSurface) {
			LogError("Failed to create surface for letter!");
			continue;
		}
		getLetterContentName(t, letterString, letter);
		geImageNewFromSurface(letterString, letterSurface);
	}
}

static void loadLetters(geText *t, int startLoc) {
	char letterString[LETTER_BUFFER_LENGTH];
	for (size_t i = startLoc; i < t->LettersToDraw; i++) {
		char letter = t->Text[i];
		if (letter == ' ' || letter == '\n') {
			continue;
		}
		getLetterContentName(t, letterString, letter);
		geContent *content;
		content = geGetLoadedContentWeak(geContentTypeImage, letterString);
		if (!content) {
			LogError("Content not found when looking for %s", letterString);
			continue;
		}
		geImage *image = content->Data.Image;
		if (!image) {
			LogError("Image is null?");
			continue;
		}
		geRectangle r;
		r.x = t->LetterPoints[i].x;
		r.y = t->LetterPoints[i].y;
		r.w = geImageWidth(image);
		r.h = geImageHeight(image);
		geImageDrawImageToImage(image, t->Texture, NULL, &r);
	}
}
static void unloadLetters(geText *t) {
	char letterString[LETTER_BUFFER_LENGTH];
	size_t size = strlen(t->Text);
	for (size_t i = 0; i < size; i++) {
		char letter = t->Text[i];
		if (letter == ' ' || letter == '\n') {
			continue;
		}
		getLetterContentName(t, letterString, letter);
		// LogWarn("Letter is %c and letterstring is %s, trying to delete", letter, letterString);
		geContent *content = geGetLoadedContentWeak(geContentTypeImage, letterString);
		if (!content) {
			LogError("Content not found when trying to delete letter %s", letterString);
			continue;
		}
		geImage *image = content->Data.Image;
		if (!image) {
			LogError("Image is null? when trying to delete letter");
			continue;
		}
		geImageFree(image);
	}
}

static void textFree(geText *t) {
	LogDebug("Freeing text %s", t->Text);
	if (t->Font) {
		geFontFree(t->Font);
	}
	t->Font = NULL;
	if (t->LetterPoints) free(t->LetterPoints);
	t->LetterPoints = NULL;
	if (t->Texture) {
		geImageFree(t->Texture);
	}
	unloadLetters(t);
	if (t->Text) {
		free(t->Text);
	}
	t->Texture = NULL;
	free(t);
	t = NULL;
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
	geAddContentTypeFunctions(geContentTypeText, textNewContent, textDeleteContent, textLoadContent, textFindContent);
}

geText *geTextNew(const char *text, const char *fontName, int fontSize) {
	geContent *loadedContent = geGetLoadedContent(geContentTypeText, text);
	if (loadedContent) {
		return loadedContent->Data.Text;
	}

	geText *t = malloc(sizeof(*t));
	t->Text = strdup(text);
	// TODO trying 10 here as I kept getting a write out of bounds.., I thought 1 would be good enough for null terminator but..
	t->LetterPoints = calloc(strlen(text) + 10, sizeof(gePoint));
	t->PaddingB = t->PaddingL = t->PaddingR = t->PaddingL = 0;
	t->Texture = NULL;
	t->Font = NULL;
	t->FontSize = fontSize;
	t->FontName = fontName;
	t->BoundingBox = geRectangleZero();
	t->TextBounds = gePointZero();
	t->WordWrap = true;
	t->LettersToDraw = strlen(text);
	t->Loaded = false;
	t->CurrentLettersDrawn = 0;
	t->Color.R = t->Color.G = t->Color.B = t->Color.A = 255;
	geAddContent(geContentTypeText, t);
	return t;
}

void geTextLoad(geText *t) {
	if (t->Loaded) return;
	if (!t->Font) {
		t->Font = geFontNew(t->FontName, t->FontSize);
		if (!t->Font) {
			LogWarn("Could not load font name %s for text, %s", t->FontName,
					t->Text);
		}
	}
	geFontLoad(t->Font);
	createTexturesForText(t);
	gePoint textSize = measure(t);
	t->BoundingBox.w = textSize.x;
	t->BoundingBox.h = textSize.y;
	char buf[BUFSIZ];
	snprintf(buf, BUFSIZ, "%s_%s_%d", t->Text, t->FontName, t->FontSize);
	if (!t->Texture) {
		// TODO right now the texture color is always going to be this, can this be updated?
		geColor c = {0, 0, 100, 200};
		t->Texture = geImageNewRenderTarget(buf, t->BoundingBox.w, t->BoundingBox.h, &c);
	}
	loadLetters(t, 0);
	t->Loaded = true;
}

void geTextSetPadding(geText *t, int l, int r, int top, int b) {
	t->PaddingL = l;
	t->PaddingB = b;
	t->PaddingT = top;
	t->PaddingR = r;
}

void geTextSetColor(geText *t, geColor *color) {
	t->Color.R = color->R;
	t->Color.G = color->G;
	t->Color.B = color->B;
	t->Color.A = color->A;
}
void geTextSetTextureBackgroundColor(geColor *c) {
	_textBackgroundColor.R = c->R;
	_textBackgroundColor.G = c->G;
	_textBackgroundColor.B = c->B;
	_textBackgroundColor.A = c->A;
}

void geTextSetBounds(geText *t, int x, int y) {
	t->TextBounds.x = x;
	t->TextBounds.y = y;
}

void geTextDraw(geText *t) {
	// geImageDraw(t->Texture, NULL, &t->BoundingBox);
	geRectangle d;
	d.x = t->BoundingBox.x;
	d.y = t->BoundingBox.y;
	// Try setting to image size.
	d.w = t->TextBounds.x;
	d.h = t->TextBounds.y;
	// geImageDraw(t->Texture, NULL, &t->BoundingBox);
	geImageDraw(t->Texture, NULL, &d);
}

void geTextFree(geText *f) {
	geUnloadContent(geContentTypeText, f->Text);
}

void geTextSetLocation(geText *t, int x, int y) {
	t->BoundingBox.x = x;
	t->BoundingBox.y = y;
}

gePoint geTextGetTextSize(geText *text) {
	gePoint p;
	p.x = text->BoundingBox.w;
	p.y = text->BoundingBox.h;
	return p;
}
void geTextSetImage(geText *t, geImage *i) {
	t->Texture = i;
}

void geTextSetNumDrawCharacters(geText *t, int num) {
	if (num > strlen(t->Text)) {
		return;
	}
	if (num < t->CurrentLettersDrawn && t->Texture) {
		// We need to clear and then draw.
		geColor c = {0, 0, 100, 200};
		geImageClear(t->Texture, &c);
	}

	int current = t->LettersToDraw;
	t->LettersToDraw = num;
	if (t->Texture) {
		loadLetters(t, current);
	}
	t->CurrentLettersDrawn = num;
}
int geTextLength(geText *text) {
	return strlen(text->Text);
}

gePoint geTextLocation(geText *t) {
	gePoint p;
	p.x = t->BoundingBox.x;
	p.y = t->BoundingBox.y;
	return p;
}

geRectangle geTextBoundingBox(geText *t) {
	return t->BoundingBox;
}
// TODO sometimes this is bigger than the background?  Probably something with the bounding box measurement?
geRectangle geTextBoundingBoxWithBackground(geText *t) {
	geRectangle r = t->BoundingBox;
	r.w = MAX(r.w, geImageWidth(t->Texture));
	r.h = MAX(r.h, geImageHeight(t->Texture));
	return r;
}