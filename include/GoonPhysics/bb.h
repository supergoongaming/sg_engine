/**
 * @file bb.h
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

    typedef struct gpBB
    {
        double x, y, w, h;
    } gpBB;

    gpBB gpBBNew(double x, double y, double w, double h);
    gpBB gpBBCopy(gpBB body);
#ifdef __cplusplus
}
#endif