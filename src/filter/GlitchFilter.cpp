//
// Created by Nemo li on 2021/12/27.
//

#include "GlitchFilter.h"

void GlitchFilter::drawFilter() {
    float slDisplacement = mJitterSequence[mFrames];
    float slThreshold = mThreshHoldSequence[mFrames];
    float drift = mDriftSequence[mFrames];
    mFrames++;
    if (mFrames > mMaxFrames) {
        mFrames = 0;
    }
    shader->use();
    shader->setVec2("uScanLineJitter", slDisplacement, slThreshold);
    shader->setFloat("uColorDrift", drift);
    shader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GlitchFilter::init() {
    this->shader = new Shader("../res/shader/GlitchShader.vert", "../res/shader/GlitchShader.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void GlitchFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}
