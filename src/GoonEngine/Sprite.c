#include <GoonEngine/Sprite.h>
#include <GoonEngine/Shader.h>
#include <GoonEngine/Texture2D.h>
#include <cglm/mat4.h>
#include <cglm/call.h>

extern unsigned int USE_GL_ES;

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
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
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
                          vec3 color)
{

    // prepare transformations
    geShaderUse(sprite->shader);
    // mat4 model = glm::mat4(1.0f);
    mat4 model;
    glm_mat4_identity(model);
    // Translate to the position
    vec3 position = {pos[0], pos[1], 0.0f}; // Assuming x and y are the position coordinates
    glm_translate(model, position);
    // Translate so we can rotate if we want to around the origin.
    vec3 centerTranslation = {0.5f * size[0], 0.5f * size[1], 0.0f};
    glm_translate(model, centerTranslation);
    // Rotate around z-axis
    glm_rotate(model, glm_rad(rotate), (vec3){0.0f, 0.0f, 1.0f});
    // Move back to position after rotation
    vec3 inverseCenterTranslation = {-0.5f * size[0], -0.5f * size[1], 0.0f};
    glm_translate(model, inverseCenterTranslation);
    // scale image
    vec3 scaleVec = {size[0], size[1], 1.0f}; // Assuming z-scale is 1.0
    glm_scale(model, scaleVec);
    // Set the shader model and sprite color
    geShaderSetMatrix4(sprite->shader, "model", &model, false);
    geShaderSetVector3f(sprite->shader, "spriteColor", color, false);

    if (!USE_GL_ES)
    {
        glActiveTexture(GL_TEXTURE0);
        geTexture2DBind(texture);
        glBindVertexArray(sprite->quadVAO);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        geTexture2DBind(texture);
        glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    }
    // Draw arrays is what is causing that issue.
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // Don't unbind currently, vao not allowed in es
    // glBindVertexArray(0);
}
