#include <cglm/vec2.h>
#include <cglm/vec3.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct geShader geShader;
    typedef struct geTexture2D geTexture2D;
    typedef struct geSpriteRenderer
    {
        geShader *shader;
        unsigned int quadVAO; // Needed for gl
        unsigned int VBO; //Needed for gles

    } geSpriteRenderer;
    geSpriteRenderer *geSpriteRendererNew(geShader *shader);
    void geSpriteRendererDraw(geSpriteRenderer *sprite,
                              geTexture2D *texture,
                              vec2 pos,
                              vec2 size,
                              float rotate,
                              vec3 color);
#ifdef __cplusplus
}
#endif