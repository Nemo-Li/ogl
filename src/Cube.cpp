//
// Created by Nemo li on 2021/12/16.
//

#include "Cube.h"

void Cube::initVAO() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    texture = Texture();
    texture.load("res/pic/container.jpeg");

    /* Matrices */
    glm::vec3 cam_position = glm::vec3(2.0f, 2.0f, 3.0f);
    glm::vec3 cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

    view_matrix = glm::lookAt(cam_position, cam_look_at, cam_up);

    projection_matrix = glm::perspectiveFov(glm::radians(45.0f), float(3360), float(2010),
                                            0.1f,
                                            10.0f);
}

void Cube::draw(Shader &shader) {
    shader.use();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projectionMatrix"), 1, GL_FALSE,
                       glm::value_ptr(projection_matrix));

    glBindVertexArray(VAO);
    texture.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
