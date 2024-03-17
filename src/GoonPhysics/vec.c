#include <GoonPhysics/vec.h>

gpVec gpV(float x, float y)
{
    gpVec vec;
    vec.x = x;
    vec.y = y;
    return vec;
}
gpVec gpVAdd(gpVec lhs, gpVec rhs)
{
    return gpV(lhs.x + rhs.x, lhs.y + rhs.y);
}