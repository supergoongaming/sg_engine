#pragma once
struct InputEvent;
#include <GoonEngine/InputEvent.h>
#ifdef __cplusplus
extern "C"
{
#endif
    int gePlayLoop();
    int ExitEngine();
    int geInitializeEngine();
    void geSetCurrentScene(void *scene);
    void geGameSetUpdateFunc(void (*updateFunc)(double deltaTime));
    void geGameSetDrawFunc(void (*drawFunc)(double accum));
#ifdef __cplusplus
}
#endif