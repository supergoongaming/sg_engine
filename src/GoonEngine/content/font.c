#include <GoonEngine/content/content.h>
#include <GoonEngine/content/font.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/utils.h>
#include <SDL2/SDL_render.h>
#include <ft2build.h>
#include <string.h>
#include FT_FREETYPE_H

#define BUFFER_SIZE 256
// #ifdef GN_PLATFORM_MACOS
// static const char *_fontPrefix = "../Resources/assets/fonts";
// #else
static const char *_fontPrefix = "assets/fonts";
// #endif
static const char *_fontFileType = "ttf";
// Buffer size for fonts, 3 == 999 is max size.
static const int _numBuffer = 3;
static FT_Library _loadedLibrary;

typedef struct geFont {
	FT_Face Face;
	int Size;
	char *Path;
	char *ContentName;
} geFont;

static void fontFree(geFont *f) {
	if (f->Face) {
		FT_Done_Face(f->Face);
	}
LogWarn("Freeing font %s", f->ContentName);
	free(f->Path);
	free(f->ContentName);
	f->Path = NULL;
	f->ContentName = NULL;
	free(f);
	f = NULL;
}

static char *fontGetContentName(geFont *f) {
	if (!f->Path) return "";
	int lenPath = strlen(f->Path);
	char *buffer = malloc(sizeof(char) * (lenPath + _numBuffer + 1));
	sprintf(buffer, "%s_%d", f->Path, f->Size);
	return buffer;
}

static void fontNewContent(geContent *content, void *data) {
	geFont *f = (geFont *)data;
	content->Data.Font = f;
}

static void fontDeleteContent(geContent *content) {
	if (!content) return;
	fontFree(content->Data.Font);
	content->Data.Font = NULL;
	content = NULL;
}

static void fontLoadContent(geContent *content) {
	geFont *f = content->Data.Font;
	if (f) {
		geFontLoad(f);
	}
}

static int fontFindContent(const char *path, geContent *content) {
	geFont *f = (geFont *)content->Data.Font;
	// TODO why do we need to use f->Path not null?  Getting crashes without it.
	if (!f || !f->Path) {
		return 0;
	}
	char *contentPath = fontGetContentName(f);
	int result = strcmp(path, contentPath);
	free(contentPath);
	return result == 0;
}

void geInitializeFontContentType() {
	geAddContentTypeFunctions(geContentTypeFont, fontNewContent,
							  fontDeleteContent, fontLoadContent,
							  fontFindContent);
}

geFont *geFontNew(const char *name, int size) {
	if (!_loadedLibrary) {
		if (FT_Init_FreeType(&_loadedLibrary)) {
			LogCritical("Could not initialize FreeType library\n");
			return 0;
		}
	}

	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%s/%s.%s_%d", _fontPrefix, name, _fontFileType, size);
	char buf[1000];
	GetLoadFilename(buf, sizeof(buf), buffer);

	geContent *loadedContent = geGetLoadedContent(geContentTypeFont, buf);
	if (loadedContent) {
		LogDebug("Using cached font for %s",
				 loadedContent->Data.Font->ContentName);
		return loadedContent->Data.Font;
	}
	if (size > 999 || size < 1) {
		LogWarn("Improper size passed into font, must be between 1 and 1000");
		return NULL;
	}
	sprintf(buffer, "%s/%s.%s", _fontPrefix, name, _fontFileType);

	GetLoadFilename(buf, sizeof(buf), buffer);
	geFont *f = malloc(sizeof(*f));
	f->Face = NULL;
	f->Size = size;
	f->Path = strdup(buf);
	f->ContentName = fontGetContentName(f);
	geAddContent(geContentTypeFont, (void *)f);
	return f;
}

void geFontLoad(geFont *f) {
	if (f->Face) {
		return;
	}
	FT_Face fontFace;
	int result = FT_New_Face(_loadedLibrary, f->Path, 0, &fontFace);
	if (result) {
		LogWarn("Could not open font %s with error %d\n", f->Path, result);
		return;
	}
	FT_Set_Pixel_Sizes(fontFace, 0, f->Size);
	f->Face = fontFace;
}
void geFontFree(geFont *f) {
	if (f->ContentName) {
		geUnloadContent(geContentTypeFont, f->ContentName);
	}
}

FT_Face geFontGetFont(geFont *f) { return f->Face; }
