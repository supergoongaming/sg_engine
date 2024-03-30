#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct geTexture2D
    {
        // holds the ID of the texture object, used for all texture operations to reference to this particular texture
        unsigned int ID;
        // texture image dimensions
        unsigned int Width, Height; // width and height of loaded image in pixels
        // texture Format
        unsigned int Internal_Format; // format of texture object
        unsigned int Image_Format;    // format of loaded image
        // texture configuration
        unsigned int Wrap_S;     // wrapping mode on S axis
        unsigned int Wrap_T;     // wrapping mode on T axis
        unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
        unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels

    } geTexture2D;

    geTexture2D *geTexture2DNew();
    void geTexture2DFree(geTexture2D *texture);
    void geTexture2DGenerate(geTexture2D *texture, const char *filename);
    void geTexture2DBind(geTexture2D *texture);
#ifdef __cplusplus
}
#endif