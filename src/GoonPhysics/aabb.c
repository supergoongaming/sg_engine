#include <math.h>
#include <stdio.h>
#include <GoonPhysics/aabb.h>
#include <GoonPhysics/overlap.h>

// typedef enum
// {
//     NoOverlap,
//     OverlapTop,
//     OverlapBottom,
//     OverlapLeft,
//     OverlapRight,
// } OverlapDirection;
bool gpBBEmpty(const gpBB *r)
{
    return ((!r) || (r->w <= 0) || (r->h <= 0)) ? true : false;
}

bool gpIntersectRect(const gpBB *A, const gpBB *B, gpBB *result)
{
    int Amin, Amax, Bmin, Bmax;

    if (!A || !B || !result)
    {
        // TODO error message
        return false;
    }

    /* Special cases for empty rects */
    if (gpBBEmpty(A) || gpBBEmpty(B))
    {
        return false;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result->x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->w = Amax - Amin;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result->y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->h = Amax - Amin;

    return !gpBBEmpty(result);
}
int gpCalculateIntersectionDirection(const gpBB *intersectionAreaRect, const gpBB *yourCollisionBox)
{
    if (intersectionAreaRect->w < intersectionAreaRect->h)
    {
        if (intersectionAreaRect->x > yourCollisionBox->x)
            return gpOverlapRight;
        return gpOverlapLeft;
    }
    if (intersectionAreaRect->y > yourCollisionBox->y)
        return gpOverlapDown;
    return gpOverlapUp;
}

int gpGetOverlapDirection(gpBB *lhs, gpBB *rhs)
{
    // Check for overlap on the top side
    if (lhs->y + lhs->h > rhs->y && lhs->y < rhs->y)
    {
        return gpOverlapUp;
    }

    // Check for overlap on the bottom side
    if (lhs->y < rhs->y + rhs->h && lhs->y + lhs->h > rhs->y)
    {
        return gpOverlapDown;
    }

    // Check for overlap on the left side
    if (lhs->x + lhs->w > rhs->x && lhs->x < rhs->x)
    {
        return gpOverlapLeft;
    }

    // Check for overlap on the right side
    if (lhs->x < rhs->x + rhs->w && lhs->x + lhs->w > rhs->x + rhs->w)
    {
        return gpOverlapRight;
    }

    return gpOverlapNoOverlap;
}

int gpIntersectBoxBox(gpBB *lhs, gpBB *rhs)
{
    if (lhs->x < rhs->x + rhs->w &&
        lhs->x + lhs->w > rhs->x &&
        lhs->y < rhs->y + rhs->h &&
        lhs->y + lhs->h > rhs->y)
    {
        return 1;
    }
    return 0;
}

void gpResolveOverlap(gpBB *lhs, gpBB *rhs)
{
    gpBB result;

    double Amin, Amax, Bmin, Bmax;

    /* Horizontal intersection */
    Amin = lhs->x;
    Amax = Amin + lhs->w;
    Bmin = rhs->x;
    Bmax = Bmin + rhs->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result.x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.w = Amax - Amin;

    /* Vertical intersection */
    Amin = lhs->y;
    Amax = Amin + lhs->h;
    Bmin = rhs->y;
    Bmax = Bmin + rhs->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result.y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.h = Amax - Amin;

    // Resolve overlap independently in x and y directions
    if (result.h > 0)
    {
        // Determine the direction of overlap for the y-axis
        double overlapY = (lhs->y + lhs->h / 2 < rhs->y + rhs->h / 2) ? -result.h : result.h;

        // Resolve overlap in the y-axis
        lhs->y += overlapY;
    }

    if (result.w > 0)
    {
        // Determine the direction of overlap for the x-axis
        double overlapX = (lhs->x + lhs->w / 2 < rhs->x + rhs->w / 2) ? -result.w : result.w;

        // Resolve overlap in the x-axis
        lhs->x += overlapX;
    }
}
void gpResolveOverlapY(gpBB *lhs, gpBB *rhs)
{
    gpBB result;

    double Amin, Amax, Bmin, Bmax;

    /* Horizontal intersection */
    Amin = lhs->x;
    Amax = Amin + lhs->w;
    Bmin = rhs->x;
    Bmax = Bmin + rhs->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result.x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.w = Amax - Amin;

    /* Vertical intersection */
    Amin = lhs->y;
    Amax = Amin + lhs->h;
    Bmin = rhs->y;
    Bmax = Bmin + rhs->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result.y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.h = Amax - Amin;

    // Resolve overlap independently in x and y directions
    // if (result.w > 0) {
    //     // Determine the direction of overlap for the x-axis
    //     double overlapX = (lhs->x + lhs->w / 2 < rhs->x + rhs->w / 2) ? -result.w : result.w;

    //     // Resolve overlap in the x-axis
    //     lhs->x += overlapX;
    // }

    if (result.h > 0)
    {
        // Determine the direction of overlap for the y-axis
        double overlapY = (lhs->y + lhs->h / 2 < rhs->y + rhs->h / 2) ? -result.h : result.h;

        // Resolve overlap in the y-axis
        lhs->y += overlapY;
    }
}

void gpResolveOverlapX(gpBB *lhs, gpBB *rhs)
{
    gpBB result;

    double Amin, Amax, Bmin, Bmax;

    /* Horizontal intersection */
    Amin = lhs->x;
    Amax = Amin + lhs->w;
    Bmin = rhs->x;
    Bmax = Bmin + rhs->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result.x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.w = Amax - Amin;

    /* Vertical intersection */
    Amin = lhs->y;
    Amax = Amin + lhs->h;
    Bmin = rhs->y;
    Bmax = Bmin + rhs->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result.y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result.h = Amax - Amin;

    // Resolve overlap independently in x and y directions
    if (result.w > 0)
    {
        // Determine the direction of overlap for the x-axis
        double overlapX = (lhs->x + lhs->w / 2 < rhs->x + rhs->w / 2) ? -result.w : result.w;

        // Resolve overlap in the x-axis
        lhs->x += overlapX;
    }

    // if (result.h > 0) {
    //     // Determine the direction of overlap for the y-axis
    //     double overlapY = (lhs->y + lhs->h / 2 < rhs->y + rhs->h / 2) ? -result.h : result.h;

    //     // Resolve overlap in the y-axis
    //     lhs->y += overlapY;
    // }
}
