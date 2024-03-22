#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GoonPhysics/scene.h>
#include <GoonPhysics/body.h>
#include <GoonPhysics/gravity.h>
#include <GoonPhysics/aabb.h>
#include <GoonPhysics/overlap.h>
#include <GoonPhysics/boxCollider.h>

// Rigidbodies
static int _currentNumBodies = 0;
static int _currentCapacityBodies = 4;
static gpBody **_currentBodies;
// Static Bodies
static int _currentNumStaticBodies = 0;
static int _currentCapacityStaticBodies = 4;
static gpBody **_currentStaticBodies;
// BoxColliders
static int _currentNumBoxColliders = 0;
static int _currentCapacityBoxColliders = 4;
static gpBoxCollider **_currentBoxColliders;

// Should we be doing things
static int _isEnabled = 1;

static void ApplyYVelocity(gpBody *body, float gameTime);
static void ApplyXVelocity(gpBody *body, float gameTime);
static void CheckForNonStaticOverlaps(gpBody *body);

void gpSceneUpdate(gpScene *scene, float gameTime)
{
    if (!_isEnabled)
        return;
    gpSceneGravity sceneGravity;
    sceneGravity.sceneGravity = scene->gravity;
    sceneGravity.sceneFriction = scene->friction;
    sceneGravity.sceneMaxXVelocity = 200;
    sceneGravity.sceneMaxYVelocity = 500;
    // sceneGravity.sceneMinYVelocity = 0.001;
    sceneGravity.sceneMinYVelocity = 0.1;
    sceneGravity.sceneMinXVelocity = 10;

    for (size_t i = 0; i < _currentNumBodies; i++)
    {
        if (!_isEnabled)
            return;
        gpBody *body = _currentBodies[i];
        if (!body)
            continue;
        memcpy(body->lastFrameOverlaps, body->overlaps, sizeof(gpOverlap) * body->numOverlappingBodies);
        body->lastFrameNumOverlappingBodies = body->numOverlappingBodies;
        body->numOverlappingBodies = 0;
        if (!body->gravityEnabled)
        {
            // We should still check if we are overlapping with other things, even if we aren't checking for movement
            CheckForNonStaticOverlaps(body);
            continue;
        }
        gpGravityBodyStep(body, &sceneGravity, gameTime);
        ApplyYVelocity(body, gameTime);
        ApplyXVelocity(body, gameTime);
        if (body->updateFunc)
        {
            body->updateFunc(body->funcArgs, body);
        }
    }
}

static void CheckForNonStaticOverlaps(gpBody *body)
{
    if (!_isEnabled)
        return;
    // Check for non static bodies
    for (size_t i = 0; i < _currentNumBodies; i++)
    {
        gpBody *overlapBody = _currentBodies[i];
        if (overlapBody == body)
            continue;
        // Check to see if this overlap already happened (this happens inside of static body)
        bool newOverlap = true;
        for (size_t j = 0; j < body->numOverlappingBodies; j++)
        {
            if (body->overlaps[j].overlapBody == overlapBody)
            {
                newOverlap = false;
                break;
            }
        }
        if (!newOverlap)
            continue;
        gpBB overlapArea;
        int intersect = gpIntersectRect(&body->boundingBox, &overlapBody->boundingBox, &overlapArea);
        if (intersect)
        {
            int direction = gpCalculateIntersectionDirection(&overlapArea, &body->boundingBox);
            gpBodyAddOverlap(body, overlapBody, direction);
        }
    }
}

