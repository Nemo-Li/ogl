//
// Created by Nemo li on 2021/12/17.
//

#include "LightCube.h"

extern const int width;
extern const int height;

LightCube::LightCube(Camera &camera) : camera(camera) {
}

void LightCube::initVAO() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void LightCube::draw(Shader &shader, glm::mat4 &modelMatrix, float currentFrame) {
    shader.use();
    shader.setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
    shader.setVec3("viewPos", camera.Position);

    // light properties
    glm::vec3 lightColor;
    lightColor.x = sin(currentFrame * 2.0f);
    lightColor.y = sin(currentFrame * 0.7f);
    lightColor.z = sin(currentFrame * 1.3f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    shader.setVec3("light.ambient", ambientColor);
    shader.setVec3("light.diffuse", diffuseColor);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // material properties
    shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    shader.setVec3("material.specular", 0.5f, 0.5f,
                   0.5f); // specular lighting doesn't have full effect on this object's material
    shader.setFloat("material.shininess", 32.0f);

    shader.setMat4("modelMatrix", modelMatrix);
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) 3360 / (float) 2010, 0.1f,
                                            100.0f);
    shader.setMat4("projectionMatrix", projection);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("viewMatrix", view);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
