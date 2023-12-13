//
// Created by Nemo li on 2023/12/6.
//

#include <fstream>
#include <glad/glad.h>
#include <framework/application.h>
#include <render_async/gpu_resource_mapper.h>
#include <render_async/render_task_producer.h>
#include "shader.h"

std::unordered_map<std::string, Shader *> Shader::kShaderMap;

Shader::Shader() = default;

Shader::~Shader() = default;

Shader *Shader::Find(std::string shader_name) {
    auto iter = kShaderMap.find(shader_name);
    if (iter != kShaderMap.end()) {
        return iter->second;
    }

    Shader *shader = new Shader();
    shader->Load(shader_name);

    kShaderMap.insert(std::pair<std::string, Shader *>(shader_name, shader));

    return shader;
}

void Shader::Load(const std::string &shader_name) {
    shader_name_ = shader_name;

    //组装完整文件路径
    std::string vertex_shader_file_path = Application::data_path() + shader_name + ".vert";
    std::string fragment_shader_file_path = Application::data_path() + shader_name + ".frag";

    //读取顶点Shader代码
    std::ifstream vertex_shader_input_file_stream(vertex_shader_file_path);
    std::string vertex_shader_source((std::istreambuf_iterator<char>(vertex_shader_input_file_stream)),
                                     std::istreambuf_iterator<char>());
    //读取片段Shader代码
    std::ifstream fragment_shader_input_file_stream(fragment_shader_file_path);
    std::string fragment_shader_source((std::istreambuf_iterator<char>(fragment_shader_input_file_stream)),
                                       std::istreambuf_iterator<char>());

    CreateShaderProgram(vertex_shader_source.c_str(), fragment_shader_source.c_str());
}

void Shader::CreateShaderProgram(const char *vertex_shader_text, const char *fragment_shader_text) {
    //编译Shader任务
    shader_program_handle_ = GPUResourceMapper::GenerateShaderProgramHandle();
    RenderTaskProducer::ProduceRenderTaskCompileShader(vertex_shader_text, fragment_shader_text,
                                                       shader_program_handle_);
}

void Shader::Use() {
    //使用Shader程序任务
    RenderTaskProducer::ProduceRenderTaskUseShaderProgram(shader_program_handle_);
}

void Shader::Disuse() {

}
