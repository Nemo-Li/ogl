//
// Created by Nemo li on 2023/12/16.
//

#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace EXP {
    class Shader {
    public:
        // 程序ID
        unsigned int ID;

        // 构造器读取并构建着色器
        Shader(const GLchar *vertexPath, const GLchar *fragmentPath);

        // 使用/激活程序
        void use();

        // uniform工具函数
        void setBool(const std::string &name, bool value) const;

        void setInt(const std::string &name, int value) const;

        void setFloat(const std::string &name, float value) const;
    };
}