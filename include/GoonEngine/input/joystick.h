#pragma once
typedef union SDL_Event SDL_Event;

#ifdef __cplusplus
extern "C"
{
#endif
    void geHandleJoystickEvent(const SDL_Event *event);
    void geInitializeJoysticks();
    void geUpdateControllerLastFrame();
    bool geGamepadButtonJustReleased(const int padNum, const int button);
    bool geGamepadButtonJustPressed(const int padNum, const int button);
    bool geGamepadButtonHeldDown(const int padNum, const int button);
#ifdef __cplusplus
}
#endif