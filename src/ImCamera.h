//
// Created by Nemo li on 2021/12/11.
//

#ifndef OGL_IMCAMERA_H
#define OGL_IMCAMERA_H

#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include "imgui.h"

class ImCamera {
public:
    unsigned int cameraTextureId;
    bool FlagWetCamera = false;
    bool FlagOpenCamera = false;
    bool WetCamera_window;
    cv::VideoCapture capture;

    virtual ~ImCamera();

    void CreateCameraTexture(cv::VideoCapture capture);

    void draw();

    void startCamera();
};


#endif //OGL_IMCAMERA_H
