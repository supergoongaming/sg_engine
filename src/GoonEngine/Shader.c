#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GoonEngine/Shader.h>

geShader *geShaderNew()
{
    geShader *shader = calloc(1, sizeof(*shader));
    return shader;
}

void geShaderFree(geShader *shader)
{
    glDeleteProgram(shader->ID);
    free(shader);
}

static void checkCompileErrors(geShader *shader, unsigned int object, const char *type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            printf("| ERROR::SHADER: Compile-time error: Type: %s\n%s\n-- ----------- -- \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            printf("| ERROR::Shader: Link-time error: Type: %s\n%s\n", type, infoLog);
        }
    }
}
static char *readShaderFromFile(const char *filename)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(filename, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
        buffer[length] = '\0';
    }
    return buffer;
}

geShader *geShaderUse(geShader *shader)
{
    glUseProgram(shader->ID);
    return shader;
}

void geShaderCompile(geShader *shader, const char *vertexFilename, const char *fragmentFilename, const char *geometryFilename)
{
    unsigned int sVertex, sFragment, gShader;
    char *vertexSource = readShaderFromFile(vertexFilename);
    char *fragmentSource = readShaderFromFile(fragmentFilename);
    // char *geometrySource = readShaderFromFile(geometryFilename);
    char *geometrySource = NULL;

    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(shader, sVertex, "VERTEX");
    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(shader, sFragment, "FRAGMENT");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != NULL)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(shader, gShader, "GEOMETRY");
    }
    // shader program
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, sVertex);
    glAttachShader(shader->ID, sFragment);
    if (geometrySource != NULL)
        glAttachShader(shader->ID, gShader);
    glLinkProgram(shader->ID);
    checkCompileErrors(shader, shader->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != NULL)
        glDeleteShader(gShader);
}
void geShaderSetVector3f(geShader *shader, const char *name, vec3 value, bool useShader)
{
    if (useShader)
        geShaderUse(shader);
    glUniform3f(glGetUniformLocation(shader->ID, name), value[0], value[1], value[2]);
}

void geShaderSetMatrix4(geShader *shader, const char *name, mat4 *matrix, bool useShader)
{
    if (useShader)
        geShaderUse(shader);
    // glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, false, glm::value_ptr(matrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float *)matrix);
}

void geShaderSetInteger(geShader *shader, const char *name, int value, bool useShader)
{
    if (useShader)
        geShaderUse(shader);
    // glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, false, glm::value_ptr(matrix));
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

// void Shader::SetFloat(const char *name, float value, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform1f(glGetUniformLocation(this->ID, name), value);
// }
// void Shader::SetInteger(const char *name, int value, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform1i(glGetUniformLocation(this->ID, name), value);
// }
// void Shader::SetVector2f(const char *name, float x, float y, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform2f(glGetUniformLocation(this->ID, name), x, y);
// }
// void Shader::SetVector2f(const char *name, const glm::vec2 &value, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
// }
// void Shader::SetVector3f(const char *name, float x, float y, float z, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
// }
// void Shader::SetVector3f(const char *name, const glm::vec3 &value, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
// }
// void Shader::SetVector4f(const char *name, float x, float y, float z, float w, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
// }
// void Shader::SetVector4f(const char *name, const glm::vec4 &value, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
// }
// void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader)
// {
//     if (useShader)
//         this->Use();
//     glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
// }

// void Shader::checkCompileErrors(unsigned int object, std::string type)
// {
// }