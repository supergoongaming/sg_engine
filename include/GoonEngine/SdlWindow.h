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

    int geInitializeRenderingWindow(unsigned int windowWidth, unsigned int windowHeight, unsigned int gameWidth, unsigned int gameHeight, const char *windowName);
#ifdef __cplusplus
}
#endif
