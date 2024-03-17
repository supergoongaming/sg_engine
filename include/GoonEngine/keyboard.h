/**
 * @file keyboard.h
 * @author Kevin Blanchard (kevin@supergoon.com)
 * @brief Handles keyboard events
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
#include <GoonEngine/InputEvent.h>

typedef union SDL_Event SDL_Event;

int HandleKeyboardEvent(SDL_Event *event);
void geInitializeKeyboard();
void geUpdateKeyboard();
bool geKeyJustReleased(const int key);
bool geKeyJustPressed(const int key);
bool geKeyHeldDown(int key);
#ifdef __cplusplus
}
#endif