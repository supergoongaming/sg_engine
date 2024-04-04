#include <cglm/vec2.h>
#include <cglm/vec3.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct geShader geShader;
    typedef struct geTexture2D geTexture2D;
    typedef struct geCamera geCamera;
    typedef struct geRectangle geRectangle;

    typedef struct geTileSheet
    {
        geShader *shader;
        unsigned int quadVAO; // Needed for gl
        unsigned int VBO;     // Needed for gles

    } geTileSheet;

    geTileSheet *geTileSheetNew();
    void geTileSheetFree(geTileSheet tilesheet);
    void geTileSheetAddTile(geTileSheet *tilesheet,
                            geRectangle *dstRect,
                            geRectangle *srcRect,
                            geTexture2D *texture);
    void geTileSheetBufferData(geTileSheet *tilesheet);
    void geTileSheetDraw(geTileSheet *tilesheet);
#ifdef __cplusplus
}
#endif