#pragma once
typedef struct geRectangle
{
    int x, y, w, h;
} geRectangle;

inline geRectangle geRectangleZero()
{
    geRectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = 0;
    return rect;
}
inline int geRectangleIsZero(geRectangle *lhs)
{
    if (lhs->h == 0 &&
        lhs->w == 0 &&
        lhs->x == 0 &&
        lhs->y == 0)
        return 1;
    return 0;
}
