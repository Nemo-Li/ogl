//
// Created by Nemo li on 2023/12/12.
//

#ifndef OGL_GPU_RESOURCE_MAPPER_H
#define OGL_GPU_RESOURCE_MAPPER_H

#define GLFW_INCLUDE_NONE

#include "unordered_map"
#include "glad/glad.h"

/// GPU资源管理，负责管理GPU资源ID与其在引擎逻辑ID的映射关系。
class GPUResourceMapper {

public:
    /// 生成Shader程序句柄
    /// \return
    static unsigned int GenerateShaderProgramHandle() {
        return ++shader_program_index_;
    }

    /// 生成VAO句柄
    /// \return
    static unsigned int GenerateVAOHandle() {
        return ++vao_index_;
    }

    /// 生成VBO句柄
    static unsigned int GenerateVBOHandle() {
        return ++vbo_index_;
    }

    /// 生成Texture句柄
    static unsigned int GenerateTextureHandle() {
        return ++texture_index_;
    }

    /// 映射Shader程序
    /// \param shader_program_handle
    /// \param shader_program_id
    static void MapShaderProgram(unsigned int shader_program_handle, GLuint shader_program_id) {
        shader_program_map_[shader_program_handle] = shader_program_id;
    }

    /// 映射VAO
    /// \param vao_handle
    /// \param vao_id
    static void MapVAO(unsigned int vao_handle, GLuint vao_id) {
        vao_map_[vao_handle] = vao_id;
    }

    /// 映射VBO
    /// \param vbo_handle
    /// \param vbo_id
    static void MapVBO(unsigned int vbo_handle, GLuint vbo_id) {
        vbo_map_[vbo_handle] = vbo_id;
    }

    /// 映射Texture
    static void MapTexture(unsigned int texture_handle, GLuint texture_id) {
        texture_map_[texture_handle] = texture_id;
    }

    /// 获取Shader程序
    /// \param shader_program_handle
    /// \return
    static GLuint GetShaderProgram(unsigned int shader_program_handle) {
        return shader_program_map_[shader_program_handle];
    }

    /// 获取VAO
    /// \param vao_handle
    /// \return
    static GLuint GetVAO(unsigned int vao_handle) {
        return vao_map_[vao_handle];
    }

    /// 获取VBO
    /// \param vbo_handle
    /// \return
    static GLuint GetVBO(unsigned int vbo_handle) {
        return vbo_map_[vbo_handle];
    }

    /// 获取Texture
    static GLuint GetTexture(unsigned int texture_handle) {
        return texture_map_[texture_handle];
    }

private:
    static unsigned int shader_program_index_;//Shader程序索引
    static unsigned int vao_index_;//VAO索引
    static unsigned int vbo_index_;//VBO索引
    static unsigned int texture_index_;//Texture索引

    static std::unordered_map<unsigned int, GLuint> shader_program_map_;//Shader程序映射表
    static std::unordered_map<unsigned int, GLuint> vao_map_;//VAO映射表
    static std::unordered_map<unsigned int, GLuint> vbo_map_;//VBO映射表
    static std::unordered_map<unsigned int, GLuint> texture_map_;//Texture映射表
};


#endif //OGL_GPU_RESOURCE_MAPPER_H
