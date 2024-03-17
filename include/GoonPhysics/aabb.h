/**
 * @file aabb.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <stdbool.h>
#include <GoonPhysics/bb.h>

#ifdef __cplusplus
extern "C"
{
#endif
    bool gpBBEmpty(const gpBB *r);
    bool gpIntersectRect(const gpBB *A, const gpBB *B, gpBB *result);
    void gpResolveOverlapY(gpBB *lhs, gpBB *rhs);
    void gpResolveOverlapX(gpBB *lhs, gpBB *rhs);
    void gpResolveOverlap(gpBB *lhs, gpBB *rhs);
    int gpIntersectBoxBox(gpBB *lhs, gpBB *rhs);
    int gpGetOverlapDirection(gpBB *lhs, gpBB *rhs);
    int gpCalculateIntersectionDirection(const gpBB *intersectionAreaRect, const gpBB *yourCollisionBox);
#ifdef __cplusplus
}
#endif