static void ApplyYVelocity(gpBody *body, float gameTime)
{
    if (!body->yGravityEnabled)
        return;
    body->lastFrameOnGround = body->thisFrameOnGround;
    body->thisFrameOnGround = 0;
    double initialYStep = body->velocity.y * gameTime;
    double iterYStep = initialYStep;
    double stepSize = (int)initialYStep != 0 ? initialYStep > 0 ? 1 : -1 : initialYStep;
    int shouldStep = stepSize != 0 ? 1 : 0;
    while (shouldStep && _isEnabled)
    {
        body->boundingBox.y += stepSize;
        // Check for collisions for each static body
        // If it is a blocking body, then we should set shouldStep to False
        int direction = stepSize > 0 ? gpOverlapDown : gpOverlapUp;
        // Problem, if we are moving down on ground (due to gravity)
        // This direction gets passed into check for nonstatic overlaps
        // This causes a problem.
        for (size_t i = 0; i < _currentNumStaticBodies; i++)
        {
            gpBody *staticBody = _currentStaticBodies[i];
            int intersect = gpIntersectBoxBox(&body->boundingBox, &staticBody->boundingBox);
            if (intersect)
            {
                if (body->staticCollisionEnabled)
                {
                    gpResolveOverlapY(&body->boundingBox, &staticBody->boundingBox);
                    shouldStep = 0;
                    if (direction == gpOverlapDown)
                        body->thisFrameOnGround = 1;
                }
                gpBodyAddOverlap(body, staticBody, direction);
            }
        }
        CheckForNonStaticOverlaps(body);

        // For body in bodies, if collides,
        // then send out notify for subscribers with info of collision bounding box and body num
        if (!shouldStep)
        {
            // If we are set to be blocked by the other body,
            // then set should step to 0, and revert body back to initial
            // body->boundingBox.y -= stepSize;
            if (body->staticCollisionEnabled)
                body->velocity.y = 0.0;
            continue;
        }
        iterYStep -= stepSize;
        if (iterYStep && fabs(iterYStep) < 1)
        {
            // We have a partial step remaining, so add that in a final round.
            stepSize = iterYStep;
        }
        else if (!iterYStep)
        {
            shouldStep = 0;
        }
    }
}

static void ApplyXVelocity(gpBody *body, float gameTime)
{
    if (!body->xGravityEnabled)
        return;
    double initialXStep = body->velocity.x * gameTime;
    double iterXStep = initialXStep;
    double stepSize = (int)initialXStep != 0 ? initialXStep > 0 ? 1 : -1 : initialXStep;
    int shouldStep = stepSize != 0 ? 1 : 0;
    int direction = stepSize > 0 ? gpOverlapRight : gpOverlapLeft;
    while (shouldStep && _isEnabled)
    {
        body->boundingBox.x += stepSize;
        // Check for collisions for each body
        // For body in bodies, if collides,
        // then send out notify for subscribers with info of collision bounding box and body num
        // If it is a blocking body, then we should set shouldStep to False
        for (size_t i = 0; i < _currentNumStaticBodies; i++)
        {
            gpBody *staticBody = _currentStaticBodies[i];
            int intersect = gpIntersectBoxBox(&body->boundingBox, &staticBody->boundingBox);
            if (intersect)
            {

                if (body->staticCollisionEnabled)
                {
                    gpResolveOverlapX(&body->boundingBox, &staticBody->boundingBox);
                    shouldStep = 0;
                }
                gpBodyAddOverlap(body, staticBody, direction);
            }
        }

        if (!shouldStep)
        {
            // If we are set to be blocked by the other body,
            // then set should step to 0, and revert body back to initial
            // body->boundingBox.x -= stepSize;
            body->velocity.x = 0.0;
            continue;
        }
        iterXStep -= stepSize;
        if (iterXStep && fabs(iterXStep) < 1)
        {
            // We have a partial step remaining, so add that in a final round.
            stepSize = iterXStep;
        }
        else if (!iterXStep)
        {
            shouldStep = 0;
        }
    }
    CheckForNonStaticOverlaps(body);
}

gpScene *gpInitScene(void)
{
    gpScene *scene = calloc(1, sizeof(*scene));
    _currentNumBodies = 0;
    _currentNumStaticBodies = 0;
    _currentCapacityBodies = 4;
    _currentCapacityStaticBodies = 4;
    _currentCapacityBoxColliders = 4;
    _currentBodies = calloc(_currentCapacityBodies, _currentCapacityBodies * sizeof(gpBody *));
    _currentStaticBodies = calloc(_currentCapacityStaticBodies, _currentCapacityBoxColliders * sizeof(gpBody *));
    _currentBoxColliders = calloc(_currentCapacityBoxColliders, _currentCapacityBoxColliders * sizeof(gpBoxCollider *));
    return scene;
}

