/**
 * @file SdlWindow.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
typedef struct SDL_Rect SDL_Rect;

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct geWindowSettings {
    int WindowX, WindowY, WorldX, WorldY;
    const char* Title
} geWindowSettings;

    // int geInitializeRenderingWindow(unsigned int windowWidth, unsigned int windowHeight, unsigned int gameWidth, unsigned int gameHeight, const char *windowName);
    int geInitializeRenderingWindow();
#ifdef __cplusplus
}
#endif
