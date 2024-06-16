#include <GoonEngine/debug.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/utils.h>
#include <GoonEngine/window.h>
#include <ini/ini.h>

SDL_Window *_window = NULL;
SDL_Renderer *_renderer = NULL;
static int g_refreshRate = 60;
const char *windowPath = "assets/config/window.ini";

static int iniHandler(void *user, const char *section, const char *name, const char *value) {
	geWindowSettings *pWindowSettings = (geWindowSettings *)user;

#define SECTION_MATCH(s) strcmp(section, s) == 0
#define NAME_MATCH(n) strcmp(name, n) == 0
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (SECTION_MATCH("window")) {
		if (NAME_MATCH("x"))
			pWindowSettings->WindowX = atoi(value);
		else if (NAME_MATCH("y"))
			pWindowSettings->WindowY = atoi(value);
		else if (NAME_MATCH("title"))
			pWindowSettings->Title = strdup(value);
	} else if (SECTION_MATCH("world")) {
		if (NAME_MATCH("x"))
			pWindowSettings->WorldX = atoi(value);
		else if (NAME_MATCH("y"))
			pWindowSettings->WorldY = atoi(value);
	} else {
		return false; /* unknown section/name, error */
	}

	return true;
}

int geInitializeRenderingWindow() {
	geWindowSettings settings;
	char buffer[1000];
	geGetLoadFilename(buffer, sizeof(buffer), windowPath);
	// if (ini_parse(windowPath, iniHandler, &settings) < 0) {
	LogWarn("Trying path %s", buffer);
	if (ini_parse(buffer, iniHandler, &settings) < 0) {
		LogCritical("Could not load window.ini to create window.");
		return 1;
	}
	_window = SDL_CreateWindow(settings.Title, SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED, settings.WindowX,
							   settings.WindowY, SDL_WINDOW_SHOWN);
	if (_window == NULL) {
		fprintf(stderr, "Window could not be created, Error: %s",
				SDL_GetError());
		return 0;
	}
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (_renderer == NULL) {
		LogCritical("Renderer could not be created, Error: %s", SDL_GetError());
	}
	LogDebug("Created window\nWidth: %d, Height: %d", settings.WindowX, settings.WindowY);
	SDL_DisplayMode mode;
	SDL_GetWindowDisplayMode(_window, &mode);
	g_refreshRate = mode.refresh_rate ? mode.refresh_rate : 60;
	LogDebug("Refresh rate is set to %d", g_refreshRate);
	SDL_RenderSetIntegerScale(_renderer, SDL_TRUE);
	SDL_RenderSetLogicalSize(_renderer, settings.WorldX, settings.WorldY);
	free((void *)settings.Title);
	return 0;
}

int geWindowGetRefreshRate() {
	return g_refreshRate;
}

SDL_Renderer *geGlobalRenderer() {
	return _renderer;
}

void geStartDrawFrame() {
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
	SDL_RenderClear(_renderer);
}

void geEndDrawFrame() {
	SDL_RenderPresent(_renderer);
}

void geWindowExit() {
}
