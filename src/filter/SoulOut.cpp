//
// Created by Nemo li on 2021/12/26.
//

#include "SoulOut.h"

void SoulOut::drawFilter() {
    mProgress = (float) mFrames / mMaxFrames;
    if (mProgress > 1.0f) {
        mProgress = 0.0f;
    }
    mFrames++;
    if (mFrames > mMaxFrames + mSkipFrames) {
        mFrames = 0;
    }

    //底层图层的透明度
    float backAlpha = 1.0f;
    //放大图层的透明度
    float alpha = 0.0f;
    if (mProgress > 0.0f) {
        alpha = 0.2f - mProgress * 0.2f;
        backAlpha = 1 - alpha;
    }

    shader->use();
    shader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    shader->setFloat("uAlpha", backAlpha);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D,
                  texture);    // use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (mProgress > 0.0f) {
        float scale = 1.0f + 1.0f * mProgress;

        shader->use();
        glm::mat4 mvpMatrix = glm::mat4(1.0f);
        mvpMatrix = glm::scale(mvpMatrix, glm::vec3(scale, scale, scale));
        shader->setMat4("uMvpMatrix", mvpMatrix);
        shader->setMat4("uTexMatrix", glm::mat4(1.0f));
        shader->setFloat("uAlpha", alpha);
        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D,
                      texture);    // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void SoulOut::init() {
    this->shader = new Shader("../res/shader/soulOut.vert", "../res/shader/soulOut.frag");
    shader->use();
    shader->setInt("screenTexture", 0);
}

void SoulOut::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}
