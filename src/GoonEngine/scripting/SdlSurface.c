#include <png.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/SdlSurface.h>
#include <GoonEngine/color.h>
#include <GoonEngine/rectangle.h>

SDL_Texture *g_BackgroundAtlas = NULL;
SDL_Rect *g_backgroundDrawRect = NULL;
extern SDL_Renderer *g_pRenderer;

void geSetBackgroundAtlas(SDL_Texture *background)
{
    g_BackgroundAtlas = background;
}
void geSetCameraRect(geRectangle *rect)
{
    g_backgroundDrawRect = (SDL_Rect *)rect;
}

static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * sizeof(Uint32);
    *(Uint32 *)target_pixel = pixel;
}

static SDL_Surface *loadPNG(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }

    // Check if the file is a PNG file
    unsigned char header[8];
    fread(header, 1, 8, file);
    if (png_sig_cmp(header, 0, 8))
    {
        fclose(file);
        fprintf(stderr, "Error: %s is not a PNG file\n", filename);
        return NULL;
    }

    // Initialize libpng structures
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(file);
        fprintf(stderr, "Error: Unable to create png_struct\n");
        return NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(file);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fprintf(stderr, "Error: Unable to create png_info\n");
        return NULL;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
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
    // See http://www.libpng.org/pub/png/libpng-manual.txt

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
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    // Read the image data
    png_read_image(png_ptr, row_pointers);

    // Determine Pixel format
    Uint32 sdl_pixel_format;
    switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
        sdl_pixel_format = (bit_depth == 16) ? SDL_PIXELFORMAT_RGB565 : SDL_PIXELFORMAT_RGB888;
        break;
    case PNG_COLOR_TYPE_PALETTE:
        sdl_pixel_format = SDL_PIXELFORMAT_RGBA8888; // You may need to handle palette conversion
        break;
    case PNG_COLOR_TYPE_RGB:
        sdl_pixel_format = SDL_PIXELFORMAT_RGB888;
        break;
    case PNG_COLOR_TYPE_RGBA:
        sdl_pixel_format = SDL_PIXELFORMAT_RGBA8888;
        break;
    default:
        fclose(file);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fprintf(stderr, "Error: Unsupported PNG color type\n");
        return NULL;
    }

    // Create an SDL surface
    // SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, sdl_pixel_format);

    // Copy pixel data from PNG to SDL surface
    for (int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++)
        {
            png_bytep px = &(row[x * 4]);
            Uint32 color = ((px[0] << 24) | (px[1] << 16) | (px[2] << 8) | px[3]);
            putpixel(surface, x, y, color);
        }
    }

    // Free libpng allocated memory
    for (int y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);

    // Clean up libpng structures
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(file);

    return surface;
}

// SDL_Surface *LoadSurfaceFromFile(const char *filePath, void **data)
SDL_Surface *LoadSurfaceFromFile(const char *filePath)
{
    SDL_Surface *surf = loadPNG(filePath);
    return surf;
}

SDL_Surface *LoadTextureAtlas(int width, int height)
{
    SDL_Surface *atlasSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
    // SDL_Surface *atlasSurface = SDL_CreateRGBSurface(0, width, height, 32, 0,0,0,0);
    if (!atlasSurface)
    {
        fprintf(stderr, "Could not create atlast surface, Error: %s", SDL_GetError());
        return NULL;
    }
    return atlasSurface;
}

void BlitSurface(
    SDL_Surface *srcSurface,
    geRectangle *srcRect,
    SDL_Surface *dstSurface,
    geRectangle *dstRect)
{
    // int result = SDL_BlitSurface(srcSurface, srcRect, dstSurface, dstRect);
    int result = SDL_BlitSurface(srcSurface, (SDL_Rect *)srcRect, dstSurface, (SDL_Rect *)dstRect);
    if (result)
    {
        fprintf(stderr, "Failed to blit surface %s", SDL_GetError());
    }
}
SDL_Texture *geCreateTextureFromFile(const char *filename)
{
    SDL_Surface *surface = LoadSurfaceFromFile(filename);
    SDL_Texture *texture = CreateTextureFromSurface(surface);
    // stbi_image_free(data);
    return texture;
}
void geDestroySurface(SDL_Surface *surface)
{
    SDL_FreeSurface(surface);
}

void geDestroyTexture(SDL_Texture *texture)
{
    SDL_DestroyTexture(texture);
}

SDL_Texture *CreateTextureFromSurface(SDL_Surface *surface)
{
    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_pRenderer, surface);
    if (texture == NULL)
    {
        fprintf(stderr, "Could not create texture, Error: %s", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(surface); // We no longer need the surface after creating the texture
    return texture;
}
void geDrawTexture(SDL_Texture *texture, geRectangle *srcRect, geRectangle *dstRect, bool shouldFlip)
{
    SDL_RenderCopyEx(g_pRenderer,
                     texture,
                     (SDL_Rect *)srcRect,
                     (SDL_Rect *)dstRect,
                     0,
                     NULL,
                     (shouldFlip) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void geUpdateTextureAlpha(SDL_Texture *texture, int alpha)
{
    SDL_SetRenderDrawBlendMode(g_pRenderer, SDL_BLENDMODE_BLEND);
    // Set the alpha modulation (transparency) for the texture
    SDL_SetTextureAlphaMod(texture, alpha); // alphaValue should be between 0 (fully transparent) and 255 (fully opaque)
}

void geDrawTextureWithCameraOffset(SDL_Texture *texture, geRectangle *srcRect, geRectangle *dstRect, bool shouldFlip)
{
    SDL_Rect translatedDstRect;
    translatedDstRect.x = (dstRect->x - g_backgroundDrawRect->x);
    translatedDstRect.y = dstRect->y;
    translatedDstRect.w = dstRect->w;
    translatedDstRect.h = dstRect->h;
    SDL_RenderCopyEx(g_pRenderer,
                     texture,
                     (SDL_Rect *)srcRect,
                     &translatedDstRect,
                     0,
                     NULL,
                     (shouldFlip) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void geDrawDebugRect(geRectangle *dstRect, geColor *color)
{
    SDL_Rect translatedDstRect;
    translatedDstRect.x = (dstRect->x - g_backgroundDrawRect->x);
    translatedDstRect.y = dstRect->y * 1;
    translatedDstRect.w = dstRect->w * 1;
    translatedDstRect.h = dstRect->h * 1;
    SDL_SetRenderDrawColor(g_pRenderer, color->R, color->G, color->B, color->A);
    SDL_RenderDrawRect(g_pRenderer, &translatedDstRect);
    SDL_SetRenderDrawColor(g_pRenderer, 100, 100, 100, 255);
}