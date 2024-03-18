# #########################################
# Initial config
# #########################################
set(LIBRARY_TARGET_NAME SupergoonEngine)
cmake_minimum_required(VERSION 3.16)
message(STATUS "Starting ${LIBRARY_TARGET_NAME} project initialization")

# set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # This exports a file that clangd can use so that we know what compile/include flags so that we can use clangd language server properly with our cmake attributes.
include(FindPkgConfig)
project(${LIBRARY_TARGET_NAME} VERSION 0.1.0 LANGUAGES C)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Adding PIC to enable linux builds test
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# #########################################
# File globbing
# #########################################
message(STATUS "Getting all files for cmake..")
file(GLOB_RECURSE ENGINE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/GoonEngine/*.c* ${CMAKE_CURRENT_SOURCE_DIR}/src/GoonPhysics/*.c*)

# #########################################
# Platform specific Project setup
# #########################################
message(STATUS "Changing the platform specific settings")

if(APPLE)
    if(NOT IOS) # MACOS #
        message(STATUS "Configuring Macos")
        set(MACOS ON)

        if(CMAKE_GENERATOR STREQUAL "Xcode")
            message(STATUS "Setting Build to Universal Binary")
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
        endif(CMAKE_GENERATOR STREQUAL "Xcode")
    else()
        # IOS
    endif(NOT IOS)
elseif(WIN32)
    message(STATUS "Configuring Windows")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /c/cmake)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring Linux")
    set(LINUX ON)
endif(APPLE)

# Subdirectory things

# We want to skip installing freetype,etc, this is a var in their CMAKE file
set(SKIP_INSTALL_ALL ON)

# Don't build the png tests
set(PNG_TESTS OFF)

# #########################################
# Initialize
# #########################################
message(STATUS "Initializing the project")
message(STATUS "Adding subdirectories for engine")

# if(LINUX)
add_library(${LIBRARY_TARGET_NAME}
    STATIC
    ${ENGINE_FILES}
)

# else()
# add_library(${LIBRARY_TARGET_NAME}
# SHARED
# ${ENGINE_FILES}
# )
# endif(LINUX)
set_property(TARGET ${LIBRARY_TARGET_NAME} PROPERTY C_STANDARD 11)
find_package(PkgConfig REQUIRED)

# find_package(ZLIB)
include(CTest)

# #########################################
# Options
# #########################################
option(CMAKE_DEBUG_VARIABLES "Runs a debug on all variables for troubleshooting" ON)
option(GOON_BUILD_PCH "Uses a PCH file to try and speed up compilation" ON)
option(GOON_DEBUG_SOUND "Builds sound from subdirectory instead of using a made package so we can debug" ON)
option(GOON_BUILD_TESTING "Should we enable testing with ctest" OFF)
set(BUILD_TESTING GOON_BUILD_TESTING) # Set the cmake variable build_testing to our option

# option(GOON_FULL_MACOS_BUILD "Full builds of all libraries, used for runners mostly, and passed in to override." OFF)

# #########################################
# Precompiled headers
# #########################################
if(GOON_BUILD_PCH)
    message(STATUS "Setting up PCH, mac needs objc")
    target_precompile_headers(${LIBRARY_TARGET_NAME} PRIVATE
        "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/GoonEngine/gnpch.h>")
endif(GOON_BUILD_PCH)

# #########################################
# Build Type specific setup
# #########################################
# If we are debug, we should use the local path, otherwise use the location of embuilder from the docker image
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE -DGN_DEBUG_BUILD)
else()
    target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE -DGN_RELEASE_BUILD)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# #########################################
# Packages
# #########################################
# Have both of these already built, and then we find them.
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

# These should be added via subdir if not FULL BUILD
message("Going to do the thing.. macos is ${MACOS} and GOON_FULL_MACOS_BUILD is ${GOON_FULL_MACOS_BUILD}")

if(MACOS AND NOT GOON_FULL_MACOS_BUILD)
    find_library(LIB_PNG
        REQUIRED
        PATH_SUFFIXES .a
        NAMES libpng png.a png.dll libpng.a
        HINTS /usr/local/lib /c/cmake/lib)

    find_library(LIB_FREETYPE
        REQUIRED
        PATH_SUFFIXES .a
        NAMES libfreetype.a libfreetype.dll freetype.a freetype.dll freetype libfreetype
        HINTS /usr/local/lib /c/cmake/lib)
else()
    if(NOT EMSCRIPTEN)
        add_subdirectory(./external/freetype)
        add_subdirectory(./external/libpng)
    endif(NOT EMSCRIPTEN)

    add_subdirectory(./external/sg_sound)
endif(MACOS AND NOT GOON_FULL_MACOS_BUILD)

# #########################################
# Platform Specific Build settings
# #########################################
message("Going to do the thing.. emscripten is is ${EMSCRIPTEN} and GOON_FULL_MACOS_BUILD is ${GOON_FULL_MACOS_BUILD}")

if(EMSCRIPTEN)
    set_target_properties(${EXECUTABLE_TARGET_NAME} PROPERTIES LINK_FLAGS ${LINK_FLAGS} " -O0 -g -s SAFE_HEAP=2 -s STACK_OVERFLOW_CHECK=1 -sUSE_LIBPNG=1 -sUSE_FREETYPE=1 --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s 'DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[\"$autoResumeAudioContext\",\"$dynCall\"]'")
endif(EMSCRIPTEN)

# #########################################
# Compile Definitions
# #########################################

# Platform-specific compile definitions
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE
    $<$<BOOL:${APPLE}>: -DGN_PLATFORM_MACOS>
    $<$<BOOL:${WIN32}>: -DGN_PLATFORM_WINDOWS>
    $<$<BOOL:${EMSCRIPTEN}>: -DGN_PLATFORM_EMSCRIPTEN>
    $<$<BOOL:${LINUX}>: -DGN_PLATFORM_LINUX>
)

# Other compile definitions
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE
    $<$<BOOL:${GOON_MULTITHREADED_SOUND}>: -DGN_MULTITHREADED>
    $<$<BOOL:${GOON_DEBUG_LUA}>: -DGN_DEBUG_LUA>
    $<$<BOOL:${GOON_BENCHMARKING}>: -DGN_BENCHMARKING_ENABLED>
)

# #########################################
# Debug
# #########################################
if(CMAKE_DEBUG_VARIABLES)
    message(STATUS "Exporting debug")
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)

    foreach(_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif(CMAKE_DEBUG_VARIABLES)

# #########################################
# Compile Options
# #########################################
set(FULL_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Wdeprecated
    -Wno-newline-eof
    -Wno-strict-prototypes
    -Wno-sign-compare
    -pthread
)

if(WIN32)
    set(FULL_COMPILE_OPTIONS /std:c99)
elseif(EMSCRIPTEN)
    target_compile_options(${EXECUTABLE_TARGET_NAME} PRIVATE -sUSE_LIBPNG=1 -sUSE_FREETYPE=1)
endif(WIN32)

target_compile_options(${LIBRARY_TARGET_NAME} PRIVATE ${FULL_COMPILE_OPTIONS})

# #########################################
# Linking Options
# #########################################
# set(LIBRARIES_TO_LINK
# SDL2::SDL2-static
# ZLIB::ZLIB
# freetype
# png
# supergoonSound::supergoonSound
# supergoonSound
# ${LIB_PNG}
# )

# foreach(LIBRARY ${LIBRARIES_TO_LINK})
# # Check if the target exists, if we are building it then we should link to it.
# if(TARGET ${LIBRARY})
# target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LIBRARY})
# endif()
# endforeach()
target_link_libraries(${LIBRARY_TARGET_NAME}
    PUBLIC
    supergoonSound
    freetype
    png
    ${LIB_PNG}
    ${LIB_FREETYPE}

    # testing
    SDL2::SDL2-static
    ${LIB_VORBISFILE}
    ${LIB_VORBIS}
    ${LIB_OGG}
    ${OGG_LIBRARIES}
    vorbis vorbisenc vorbisfile ogg
    PRIVATE
)
target_link_directories(${LIBRARY_TARGET_NAME}
    PRIVATE
    /usr/local/lib
)

target_include_directories(${LIBRARY_TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/libpng
    external/sg_sound/src/
    /usr/local/include

    # Freetype cause yeah
    /usr/local/include/freetype2) # Add the freetype2 specific include directory