#pragma once
#ifdef _cplusplus
extern "C"
{
#endif

    typedef struct Point
    {
        int x, y;
    } Point;

    inline Point gePointZero()
    {
        Point point = {0, 0};
        return point;
    };
#ifdef _cplusplus
}
#endif