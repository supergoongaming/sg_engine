#pragma once
typedef struct SDL_Rect SDL_Rect;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct geWindowSettings {
	int WindowX, WindowY, WorldX, WorldY;
	const char* Title;
} geWindowSettings;

int geWindowGetRefreshRate();
int geInitializeRenderingWindow();
#ifdef __cplusplus
}
#endif
