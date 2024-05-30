#include <GoonEngine/SdlSurface.h>
#include <GoonEngine/color.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/rectangle.h>

SDL_Texture *g_BackgroundAtlas = NULL;
SDL_Rect *g_backgroundDrawRect = NULL;
extern SDL_Renderer *g_pRenderer;

void geSetBackgroundAtlas(SDL_Texture *background) {
	g_BackgroundAtlas = background;
}
void geSetCameraRect(geRectangle *rect) {
	g_backgroundDrawRect = (SDL_Rect *)rect;
}

SDL_Surface *LoadSurfaceFromFile(const char *filePath) {
	SDL_Surface *surf = SDL_LoadBMP(filePath);
	return surf;
}

SDL_Surface *LoadTextureAtlas(int width, int height) {
	SDL_Surface *atlasSurface = SDL_CreateRGBSurfaceWithFormat(
		0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
	// SDL_Surface *atlasSurface = SDL_CreateRGBSurface(0, width, height, 32,
	// 0,0,0,0);
	if (!atlasSurface) {
		fprintf(stderr, "Could not create atlast surface, Error: %s",
				SDL_GetError());
		return NULL;
	}
	return atlasSurface;
}

void BlitSurface(SDL_Surface *srcSurface, geRectangle *srcRect,
				 SDL_Surface *dstSurface, geRectangle *dstRect) {
	int result = SDL_BlitSurface(srcSurface, (SDL_Rect *)srcRect, dstSurface,
								 (SDL_Rect *)dstRect);
	if (result) {
		fprintf(stderr, "Failed to blit surface %s", SDL_GetError());
	}
}
SDL_Texture *geCreateTextureFromFile(const char *filename) {
	SDL_Surface *surface = LoadSurfaceFromFile(filename);
	SDL_Texture *texture = geCreateTextureFromSurface(surface);
	// stbi_image_free(data);
	return texture;
}
void geDestroySurface(SDL_Surface *surface) { SDL_FreeSurface(surface); }

void geDestroyTexture(SDL_Texture *texture) { SDL_DestroyTexture(texture); }

// Used internally to create a sdl texture from a sdl surface, frees the surface
// afterwards.
SDL_Texture *geCreateTextureFromSurface(SDL_Surface *surface) {
	SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pRenderer, surface);
	if (texture == NULL) {
		fprintf(stderr, "Could not create texture, Error: %s", SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(
		surface);  // We no longer need the surface after creating the texture
	return texture;
}
void geDrawTexture(SDL_Texture *texture, geRectangle *srcRect,
				   geRectangle *dstRect, bool shouldFlip) {
	SDL_RenderCopyEx(g_pRenderer, texture, (SDL_Rect *)srcRect,
					 (SDL_Rect *)dstRect, 0, NULL,
					 (shouldFlip) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void geUpdateTextureAlpha(SDL_Texture *texture, int alpha) {
	SDL_SetRenderDrawBlendMode(g_pRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture,
						   alpha);	// alphaValue should be between 0 (fully
									// transparent) and 255 (fully opaque)
}

void geDrawTextureWithCameraOffset(SDL_Texture *texture, geRectangle *srcRect,
								   geRectangle *dstRect, bool shouldFlip) {
	SDL_Rect translatedDstRect;
	translatedDstRect.x = (dstRect->x - g_backgroundDrawRect->x);
	translatedDstRect.y = dstRect->y;
	translatedDstRect.w = dstRect->w;
	translatedDstRect.h = dstRect->h;
	SDL_RenderCopyEx(g_pRenderer, texture, (SDL_Rect *)srcRect,
					 &translatedDstRect, 0, NULL,
					 (shouldFlip) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void geDrawRect(geRectangle *dstRect, geColor *color) {
	SDL_SetRenderDrawColor(g_pRenderer, color->R, color->G, color->B, color->A);
	SDL_RenderDrawRect(g_pRenderer, (SDL_Rect *)dstRect);
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
}

void geDrawFilledRect(geRectangle *dstRect, geColor *color) {
	SDL_SetRenderDrawColor(g_pRenderer, color->R, color->G, color->B, color->A);
	SDL_RenderFillRect(g_pRenderer, (SDL_Rect *)dstRect);
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
}

void geDrawDebugRectCamera(geRectangle *dstRect, geColor *color) {
	SDL_Rect translatedDstRect;
	translatedDstRect.x = (dstRect->x - g_backgroundDrawRect->x);
	translatedDstRect.y = dstRect->y * 1;
	translatedDstRect.w = dstRect->w * 1;
	translatedDstRect.h = dstRect->h * 1;
	SDL_SetRenderDrawColor(g_pRenderer, color->R, color->G, color->B, color->A);
	SDL_RenderDrawRect(g_pRenderer, &translatedDstRect);
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
}