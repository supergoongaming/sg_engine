#include <cglm/vec2.h>
#include <cglm/vec3.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct geShader geShader;
    typedef struct geTexture2D geTexture2D;
    typedef struct geCamera geCamera;
    typedef struct geSpriteRenderer
    {
        geShader *shader;
        unsigned int quadVAO; // Needed for gl
        unsigned int VBO;     // Needed for gles

    } geSpriteRenderer;

    geSpriteRenderer *geSpriteRendererNew(geShader *shader);
    void geSpriteRendererStart(geSpriteRenderer *sprite, geCamera *camera);
    void geSpriteRendererDraw(geSpriteRenderer *sprite,
                              geTexture2D *texture,
                              vec2 pos,
                              vec2 size,
                              float rotate,
                              vec4 color,
                              vec2 texOffset,
                              vec2 texSize,
                              int flipHorizontal);
    void geSpriteRendererEnd(geSpriteRenderer *sprite);
#ifdef __cplusplus
}
#endif