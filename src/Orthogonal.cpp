//
// Created by Nemo li on 2021/12/11.
//

#include "Orthogonal.h"

Orthogonal::Orthogonal() {

}

void Orthogonal::initVAO(float left, float right, float top, float bottom, float near, float far) {
    // compute 8 vertices of the orthogonal
    float vertices[8][3];
    // near top right
    vertices[0][0] = right;
    vertices[0][1] = top;
    vertices[0][2] = -near;
    // near top left
    vertices[1][0] = left;
    vertices[1][1] = top;
    vertices[1][2] = -near;
    // near bottom left
    vertices[2][0] = left;
    vertices[2][1] = bottom;
    vertices[2][2] = -near;
    // near bottom right
    vertices[3][0] = right;
    vertices[3][1] = bottom;
    vertices[3][2] = -near;
    // far top right
    vertices[4][0] = right;
    vertices[4][1] = top;
    vertices[4][2] = -far;
    // far top left
    vertices[5][0] = left;
    vertices[5][1] = top;
    vertices[5][2] = -far;
    // far bottom left
    vertices[6][0] = left;
    vertices[6][1] = bottom;
    vertices[6][2] = -far;
    // far bottom right
    vertices[7][0] = right;
    vertices[7][1] = bottom;
    vertices[7][2] = -far;

    int indices[] = {
            0, 1, 3,
            1, 3, 2,
            5, 4, 7,
            5, 7, 6,
            0, 4,
            1, 5,
            2, 6,
            3, 7
    };

    unsigned int VBO;
    unsigned int EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &OrthogonalVAO);
    glBindVertexArray(OrthogonalVAO);
    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    // 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Orthogonal::draw() {
    glm::mat4 frustumMatrix = glm::translate(*modelMatrix, glm::vec3(0.0f, 0.0f, 3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, &frustumMatrix[0][0]);

    shader->setVec4("drawColor", 1.0f, 0.5f, 1.0f, 0.5f);
    glBindVertexArray(OrthogonalVAO);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    shader->setVec4("drawColor", 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void *) (sizeof(int) * 12));
}

Orthogonal::Orthogonal(Shader *shader, glm::mat4 *modelMatrix) {
    this->shader = shader;
    this->modelMatrix = modelMatrix;
}

void Orthogonal::onOrthoChange(float left, float right, float top, float bottom, float near, float far) {
    initVAO(left, right, top, bottom, near, far);
}
