#include <GoonEngine/gnpch.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/SdlWindow.h>
#include <glad/glad.h>

#include <cglm/mat4.h>
#include <cglm/cam.h>

SDL_Window *g_pWindow = 0;
// SDL_Renderer *g_pRenderer = 0;
SDL_GLContext *g_pContext = 0;
int g_refreshRate = 60;
int scaleX = 0;
int scaleY = 0;

mat4 projection;

/**
 * @brief
 * Lua params: string name, int window width, int window height
 *
 * @param L
 * @return int
 */
// int geInitializeRenderingWindow(uint width, uint height, const char* windowName)
int geInitializeRenderingWindow(unsigned int windowWidth, unsigned int windowHeight, unsigned int gameWidth, unsigned int gameHeight, const char *windowName)
{
#ifdef __EMSCRIPTEN__
    // Use opengles2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
    // Use opengl
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#endif
    g_pWindow = SDL_CreateWindow(windowName,
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 windowWidth,
                                 windowHeight,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    //  SDL_WINDOW_SHOWN);
    if (g_pWindow == NULL)
    {
        fprintf(stderr, "Window could not be created, Error: %s", SDL_GetError());
        return 0;
    }
    g_pContext = SDL_GL_CreateContext(g_pWindow);
    if (g_pContext == NULL)
    {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return 0;
    }

#ifdef __EMSCRIPTEN__
    if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        puts("Failed to initialize OpenGLES functions with glad!");
    }
#else
    // Use opengl
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        puts("Failed to initialize OpenGL functions with glad!");
    }
#endif

    LogDebug("Created window and opengl context\nWidth: %d, Height: %d", windowWidth, windowHeight);
    SDL_DisplayMode mode;
    SDL_GetWindowDisplayMode(g_pWindow, &mode);
    SDL_GL_MakeCurrent(g_pWindow, g_pContext);
    // blending?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // g_refreshRate = 30;
    // g_refreshRate = 120;
    // g_refreshRate = 60;
    g_refreshRate = mode.refresh_rate ? mode.refresh_rate : 60;
    LogWarn("Refresh rate is set to %d", g_refreshRate);
    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    mat4 proj;
    // setup screen camera
    glm_ortho(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f, proj);

    // SDL_RenderSetIntegerScale(g_pRenderer, SDL_TRUE);
    // SDL_RenderSetLogicalSize(g_pRenderer, gameWidth, gameHeight);
    return 0;
}

SDL_Renderer *GetGlobalRenderer()
{
    // return g_pRenderer;
    return NULL;
}
SDL_Window *GetGlobalWindow()
{
    return g_pWindow;
}
