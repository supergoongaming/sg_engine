#include <GoonEngine/content/text.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/internal/clock.h>
#include <GoonEngine/prim/color.h>
#include <GoonEngine/prim/point.h>
#include <GoonEngine/utils.h>
#include <GoonEngine/window.h>

const char *_systemDefaultFont = "Roboto-Regular";

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

int GetLoadFilename(char* buffer, size_t bufferSize, const char* filename) {
    const char* base_path = SDL_GetBasePath();
    int result = -1;  // Return -1 on error

    if (base_path == NULL) {
        // Use the current directory if SDL_GetBasePath() fails
        if (snprintf(buffer, bufferSize, "./%s", filename) < bufferSize) {
            result = 0;  // Success
        }
    } else {
        // Construct the path using the base path provided by SDL
        if (snprintf(buffer, bufferSize, "%s%s", base_path, filename) < bufferSize) {
            SDL_free((void*)base_path);  // Clean up the SDL memory
            result = 0;  // Success
        }
    }

    return result;
}