#include <stdlib.h>
#include <GoonEngine/TileSheet.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/Shader.h>
#include <GoonEngine/Texture2D.h>
#include <GoonEngine/Camera.h>
#include <GoonEngine/rectangle.h>
#include <cglm/mat4.h>
#include <cglm/call.h>

extern unsigned int USE_GL_ES;

// // Each vertex has this many attributes in it, vec2 pos vec2 texture
#define NUM_VERTICES_PER_QUAD 6
#define NUM_COMPONENTS_PER_VERTEX 13
#define NUM_TEXTURE_SLOTS 16
#define NUM_QUADS_PER_DRAW 1000

static float *_bufferedVertices = NULL;
static int _currentBufferVerticesSize = 0;
static int _currentNumTileVertices = 0;

extern geShader *tileShader;

// // Vec4 - pos/texpos
// //  float imagenum
// // vec4 color
// // vec4 source tex rect
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

static int TILE_TEXTURES[NUM_TEXTURE_SLOTS] = {0};
static unsigned int _currentNumUsedTileTextureSlots = 0;
static unsigned int _currentNumTileQuadsDrawn = 0;

static unsigned int getBindTexture(unsigned int loadedTextureId, geShader *shader)
{
    geShaderUse(shader);
    if (_currentNumUsedTileTextureSlots >= NUM_TEXTURE_SLOTS)
    {
        LogError("Too many loaded textures, please fix this logic");
    }
    for (size_t i = 0; i < _currentNumUsedTileTextureSlots; i++)
    {
        if (TILE_TEXTURES[i] == loadedTextureId)
        {
            return i;
        }
    }
    TILE_TEXTURES[_currentNumUsedTileTextureSlots] = loadedTextureId;
    // Bind to gl
    glActiveTexture(GL_TEXTURE0 + _currentNumUsedTileTextureSlots);
    glBindTexture(GL_TEXTURE_2D, loadedTextureId);
    // Add in the texture
    char uniformName[20];
    sprintf(uniformName, "timages[%d]", _currentNumUsedTileTextureSlots);
    geShaderSetInteger(shader, uniformName, _currentNumUsedTileTextureSlots, true);
    // Setup next
    ++_currentNumUsedTileTextureSlots;
    return _currentNumUsedTileTextureSlots - 1;
}

static void setTexturesForDraw(geTileSheet *tilesheet)
{
    geShaderUse(tilesheet->shader);
    for (size_t i = 0; i < _currentNumUsedTileTextureSlots; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, TILE_TEXTURES[i]);
        // Add in the texture
        char uniformName[20];
        // sprintf(uniformName, "timages[%d]", _currentNumUsedTileTextureSlots);
        sprintf(uniformName, "timages[%d]", i);
        // LogWarn("Setting the uniform %s on shader num %d", uniformName, tilesheet->shader->ID);
        geShaderSetInteger(tilesheet->shader, uniformName, _currentNumUsedTileTextureSlots, true);
    }
}
static void addVerts(float *verts, int num)
{
    if (_currentNumTileVertices + num >= _currentBufferVerticesSize)
    {
        LogWarn("Need to increase size, realloc");
        return;
    }
    memcpy(&_bufferedVertices[_currentNumTileVertices], (void *)verts, num * sizeof(float));
}

static void addVerticesToBuffer(geTileSheet *tilesheet, mat4 model, int imageNum, vec4 texOffset)
{
    //     // Send data to spritebatch
    glBindBuffer(GL_ARRAY_BUFFER, tilesheet->VBO);
    float verts[] = BASE_VERTICES;
    // Update information inside of vertices prior to send
    for (size_t i = 0; i < NUM_VERTICES_PER_QUAD; i++)
    {
        // if (flipHorizontal)
        // {
        //     verts[(i * NUM_COMPONENTS_PER_VERTEX) + 0] = 1.0 - verts[(i * NUM_COMPONENTS_PER_VERTEX) + 0];
        // }
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
        // Update the color, skip for now
        offset += 4;
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = 1;
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = 1;
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = 1;
        // verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = 1;
        // Update texture source rect
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[0];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[1];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[2];
        verts[(i * NUM_COMPONENTS_PER_VERTEX) + offset++] = texOffset[3];
    }

    // Need to add to buffer data, not sub data.
    int numFloats = sizeof(verts) / sizeof(float);
    addVerts(verts, numFloats);

    _currentNumTileVertices += numFloats;
    ++_currentNumTileQuadsDrawn;
}

