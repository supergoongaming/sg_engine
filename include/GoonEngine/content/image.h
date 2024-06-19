#pragma once
typedef struct SDL_Surface SDL_Surface;
typedef struct geImage geImage;
#include <GoonEngine/prim/color.h>
#include <GoonEngine/prim/rectangle.h>
#ifdef __cplusplus
extern "C" {
#endif

// Should be ran during engine initialization so that the font content can work
// properly
void geInitializeImageContentType();
geImage* geImageNewFromFile(const char* path);
geImage* geImageNewFromSurface(const char* contentName, SDL_Surface* surface);
geImage* geImageNewRenderTarget(const char* contentName, int width, int height, geColor* color);
// Used for drawing a texture onto a render target texture.  Otherwise this will
// fail. (use geImageNewRenderTarget to create render target)
void geImageDrawImageToImage(geImage* src, geImage* dst, geRectangle* srcRect, geRectangle* dstRect);
// this is used for render target images.
void geImageSetAlpha(geImage* i, int a);
// This is used for images that are static, from a png
void geImageStaticSetAlpha(geImage* i, int a);
void geImageClear(geImage* i, geColor* c);
void geImageFree(geImage* i);
void geImageDraw(geImage* i, geRectangle* srcRect, geRectangle* dstRect);
void geImageDrawF(geImage* i, geRectangle* srcRect, geRectangleF* dstRect);
int geImageWidth(geImage* i);
int geImageHeight(geImage* i);
const char* geImageFilename(geImage* i);

#ifdef __cplusplus
}
#endif