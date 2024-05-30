#pragma once
typedef struct SDL_Surface SDL_Surface;
typedef struct geImage geImage;
#include <GoonEngine/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

// Should be ran during engine initialization so that the font content can work
// properly
void geInitializeImageContentType();
geImage* geImageNewFromSurface(const char* contentName, SDL_Surface* surface);
geImage* geImageNewRenderTarget(const char* contentName, int width, int height);
// Used for drawing a texture onto a render target texture.  Otherwise this will
// fail. (use geImageNewRenderTarget to create render target)
void geImageDrawImageToImage(geImage* src, geImage* dst, geRectangle* srcRect,
							 geRectangle* dstRect);
void geImageLoad(geImage* i);
void geImageFree(geImage* i);
void geImageDraw(geImage* i, geRectangle* srcRect, geRectangle* dstRect);

#ifdef __cplusplus
}
#endif