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

    void Use();

    void Disuse();

    unsigned int shader_program_handle() { return shader_program_handle_; }//Shader程序句柄;

private:
    //shader名
    std::string shader_name_;
    unsigned int shader_program_handle_;//Shader程序句柄;

    //已经创建的Shader
    static std::unordered_map<std::string, Shader *> kShaderMap;

    void Load(const std::string &shader_name);

    //编译Shader,创建GPU程序
    void CreateShaderProgram(const char *vertex_shader_text, const char *fragment_shader_text);
};


#endif //OGL_SHADER_H
