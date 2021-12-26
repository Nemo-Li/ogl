//
// Created by Nemo li on 2021/12/13.
//

#include "CameraTexture.h"

void CameraTexture::initVAO(Shader *shader) {
    this->shader = shader;
    capture = cv::VideoCapture(0);

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

void CameraTexture::draw() {
    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->setMat4("modelMatrix", model);
    CreateCameraTexture();
    glBindTexture(GL_TEXTURE_2D, cameraTextureId);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CameraTexture::CreateCameraTexture() {
    cv::Mat image;
    capture >> image;
    if (image.empty()) {
        printf("Can not load Cameras\n");
    } else {
        cv::Mat result = image;
        //设置长宽
        int width = result.cols;
        int height = result.rows;
        int channel = result.channels();

        //获取图像指针
        int pixellength = width * height * channel;
        GLubyte *pixels = new GLubyte[pixellength];
        memcpy(pixels, result.data, pixellength * sizeof(char));

        glBindTexture(GL_TEXTURE_2D, cameraTextureId);
        //必须一个RGB  一个BGR（opencv的mat类的颜色通道是BGR） 否则会出现颜色偏差
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);
        //纹理放大缩小使用线性插值
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        free(pixels);
    }
}