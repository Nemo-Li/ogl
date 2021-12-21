//
// Created by Nemo li on 2021/12/11.
//

#ifndef OGL_IMCAMERA_H
#define OGL_IMCAMERA_H

#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include "imgui.h"
#include "detect/dnnfacedetect.h"

class ImCamera {
public:
    unsigned int cameraTextureId;
    bool FlagOpenCamera = false;
    bool WetCamera_window;
    cv::VideoCapture capture;

    //定义模型文件
    std::string ModelBinary = "../res/detector/opencv_face_detector_uint8.pb";
    std::string ModelDesc = "../res/detector/opencv_face_detector.pbtxt";
    dnnfacedetect *fdetect;

    ImCamera();

    virtual ~ImCamera();

    void CreateCameraTexture(cv::VideoCapture capture);

    void draw();

    void startCamera();
};


#endif //OGL_IMCAMERA_H
