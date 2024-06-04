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
extern "C" {
#endif
#include <GoonEngine/input/InputEvent.h>

typedef union SDL_Event SDL_Event;

int geHandleKeyboardEvent(SDL_Event *event);
void geInitializeKeyboard();
void geUpdateKeyboard();
int geKeyJustReleased(const int key);
int geKeyJustPressed(const int key);
int geKeyHeldDown(int key);

typedef enum geKeyboardKeys {
	geKey_UNKNOWN = 0,
	geKey_A = 4,
	geKey_B = 5,
	geKey_C = 6,
	geKey_D = 7,
	geKey_E = 8,
	geKey_F = 9,
	geKey_G = 10,
	geKey_H = 11,
	geKey_I = 12,
	geKey_J = 13,
	geKey_K = 14,
	geKey_L = 15,
	geKey_M = 16,
	geKey_N = 17,
	geKey_O = 18,
	geKey_P = 19,
	geKey_Q = 20,
	geKey_R = 21,
	geKey_S = 22,
	geKey_T = 23,
	geKey_U = 24,
	geKey_V = 25,
	geKey_W = 26,
	geKey_X = 27,
	geKey_Y = 28,
	geKey_Z = 29,
	geKey_1 = 30,
	geKey_2 = 31,
	geKey_3 = 32,
	geKey_4 = 33,
	geKey_5 = 34,
	geKey_6 = 35,
	geKey_7 = 36,
	geKey_8 = 37,
	geKey_9 = 38,
	geKey_0 = 39,
	geKey_RETURN = 40,
	geKey_ESCAPE = 41,
	geKey_BACKSPACE = 42,
	geKey_TAB = 43,
	geKey_SPACE = 44,
} geKeyboardKeys;

#ifdef __cplusplus
}
#endif