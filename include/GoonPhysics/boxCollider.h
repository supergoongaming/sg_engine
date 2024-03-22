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
    typedef struct bodyOverlapArgs bodyOverlapArgs;

    typedef struct gpBoxCollider
    {
        int bodyNum;
        int bodyType;
        gpBB boundingBox;
        int numOverlappingBodies;
        int lastFrameNumOverlappingBodies;
        bodyOverlapArgs *overlapFunctions;
        int numOverlapFunctions;
        void *funcArgs;
        struct gpOverlap *overlaps;
        struct gpOverlap *lastFrameOverlaps;

    } gpBoxCollider;

    gpBoxCollider *gpBoxColliderNew(gpBB boundingBox);
    void gpBoxColliderFree(gpBoxCollider *body);
    void gpBoxColliderSetPosition(gpBoxCollider *collider, gpVec pos);
    void gpBoxColliderAddOverlap(gpBoxCollider *body, gpBody *overlapBody, int direction);
    void gpBoxColliderAddOverlapBeginFunc(gpBoxCollider *body, bodyOverlapArgs args);
//
#ifdef __cplusplus
}
#endif