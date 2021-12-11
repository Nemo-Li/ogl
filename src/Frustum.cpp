//
// Created by Nemo li on 2021/12/9.
//

#include "Frustum.h"

Frustum::Frustum() {

}

void Frustum::onPerspectiveChange(float fov, float near, float far, float width, float height) {
    cout << "Frustum::onPerspectiveChange" << endl;
    initFrustum(fov, width / height, near, far);
}

void Frustum::initFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane) {
    float tangent = tanf(fovY / 2 * DEG2RAD);
    float nearHeight = nearPlane * tangent;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = farPlane * tangent;
    float farWidth = farHeight * aspectRatio;

    // compute 8 vertices of the frustum
    float vertices[8][3];
    // near top right
    vertices[0][0] = nearWidth;
    vertices[0][1] = nearHeight;
    vertices[0][2] = -nearPlane;
    // near top left
    vertices[1][0] = -nearWidth;
    vertices[1][1] = nearHeight;
    vertices[1][2] = -nearPlane;
    // near bottom left
    vertices[2][0] = -nearWidth;
    vertices[2][1] = -nearHeight;
    vertices[2][2] = -nearPlane;
    // near bottom right
    vertices[3][0] = nearWidth;
    vertices[3][1] = -nearHeight;
    vertices[3][2] = -nearPlane;
    // far top right
    vertices[4][0] = farWidth;
    vertices[4][1] = farHeight;
    vertices[4][2] = -farPlane;
    // far top left
    vertices[5][0] = -farWidth;
    vertices[5][1] = farHeight;
    vertices[5][2] = -farPlane;
    // far bottom left
    vertices[6][0] = -farWidth;
    vertices[6][1] = -farHeight;
    vertices[6][2] = -farPlane;
    // far bottom right
    vertices[7][0] = farWidth;
    vertices[7][1] = -farHeight;
    vertices[7][2] = -farPlane;

    float colorLine1[4] = {0.7f, 0.7f, 0.7f, 0.7f};
    float colorLine2[4] = {0.2f, 0.2f, 0.2f, 0.7f};
    float colorPlane[4] = {0.5f, 0.5f, 0.5f, 0.5f};

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
    glGenVertexArrays(1, &frustumVAO);
    glBindVertexArray(frustumVAO);
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

void Frustum::draw() {
    glm::mat4 frustumMatrix = glm::translate(*modelMatrix, glm::vec3(0.0f, 0.0f, 3.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, &frustumMatrix[0][0]);

    shader->setVec4("drawColor", 0.0f, 1.0f, 1.0f, 0.3f);
    glBindVertexArray(frustumVAO);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    shader->setVec4("drawColor", 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void *) (sizeof(int) * 12));

}

Frustum::Frustum(Shader *shader, glm::mat4 *modelMatrix) {
    this->shader = shader;
    this->modelMatrix = modelMatrix;
}
