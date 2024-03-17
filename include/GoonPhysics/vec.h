/**
 * @file vec.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Vector 2 class for goon physics
     *
     */
    typedef struct gpVec
    {
        float x, y;
    } gpVec;
    /**
     * @brief Creates a vector 2 with x/y you pass in
     *
     * @return gpVec
     */
    gpVec gpV(float x, float y);
    gpVec gpVAdd(gpVec lhs, gpVec rhs);
#ifdef __cplusplus
}
#endif