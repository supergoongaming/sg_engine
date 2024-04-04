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

#include <glad/glad.h>
#include <GoonEngine/TileSheet.h>

#define DELTA_TIME_SECONDS (1.0 / 144.0)
#define DELTA_TIME_MS (1000.0 / 144.0)

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
unsigned int USE_GL_ES = 1;

#else
unsigned int USE_GL_ES = 0;
#endif

// TODO this should be different, it is inside of SDLwindow.c
// extern SDL_Renderer *g_pRenderer;
// extern SDL_GLContext *g_pContext;
extern SDL_Window *g_pWindow;
extern SDL_GLContext *g_pContext;
extern int g_refreshRate;
extern geTileSheet *g_BackgroundAtlas;
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
    gsUpdateSound();
    if (msBuildup < DELTA_TIME_MS)
    {
        return 1;
    }
    while (msBuildup >= DELTA_TIME_MS)
    {
        geUpdateControllerLastFrame();
        shouldQuit = sdlEventLoop();
        geUpdateKeyboard();
        if (shouldQuit)
            return false;
        if (g_pScene)
        {
            gpSceneUpdate(g_pScene, DELTA_TIME_SECONDS);
        }
        if (GameUpdateFunc)
        {
            GameUpdateFunc(DELTA_TIME_MS);
        }
        msBuildup -= DELTA_TIME_MS;
    }
    // SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    // SDL_RenderClear(g_pRenderer);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (g_BackgroundAtlas)
    {
        // We always want to draw at destination 0, however source should be used from camera.
        // SDL_Rect dst;
        // dst.x = 0;
        // dst.y = 0;
        // dst.w = g_backgroundDrawRect->w;
        // dst.h = g_backgroundDrawRect->h;
        // int drawResult = SDL_RenderCopy(g_pRenderer, g_BackgroundAtlas, g_backgroundDrawRect, &dst);
        // if (drawResult != 0)
        // {
        //     LogError("Did not draw properly, Error %s\n", SDL_GetError());
        // }
        geTileSheetDraw(g_BackgroundAtlas);
    }
    if (DrawUpdateFunc)
    {
        DrawUpdateFunc();
    }
    SDL_GL_SwapWindow(g_pWindow);
    // SDL_RenderPresent(g_pRenderer);
    return true;
}

static void LoopWrap()
{
    loop_func();
}

int gePlayLoop()
{
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
int ExitEngine()
{
    return true;
}
