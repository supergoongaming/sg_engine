#include <GoonEngine/gnpch.h>
#include <GoonEngine/game.h>
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
static double secondsBuildup = 0;
static Uint64 freq = 0;
static double DELTA_TIME_SECONDS = 1.0 / 60;

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
            geHandleKeyboardEvent(&event);
            break;
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERDEVICEADDED:
            geHandleJoystickEvent(&event);
            break;
        default:
            break;
        }
    }
    return false;
}
static int loop_func()
{
    uint64_t current = SDL_GetPerformanceCounter();
    double elapsed = (current - previousTime);
    double elapsedSeconds = (((double)elapsed / (double)freq));
    secondsBuildup += elapsedSeconds;
    previousTime = current;
    gsUpdateSound();
    geUpdateControllerLastFrame();
    shouldQuit = sdlEventLoop();
    geUpdateKeyboard();
    if (shouldQuit)
        return false;
    if (g_pScene)
    {
        gpSceneUpdate(g_pScene, elapsedSeconds);
    }
    if (GameUpdateFunc)
    {
        GameUpdateFunc(elapsedSeconds);
    }
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
    freq = SDL_GetPerformanceFrequency();       // Sets the frequency, used to know per seconds
    previousTime = SDL_GetPerformanceCounter(); // Sets the initial time to start the loop.
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(LoopWrap, 0, 1);
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
int geQuitEngine()
{
    return true;
}
