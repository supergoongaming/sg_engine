#include <GoonEngine/Sprite.h>
#include <GoonEngine/Shader.h>
#include <GoonEngine/Texture2D.h>
#include <GoonEngine/Camera.h>
#include <cglm/mat4.h>
#include <cglm/call.h>

extern unsigned int USE_GL_ES;

// Each vertex has this many attributes in it, vec2 pos vec2 texture
const int ATTRIBUTE_SIZE = 4;

static void initRenderData(geSpriteRenderer *sprite)
{
    sprite->VBO = 0;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f};

    // ES cannot use VAO
    if (!USE_GL_ES)
    {
        glGenVertexArrays(1, &sprite->quadVAO);
        glBindVertexArray(sprite->quadVAO);
    }
    glGenBuffers(1, &sprite->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    if (!USE_GL_ES)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, ATTRIBUTE_SIZE * sizeof(float), (void *)0);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

geSpriteRenderer *geSpriteRendererNew(geShader *shader)
{
    geSpriteRenderer *sprite = calloc(1, sizeof(*sprite));
    sprite->shader = shader;
    initRenderData(sprite);
    return sprite;
}

void geSpriteRendererDraw(geSpriteRenderer *sprite,
                          geTexture2D *texture,
                          vec2 pos,
                          vec2 size,
                          float rotate,
                          vec3 color,
                          vec2 texOffset,
                          vec2 texSize,
                          int flipHorizontal,
                          geCamera *camera)
{

    // prepare transformations
    geShaderUse(sprite->shader);
    mat4 model;
    glm_mat4_identity(model);
    // Translate to the position
    vec3 position = {pos[0], pos[1], 0.0f};
    glm_translate(model, position);
    // Translate so we can rotate if we want to around the origin instead of the top left
    vec3 centerTranslation = {0.5f * size[0], 0.5f * size[1], 0.0f};
    glm_translate(model, centerTranslation);
    // Rotate around z-axis
    glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 0.0f, 1.0f});
    // Move back to position after rotation
    vec3 inverseCenterTranslation = {-0.5f * size[0], -0.5f * size[1], 0.0f};
    glm_translate(model, inverseCenterTranslation);
    // scale image
    vec3 scaleVec = {size[0], size[1], 1.0f};
    glm_scale(model, scaleVec);
    // Set the shader model and sprite color
    geShaderSetMatrix4(sprite->shader, "model", &model, false);
    geShaderSetMatrix4(sprite->shader, "view", &camera->CameraMatrix, false);
    geShaderSetVector3f(sprite->shader, "spriteColor", color, false);

    texOffset[0] /= texture->Width;
    texOffset[1] /= texture->Height;
    texSize[0] /= texture->Width;
    texSize[1] /= texture->Height;

    // Set texture offset and size
    geShaderSetVector2f(sprite->shader, "texOffset", texOffset[0], texOffset[1], true);
    geShaderSetVector2f(sprite->shader, "texSize", texSize[0], texSize[1], true);
    geShaderSetInteger(sprite->shader, "flipHorizontal", flipHorizontal, true);

    glActiveTexture(GL_TEXTURE0);
    geTexture2DBind(texture);
    if (!USE_GL_ES)
    {
        glBindVertexArray(sprite->quadVAO);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if (!USE_GL_ES)
    {
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
