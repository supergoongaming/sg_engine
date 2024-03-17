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