//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_SHADER_H
#define OGL_SHADER_H

#include <string>
#include <unordered_map>

class Shader {
public:
    Shader();

    ~Shader();

    //查找或创建Shader
    static Shader *Find(std::string shader_name);

    void use();

    void disuse();

    //glCreateProgram()返回的GPU程序句柄
    unsigned int program_id() { return program_id_; }

private:
    //shader名
    std::string shader_name_;
    //glCreateProgram()返回的GPU程序句柄(handle);
    unsigned int program_id_;
    //已经创建的Shader
    static std::unordered_map<std::string, Shader *> kShaderMap;

    void Load(const std::string &shader_name);

    //编译Shader,创建GPU程序
    void CreateGPUProgram(const char *vertex_shader_text, const char *fragment_shader_text);
};


#endif //OGL_SHADER_H
