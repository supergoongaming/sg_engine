#include <stdbool.h>
#include <GoonPhysics/boxCollider.h>
#include <GoonPhysics/overlap.h>
#include <GoonPhysics/bb.h>
#include <GoonPhysics/vec.h>
#include <GoonPhysics/body.h>
#define MAX_OVERLAP_BODIES 10

gpBoxCollider *gpBoxColliderNew(gpBB boundingBox)
{
    gpBoxCollider *box = calloc(1, sizeof(gpBoxCollider));
    box->bodyNum = -1;
    box->bodyType = -1;
    box->boundingBox = boundingBox;
    box->enabled = 1;
    box->numOverlappingBodies = 0;
    box->overlapFunctions = calloc(1, sizeof(bodyOverlapArgs));
    box->numOverlapFunctions = 0;
    box->funcArgs = NULL;
    box->overlaps = calloc(MAX_OVERLAP_BODIES, sizeof(gpOverlap));
    box->lastFrameOverlaps = calloc(MAX_OVERLAP_BODIES, sizeof(gpOverlap));
    return box;
}

void gpBoxColliderFree(gpBoxCollider *box)
{
    free(box->overlaps);
    free(box->lastFrameOverlaps);
    free(box->overlapFunctions);
    free(box);
}
void gpBoxColliderSetPosition(gpBoxCollider *collider, gpVec pos)
{
    collider->boundingBox.x = pos.x;
    collider->boundingBox.y = pos.y;
}
void gpBoxColliderAddOverlap(gpBoxCollider *body, gpBody *overlapBody, int direction)
{
    if (body->numOverlappingBodies >= MAX_OVERLAP_BODIES)
        return;
    bool newOverlap = true;
    for (size_t i = 0; i < body->lastFrameNumOverlappingBodies; i++)
    {
        if (body->lastFrameOverlaps[i].overlapBody == overlapBody)
        {
            newOverlap = false;
            break;
        }
    }
    int bodyType = body->bodyType;
    int overlapBodyType = overlapBody->bodyType;
    int currentOverlap = body->numOverlappingBodies;
    body->overlaps[body->numOverlappingBodies].overlapBody = overlapBody;
    body->overlaps[body->numOverlappingBodies].overlapDirection = direction;
    ++body->numOverlappingBodies;
    if (newOverlap)
    {
        for (size_t i = 0; i < body->numOverlapFunctions; i++)
        {
            if (body->overlapFunctions[i].bodyType != bodyType ||
                body->overlapFunctions[i].overlapBodyType != overlapBodyType ||
                !body->overlapFunctions[i].overlapFunc)
                continue;
            body->overlapFunctions[i].overlapFunc(body->funcArgs, body, overlapBody, &body->overlaps[currentOverlap]);
        }
    }
    else
    {
        // Already overlap, do something else
    }
}
void gpBoxColliderAddOverlapBeginFunc(gpBoxCollider *body, bodyOverlapArgs args)
{
    body->overlapFunctions[body->numOverlapFunctions++] = args;
    body->overlapFunctions = realloc(body->overlapFunctions, (body->numOverlapFunctions + 1) * sizeof(bodyOverlapArgs));
}