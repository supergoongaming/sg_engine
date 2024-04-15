#include <SDL2/SDL.h>
#include <GoonEngine/text.h>
#include <ft2build.h>
FT_Library _loadedLibrary;
FT_Face _loadedFace;

extern SDL_Renderer *g_pRenderer;

/**
 * @brief Create a Empty Surface object used as "paper" for drawing letters on
 *
 * @param width
 * @param height
 * @return SDL_Surface* Surface you can use for blitting
 */
static SDL_Surface *createEmptySurface(int width, int height);

SDL_Surface *geCreateSurfaceForCharacter(FT_Face face, int r, int g, int b)
{
    // Check if this is a space, return null if so
    if (face->glyph->bitmap.width == 0 && face->glyph->bitmap.rows == 0)
        return NULL;
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
        face->glyph->bitmap.buffer,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        8 * face->glyph->bitmap.pitch / face->glyph->bitmap.width,
        face->glyph->bitmap.pitch,
        SDL_PIXELFORMAT_INDEX8);
    SDL_Palette palette;
    palette.colors = (SDL_Color *)alloca(256 * sizeof(SDL_Color));
    int numColors = 256;
    for (int i = 0; i < numColors; ++i)
    {
        palette.colors[i].r = r;
        palette.colors[i].g = g;
        palette.colors[i].b = b;
        palette.colors[i].a = (Uint8)(i);
    }
    palette.ncolors = numColors;
    SDL_SetPaletteColors(surface->format->palette, palette.colors, 0, palette.ncolors);
    SDL_SetColorKey(surface, SDL_TRUE, 0);
    return surface;
}

SDL_Surface *createEmptySurface(int width, int height)
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
    if (surface == NULL)
    {
        printf("Failed to create empty surface: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    return surface;
}

SDL_Texture *geCreateTextureForString(const char *word, Point *textureDimensions, geColor color)
{
    int totalWidth = 0;
    int maxHeight = 0;
    for (size_t i = 0; i < strlen(word); i++)
    {
        char letter = word[i];
        int result = FT_Load_Char(_loadedFace, letter, FT_LOAD_RENDER);
        if (result)
        {
            printf("Failed to load character %c with error code %d \n", letter, result);
            continue;
        }
        totalWidth += (_loadedFace->glyph->advance.x >> 6);
        int letterHeight = (_loadedFace->ascender - _loadedFace->descender) >> 6;
        maxHeight = maxHeight > letterHeight ? maxHeight : letterHeight;
    }
    SDL_Surface *paper = createEmptySurface(totalWidth * 2, maxHeight * 2);
    int x = 0;
    int baseline = 0;
    for (size_t i = 0; i < strlen(word); i++)
    {
        char letter = word[i];
        int result = FT_Load_Char(_loadedFace, letter, FT_LOAD_RENDER);
        if (result)
        {
            printf("Failed to load character %c with error code %d \n", letter, result);
            continue;
        }
        // remember everything is in 64 per one pixel, so you must always use the bitshifter
        if (baseline == 0)
        {
            baseline = (_loadedFace->ascender - _loadedFace->descender) >> 6;
        }
        int y = baseline - (_loadedFace->glyph->metrics.horiBearingY >> 6);
        SDL_Surface *letterSurface = geCreateSurfaceForCharacter(_loadedFace, color.R, color.G, color.B);
        // Don't blit if this was a space, but still advance.
        if (!letterSurface)
        {
            x += (_loadedFace->glyph->advance.x >> 6);
            SDL_FreeSurface(letterSurface);
            continue;
        }
        SDL_Rect dst = {x, y, letterSurface->w, letterSurface->h};
        SDL_BlitSurface(letterSurface, NULL, paper, &dst);
        x += (_loadedFace->glyph->advance.x >> 6);
        SDL_FreeSurface(letterSurface);
    }
    textureDimensions->x = totalWidth;
    textureDimensions->y = maxHeight;
    SDL_Texture *charTexture = SDL_CreateTextureFromSurface(g_pRenderer, paper);
    SDL_FreeSurface(paper);
    return charTexture;
}

int geInitializeTextSubsystem(const char *fontPath, int fontSize)
{
    int result = 0;
    if (FT_Init_FreeType(&_loadedLibrary))
    {
        printf("Could not initialize FreeType library\n");
        return 0;
    }

    result = FT_New_Face(_loadedLibrary, fontPath, 0, &_loadedFace);
    if (result)
    {
        printf("Could not open font %s with error %d\n", fontPath, result);
        return 0;
    }
    FT_Set_Pixel_Sizes(_loadedFace, 0, fontSize);
    return 1;
}

int geShutdownTextSubsystem()
{
    FT_Done_Face(_loadedFace);
    FT_Done_FreeType(_loadedLibrary);
    return 1;
}