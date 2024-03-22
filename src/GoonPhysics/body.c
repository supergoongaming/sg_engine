#include <stdbool.h>
#include <stdio.h>
#include <GoonPhysics/body.h>
#include <GoonPhysics/overlap.h>

#define MAX_OVERLAP_BODIES 10
void gpBodyAddOverlapBeginFunc(gpBody *body, bodyOverlapArgs args)
{
    body->overlapFunctions[body->numOverlapFunctions++] = args;
    body->overlapFunctions = realloc(body->overlapFunctions, (body->numOverlapFunctions + 1) * sizeof(bodyOverlapArgs));
}

gpBody *gpBodyNew(gpBB boundingBox)
{
    gpBody *body;
    body = calloc(1, sizeof(*body));
    body->overlaps = calloc(MAX_OVERLAP_BODIES, sizeof(gpOverlap));
    body->lastFrameOverlaps = calloc(MAX_OVERLAP_BODIES, sizeof(gpOverlap));
    body->enabled = 1;
    body->bodyType = 1;
    body->gravityEnabled = 1;
    body->staticCollisionEnabled = 1;
    body->xGravityEnabled = 1;
    body->yGravityEnabled = 1;
    body->numOverlappingBodies = 0;
    body->numOverlapFunctions = 0;
    body->updateFunc = NULL;
    body->overlapFunctions = calloc(1, sizeof(bodyOverlapArgs));
    body->velocity = gpV(0, 0);
    body->maxVelocity = gpV(0, 0);
    body->friction = gpV(0, 0);
    body->boundingBox = boundingBox;
    body->bodyNum = -1;
    body->funcArgs = NULL;
    body->staticBody = 0;
    return body;
}

void gpBodySetPosition(gpBody *body, gpVec pos)
{
    body->boundingBox.x = pos.x;
    body->boundingBox.y = pos.y;
}

void gpBodySetVelocity(gpBody *body, gpVec vel)
{
    body->velocity.x = vel.x;
    body->velocity.y = vel.y;
}

gpBody *gpBodyNewStatic(gpBB boundingBox)
{
    gpBody *body = gpBodyNew(boundingBox);
    body->bodyType = 0;
    body->staticBody = 1;
    return body;
}

void gpBodyAddOverlap(gpBody *body, gpBody *overlapBody, int direction)
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

int gpBodyIsOnGround(gpBody *body)
{
    return body->thisFrameOnGround;
}
int gpBodyJustNotOnGround(gpBody *body)
{
    return !body->thisFrameOnGround && body->lastFrameOnGround;
}

int gpBodyJustGotOnGround(gpBody *body)
{
    return !body->lastFrameOnGround && body->thisFrameOnGround;
}

void gpBodyFree(gpBody *body)
{
    free(body->overlaps);
    free(body->lastFrameOverlaps);
    free(body->overlapFunctions);
    free(body);
}