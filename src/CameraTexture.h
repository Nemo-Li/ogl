//
// Created by Nemo li on 2021/12/13.
//

#ifndef OGL_CAMERATEXTURE_H
#define OGL_CAMERATEXTURE_H

#include "Model.h"
#include "glad/glad.h"
#include "Shader.h"
#include "Texture.h"
#include <opencv2/opencv.hpp>

class CameraTexture : Model {
public:
    unsigned int VAO;
    Shader *shader;
    cv::VideoCapture capture;
    int cameraTextureId;

    float vertices[32] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // 右下
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 左下
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // 左上
    };

    unsigned int indices[6] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };

    void CreateCameraTexture();

    void initVAO(Shader *shader);

    void draw() override;

};


#endif //OGL_CAMERATEXTURE_H
