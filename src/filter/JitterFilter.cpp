//
// Created by Nemo li on 2021/12/26.
//

#include "JitterFilter.h"

void JitterFilter::drawFilter() {
    mProgress = (float) mFrames / mMaxFrames;
    if (mProgress > 1.0f) {
        mProgress = 0.0f;
    }
    mFrames++;
    if (mFrames > mMaxFrames + mSkipFrames) {
        mFrames = 0;
    }
    float scale = 1.0f + 0.2f * mProgress;
    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, 1.0f));
    shader->use();
    shader->setMat4("uMvpMatrix", scaleMatrix);
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    //设置色值偏移的量
    float textureCoordOffset = 0.01f * mProgress;
    shader->setFloat("uTextureCoordOffset", textureCoordOffset);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void JitterFilter::init() {
    this->shader = new Shader("../res/shader/jitterShader.vert", "../res/shader/jitterShader.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void JitterFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}
