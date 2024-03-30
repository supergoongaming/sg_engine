#pragma once

#include <stdbool.h>
#include <glad/glad.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // General purpose shader object. Compiles from file, generates
    // compile/link-time error messages and hosts several utility
    // functions for easy management.
    typedef struct geShader
    {
        unsigned int ID;
    } geShader;

    geShader *geShaderNew();
    void geShaderFree(geShader *shader);
    geShader *geShaderUse(geShader *shader);
    void geShaderCompile(geShader *shader, const char *vertexSource, const char *fragmentSource, const char *geometrySource);

    // utility functions
    // void SetFloat(const char *name, float value, bool useShader = false);
    // void SetInteger(const char *name, int value, bool useShader = false);
    void geShaderSetInteger(geShader *shader, const char *name, int value, bool useShader);
    void geShaderSetVector2f(geShader *shader, const char *name, float x, float y, bool useShader);
    // void SetVector2f(const char *name, const glm::vec2 &value, bool useShader = false);
    // void SetVector3f(const char *name, float x, float y, float z, bool useShader = false);
    void geShaderSetVector3f(geShader *shader, const char *name, vec3 value, bool useShader);
    // void SetVector4f(const char *name, float x, float y, float z, float w, bool useShader = false);
    // void SetVector4f(const char *name, const glm::vec4 &value, bool useShader = false);
    void geShaderSetMatrix4(geShader *shader, const char *name, mat4 *matrix, bool useShader);

#ifdef __cplusplus
}
#endif