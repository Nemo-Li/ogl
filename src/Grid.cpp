//
// Created by Nemo li on 2021/12/9.
//

#include "Grid.h"

void Grid::initVAO() {
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);

    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Grid::draw() {
    shader->use();
    shader->setVec3("drawColor", 1.0f, 1.0f, 1.0f);
    shader->setBool("useTexture", false);
    glBindVertexArray(lineVAO);

    for (int i = 0; i < 80; ++i) {
        glm::mat4 lineModel = *modelMatrix;
        if (i < 20) {
            lineModel = glm::translate(lineModel, glm::vec3(float(i) * 0.1f, 0.0, 0.0f));
        } else if (i < 40) {
            lineModel = glm::translate(lineModel, glm::vec3(float(20 - i) * 0.1f, 0.0, 0.0f));
        } else if (i < 60) {
            lineModel = glm::translate(lineModel, glm::vec3(0.0f, 0.0f, float(i - 40) * 0.1f));
            lineModel = glm::rotate(lineModel, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0f));
        } else {
            lineModel = glm::translate(lineModel, glm::vec3(0.0f, 0.0f, float(60 - i) * 0.1f));
            lineModel = glm::rotate(lineModel, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0f));
        }
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, &lineModel[0][0]);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

Grid::Grid(Shader *shader, glm::mat4 *modelMatrix) {
    this->shader = shader;
    this->modelMatrix = modelMatrix;
}

