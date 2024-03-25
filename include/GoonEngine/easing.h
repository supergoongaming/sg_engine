//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//
// For more information, please refer to <https://unlicense.org>
//

#pragma once

#if defined(__LP64__) && !defined(AH_EASING_USE_DBL_PRECIS)
#define AH_EASING_USE_DBL_PRECIS
#endif

#ifdef AH_EASING_USE_DBL_PRECIS
#define AH_FLOAT_TYPE double
#else
#define AH_FLOAT_TYPE float
#endif
typedef AH_FLOAT_TYPE AHFloat;

#if defined __cplusplus
extern "C"
{
#endif

    typedef AHFloat (*AHEasingFunction)(AHFloat);

    // Linear interpolation (no easing)
    AHFloat geLinearInterpolation(AHFloat p);

    // Quadratic easing; p^2
    AHFloat geQuadraticEaseIn(AHFloat p);
    AHFloat geQuadraticEaseOut(AHFloat p);
    AHFloat geQuadraticEaseInOut(AHFloat p);

    // Cubic easing; p^3
    AHFloat geCubicEaseIn(AHFloat p);
    AHFloat geCubicEaseOut(AHFloat p);
    AHFloat geCubicEaseInOut(AHFloat p);

    // Quartic easing; p^4
    AHFloat geQuarticEaseIn(AHFloat p);
    AHFloat geQuarticEaseOut(AHFloat p);
    AHFloat geQuarticEaseInOut(AHFloat p);

    // Quintic easing; p^5
    AHFloat geQuinticEaseIn(AHFloat p);
    AHFloat geQuinticEaseOut(AHFloat p);
    AHFloat geQuinticEaseInOut(AHFloat p);

    // Sine wave easing; sin(p * PI/2)
    AHFloat geSineEaseIn(AHFloat p);
    AHFloat geSineEaseOut(AHFloat p);
    AHFloat geSineEaseInOut(AHFloat p);

    // Circular easing; sqrt(1 - p^2)
    AHFloat geCircularEaseIn(AHFloat p);
    AHFloat geCircularEaseOut(AHFloat p);
    AHFloat geCircularEaseInOut(AHFloat p);

    // Exponential easing, base 2
    AHFloat geExponentialEaseIn(AHFloat p);
    AHFloat geExponentialEaseOut(AHFloat p);
    AHFloat geExponentialEaseInOut(AHFloat p);

    // Exponentially-damped sine wave easing
    AHFloat geElasticEaseIn(AHFloat p);
    AHFloat geElasticEaseOut(AHFloat p);
    AHFloat geElasticEaseInOut(AHFloat p);

    // Overshooting cubic easing;
    AHFloat geBackEaseIn(AHFloat p);
    AHFloat geBackEaseOut(AHFloat p);
    AHFloat geBackEaseInOut(AHFloat p);

    // Exponentially-decaying bounce easing
    AHFloat geBounceEaseIn(AHFloat p);
    AHFloat geBounceEaseOut(AHFloat p);
    AHFloat geBounceEaseInOut(AHFloat p);

#ifdef __cplusplus
}
#endif
