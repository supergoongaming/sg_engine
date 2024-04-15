#include <GoonEngine/gnpch.h>
#include <GoonEngine/keyboard.h>

static Uint8 _currentKeyboardState[SDL_NUM_SCANCODES];
static Uint8 _previousKeyboardState[SDL_NUM_SCANCODES];

static void InitializeKeyboardStateArrays()
{
    memset(_previousKeyboardState, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
    memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);
}

void geInitializeKeyboard()
{
    InitializeKeyboardStateArrays();
}

int geHandleKeyboardEvent(SDL_Event *event)
{
    if (event->key.repeat)
    {
        return true;
    }

    if (event->type == SDL_KEYDOWN && (event->key.keysym.sym == SDLK_q || event->key.keysym.sym == SDLK_ESCAPE))
    {
        SDL_Event quit;
        quit.type = SDL_QUIT;
        SDL_PushEvent(&quit);
        return true;
    }

    return true;
}

bool geKeyHeldDown(int key)
{
    return _previousKeyboardState[key] && _currentKeyboardState[key];
}

bool geKeyJustPressed(const int key)
{
    return _currentKeyboardState[key] && !_previousKeyboardState[key];
}

bool geKeyJustReleased(const int key)
{
    return !_currentKeyboardState[key] && _previousKeyboardState[key];
}

void geUpdateKeyboard()
{
    memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(Uint8) * SDL_NUM_SCANCODES);
    memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);
}