void gpSceneSetGravity(gpScene *scene, float gravity)
{
    scene->gravity = gravity;
}
void gpSceneSetFriction(gpScene *scene, float friction)
{
    scene->friction = friction;
}

int gpSceneAddBody(gpBody *body)
{
    if (_currentNumBodies > _currentCapacityBodies / 2)
    {
        _currentBodies = realloc(_currentBodies, _currentCapacityBodies * 2 * sizeof(gpBody *));
        if (_currentBodies == NULL)
        {
            fprintf(stderr, "Couldn't reallocate to increase body size, what the");
        }
        _currentCapacityBodies *= 2;
    }
    _currentBodies[_currentNumBodies] = body;
    body->bodyNum = _currentNumBodies;
    ++_currentNumBodies;
    return body->bodyNum;
}
int gpSceneAddStaticBody(gpBody *body)
{
    if (_currentNumStaticBodies > _currentCapacityStaticBodies / 2)
    {
        _currentStaticBodies = realloc(_currentStaticBodies, _currentCapacityStaticBodies * 2 * sizeof(gpBody *));
        if (_currentStaticBodies == NULL)
        {
            fprintf(stderr, "Couldn't reallocate to increase static body size, what the");
        }
        _currentCapacityStaticBodies *= 2;
    }
    // body->bodyType = 0;
    // _currentBodies[_currentNumBodies] = body;
    _currentStaticBodies[_currentNumStaticBodies] = body;
    ++_currentNumStaticBodies;
    return _currentNumStaticBodies - 1;
}

gpBody *gpSceneGetBody(int bodyRef)
{
    if (bodyRef < _currentNumBodies && _currentBodies[bodyRef])
    {
        return _currentBodies[bodyRef];
    }
    return NULL;
}

void gpSceneSetEnabled(int isEnabled)
{
    _isEnabled = isEnabled;
}

void gpSceneRemoveBody(int bodyNum)
{
    if (bodyNum < _currentNumBodies && _currentBodies[bodyNum])
    {
        gpBodyFree(_currentBodies[bodyNum]);
        _currentBodies[bodyNum] = NULL;
    }
}
void gpSceneFree(gpScene *scene)
{
    for (size_t i = 0; i < _currentNumBodies; i++)
    {
        if (_currentBodies[i])
        {
            gpBodyFree(_currentBodies[i]);
        }
    }
    for (size_t i = 0; i < _currentNumStaticBodies; i++)
    {
        if (_currentStaticBodies[i])
        {
            gpBodyFree(_currentStaticBodies[i]);
        }
    }
    free(_currentBodies);
    free(_currentStaticBodies);
    free(scene);
}

int gpSceneAddBoxCollider(gpBoxCollider *boxCollider)
{
    if (_currentNumBoxColliders > _currentCapacityBoxColliders / 2)
    {
        _currentBoxColliders = realloc(_currentStaticBodies, _currentCapacityBoxColliders * 2 * sizeof(gpBoxCollider *));
        if (_currentBoxColliders == NULL)
        {
            fprintf(stderr, "Couldn't reallocate to increase body size, what the");
        }
        _currentCapacityBoxColliders *= 2;
    }
    _currentBoxColliders[_currentNumBoxColliders] = boxCollider;
    boxCollider->bodyNum = _currentNumBoxColliders;
    ++_currentNumBoxColliders;
    return boxCollider->bodyNum;
}

void gpSceneRemoveBoxCollider(int boxNum)
{
    if (boxNum < _currentNumBoxColliders && _currentBoxColliders[boxNum])
    {
        gpBoxColliderFree(_currentBoxColliders[boxNum]);
        _currentBoxColliders[boxNum] = NULL;
    }
}