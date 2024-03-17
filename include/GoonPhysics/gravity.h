/**
 * @file gravity.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-31
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
    typedef struct gpSceneGravity
    {
        float sceneGravity;
        float sceneMaxXVelocity;
        float sceneMinXVelocity;
        float sceneMaxYVelocity;
        float sceneMinYVelocity;
        float sceneFriction;

    } gpSceneGravity;

    // void gpApplyGravity()
    void gpGravityBodyStep(gpBody *body, gpSceneGravity *sceneGravity, float deltaTime);
    // void gpApplyGravity(gpBody body, gpBody* collisionBodies, int numCollisionBodies);
#ifdef __cplusplus
}
#endif