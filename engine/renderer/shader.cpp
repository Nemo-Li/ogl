//
// Created by Nemo li on 2023/12/6.
//

#include <fstream>
#include <glad/glad.h>
#include <framework/application.h>
#include "shader.h"

std::unordered_map<std::string, Shader *> Shader::kShaderMap;

Shader::Shader() {

}

Shader::~Shader() {
    if (program_id_ != 0) {
        glDeleteProgram(program_id_);
        program_id_ = 0;
    }
}

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

    CreateGPUProgram(vertex_shader_source.c_str(), fragment_shader_source.c_str());
}

void Shader::CreateGPUProgram(const char *vertex_shader_text, const char *fragment_shader_text) {
    //创建顶点Shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //指定Shader源码
    glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
    //编译Shader
    glCompileShader(vertex_shader);
    //获取编译结果
    GLint compile_status = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        GLchar message[256];
        glGetShaderInfoLog(vertex_shader, sizeof(message), nullptr, message);
        printf("compile vs error: %s", message);
    }

    //创建片段Shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //指定Shader源码
    glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
    //编译Shader
    glCompileShader(fragment_shader);
    //获取编译结果
    compile_status = GL_FALSE;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE) {
        GLchar message[256];
        glGetShaderInfoLog(fragment_shader, sizeof(message), nullptr, message);
        printf("compile fs error: %s", message);
    }

    //创建GPU程序
    program_id_ = glCreateProgram();
    //附加Shader
    glAttachShader(program_id_, vertex_shader);
    glAttachShader(program_id_, fragment_shader);
    //Link
    glLinkProgram(program_id_);
    //获取编译结果
    GLint link_status = GL_FALSE;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE) {
        GLchar message[256];
        glGetProgramInfoLog(program_id_, sizeof(message), 0, message);
        printf("link error: %s", message);
    }
}

void Shader::use() {
    glUseProgram(program_id_);
}

void Shader::disuse() {

}
