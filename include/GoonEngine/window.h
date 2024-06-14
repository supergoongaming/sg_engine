#pragma once
typedef struct SDL_Rect SDL_Rect;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct geWindowSettings {
	int WindowX, WindowY, WorldX, WorldY;
	const char* Title;
} geWindowSettings;

SDL_Renderer* geGlobalRenderer();
int geWindowGetRefreshRate();
int geInitializeRenderingWindow();
void geStartDrawFrame();
void geEndDrawFrame();
void geWindowExit();
#ifdef __cplusplus
}
#endif
