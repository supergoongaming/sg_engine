#include <GoonEngine/clock.h>
#include <GoonEngine/content/content.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/game.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/joystick.h>
#include <GoonEngine/keyboard.h>
#include <GoonEngine/window.h>
#include <GoonPhysics/scene.h>
#include <SupergoonSound/include/sound.h>
#include <ini/ini.h>

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
static double secondsBuildup = 0;
static Uint64 freq = 0;
static geClock *frameClock;

void (*DrawUpdateFunc)() = NULL;
void (*GameUpdateFunc)(double deltaTime) = NULL;

/**
 * @brief Handles all SDL events every frame.
 *
 * @return true If we should quit or not
 * @return false If we should quit or not
 */
static bool sdlEventLoop() {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
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

static int loop_func() {
	geClockUpdate(frameClock);
	while (geClockTick(frameClock)) {
		gsUpdateSound();
		geUpdateControllerLastFrame();
		shouldQuit = sdlEventLoop();
		geUpdateKeyboard();
		if (shouldQuit)
			return false;
		if (g_pScene) {
			gpSceneUpdate(g_pScene, geClockGetUpdateTimeSeconds());
		}
		if (GameUpdateFunc) {
			GameUpdateFunc(geClockGetUpdateTimeSeconds());
		}
	}

	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(g_pRenderer);
	if (DrawUpdateFunc) {
		DrawUpdateFunc();
	}
	SDL_RenderPresent(g_pRenderer);
	return true;
}

static void LoopWrap() {
	loop_func();
}

int gePlayLoop() {
	frameClock = geClockNew();
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(LoopWrap, 0, 1);
#else
	while (!shouldQuit) {
		loop_func();
	}
	return true;
#endif
}
int geInitializeEngine() {
	InitializeDebugLogFile();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
		fprintf(stderr, "Could not Initialize SDL!\nError: %s", SDL_GetError());
		return false;
	}
	geInitializeKeyboard();
	geInitializeJoysticks();
	gsInitializeSound();
	geInitializeRenderingWindow();
	geContentInitializeAllContentTypes();
	shouldQuit = sdlEventLoop();
	return true;
}

void geSetCurrentScene(void *scene) {
	g_pScene = scene;
}

void geGameSetDrawFunc(void (*drawFunc)()) {
	DrawUpdateFunc = drawFunc;
}
void geGameSetUpdateFunc(void (*updateFunc)(double deltaTime)) {
	GameUpdateFunc = updateFunc;
}
int geQuitEngine() {
	return true;
}
