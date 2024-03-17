#include <GoonEngine/gnpch.h>
#include <GoonEngine/keyboard.h>

Uint8 _currentKeyboardState[SDL_NUM_SCANCODES];
Uint8 _previousKeyboardState[SDL_NUM_SCANCODES];

static void InitializeKeyboardStateArrays()
{
    memset(_previousKeyboardState, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
    memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);
}

void geInitializeKeyboard()
{
    InitializeKeyboardStateArrays();
}

int HandleKeyboardEvent(SDL_Event *event)
{
    // Handle quit events currently
    if (event->type == SDL_KEYDOWN && (event->key.keysym.sym == SDLK_q || event->key.keysym.sym == SDLK_ESCAPE))
    {
        SDL_Event quit;
        quit.type = SDL_QUIT;
        SDL_PushEvent(&quit);
        return true;
    }

    // Don't pass repeat keys to lua, as he handles the processing
    if (event->key.repeat)
    {
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

// bool geKeyHeldDown(SDL_Scancode key)
// bool geKeyJustPressed(const SDL_Scancode key)
bool geKeyJustReleased(const int key)
{
    return !_currentKeyboardState[key] && _previousKeyboardState[key];
}

void geUpdateKeyboard()
{
    memcpy(_previousKeyboardState, _currentKeyboardState, sizeof(Uint8) * SDL_NUM_SCANCODES);
    memcpy(_currentKeyboardState, SDL_GetKeyboardState(NULL), sizeof(Uint8) * SDL_NUM_SCANCODES);
}
