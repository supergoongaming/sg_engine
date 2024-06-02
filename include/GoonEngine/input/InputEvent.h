#pragma once
#include <SDL2/SDL_types.h>

typedef struct InputEvent
{
    int Scancode;
    int Button;
    int PressType;

} InputEvent;

enum geButtonPressType
{
    geButtonPressPressed,
    geButtonPressReleased,
    geButtonPressHeld,
    geButtonPressMax,
};

typedef void (*geInputEventFunc)(struct InputEvent *event);