#include <GoonEngine/content/content.h>
#include <GoonEngine/content/image.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/window.h>
#include <SDL2/SDL_render.h>
#include <png.h>

typedef struct geImage {
	SDL_Texture *Texture;
	char *Name;
} geImage;

static void imageFree(geImage *i) {
	if (i->Texture) SDL_DestroyTexture(i->Texture);
	i->Texture = NULL;
	LogWarn("Freeing image %s", i->Name);
	if (i->Name) free(i->Name);
	i->Name = NULL;
	free(i);
	i = NULL;
}
static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * sizeof(Uint32);
	*(Uint32 *)target_pixel = pixel;
}

static SDL_Surface *loadPNG(const char *filename) {
	FILE *file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Error: Unable to open file %s\n", filename);
		return NULL;
	}

	// Check if the file is a PNG file
	unsigned char header[8];
	fread(header, 1, 8, file);
	if (png_sig_cmp(header, 0, 8)) {
		fclose(file);
		fprintf(stderr, "Error: %s is not a PNG file\n", filename);
		return NULL;
	}

	// Initialize libpng structures
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(file);
		fprintf(stderr, "Error: Unable to create png_struct\n");
		return NULL;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(file);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fprintf(stderr, "Error: Unable to create png_info\n");
		return NULL;
	}

	// Set error handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(file);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fprintf(stderr, "Error: Error during png read\n");
		return NULL;
	}

	// Initialize PNG IO
	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);

	// Read PNG info
	png_read_info(png_ptr, info_ptr);
	int width = png_get_image_width(png_ptr, info_ptr);
	int height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	// Read any color_type to 8-bit depth, RGBA format.
	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Allocate memory for image data
	png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}

	// Read the image data
	png_read_image(png_ptr, row_pointers);

	// Create an SDL surface with the correct format
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
	if (!surface) {
		fprintf(stderr, "Error: Unable to create SDL surface\n");
		for (int y = 0; y < height; y++) {
			free(row_pointers[y]);
		}
		free(row_pointers);
		fclose(file);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return NULL;
	}

	// Copy pixel data from PNG to SDL surface
	for (int y = 0; y < height; y++) {
		png_bytep row = row_pointers[y];
		for (int x = 0; x < width; x++) {
			png_bytep px = &(row[x * 4]);
			Uint32 color = SDL_MapRGBA(surface->format, px[0], px[1], px[2], px[3]);
			putpixel(surface, x, y, color);
		}
	}

	// Free libpng allocated memory
	for (int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	// Clean up libpng structures
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(file);

	return surface;
}

static void imageNewContent(geContent *content, void *data) {
	geImage *i = (geImage *)data;
	content->Data.Image = i;
}

static void imageDeleteContent(geContent *content) {
	if (!content) return;
	imageFree(content->Data.Image);
	content->Data.Image = NULL;
}

static void imageLoadContent(geContent *content) {
	geImage *i = content->Data.Image;
	if (i) {
	}
}

static int imageFindContent(const char *path, geContent *content) {
	// TODO why do we need these?
	if (!content || !path) {
		return 0;
	}
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

geImage *geImageNewRenderTarget(const char *contentName, int width, int height, geColor *color) {
	geContent *loadedContent = geGetLoadedContent(geContentTypeImage, contentName);
	if (loadedContent) {
		return loadedContent->Data.Image;
	}
	SDL_Renderer *r = geGlobalRenderer();
	SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(r, t);
	if (SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND) != 0) {
		// Handle error
		SDL_Log("Error setting blend mode: %s", SDL_GetError());
	}
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
	if (color) {
		if (SDL_SetRenderDrawColor(r, color->R, color->G, color->B, color->A) != 0) {
			// Handle error
			SDL_Log("Error setting render draw color: %s", SDL_GetError());
		}
	} else {
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	}
	SDL_RenderClear(r);
	SDL_SetRenderTarget(r, NULL);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	geImage *i = malloc(sizeof(*i));
	i->Texture = t;
	i->Name = strdup(contentName);
	geAddContent(geContentTypeImage, (void *)i);
	return i;
}

void geImageSetAlpha(geImage *i, int a) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderTarget(r, i->Texture);
	if (SDL_SetRenderDrawColor(r, 0, 0, 0, a) != 0) {
		// Handle error
		SDL_Log("Error setting render draw color: %s", SDL_GetError());
	}
	SDL_RenderClear(r);
	SDL_SetRenderTarget(r, NULL);
}
void geImageClear(geImage *i, geColor *c) {
	SDL_Renderer *r = geGlobalRenderer();

	SDL_SetRenderTarget(r, i->Texture);
	if (SDL_SetRenderDrawColor(r, c->R, c->G, c->B, c->A) != 0) {
		// Handle error
		SDL_Log("Error setting render draw color: %s", SDL_GetError());
	}
	SDL_RenderClear(r);
	SDL_SetRenderTarget(r, NULL);
}

void geImageDrawImageToImage(geImage *src, geImage *dst, geRectangle *srcRect, geRectangle *dstRect) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_SetRenderTarget(r, dst->Texture);
	SDL_RenderCopy(r, src->Texture, (SDL_Rect *)srcRect, (SDL_Rect *)dstRect);
	SDL_SetRenderTarget(r, NULL);
}
geImage *geImageNewFromFile(const char *path) {
	geContent *loadedContent = geGetLoadedContent(geContentTypeImage, path);
	if (loadedContent) {
		return loadedContent->Data.Image;
	}
	SDL_Surface *s = loadPNG(path);
	if (!s) {
		LogError("Could not load PNG properly, content not fully loaded");
		return NULL;
	}
	SDL_Renderer *r = geGlobalRenderer();
	SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
	if (t == NULL) {
		LogError("Could not create texture, Error: %s", SDL_GetError());
		return NULL;
	}
	SDL_FreeSurface(s);
	geImage *i = malloc(sizeof(*i));
	i->Texture = t;
	i->Name = strdup(path);
	geAddContent(geContentTypeImage, (void *)i);
	return i;
}

// This does free the surface afterwards, either increases refcount or creates new.
geImage *geImageNewFromSurface(const char *contentName, SDL_Surface *surface) {
	geContent *loadedContent = geGetLoadedContent(geContentTypeImage, contentName);
	if (loadedContent) {
		SDL_FreeSurface(surface);
		return loadedContent->Data.Image;
	}
	SDL_Renderer *r = geGlobalRenderer();
	SDL_Texture *t = SDL_CreateTextureFromSurface(r, surface);
	if (t == NULL) {
		LogError("Could not create texture, Error: %s", SDL_GetError());
		SDL_FreeSurface(surface);
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
void geImageDrawF(geImage *i, geRectangle *srcRect, geRectangleF *dstRect) {
	SDL_Renderer *r = geGlobalRenderer();
	SDL_RenderCopyExF(r, i->Texture, (SDL_Rect *)srcRect, (SDL_FRect *)dstRect, 0, NULL, SDL_FLIP_NONE);
}
void geImageFree(geImage *i) {
	if (i && i->Name && i->Texture) {
		LogWarn("Freeing an image from something");
		geUnloadContent(geContentTypeImage, i->Name, 0);
	}
}
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