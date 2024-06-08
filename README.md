# Supergoon Games Engine
- A C engine that wraps multiple libraries
- sound, libpng, freetype, etc

## Development
- Mostly developed on macos apple silicon

## Goals
- Better at C
- Learn about low-level programming for pictures (libpng) sound (ogg/openal), text (freetype) instead of using pre-made libraries
- Better at cross-platform building with cmake (Macos, Linux, Windows, Emscripten)

## Components
- SDL2: Low Level handling of windowing, events, input
- OpenAL: Cross-Platform sound abstraction

## Licenses
### Overall
- MIT

## Libraries
- [SDL](https://www.libsdl.org/license.php) - ZLIB - Low level Windowing / Eventing
- [Supergoon Sound](https://github.com/supergoongaming/sg_sound) - MIT - OpenAL wrapper
- [libpng](https://github.com/pnggroup/libpng) - MIT - Reading png files
- [freetype](https://github.com/freetype/freetype?tab=License-1-ov-file#readme) - Attribution - For handling typing
- [zlib](https://github.com/madler/zlib?tab=License-1-ov-file#readme) - MIT - Compression

## Valgrind Checks
- valgrind --track-origins=yes --leak-check=yes --leak-resolution=low --show-leak-kinds=definite ./SupergoonWorld 2>&1 | tee memcheck.txt