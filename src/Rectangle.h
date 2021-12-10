//
// Created by Nemo li on 2021/12/10.
//

#ifndef OGL_RECTANGLE_H
#define OGL_RECTANGLE_H

#include "Model.h"
#include "glad/glad.h"
#include "Shader.h"
#include "Texture.h"

class Rectangle : Model {
public:
    unsigned int VAO;
    Shader *shader;
    Texture *texture;
    glm::mat4 *modelMatrix;
    glm::mat4 *modelTransMatrix;
    glm::mat4 *viewMatrix;
    glm::mat4 *projectionMatrix;
    glm::mat4 *textureMatrix;
    bool useTexture;

    float vertices[32] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // 右下
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 左下
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // 左上
    };

    unsigned int indices[6] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };

    Rectangle();

    Rectangle(Shader *shader);

    void setTexture(Texture *texture);

    void initVAO() override;

    void draw() override;
};


#endif //OGL_RECTANGLE_H
