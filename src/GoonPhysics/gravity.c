#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <GoonPhysics/gravity.h>

static void GravityConstraintX(gpBody *body, double frictionStep, gpSceneGravity *sceneGravity)
{
    body->velocity.x += body->acceleration.x;
    body->acceleration.x = 0;
    if (body->velocity.x == 0)
        return;
    bool currentVelocityPositive = body->velocity.x > 0;
    double signedFrictionStep = currentVelocityPositive ? -frictionStep : frictionStep;

    double step = body->velocity.x + signedFrictionStep;
    if (step >= 0)
    {
        if (body->maxVelocity.x)
        {
            if (step > body->maxVelocity.x)
                step = body->maxVelocity.x;
        }
        if (step > sceneGravity->sceneMaxXVelocity)
            step = sceneGravity->sceneMaxXVelocity;
        if (step < sceneGravity->sceneMinXVelocity)
            step = 0;
    }
    else
    {
        if (body->maxVelocity.x)
        {
            if (step < -body->maxVelocity.x)
                step = -body->maxVelocity.x;
        }
        if (step < -sceneGravity->sceneMaxXVelocity)
            step = -sceneGravity->sceneMaxXVelocity;
        if (step > sceneGravity->sceneMinXVelocity)
            step = 0;
    }
    body->velocity.x = (float)step;

    // Test if max or min is hit
    // if (fabs(body->velocity.x) > sceneGravity->sceneMaxXVelocity)
    // {
    //     body->velocity.x = copysignf(sceneGravity->sceneMaxXVelocity, body->velocity.x);
    // }
    // // Check if the velocity has reached a minimum threshold to stop
    // if (fabs(body->velocity.x) < sceneGravity->sceneMinXVelocity)
    // {
    //     body->velocity.x = 0;
    // }
}

static void GravityConstraintY(gpBody *body, double gravityStep, gpSceneGravity *sceneGravity)
{
    body->velocity.y += body->acceleration.y;
    body->acceleration.y = 0;
    double step = body->velocity.y + gravityStep;
    if (step >= 0)
    {
        if (step > sceneGravity->sceneMaxYVelocity)
            step = sceneGravity->sceneMaxYVelocity;
        if (step < sceneGravity->sceneMinYVelocity)
            step = 0;
    }
    else
    {
        if (step < -sceneGravity->sceneMaxYVelocity)
            step = -sceneGravity->sceneMaxYVelocity;
        if (step > sceneGravity->sceneMinYVelocity)
            step = 0;
    }
    body->velocity.y = (float)step;
}

void gpGravityBodyStep(gpBody *body, gpSceneGravity *sceneGravity, float deltaTime)
{
    double gravityStep = sceneGravity->sceneGravity * deltaTime;
    double frictionStep = (sceneGravity->sceneFriction + body->friction.x) * deltaTime;

    // Keep Y in Bounds for Max Speed
    GravityConstraintY(body, gravityStep, sceneGravity);
    // Keep X in Bounds for max Speed
    GravityConstraintX(body, frictionStep, sceneGravity);
}