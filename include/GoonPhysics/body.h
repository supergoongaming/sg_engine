/**
 * @file body.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <GoonPhysics/bb.h>
#include <GoonPhysics/vec.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct gpBody gpBody;
    typedef struct gpOverlap gpOverlap;
    typedef void (*OverlapFunc)(void *args, gpBody *body, gpBody *overlapBody, gpOverlap *overlap);
    typedef void (*BodyLocationUpdateFunc)(void *args, gpBody *body);

    typedef struct bodyOverlapArgs
    {
        int bodyType, overlapBodyType;
        OverlapFunc overlapFunc;

    } bodyOverlapArgs;

    typedef struct gpBody
    {
        int bodyNum;
        int bodyType;
        int staticCollisionEnabled;
        int staticBody;
        int enabled;
        gpBB boundingBox;
        gpVec acceleration;
        gpVec velocity;
        gpVec maxVelocity;
        gpVec friction;
        int numOverlappingBodies;
        int lastFrameNumOverlappingBodies;
        int gravityEnabled;
        int xGravityEnabled;
        int yGravityEnabled;
        BodyLocationUpdateFunc updateFunc;
        int lastFrameOnGround;
        int thisFrameOnGround;
        bodyOverlapArgs *overlapFunctions;
        int numOverlapFunctions;
        void *funcArgs;
        struct gpOverlap *overlaps;
        struct gpOverlap *lastFrameOverlaps;

    } gpBody;

    gpBody *gpBodyNew(gpBB boundingBox);
    gpBody *gpBodyNewStatic(gpBB boundingBox);
    void gpBodyFree(gpBody *body);
    void gpBodySetPosition(gpBody *body, gpVec pos);
    void gpBodySetVelocity(gpBody *body, gpVec vel);
    void gpBodySetMaxVelocityX(gpBody *body, float maxVel);
    void gpBodyAddOverlap(gpBody *body, gpBody *overlapBody, int direction);
    int gpBodyIsOnGround(gpBody *body);
    int gpBodyJustGotOnGround(gpBody *body);
    int gpBodyJustNotOnGround(gpBody *body);

    // Overlap functions
    void gpBodyAddOverlapBeginFunc(gpBody *body, bodyOverlapArgs args);

#ifdef __cplusplus
}
#endif