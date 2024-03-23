#include <GoonEngine/gnpch.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/SdlWindow.h>

SDL_Window *g_pWindow = 0;
SDL_Renderer *g_pRenderer = 0;
int g_refreshRate = 60;
int scaleX = 0;
int scaleY = 0;

/**
 * @brief
 * Lua params: string name, int window width, int window height
 *
 * @param L
 * @return int
 */
// int geInitializeRenderingWindow(uint width, uint height, const char* windowName)
int geInitializeRenderingWindow(unsigned int windowWidth, unsigned int windowHeight, unsigned int gameWidth, unsigned int gameHeight, const char *windowName)
{
    g_pWindow = SDL_CreateWindow(windowName,
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 windowWidth,
                                 windowHeight,
                                 SDL_WINDOW_SHOWN);
    if (g_pWindow == NULL)
    {
        fprintf(stderr, "Window could not be created, Error: %s", SDL_GetError());
        return 0;
    }
#ifdef __EMSCRIPTEN__
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED);
#else
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#endif
    if (g_pRenderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created, Error: %s", SDL_GetError());
    }
    LogDebug("Created window\nWidth: %d, Height: %d", windowWidth, windowHeight);
    SDL_DisplayMode mode;
    SDL_GetWindowDisplayMode(g_pWindow, &mode);
    // g_refreshRate = 30;
    // g_refreshRate = 120;
    // g_refreshRate = 60;
    g_refreshRate = mode.refresh_rate ? mode.refresh_rate : 60;
    LogWarn("Refresh rate is set to %d", g_refreshRate);
    SDL_RenderSetIntegerScale(g_pRenderer, SDL_TRUE);
    SDL_RenderSetLogicalSize(g_pRenderer, gameWidth, gameHeight);
    return 0;
}

SDL_Renderer *GetGlobalRenderer()
{
    return g_pRenderer;
}
SDL_Window *GetGlobalWindow()
{
    return g_pWindow;
}
