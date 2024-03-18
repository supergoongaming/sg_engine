#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
// #include <SDL2/SDL_image.h>