geTileSheet *geTileSheetNew()
{

    geTileSheet *tilesheet = calloc(1, sizeof(*tilesheet));
    // Start the buffer we will use.
    _currentBufferVerticesSize = 500000;
    _currentNumTileVertices = 0;
    // geShader* shad = geShaderNew();
    // This is compiled in main.cpp for now
    // const char *vertexShaderFile = USE_GL_ES ? "assets/shaders/vertex_es.vs" : "assets/shaders/v_tile.vs";
    // const char *fragmentShaderFile = USE_GL_ES ? "assets/shaders/tfragment_es.vs" : "assets/shaders/fragment.vs";
    // geShaderCompile(shad, vertexShaderFile, fragmentShaderFile, NULL);
    tilesheet->shader = tileShader;
    _bufferedVertices = calloc(_currentBufferVerticesSize, sizeof(float));
    // initRenderData(sprite);
    return tilesheet;
}

void geTileSheetBufferData(geTileSheet *tilesheet)
{
    tilesheet->VBO = 0;
    // ES cannot use VAO
    if (!USE_GL_ES)
    {
        glGenVertexArrays(1, &tilesheet->quadVAO);
        glBindVertexArray(tilesheet->quadVAO);
    }
    glGenBuffers(1, &tilesheet->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, tilesheet->VBO);
    int bufferSize = sizeof(float) * _currentNumTileVertices;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, _bufferedVertices, GL_STATIC_DRAW);
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
    // Clear our buffered vertices after we buffer it.
    free(_bufferedVertices);
}

void geTileSheetFree(geTileSheet tilesheet)
{
}

void geTileSheetAddTile(geTileSheet *tilesheet,
                        geRectangle *dstRect,
                        geRectangle *srcRect,
                        geTexture2D *texture)
{
    // prepare transformations, this is unique per call and must be done, handles rotation, scaling, pos.
    geShaderUse(tilesheet->shader);
    mat4 model;
    glm_mat4_identity(model);
    // Translate to the position
    vec3 position = {dstRect->x, dstRect->y, 0.0f};
    glm_translate(model, position);
    // Translate so we can rotate if we want to around the origin instead of the top left
    vec3 centerTranslation = {0.5f * dstRect->w, 0.5f * dstRect->h, 0.0f};
    glm_translate(model, centerTranslation);
    // Rotate around z-axis
    glm_rotate(model, glm_rad(0), (vec3){0.0f, 0.0f, 1.0f});
    // Move back to position after rotation
    vec3 inverseCenterTranslation = {-0.5f * dstRect->w, -0.5f * dstRect->h, 0.0f};
    glm_translate(model, inverseCenterTranslation);
    // scale image
    vec3 scaleVec = {dstRect->w, dstRect->h, 1.0f};
    glm_scale(model, scaleVec);
    // Calculate texture offset in normal
    vec4 texOffset = {
        srcRect->x / texture->Width,
        srcRect->y / texture->Height,
        srcRect->w / texture->Width,
        srcRect->h / texture->Height,
    };
    // Get or load texture into slot
    unsigned int imageNum = getBindTexture(texture->ID, tilesheet->shader);

    addVerticesToBuffer(tilesheet, model, imageNum, texOffset);
}

void geTileSheetDraw(geTileSheet *tilesheet)
{
    geShaderUse(tilesheet->shader);
    setTexturesForDraw(tilesheet);

    if (!USE_GL_ES)
    {
        glBindVertexArray(tilesheet->quadVAO);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, tilesheet->VBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    }
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_PER_QUAD * _currentNumTileQuadsDrawn);
    if (!USE_GL_ES)
    {
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
