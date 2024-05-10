#include <GoonEngine/gnpch.h>
#include <ini/ini.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/SdlWindow.h>

SDL_Window *g_pWindow = NULL;
SDL_Renderer *g_pRenderer = NULL;
int g_refreshRate = 60;

static int iniHandler(void *user, const char *section, const char *name,
                      const char *value)
{
    geWindowSettings *pWindowSettings = (geWindowSettings *)user;

#define SECTION_MATCH(s) strcmp(section, s) == 0
#define NAME_MATCH(n) strcmp(name, n) == 0
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (SECTION_MATCH("window"))
    {
        if (NAME_MATCH("x"))
            pWindowSettings->WindowX = atoi(value);
        else if (NAME_MATCH("y"))
            pWindowSettings->WindowY = atoi(value);
        else if (NAME_MATCH("title"))
            pWindowSettings->Title = strdup(value);
    }
    else if (SECTION_MATCH("world"))
    {
        if (NAME_MATCH("x"))
            pWindowSettings->WorldX = atoi(value);
        else if (NAME_MATCH("y"))
            pWindowSettings->WorldY = atoi(value);
    }
    else
    {
        return false; /* unknown section/name, error */
    }

    return true;
}

// int geInitializeRenderingWindow(unsigned int windowWidth, unsigned int windowHeight, unsigned int gameWidth, unsigned int gameHeight, const char *windowName)
int geInitializeRenderingWindow()
{
    geWindowSettings settings;

    if (ini_parse("assets/config/window.ini", iniHandler, &settings) < 0)
    {
        LogCritical("Could not load window.ini to create window.");
        return 1;
    }
    g_pWindow = SDL_CreateWindow(settings.Title,
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 settings.WindowX,
                                 settings.WindowY,
                                 SDL_WINDOW_SHOWN);
    if (g_pWindow == NULL)
    {
        fprintf(stderr, "Window could not be created, Error: %s", SDL_GetError());
        return 0;
    }
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (g_pRenderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created, Error: %s", SDL_GetError());
    }
    LogDebug("Created window\nWidth: %d, Height: %d", settings.WindowX, settings.WindowY);
    SDL_DisplayMode mode;
    SDL_GetWindowDisplayMode(g_pWindow, &mode);
    // g_refreshRate = 30;
    // g_refreshRate = 120;
    // g_refreshRate = 60;
    g_refreshRate = mode.refresh_rate ? mode.refresh_rate : 60;
    LogDebug("Refresh rate is set to %d", g_refreshRate);
    SDL_RenderSetIntegerScale(g_pRenderer, SDL_TRUE);
    SDL_RenderSetLogicalSize(g_pRenderer, settings.WorldX, settings.WorldY);
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
