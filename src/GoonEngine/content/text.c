
#include <GoonEngine/content/content.h>
#include <GoonEngine/content/font.h>
#include <GoonEngine/content/image.h>
#include <GoonEngine/content/text.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/primitives/color.h>
#include <GoonEngine/primitives/rectangle.h>
#include <GoonEngine/utils.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include <math.h>
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
	gePoint TextBounds;
	gePoint TextLocation;
	gePoint TextDrawSize;
	gePoint TextSize;
	gePoint *LetterPoints;
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

// Check if we should start a new word
bool isLetterSpaceOrNewline(char l) {
	return l == ' ' || l == '\n';
}

// Gets the amount of distance to move the y value on word wrap or newline
int getLineHeight(FT_Face face) {
	return face->size->metrics.height >> 6;
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

gePoint measureFullText(geText *t) {
	int maxWidth = 9999;
	int maxHeight = 9999;
	// Set the bounds of the text if set.
	if (!gePointIsZero(&t->TextBounds)) {
		if(t->TextBounds.x) {
			maxWidth = t->TextBounds.x;
		}
		if(t->TextBounds.y) {
			maxHeight = t->TextBounds.y;
		}
	}
	// Measure and see where we end up.
	gePoint textSize = gePointZero();
	int currentWordLength = 0;
	int currentWordLetters = 0;
	FT_Face f = geFontGetFont(t->Font);
	int unitsPerEM = f->units_per_EM;

	// Convert ascender and descender to pixels
	int ascenderInPixels = (f->ascender * t->FontSize) / unitsPerEM;
	int descenderInPixels = (f->descender * t->FontSize) / unitsPerEM;
	int heightInPixels = (f->height * t->FontSize) / unitsPerEM;
	int startLoc = ascenderInPixels;
	int lineSpace = heightInPixels;
	int penX = 0, penY = startLoc;
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
			penX = 0;
			penY += lineSpace;
			currentWordLength = 0;
			currentWordLetters = 0;
			continue;
		}
		if (letter == ' ') {
			addWordToLetterPoints(t, i, currentWordLetters, penX, penY);
			penX += currentWordLength;
			currentWordLength = 0;
			currentWordLetters = 0;
		}
		int letterSize = getLetterWidth(t, letter);
		if (checkShouldWrap(penX, currentWordLength, letterSize, maxWidth)) {
			if (penX > textSize.x) {
				textSize.x = penX;
			}
			penX = 0;
			penY += lineSpace;
		}
		currentWordLength += letterSize;
		++currentWordLetters;
	}
	// If we are done looping, and there is a word left, write the word
	if (currentWordLength) {
		addWordToLetterPoints(t, strlen(t->Text), currentWordLetters, penX, penY);
		penX += currentWordLength;
		// Write letters to points.
	}
	textSize.x = MAX(textSize.x, penX);
	textSize.y = penY - descenderInPixels;
	if (textSize.y > maxHeight) {
		LogWarn("Your text overflowed through Y, please adjust your bounds else it will flow past");
	}
	return textSize;
}

geImage *createEmptyTexture(int width, int height) {
	return geImageNewRenderTarget("thing", width, height);
}

