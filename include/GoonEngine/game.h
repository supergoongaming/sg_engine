#pragma once
struct InputEvent;
#include <GoonEngine/InputEvent.h>
#ifdef __cplusplus
extern "C"
{
#endif
    int gePlayLoop();
    int geQuitEngine();
    int geInitializeEngine();
    void geSetCurrentScene(void *scene);
    void geGameSetUpdateFunc(void (*updateFunc)(double deltaTime));
    void geGameSetDrawFunc(void (*drawFunc)());
#ifdef __cplusplus
}
#endif