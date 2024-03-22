#include <GoonEngine/gnpch.h>
#include <GoonEngine/joystick.h>
#include <GoonEngine/debug.h>

typedef struct GamePad
{
    bool lastFrameButtons[SDL_CONTROLLER_BUTTON_MAX];
    bool thisFrameButtons[SDL_CONTROLLER_BUTTON_MAX];
    int lastFrameAxis[SDL_CONTROLLER_AXIS_MAX];
    int thisFrameAxis[SDL_CONTROLLER_AXIS_MAX];
    SDL_GameController *SdlController;
} GamePad;

#define MAX_GAMEPADS 4
static int _numGamePads;
static void CountPluggedInControllers();
GamePad _connectedGamepads[MAX_GAMEPADS];

static void InitializeEngineGamepad(int padNum)
{
    GamePad *gamepad = &_connectedGamepads[padNum];
    SDL_GameController *pad = SDL_GameControllerOpen(padNum);
    if (!pad)
    {
        LogError("Could not open gamecontroller");
    }
    gamepad->SdlController = pad;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
    {
        gamepad->lastFrameButtons[i] = false;
        gamepad->thisFrameButtons[i] = false;
    }
}

void geInitializeJoysticks()
{
    SDL_GameControllerEventState(SDL_ENABLE);
    CountPluggedInControllers();
}

void HandleJoystickEvent(const SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_CONTROLLERDEVICEADDED:
        LogInfo("Controller added %s", event->cdevice.which);
        break;

    case SDL_CONTROLLERBUTTONDOWN:
        if (event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        {
            SDL_Event quit;
            quit.type = SDL_QUIT;
            SDL_PushEvent(&quit);
        }
        for (int i = 0; i < _numGamePads; i++)
        {
            if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(_connectedGamepads[i].SdlController)))
            {
                _connectedGamepads[i].thisFrameButtons[event->cbutton.button] = true;
            }
        }
        break;
    case SDL_CONTROLLERBUTTONUP:
        for (int i = 0; i < _numGamePads; i++)
        {
            if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(_connectedGamepads[i].SdlController)))
            {
                _connectedGamepads[i].thisFrameButtons[event->cbutton.button] = false;
            }
        }
        break;
    case SDL_CONTROLLERAXISMOTION:
        for (int i = 0; i < _numGamePads; i++)
        {
            if (event->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(_connectedGamepads[i].SdlController)))
            {
                _connectedGamepads[i].thisFrameAxis[event->caxis.axis] = event->caxis.value;
            }
        }
        break;
    default:
        break;
    }
}

static void CountPluggedInControllers()
{
    int nJoysticks = SDL_NumJoysticks();
    LogDebug("There is a total of %d controllers initializing", nJoysticks);
    _numGamePads = 0;
    for (int i = 0; i < nJoysticks; i++)
        if (SDL_IsGameController(i))
        {
            _numGamePads++;
            InitializeEngineGamepad(i);
        }
}

void geUpdateControllerLastFrame()
{
    for (int i = 0; i < _numGamePads; i++)
    {
        memcpy(_connectedGamepads[i].lastFrameAxis, _connectedGamepads[i].thisFrameAxis, sizeof(Uint8) * SDL_CONTROLLER_AXIS_MAX);
        memcpy(_connectedGamepads[i].lastFrameButtons, _connectedGamepads[i].thisFrameButtons, sizeof(Uint8) * SDL_CONTROLLER_BUTTON_MAX);
    }
}

bool geGamepadButtonJustReleased(const int padNum, const int button)
{
    return _numGamePads > padNum && (!_connectedGamepads[padNum].thisFrameButtons[button] && _connectedGamepads[padNum].lastFrameButtons[button]);
}
bool geGamepadButtonJustPressed(const int padNum, const int button)
{
    // if (padNum > _numGamePads)
    //     return false;
    // return _connectedGamepads[padNum].thisFrameButtons[button] && !_connectedGamepads[padNum].lastFrameButtons[button];
    return _numGamePads > padNum && (_connectedGamepads[padNum].thisFrameButtons[button] && !_connectedGamepads[padNum].lastFrameButtons[button]);
}
bool geGamepadButtonHeldDown(const int padNum, const int button)
{
    return _numGamePads > padNum && (_connectedGamepads[padNum].lastFrameButtons[button] && _connectedGamepads[padNum].thisFrameButtons[button]);
}