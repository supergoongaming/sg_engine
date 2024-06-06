
#include <GoonEngine/gnpch.h>
#include <GoonEngine/prim/rectangle.h>

geRectangle geRectangleZero() {
	geRectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	return rect;
}
int geRectangleIsZero(geRectangle* lhs) {
	if (lhs->h == 0 && lhs->w == 0 && lhs->x == 0 && lhs->y == 0) return 1;
	return 0;
}

int geRectangleIsOverlap(geRectangle* lhs, geRectangle* rhs) {
	return SDL_HasIntersection((SDL_Rect*)lhs, (SDL_Rect*)rhs);
}
geRectangle geRectangleGetOverlapRect(geRectangle* lhs, geRectangle* rhs) {
	if (!geRectangleIsOverlap(lhs, rhs)) {
		return geRectangleZero();
	}
	geRectangle r = geRectangleZero();
	SDL_IntersectRect((SDL_Rect*)lhs, (SDL_Rect*)rhs, (SDL_Rect*)&r);
	return r;
}