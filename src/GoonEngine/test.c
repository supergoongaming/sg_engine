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

static gpScene *g_pScene;
extern SDL_Texture *g_BackgroundAtlas;
extern SDL_Rect *g_backgroundDrawRect;
static SDL_Texture *nerdText;
static SDL_Texture *cuteText;

#define MAX_STARTUP_FRAMES 1000

static SDL_Event event;
static bool shouldQuit = false;

static uint64_t lastFrameMilliseconds;
static double msBuildup;

// TODO this should be different, it is inside of SDLwindow.c
extern SDL_Renderer *g_pRenderer;
extern int g_refreshRate;

void (*DrawUpdateFunc)() = NULL; void (*GameUpdateFunc)(double deltaTime) = NULL;

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
    // static float deltaBuffer = 0;
    Uint64 beginFrame = SDL_GetTicks64();
    Uint64 delta = beginFrame - lastFrameMilliseconds;
    // delta += deltaBuffer;
    // const float oldDelta = delta;
    msBuildup += delta;
    lastFrameMilliseconds = beginFrame;
    shouldQuit = sdlEventLoop();
    if (shouldQuit)
        return false;
    // TODO make these static and pass into as ref to stop allocations
    // Initialize time this frame
    double deltaTimeSeconds = 1 / (double)g_refreshRate;
    double deltaTimeMs = 1000 / (double)g_refreshRate;
    if (msBuildup < deltaTimeMs)
    {
        SDL_Delay(1);
        return true;
    }

#ifndef __EMSCRIPTEN__
    while (msBuildup >= deltaTimeMs)
    {
#endif
        geUpdateKeyboard();
        geUpdateControllers();
        gsUpdateSound();
        // LogWarn("Update time: %f", deltaTimeMs);
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

    // LogInfo("Num iters this frame %d", iters);
    SDL_RenderPresent(g_pRenderer);
    // deltaBuffer = oldDelta - delta;
    // Handle waiting if Vsync is off
    // Uint64 endTime = beginFrame + deltaTimeMs;
    Uint64 currentTime = SDL_GetTicks64();
    Uint64 endTime = currentTime + deltaTimeMs;

    // // If there's time remaining until the next frame, delay the execution
#ifndef __EMSCRIPTEN__
    // if (endTime > currentTime)
    // {
    //     Uint32 delayTime = (Uint32)(endTime - currentTime);
    //     SDL_Delay(delayTime);
    // }
#endif
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
    // geColor nerdColor = {0, 255, 255, 255};
    // geColor cuteColor = {255, 100, 0, 255};

    // nerdText = geCreateTextureForString("Kevin is a nerd", nerdColor);
    // cuteText = geCreateTextureForString("Misha is a cute", cuteColor);

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
    // geInitializeTextSubsystem("assets/fonts/himalaya.ttf", 72);
    // Pump out initial events, to prevent music problems.
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
