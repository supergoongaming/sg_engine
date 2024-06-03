#include <GoonEngine/content/content.h>
#include <GoonEngine/content/image.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/window.h>
#include <SDL2/SDL_render.h>

typedef struct geImage {
	SDL_Texture *Texture;
	char *Name;
} geImage;

static void imageFree(geImage *i) {
	LogWarn("Freeing image %s", i->Name);
	if (i->Texture) SDL_DestroyTexture(i->Texture);
	i->Texture = NULL;
	if (i->Name) free(i->Name);
	free(i);
}

static void imageNewContent(geContent *content, void *data) {
	geImage *i = (geImage *)data;
	content->Data.Image = i;
}

static void imageDeleteContent(geContent *content) {
	if (!content) return;
	imageFree(content->Data.Image);
}

static void imageLoadContent(geContent *content) {
	geImage *i = content->Data.Image;
	if (i) {
		geImageLoad(i);
	}
}

static int imageFindContent(const char *path, geContent *content) {
	geImage *i = (geImage *)content->Data.Image;
	if (!i) {
		return 0;
	}
	return strcmp(path, i->Name) == 0;
}

void geInitializeImageContentType() {
	geAddContentTypeFunctions(geContentTypeImage, imageNewContent,
							  imageDeleteContent, imageLoadContent,
							  imageFindContent);
}

geImage *geImageNewRenderTarget(const char *contentName, int width, int height) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	geImage *i = malloc(sizeof(*i));
	i->Texture = t;
	i->Name = strdup(contentName);
	geAddContent(geContentTypeImage, (void *)i);
	return i;
}
void geImageDrawImageToImage(geImage *src, geImage *dst, geRectangle *srcRect,
							 geRectangle *dstRect) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderTarget(r, dst->Texture);
	SDL_RenderCopy(r, src->Texture, (SDL_Rect *)srcRect,
				   (SDL_Rect *)dstRect);
	SDL_SetRenderTarget(r, NULL);
}

// This does free the surface afterwards, either increases refcount or creates new.
geImage *geImageNewFromSurface(const char *contentName, SDL_Surface *surface) {
	geContent *loadedContent = geGetLoadedContent(geContentTypeImage, contentName);
	if (loadedContent) {
		return loadedContent->Data.Image;
	}
	SDL_Renderer *r = geGlobalRenderer();
	SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);
	if (t == NULL) {
		LogError("Could not create texture, Error: %s", SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(surface);
	geImage *i = malloc(sizeof(*i));
	i->Texture = t;
	i->Name = strdup(contentName);
	geAddContent(geContentTypeImage, (void *)i);
	return i;
}

void geImageDraw(geImage *i, geRectangle *srcRect, geRectangle *dstRect) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_RenderCopyEx(r, i->Texture, (SDL_Rect *)srcRect,
					 (SDL_Rect *)dstRect, 0, NULL, SDL_FLIP_NONE);
}
void geImageLoad(geImage *i) {}
void geImageFree(geImage *i) { geUnloadContent(geContentTypeImage, i->Name); }
int geImageWidth(geImage *i) {
	int w, h;
	SDL_QueryTexture(i->Texture, NULL, NULL, &w, &h);
	return w;
}

int geImageHeight(geImage *i) {
	int w, h;
	SDL_QueryTexture(i->Texture, NULL, NULL, &w, &h);
	return h;
}