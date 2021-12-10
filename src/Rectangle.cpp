//
// Created by Nemo li on 2021/12/10.
//

#include "Rectangle.h"

Rectangle::Rectangle() {

}

void Rectangle::initVAO() {
    unsigned int VBO;
    unsigned int EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1. 绑定顶点数组对象
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 设定顶点属性指针
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
    //glEnableVertexAttribArray(1);
    // 纹理属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Rectangle::draw() {
    shader->use();
    if (useTexture) {
        shader->setBool("useTexture", true);
    }
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, &(*modelMatrix)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "viewMatrix"), 1, GL_FALSE, &(*viewMatrix)[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projectionMatrix"), 1, GL_FALSE,
                       &(*projectionMatrix)[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "textureMatrix"), 1, GL_FALSE, &(*textureMatrix)[0][0]);

    glBindVertexArray(VAO);
    texture->bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Rectangle::Rectangle(Shader *shader) {
    this->shader = shader;
}

void Rectangle::setTexture(Texture *texture) {
    Rectangle::texture = texture;
}
