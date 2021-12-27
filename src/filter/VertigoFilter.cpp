//
// Created by Nemo li on 2021/12/27.
//

#include "VertigoFilter.h"

extern int width;
extern int height;

void VertigoFilter::drawFilter() {
    if (frameBuffer1 == nullptr) {
        frameBuffer1 = new FrameBuffer();
        frameBuffer1->init(GL_TEXTURE0, width, height);
        frameBuffer2 = new FrameBuffer();
        frameBuffer2->init(GL_TEXTURE1, width, height);
        frameBuffer3 = new FrameBuffer();
        frameBuffer3->init(GL_TEXTURE2, width, height);
    }
    frameBuffer1->bind();

    lastShader->use();
    lastShader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    lastShader->setMat4("uTexMatrix", glm::mat4(1.0f));

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    frameBuffer1->unbind();

    //绘制当前帧
    glClear(GL_COLOR_BUFFER_BIT);
    drawCurrentFrame();
    frameBuffer3->bind();
    drawCurrentFrame();
    frameBuffer3->unbind();
//    //只用两个buffer的话，屏幕中会有黑格子
//    //把3中的内容画到2中
    frameBuffer2->bind();
    drawToBuffer();
    frameBuffer2->unbind();
    mFirst = false;
}

void VertigoFilter::init() {
    this->currentShader = new Shader("../res/shader/vertigoShader.vert", "../res/shader/vertigoShader.frag");

    this->lastShader = new Shader("../res/shader/filterNormalShader.vert", "../res/shader/filterNormalShader.frag");
    lastShader->use();
    lastShader->setInt("screenTexture", 0);

    lutTexture.load("res/pic/lookup_vertigo.png");
}

void VertigoFilter::setVAOTexture(int vao, int texture) {
    this->vao = vao;
    this->texture = texture;
}

void VertigoFilter::drawToBuffer() {
    setup(lastShader, new unsigned int[1]{frameBuffer3->getTextureId()}, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    lastShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    lastShader->setMat4("modelMatrix", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void VertigoFilter::drawCurrentFrame() {
    glViewport(0, 0, width, height);
    unsigned int textureId = frameBuffer1->getTextureId();
    setup(currentShader,
          new unsigned int[3]{textureId, mFirst ? textureId : frameBuffer2->getTextureId(), lutTexture.getTextureId()},
          3);
//    setup(lastShader,
//          new unsigned int[1]{textureId},
//          1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void VertigoFilter::setup(Shader *shader, unsigned int textureId[], int size) {
    shader->use();
    shader->setMat4("uMvpMatrix", glm::mat4(1.0f));
    shader->setMat4("uTexMatrix", glm::mat4(1.0f));
    glBindVertexArray(vao);

    for (int i = 0; i < size; i++) {
        string sTexture = "uTexture";
        sTexture = sTexture.append(std::to_string(i));
//        cout << "sTexture:" << sTexture.data() << endl;
        int textureLocation = glGetUniformLocation(shader->ID, sTexture.data());
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureId[i]);
        glUniform1i(textureLocation, i);
    }
}

VertigoFilter::~VertigoFilter() {
    delete currentShader;
    delete lastShader;
    delete frameBuffer1;
    delete frameBuffer2;
    delete frameBuffer3;
    currentShader = nullptr;
    lastShader = nullptr;
    frameBuffer1 = nullptr;
    frameBuffer2 = nullptr;
    frameBuffer3 = nullptr;
}