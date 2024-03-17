/**
 * @file scene.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <GoonPhysics/body.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct gpScene
    {
        float gravity;
        float friction;
    } gpScene;

    gpScene *gpInitScene(void);
    void gpSceneSetGravity(gpScene *scene, float gravity);
    void gpSceneSetFriction(gpScene *scene, float friction);
    int gpSceneAddBody(gpBody *body);
    int gpSceneAddStaticBody(gpBody *body);
    void gpSceneUpdate(gpScene *scene, float gameTime);
    void gpSceneSetEnabled(int isEnabled);
    gpBody *gpSceneGetBody(int bodyRef);
    void gpSceneRemoveBody(int bodyNum);
    void gpSceneFree(gpScene* scene);

#ifdef __cplusplus
}
#endif