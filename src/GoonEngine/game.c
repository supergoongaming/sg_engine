#include <GoonEngine/content/content.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/game.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/internal/clock.h>
#include <GoonEngine/input/joystick.h>
#include <GoonEngine/input/keyboard.h>
#include <GoonEngine/window.h>
#include <SupergoonSound/include/sound.h>
#include <ini/ini.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static SDL_Event event;
static bool shouldQuit = false;
static geClock gameClock;
static void (*DrawUpdateFunc)() = NULL;
static void (*GameUpdateFunc)(double deltaTime) = NULL;

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
			case SDL_CONTROLLERAXISMOTION:
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
	geClockUpdate(&gameClock);
	gsUpdateSound();
	geUpdateControllerLastFrame();
	shouldQuit = sdlEventLoop();
	geUpdateKeyboard();
	if (shouldQuit)
		return false;
	if (GameUpdateFunc) {
		GameUpdateFunc(geClockGetUpdateTimeSeconds());
	}
	geStartDrawFrame();
	if (DrawUpdateFunc) {
		DrawUpdateFunc();
	}
	geEndDrawFrame();
	return true;
}

#ifdef __EMSCRIPTEN__
static void LoopWrap() {
	loop_func();
}
#endif

int gePlayLoop() {
	geClockStart(&gameClock);
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

void geGameSetDrawFunc(void (*drawFunc)()) {
	DrawUpdateFunc = drawFunc;
}

void geGameSetUpdateFunc(void (*updateFunc)(double deltaTime)) {
	GameUpdateFunc = updateFunc;
}

int geQuitEngine() {
	return true;
}
