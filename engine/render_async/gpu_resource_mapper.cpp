//
// Created by Nemo li on 2023/12/12.
//

#include "gpu_resource_mapper.h"

unsigned int GPUResourceMapper::shader_program_index_ = 0;//Shader程序索引
unsigned int GPUResourceMapper::vao_index_;//VAO索引
unsigned int GPUResourceMapper::vbo_index_;//VBO索引
unsigned int GPUResourceMapper::texture_index_;//Texture索引
std::unordered_map<unsigned int, GLuint> GPUResourceMapper::shader_program_map_;//Shader程序映射表
std::unordered_map<unsigned int, GLuint> GPUResourceMapper::vao_map_;//VAO映射表
std::unordered_map<unsigned int, GLuint> GPUResourceMapper::vbo_map_;//VBO映射表
std::unordered_map<unsigned int, GLuint> GPUResourceMapper::texture_map_;//Texture映射表