void createTexturesForText(geText *t) {
	FT_Face f = geFontGetFont(t->Font);
	for (size_t i = 0; i < strlen(t->Text); i++) {
		char letter = t->Text[i];
		if (letter == ' ') {
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
		char letterString[2];
		letterString[0] = letter;
		letterString[1] = '\0';
		geImageNewFromSurface(letterString, letterSurface);
	}
}

// geImage *createTextureForText(geText *t) {
// 	int maxWidth = 0;
// 	int maxHeight = 0;
// 	FT_Face f = geFontGetFont(t->Font);
// 	for (size_t i = 0; i < strlen(t->Text); i++) {
// 		char letter = t->Text[i];
// 		int result = FT_Load_Char(f, letter, FT_LOAD_DEFAULT);
// 		if (result) {
// 			LogError("Failed to load character %c with error code %d \n", letter, result);
// 			continue;
// 		}
// 		maxWidth += (f->glyph->advance.x >> 6);
// 		int letterHeight = (f->ascender - f->descender) >> 6;
// 		maxHeight = maxHeight > letterHeight ? maxHeight : letterHeight;
// 	}
// 	geImage *paper = createEmptyTexture(maxWidth, maxHeight);
// 	int x = 0;
// 	int baseline = f->ascender >> 6;
// 	for (size_t i = 0; i < strlen(t->Text); i++) {
// 		char letter = t->Text[i];
// 		int result = FT_Load_Char(f, letter, FT_LOAD_RENDER);
// 		if (result) {
// 			printf("Failed to load character %c with error code %d \n", letter,
// 				   result);
// 			continue;
// 		}
// 		int y = baseline - (f->glyph->metrics.horiBearingY >> 6);
// 		SDL_Surface *letterSurface = geCreateSurfaceForCharacter(f, t->Color.R, t->Color.G, t->Color.B);
// 		// Don't blit if this was a space, but still advance.
// 		if (!letterSurface) {
// 			x += (f->glyph->advance.x >> 6);
// 			SDL_FreeSurface(letterSurface);
// 			continue;
// 		}
// 		geRectangle dst = {x, y, letterSurface->w, letterSurface->h};
// 		char letterString[2];
// 		letterString[0] = letter;
// 		letterString[1] = '\0';
// 		geImage *letterTexture = geImageNewFromSurface(letterString, letterSurface);
// 		// Draw this onto the paper
// 		geImageDrawImageToImage(letterTexture, paper, NULL, &dst);
// 		x += (f->glyph->advance.x >> 6);
// 	}
// 	t->TextSize.x = maxWidth;
// 	t->TextSize.y = maxHeight;
// 	return paper;
// }

static void textFree(geText *t) {
	LogWarn("Freeing text %s", t->Text);
	if (t->Font) geFontFree(t->Font);
	free(t->LetterPoints);
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
	t->LetterPoints = calloc(strlen(text), sizeof(gePoint));
	t->Texture = NULL;
	t->Font = NULL;
	t->FontSize = fontSize;
	t->FontName = fontName;
	t->TextLocation = gePointZero();
	t->TextSize = gePointZero();
	t->TextDrawSize = gePointZero();
	t->TextBounds = gePointZero();
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
	createTexturesForText(t);
	t->TextSize = measureFullText(t);
	char buf[200];
	snprintf(buf, 200, "%s_%s_%d", t->Text, t->FontName, t->FontSize);
	t->Texture = geImageNewRenderTarget(buf, t->TextSize.x, t->TextSize.y);
	for (size_t i = 0; i < strlen(t->Text); i++) {
		char letter = t->Text[i];
		if (letter == ' ') {
			continue;
		}
		char letterString[2];
		letterString[0] = letter;
		letterString[1] = '\0';
		geContent *content = geGetLoadedContent(geContentTypeImage, letterString);
		if (!content) {
			LogError("Content not found");
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

void geTextSetColor(geText *t, geColor *color) {
	t->Color.R = color->R;
	t->Color.G = color->G;
	t->Color.B = color->B;
	t->Color.A = color->A;
}

void geTextSetBounds(geText *t, int x, int y) {
	t->TextBounds.x = x;
	t->TextBounds.y = y;
}

void geTextDraw(geText *t) {
	geRectangle r;
	r.x = t->TextLocation.x;
	r.y = t->TextLocation.y;
	r.w = t->TextDrawSize.x;
	r.h = t->TextDrawSize.y;
	geImageDraw(t->Texture, NULL, &r);
}

void geTextDrawNative(geText *t) {
	geRectangle r;
	r.x = t->TextLocation.x;
	r.y = t->TextLocation.y;
	r.w = t->TextSize.x;
	r.h = t->TextSize.y;
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

gePoint geTextGetTextSize(geText *text) {
	gePoint p;
	p.x = text->TextSize.x;
	p.y = text->TextSize.y;
	return p;
}