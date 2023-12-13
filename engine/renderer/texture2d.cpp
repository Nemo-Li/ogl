//
// Created by Nemo li on 2023/12/7.
//

#include <stb_image.h>
#include <app/application.h>
#include <render_async/render_task_producer.h>
#include <render_async/gpu_resource_mapper.h>
#include "texture2d.h"

Texture2D::Texture2D() : width_(0), height_(0), texture_format_(0), texture_handle_(0) {

}

Texture2D::~Texture2D() {
    if (texture_handle_ > 0) {
        RenderTaskProducer::ProduceRenderTaskDeleteTextures(1, &texture_handle_);
    }
}

Texture2D *Texture2D::LoadFromFile(std::string &image_file_path) {
    Texture2D *texture2d = new Texture2D();
    int width, height, components;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *pixels = stbi_load((Application::data_path() + image_file_path).c_str(), &width, &height,
                                      &components, 4);
    printf("image path: %s\n", (Application::data_path() + image_file_path).c_str());

    if (pixels != nullptr) {
        texture2d->width_ = width;
        texture2d->height_ = height;
        texture2d->texture_format_ = GL_RGBA;
        texture2d->texture_handle_ = GPUResourceMapper::GenerateTextureHandle();
        printf("width %d height %d components %d\n", width, height, components);

        RenderTaskProducer::ProduceRenderTaskCreateTexImage2D(texture2d->texture_handle_, texture2d->width_,
                                                              texture2d->height_, texture2d->texture_format_,
                                                              GL_RGBA, GL_UNSIGNED_BYTE, width * height * components,
                                                              pixels);
    } else {
        printf("Could not load file %s\n", image_file_path.c_str());
    }

    stbi_image_free(pixels);
    return texture2d;
}
