/** 
 * Copyright (C) 2018 Tomasz Gałaj
 **/

#include <stb_image.h>

#include "Texture.h"
#include <iostream>
#include <helpers/RootDir.h>

Texture::Texture()
        : use_linear(true), to_id(0) {
}

Texture::~Texture() {
    if (to_id != 0) {
        glDeleteTextures(1, &to_id);
        to_id = 0;
    }
}

bool Texture::load(const std::string &file_name) {
    if (file_name.empty()) {
        return false;
    }

    bool is_loaded = false;
    int width, height, components;

    const std::string &dds = "dds";
    //文件为dds图片文件
    if (file_name.rfind(dds) == (file_name.length() - dds.length()) ? 1 : 0) {
        loadDds((ROOT_DIR + file_name).c_str());
    } else {
        unsigned char *pixels = stbi_load((ROOT_DIR + file_name).c_str(), &width, &height, &components, 4);

        if (pixels != nullptr) {
            glGenTextures(1, &to_id);
            glBindTexture(GL_TEXTURE_2D, to_id);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
            glTexSubImage2D(GL_TEXTURE_2D, 0 /* mip map level */, 0 /* xoffset */, 0 /* yoffset */, width, height,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            std::cout << "Could not load file " << file_name << std::endl;
        }

        stbi_image_free(pixels);

    }


    return is_loaded;
}

void Texture::loadDds(const char *imagePath) {
    // lay out variables to be used
    unsigned char *header;

    unsigned int width;
    unsigned int height;
    unsigned int mipMapCount;

    unsigned int blockSize;
    unsigned int format;

    unsigned int w;
    unsigned int h;

    unsigned char *buffer = 0;

    // open the DDS file for binary reading and get file size
    FILE *f;
    if ((f = fopen(imagePath, "rb")) == 0)
        return;
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
    // read in 128 bytes from the file
    header = static_cast<unsigned char *>(malloc(128));
    fread(header, 1, 128, f);

    // compare the `DDS ` signature
    if (memcmp(header, "DDS ", 4) != 0)
        return;

    // extract height, width, and amount of mipmaps - yes it is stored height then width
    height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
    width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
    mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

    // figure out what format to use for what fourCC file type it is
    // block size is about physical chunk storage of compressed data in file (important)
    if (header[84] == 'D') {
        switch (header[87]) {
            case '1': // DXT1
                format = GL_VIEW_CLASS_S3TC_DXT1_RGBA;
                blockSize = 8;
                break;
            case '3': // DXT3
                format = GL_VIEW_CLASS_S3TC_DXT3_RGBA;
                blockSize = 16;
                break;
            case '5': // DXT5
                format = GL_VIEW_CLASS_S3TC_DXT5_RGBA;
                blockSize = 16;
                break;
            case '0': // DX10
                // unsupported, else will error
                // as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
                // so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
            default:
                return;
        }
    } else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
        return;

    // allocate new unsigned char space with file_size - (file_code + header_size) magnitude
    // read rest of file
    buffer = static_cast<unsigned char *>(malloc(file_size - 128));
    if (buffer == 0)
        return;
    fread(buffer, 1, file_size, f);

    // prepare new incomplete texture
    glGenTextures(1, &to_id);
    if (to_id == 0)
        return;

    // bind the texture
    // make it complete by specifying all needed parameters and ensuring all mipmaps are filled
    glBindTexture(GL_TEXTURE_2D, to_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // prepare some variables
    unsigned int offset = 0;
    unsigned int size = 0;
    w = width;
    h = height;

    // loop through sending block at a time with the magic formula
    // upload to opengl properly, note the offset transverses the pointer
    // assumes each mipmap is 1/2 the size of the previous mipmap
    for (unsigned int i = 0; i < mipMapCount; i++) {
        if (w == 0 || h == 0) { // discard any odd mipmaps 0x1 0x2 resolutions
            mipMapCount--;
            continue;
        }
        size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    // discard any odd mipmaps, ensure a complete texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount - 1);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    // easy macro to get out quick and uniform (minus like 15 lines of bulk)
    free(buffer);
    free(header);
    fclose(f);
}
