#include <stdio.h>
#include <GoonEngine/test.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/SdlWindow.h>
#include <GoonEngine/keyboard.h>
#include <GoonEngine/joystick.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/text.h>
#include <SupergoonSound/include/sound.h>
#include <GoonPhysics/scene.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// TODO this should be different, it is inside of SDLwindow.c
extern SDL_Renderer *g_pRenderer;
extern int g_refreshRate;
extern SDL_Texture *g_BackgroundAtlas;
extern SDL_Rect *g_backgroundDrawRect;

static gpScene *g_pScene = NULL;
static SDL_Event event;
static bool shouldQuit = false;
static uint64_t previousTime = 0;
static double msBuildup = 0;

void (*DrawUpdateFunc)() = NULL;
void (*GameUpdateFunc)(double deltaTime) = NULL;

void *MusicUpdateWrapper(void *arg)
{
    (void)arg;
    return NULL;
}

/**
 * @brief Handles all SDL events every frame.
 *
 * @return true If we should quit or not
 * @return false If we should quit or not
 */
static bool sdlEventLoop()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return true;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            HandleKeyboardEvent(&event);
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERDEVICEADDED:
            HandleJoystickEvent(&event);
            break;
        default:
            break;
        }
    }
    return false;
}

static int loop_func()
{
    uint64_t current = SDL_GetTicks64();
    uint64_t elapsed = current - previousTime;
    previousTime = current;
    msBuildup += elapsed;
    double deltaTimeSeconds = 1 / (double)144;
    double deltaTimeMs = 1000 / (double)144;
    gsUpdateSound();
    if (msBuildup < deltaTimeMs)
    {
        return 1;
    }
#ifndef __EMSCRIPTEN__
    while (msBuildup >= deltaTimeMs)
    {
#endif
        geUpdateControllerLastFrame();
        shouldQuit = sdlEventLoop();
        geUpdateKeyboard();
        if (shouldQuit)
            return false;
        if (g_pScene)
        {
            gpSceneUpdate(g_pScene, deltaTimeSeconds);
        }
        if (GameUpdateFunc)
        {
            GameUpdateFunc(deltaTimeMs);
        }
        msBuildup -= deltaTimeMs;
#ifndef __EMSCRIPTEN__
    }
#endif
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(g_pRenderer);
    if (g_BackgroundAtlas)
    {
        // We always want to draw at destination 0, however source should be used from camera.
        SDL_Rect dst;
        dst.x = 0;
        dst.y = 0;
        dst.w = g_backgroundDrawRect->w;
        dst.h = g_backgroundDrawRect->h;
        int drawResult = SDL_RenderCopy(g_pRenderer, g_BackgroundAtlas, g_backgroundDrawRect, &dst);
        if (drawResult != 0)
        {
            LogError("Did not draw properly, Error %s\n", SDL_GetError());
        }
    }
    if (DrawUpdateFunc)
    {
        DrawUpdateFunc();
    }
    SDL_RenderPresent(g_pRenderer);
    return true;
}

static void LoopWrap()
{
    loop_func();
}

int gePlayLoop()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(LoopWrap, g_refreshRate, 1);
#else
    while (!shouldQuit)
    {
        loop_func();
    }
    return true;
#endif
}
int geInitializeEngine()
{
    InitializeDebugLogFile();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        fprintf(stderr, "Could not Initialize SDL!\nError: %s", SDL_GetError());
        return false;
    }

    geInitializeKeyboard();
    geInitializeJoysticks();
    geInitializeTextSubsystem("assets/fonts/main.ttf", 36);
    shouldQuit = sdlEventLoop();
    return true;
}

void geSetCurrentScene(void *scene)
{
    g_pScene = scene;
}

void geGameSetDrawFunc(void (*drawFunc)())
{
    DrawUpdateFunc = drawFunc;
}
void geGameSetUpdateFunc(void (*updateFunc)(double deltaTime))
{
    GameUpdateFunc = updateFunc;
}
int ExitEngine()
{
    return true;
}
