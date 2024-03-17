/**
 * @file overlap.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
// #include <GoonPhysics/body.h>

#ifdef __cplusplus
extern "C"
{
#endif
    struct gpBody;
    typedef enum gpOverlapDirections
    {
        gpOverlapNoOverlap = 0,
        gpOverlapUp,
        gpOverlapRight,
        gpOverlapDown,
        gpOverlapLeft
    } gpOverlapDirections;

    typedef struct gpOverlap
    {
        int overlapDirection;
        struct gpBody *overlapBody;
    } gpOverlap;
#ifdef __cplusplus
}
#endif