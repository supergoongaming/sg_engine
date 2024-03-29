#include <stdlib.h>
#include <glad/glad.h>
#include <png.h>
#include <GoonEngine/Texture2D.h>

static unsigned char *loadPNG(const char *filename, int *widthp, int *heightp)
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

    // Read any color_type to 8-bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Allocate memory for image data
    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    // Read the image data
    png_read_image(png_ptr, row_pointers);
    unsigned char *imageData = (unsigned char *)malloc(width * height * 4);

    // Copy image data from row_pointers to imageData in RGBA format
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Each pixel in row_pointers consists of RGB (and optionally alpha) channels
            // You need to convert it to RGBA format
            int srcIndex = y * width * 3 + x * 3;  // Index in row_pointers
            int destIndex = y * width * 4 + x * 4; // Index in imageData

            // Copy RGB channels
            imageData[destIndex] = row_pointers[y][srcIndex];         // Red
            imageData[destIndex + 1] = row_pointers[y][srcIndex + 1]; // Green
            imageData[destIndex + 2] = row_pointers[y][srcIndex + 2]; // Blue

            // Set alpha channel to 255 (fully opaque)
            imageData[destIndex + 3] = 255;
        }
    }

    // // Free libpng allocated memory
    for (int y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }
    free(row_pointers);

    // // Clean up libpng structures
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
    texture->Internal_Format = GL_RGB;
    texture->Wrap_S = GL_REPEAT;
    texture->Filter_Min = GL_LINEAR;
    texture->Filter_Max = GL_LINEAR;
    glGenTextures(1, &texture->ID);
}

void geTexture2DFree(geTexture2D *texture)
{
    glDeleteTextures(1, &texture->ID);
}

void geTexture2DGenerate(geTexture2D *texture, const char *filename)
{
    int width = 0;
    int height = 0;
    unsigned char *data = loadPNG(filename, &width, &height);
    texture->Width = width;
    texture->Height = height;
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->Internal_Format, width, height, 0, texture->Image_Format, GL_UNSIGNED_BYTE, data);
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
