// #include <stdlib.h>
// #include <stdio.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/Sprite.h>
#include <GoonEngine/Shader.h>
#include <GoonEngine/Texture2D.h>
#include <GoonEngine/Camera.h>
#include <cglm/mat4.h>
#include <cglm/call.h>

extern unsigned int USE_GL_ES;

// Each vertex has this many attributes in it, vec2 pos vec2 texture
#define NUM_VERTICES_PER_QUAD 6
#define NUM_COMPONENTS_PER_VERTEX 13
#define NUM_TEXTURE_SLOTS 16

// Vec4 - pos/texpos
//  float imagenum
// vec4 color
// vec4 source tex rect
#define BASE_VERTICES                                          \
    {                                                          \
        0.0f, 1.0f, 0.0f, 1.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0,     \
            1.0f, 0.0f, 1.0f, 0.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0, \
            0.0f, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0, \
                                                               \
            0.0f, 1.0f, 0.0f, 1.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0, \
            1.0f, 1.0f, 1.0f, 1.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0, \
            1.0f, 0.0f, 1.0f, 0.0f, 0, 1, 1, 1, 1, 0, 0, 0, 0, \
    }

int TEXTURES[NUM_TEXTURE_SLOTS] = {0};
static unsigned int _currentNumUsedTextureSlots = 0;

static void initRenderData(geSpriteRenderer *sprite)
{
    sprite->VBO = 0;
    float vertices[] = BASE_VERTICES;

    // ES cannot use VAO
    if (!USE_GL_ES)
    {
        glGenVertexArrays(1, &sprite->quadVAO);
        glBindVertexArray(sprite->quadVAO);
    }
    glGenBuffers(1, &sprite->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    if (!USE_GL_ES)
    {
        // Pos / tex pos vec4
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)0);
        // Texture num, float
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)(4 * sizeof(float)));
        // Color, vec4
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)(5 * sizeof(float)));
        // Texture Source rect, vec4
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)(9 * sizeof(float)));

        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static unsigned int getBindTexture(unsigned int loadedTextureId, geShader *shader)
{
    if (_currentNumUsedTextureSlots >= NUM_TEXTURE_SLOTS)
    {
        LogError("Too many loaded textures, please fix this logic");
    }
    for (size_t i = 0; i < _currentNumUsedTextureSlots; i++)
    {
        if (TEXTURES[i] == loadedTextureId)
        {
            return i;
        }
    }
    TEXTURES[_currentNumUsedTextureSlots] = loadedTextureId;
    // Bind to gl
    glActiveTexture(GL_TEXTURE0 + _currentNumUsedTextureSlots);
    glBindTexture(GL_TEXTURE_2D, loadedTextureId);
    // Add in the texture
    char uniformName[20];
    sprintf(uniformName, "images[%d]", _currentNumUsedTextureSlots);
    geShaderSetInteger(shader, uniformName, _currentNumUsedTextureSlots, true);
    // Setup next
    ++_currentNumUsedTextureSlots;
    return _currentNumUsedTextureSlots - 1;
}

geSpriteRenderer *geSpriteRendererNew(geShader *shader)
{
    geSpriteRenderer *sprite = calloc(1, sizeof(*sprite));
    sprite->shader = shader;
    initRenderData(sprite);
    return sprite;
}

void geSpriteRendererStart(geSpriteRenderer *sprite, geCamera *camera)
{
    geShaderSetViewUniform(sprite->shader, camera);
}

void geSpriteRendererDraw(geSpriteRenderer *sprite,
                          geTexture2D *texture,
                          vec2 pos,
                          vec2 size,
                          float rotate,
                          vec4 color,
                          vec2 texOffset,
                          vec2 texSize,
                          int flipHorizontal)
{

    // prepare transformations, this is unique per call and must be done, handles rotation, scaling, pos.
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
    // Calculate texture offset in normal
    texOffset[0] /= texture->Width;
    texOffset[1] /= texture->Height;
    texSize[0] /= texture->Width;
    texSize[1] /= texture->Height;
    // Get or load texture into slot
    unsigned int imageNum = getBindTexture(texture->ID, sprite->shader);
    // Send data to spritebatch
    glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);
    float verts[] = BASE_VERTICES;
    // Update information inside of vertices prior to send
    for (size_t i = 0; i < NUM_VERTICES_PER_QUAD; i++)
    {
        if (flipHorizontal)
        {
            verts[(i * NUM_COMPONENTS_PER_VERTEX) + 0] = 1.0 - verts[(i * NUM_COMPONENTS_PER_VERTEX) + 0];
        }
        vec4 oldpos = {verts[(i * NUM_COMPONENTS_PER_VERTEX) + 0], verts[(i * NUM_COMPONENTS_PER_VERTEX) + 1], 0.0, 1.0};
        vec4 newpos = {0, 0, 0, 0};
        glm_mat4_mulv(model, oldpos, newpos);
        int offset = 0;
        // Update the pos and tex coords
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = newpos[0];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = newpos[1];
        // Don't need to update texture coords just yet, so skip and leave placeholder
        offset += 2;
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + 2] = color[2];
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + 3] = color[3];
        // Update the image num
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = imageNum;
        // Update the color
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = color[0];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = color[1];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = color[2];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = color[3];
        // Update texture source rect
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[0];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[1];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texSize[0];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texSize[1];
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), (void *)verts);
    // This probably should only happen at end.
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
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_PER_QUAD);
    if (!USE_GL_ES)
    {
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
