#include <GoonEngine/content/text.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/internal/clock.h>
#include <GoonEngine/prim/color.h>
#include <GoonEngine/prim/point.h>
#include <GoonEngine/utils.h>
#include <GoonEngine/window.h>

const char *_systemDefaultFont = "Roboto-Regular";
static char *_systemBasePath = NULL;
static char *_systemFilePath = NULL;

void geUtilsDrawDebugText(const char *text) {
	geText *t = geTextNew(text, _systemDefaultFont, 32);
	int x = 0, y = 0;
	geTextSetLocation(t, x, y);
	geTextLoad(t);
	geTextDraw(t);
}

void geUtilsDrawRect(geRectangle *dstRect, geColor *color) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderDrawColor(r, color->R, color->G, color->B, color->A);
	SDL_RenderDrawRect(r, (SDL_Rect *)dstRect);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
}

void geUtilsDrawRectF(geRectangleF *dstRect, geColor *color) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderDrawColor(r, color->R, color->G, color->B, color->A);
	SDL_RenderDrawRectF(r, (SDL_FRect *)dstRect);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
}

void geUtilsDrawFilledRect(geRectangle *dstRect, geColor *color) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderDrawColor(r, color->R, color->G, color->B, color->A);
	SDL_RenderFillRect(r, (SDL_Rect *)dstRect);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
}

int geUtilsIsPointInRect(geRectangle *rect, gePoint *point) {
	return SDL_PointInRect((SDL_Point *)point, (SDL_Rect *)rect);
}

int geGetLoadFilename(char *buffer, size_t bufferSize, const char *filename) {
	if (_systemBasePath != NULL) {
		snprintf(buffer, bufferSize, "%s%s", _systemBasePath, filename);
		return 0;
	}
	const char *base_path = SDL_GetBasePath();
	int result = -1;

	if (base_path == NULL) {
		if (snprintf(buffer, bufferSize, "./%s", filename) < bufferSize) {
			result = 0;
		}
	} else {
		if (snprintf(buffer, bufferSize, "%s%s", base_path, filename) < bufferSize) {
			_systemBasePath = strdup(base_path);
			SDL_free((void *)base_path);

			result = 0;
		}
	}
	return result;
}

int geGetFileFilepath(char *buffer, size_t bufferSize, const char *filename) {
	if (_systemFilePath != NULL) {
		snprintf(buffer, bufferSize, "%s%s", _systemFilePath, filename);
		return 0;
	}
	const char *base_path = SDL_GetPrefPath("Supergoon Games", "BbAdventures");
	int result = -1;  // Return -1 on error

	if (base_path == NULL) {
		// Use the current directory if SDL_GetBasePath() fails
		if (snprintf(buffer, bufferSize, "./%s", filename) < bufferSize) {
			result = 0;	 // Success
		}
	} else {
		// Construct the path using the base path provided by SDL
		if (snprintf(buffer, bufferSize, "%s%s", base_path, filename) < bufferSize) {
			_systemFilePath = strdup(base_path);
			SDL_free((void *)base_path);  // Clean up the SDL memory
			result = 0;					  // Success
		}
	}
	LogWarn("Path is %s", buffer);
	return result;
}