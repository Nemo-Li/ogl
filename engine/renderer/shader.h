//
// Created by Nemo li on 2023/12/6.
//

#ifndef OGL_SHADER_H
#define OGL_SHADER_H

#include <string>
#include <unordered_map>
#include <glm/mat2x2.hpp>

class Shader {
public:
    Shader();

    ~Shader();

    //查找或创建Shader
    static Shader *Find(std::string shader_name, bool geom = false, bool tesc = false, bool tese = false);

    void Use();

    void Disuse();

    //glCreateProgram()返回的GPU程序句柄
    unsigned int program_id() { return program_id_; }

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setVec3(const std::string &name, const glm::vec3 &vec3) const;

private:
    //shader名
    std::string shader_name_;
    //glCreateProgram()返回的GPU程序句柄(handle);
    unsigned int program_id_;
    //已经创建的Shader
    static std::unordered_map<std::string, Shader *> kShaderMap;

    void Load(const std::string &shader_name, bool geom = false, bool tesc = false, bool tese = false);

    //编译Shader,创建GPU程序
    void
    CreateGPUProgram(const char *vertex_shader_text, const char *fragment_shader_text, const char *geometry_shader_text,
                     const char *tesc_shader_text, const char *tese_shader_text);
};


#endif //OGL_SHADER_H
