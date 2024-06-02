#include <GoonEngine/content/text.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/internal/clock.h>
#include <GoonEngine/primitives/color.h>
#include <GoonEngine/primitives/point.h>
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

void geUtilsDrawFilledRect(geRectangle *dstRect, geColor *color) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderDrawColor(r, color->R, color->G, color->B, color->A);
	SDL_RenderFillRect(r, (SDL_Rect *)dstRect);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
}
