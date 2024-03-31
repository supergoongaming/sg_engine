/**
 * @file SdlSurface.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <SDL2/SDL_types.h>
#include <GoonEngine/rectangle.h>
#include <GoonEngine/color.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct SDL_Texture SDL_Texture;
    typedef struct SDL_Surface SDL_Surface;

    // SDL_Surface *LoadSurfaceFromFile(const char *filePath, void **data);
    SDL_Surface *LoadSurfaceFromFile(const char *filePath);
    SDL_Surface *LoadTextureAtlas(int width, int height);
    void BlitSurface(SDL_Surface *srcSurface, geRectangle *srcRect, SDL_Surface *dstSurface, geRectangle *dstRect);
    SDL_Texture *CreateTextureFromSurface(SDL_Surface *surface);
    void DestroyPixelData(void *data);
    void DestroySurface(SDL_Surface *surface);
    void DestroyTexture(SDL_Texture *texture);
    // void SetBackgroundAtlas(SDL_Texture *background);
    void SetBackgroundAtlas(unsigned int background);
    void SetCameraRect(geRectangle *rect);
    SDL_Texture *CreateTextureFromFile(const char *filename);
    void geUpdateTextureAlpha(SDL_Texture *texture, int alpha);
    void geDrawTextureWithCameraOffset(SDL_Texture *texture, geRectangle *srcRect, geRectangle *dstRect, bool shouldFlip);
    void geDrawTexture(SDL_Texture *texture, geRectangle *srcRect, geRectangle *dstRect, bool shouldFlip);
    void geDrawDebugRect(geRectangle *rect, geColor *color);
#ifdef __cplusplus
}
#endif