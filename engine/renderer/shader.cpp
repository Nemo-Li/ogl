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

Shader *Shader::Find(std::string shader_name, bool geom, bool tesc, bool tese) {
    auto iter = kShaderMap.find(shader_name);
    if (iter != kShaderMap.end()) {
        return iter->second;
    }

    Shader *shader = new Shader();
    shader->Load(shader_name, geom, tesc, tese);

    kShaderMap.insert(std::pair<std::string, Shader *>(shader_name, shader));

    return shader;
}

void Shader::Load(const std::string &shader_name, bool geom, bool tesc, bool tese) {
    shader_name_ = shader_name;

    //组装完整文件路径
    std::string vertex_shader_file_path = Application::data_path() + shader_name + ".vert";
    std::string fragment_shader_file_path = Application::data_path() + shader_name + ".frag";
    std::string geom_shader_file_path = Application::data_path() + shader_name + ".geom";
    std::string tesc_shader_file_path = Application::data_path() + shader_name + ".tesc";
    std::string tese_shader_file_path = Application::data_path() + shader_name + ".tese";

    //读取顶点Shader代码
    std::ifstream vertex_shader_input_file_stream(vertex_shader_file_path);
    std::string vertex_shader_source((std::istreambuf_iterator<char>(vertex_shader_input_file_stream)),
                                     std::istreambuf_iterator<char>());
    //读取片段Shader代码
    std::ifstream fragment_shader_input_file_stream(fragment_shader_file_path);
    std::string fragment_shader_source((std::istreambuf_iterator<char>(fragment_shader_input_file_stream)),
                                       std::istreambuf_iterator<char>());

    std::string geom_shader_source;
    std::string tesc_shader_source;
    std::string tese_shader_source;
    if (geom) {
        std::ifstream geom_shader_input_file_stream(geom_shader_file_path);
        geom_shader_source = std::string(std::istreambuf_iterator<char>(geom_shader_input_file_stream),
                                         std::istreambuf_iterator<char>());
    }
    if (tesc) {
        std::ifstream tesc_shader_input_file_stream(tesc_shader_file_path);
        tesc_shader_source = std::string(std::istreambuf_iterator<char>(tesc_shader_input_file_stream),
                                         std::istreambuf_iterator<char>());
    }
    if (tese) {
        std::ifstream tese_shader_input_file_stream(tese_shader_file_path);
        tese_shader_source = std::string(std::istreambuf_iterator<char>(tese_shader_input_file_stream),
                                         std::istreambuf_iterator<char>());
    }
    CreateGPUProgram(vertex_shader_source.c_str(), fragment_shader_source.c_str(),
                     geom ? geom_shader_source.c_str() : nullptr,
                     tesc ? tesc_shader_source.c_str() : nullptr, tese ? tese_shader_source.c_str() : nullptr);
}

void Shader::CreateGPUProgram(const char *vertex_shader_text, const char *fragment_shader_text,
                              const char *geometry_shader_text,
                              const char *tesc_shader_text, const char *tese_shader_text) {
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

    unsigned int geom_shader = 0;
    if (geometry_shader_text) {
        geom_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom_shader, 1, &geometry_shader_text, nullptr);
        glCompileShader(geom_shader);
        compile_status = GL_FALSE;
        glGetShaderiv(geom_shader, GL_COMPILE_STATUS, &compile_status);
        if (compile_status == GL_FALSE) {
            GLchar message[256];
            glGetShaderInfoLog(geom_shader, sizeof(message), nullptr, message);
            printf("compile geom error: %s", message);
        }
    }

    unsigned int tesc_shader = 0;
    if (tesc_shader_text) {
        tesc_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tesc_shader, 1, &tesc_shader_text, nullptr);
        glCompileShader(tesc_shader);
        compile_status = GL_FALSE;
        glGetShaderiv(tesc_shader, GL_COMPILE_STATUS, &compile_status);
        if (compile_status == GL_FALSE) {
            GLchar message[256];
            glGetShaderInfoLog(tesc_shader, sizeof(message), nullptr, message);
            printf("compile tesc error: %s", message);
        }
    }

    unsigned int tese_shader = 0;
    if (tese_shader_text) {
        tese_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tese_shader, 1, &tese_shader_text, nullptr);
        glCompileShader(tese_shader);
        compile_status = GL_FALSE;
        glGetShaderiv(tese_shader, GL_COMPILE_STATUS, &compile_status);
        if (compile_status == GL_FALSE) {
            GLchar message[256];
            glGetShaderInfoLog(tese_shader, sizeof(message), nullptr, message);
            printf("compile tese error: %s", message);
        }
    }

    //创建GPU程序
    program_id_ = glCreateProgram();
    //附加Shader
    glAttachShader(program_id_, vertex_shader);
    glAttachShader(program_id_, fragment_shader);
    if (geom_shader != 0) {
        glAttachShader(program_id_, geom_shader);
    }
    if (tesc_shader != 0) {
        glAttachShader(program_id_, tesc_shader);
    }
    if (tese_shader != 0) {
        glAttachShader(program_id_, tese_shader);
    }
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

void Shader::Use() {
    glUseProgram(program_id_);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(program_id_, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value);
}

void Shader::Disuse() {

}
