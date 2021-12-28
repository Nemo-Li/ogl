//
// Created by Nemo li on 2021/12/28.
//

#include "FboFilter.h"

void FboFilter::drawFilter() {
    shader->use();
    shader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    shader->setInt("effect", effect);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FboFilter::init() {
    this->shader = new Shader("../res/shader/fboShader.vert", "../res/shader/fboShader.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void FboFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}

void FboFilter::setEffect(int effect) {
    this->effect = effect;
}
