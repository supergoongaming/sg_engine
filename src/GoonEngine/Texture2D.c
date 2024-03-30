#include <stdlib.h>
#include <glad/glad.h>
#include <png.h>
#include <GoonEngine/Texture2D.h>
#include <stb/image.h>

static void flipImageVertically(unsigned char *imageData, int width, int height, int channels)
{
    unsigned char *tempRow = (unsigned char *)malloc(width * channels);
    if (!tempRow)
    {
        fprintf(stderr, "Error: Unable to allocate memory for temporary row\n");
        return;
    }

    int rowSize = width * channels;
    for (int y = 0; y < height / 2; y++)
    {
        int topRowIndex = y * rowSize;
        int bottomRowIndex = (height - y - 1) * rowSize;

        // Swap rows
        memcpy(tempRow, imageData + topRowIndex, rowSize);
        memcpy(imageData + topRowIndex, imageData + bottomRowIndex, rowSize);
        memcpy(imageData + bottomRowIndex, tempRow, rowSize);
    }

    free(tempRow);
}

static unsigned char *loadPNG(const char *filename, int *widthp, int *heightp, int *channels)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }

    // Check if the file is a PNG file
    unsigned char header[8];
    fread(header, 1, 8, file);
    if (png_sig_cmp(header, 0, 8))
    {
        fclose(file);
        fprintf(stderr, "Error: %s is not a PNG file\n", filename);
        return NULL;
    }

    // Initialize libpng structures
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(file);
        fprintf(stderr, "Error: Unable to create png_struct\n");
        return NULL;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(file);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fprintf(stderr, "Error: Unable to create png_info\n");
        return NULL;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(file);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fprintf(stderr, "Error: Error during png read\n");
        return NULL;
    }

    // Initialize PNG IO
    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);

    // Read PNG info
    png_read_info(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    switch (color_type)
    {
    case PNG_COLOR_TYPE_RGBA:
        *channels = 4;
        break;
    case PNG_COLOR_TYPE_RGB:
        *channels = 3;
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        *channels = 2;
        break;
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_PALETTE:
        *channels = 1;
        break;
    default:
        // Unsupported color type
        fclose(file);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fprintf(stderr, "Error: Unsupported color type\n");
        return NULL;
    }

    // Allocate memory for image data
    unsigned char *imageData = (unsigned char *)malloc(width * height * 4);
    if (!imageData)
    {
        fclose(file);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fprintf(stderr, "Error: Unable to allocate memory for image data\n");
        return NULL;
    }

    // Read the image data
    png_bytep row_pointers[height];
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
        if (!row_pointers[y])
        {
            // Handle memory allocation failure
            fclose(file);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fprintf(stderr, "Error: Unable to allocate memory for row pointers\n");
            free(imageData);
            return NULL;
        }
    }

    // Read the image row by row
    png_read_image(png_ptr, row_pointers);

    // Copy image data from row_pointers to imageData in RGBA format
    for (int y = 0; y < height; y++)
    {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++)
        {
            png_bytep px = &(row[x * 4]);
            int destIndex = (y * width + x) * 4;
            imageData[destIndex] = px[0];     // Red
            imageData[destIndex + 1] = px[1]; // Green
            imageData[destIndex + 2] = px[2]; // Blue
            imageData[destIndex + 3] = px[3]; // Alpha
        }
        free(row_pointers[y]);
    }

    // Clean up libpng structures
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(file);

    // Set returns
    *widthp = width;
    *heightp = height;
    return imageData;
}

geTexture2D *geTexture2DNew()
{
    geTexture2D *texture = calloc(1, sizeof(*texture));
    // Only using pngs for now, so always set RGBA
    texture->Internal_Format = GL_RGBA;
    texture->Image_Format = GL_RGBA;
    // For non power of 2 textures in opengles, we need to use clamp to edge and not repeat.  Also can't use minmaps. probably look into using pot
    texture->Wrap_S = GL_CLAMP_TO_EDGE;
    texture->Wrap_T = GL_CLAMP_TO_EDGE;
    // We want it to look classic, so use nearest and not linear
    texture->Filter_Min = GL_NEAREST;
    texture->Filter_Max = GL_NEAREST;
    glGenTextures(1, &texture->ID);
    return texture;
}

void geTexture2DFree(geTexture2D *texture)
{
    glDeleteTextures(1, &texture->ID);
}

void geTexture2DGenerate(geTexture2D *texture, const char *filename)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char *data = loadPNG(filename, &width, &height, &channels);
    flipImageVertically(data, width, height, channels);
    // If we want to test with stb.
    // stbi_set_flip_vertically_on_load(1);
    // unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    texture->Width = width;
    texture->Height = height;
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->Internal_Format, width, height, 0, texture->Image_Format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->Filter_Max);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void geTexture2DBind(geTexture2D *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}
