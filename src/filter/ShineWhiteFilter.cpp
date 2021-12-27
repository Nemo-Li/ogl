//
// Created by Nemo li on 2021/12/27.
//

#include "ShineWhiteFilter.h"

void ShineWhiteFilter::drawFilter() {
    float progress;
    if (mFrames <= mHalfFrames) {
        progress = mFrames * 1.0f / mHalfFrames;
    } else {
        progress = 2.0f - mFrames * 1.0f / mHalfFrames;
    }
    mFrames++;
    if (mFrames > mMaxFrames) {
        mFrames = 0;
    }
    shader->use();
    shader->setFloat("uAdditionalColor", progress);
    shader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ShineWhiteFilter::init() {
    this->shader = new Shader("../res/shader/shineWhiteShader.vert", "../res/shader/shineWhiteShader.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void ShineWhiteFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}
