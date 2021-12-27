//
// Created by Nemo li on 2021/12/27.
//

#include "ScaleFilter.h"

void ScaleFilter::drawFilter() {
    float progress;
    if (mFrames <= mMiddleFrames) {
        progress = mFrames * 1.0f / mMiddleFrames;
    } else {
        progress = 2.0f - mFrames * 1.0f / mMiddleFrames;
    }
    float scale = 1.0f + mScale * progress;
    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
    shader->use();
    shader->setMat4("uMvpMatrix", scaleMatrix);
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    mFrames++;
    if (mFrames > mMaxFrames) {
        mFrames = 0;
    }
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ScaleFilter::init() {
    this->shader = new Shader("../res/shader/scaleShader.vert", "../res/shader/scaleShader.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void ScaleFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}
