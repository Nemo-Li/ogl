//
// Created by Nemo li on 2021/12/11.
//

#include "ImCamera.h"

void ImCamera::CreateCameraTexture(cv::VideoCapture capture) {
    cv::Mat image;
    capture >> image;
    if (image.empty()) {
        printf("Can not load Cameras\n");
    } else {
        vector<Mat> dst = fdetect->detectRect(image);
        if (!dst.empty()) {
            cv::Mat result = dst[0];
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
}

void ImCamera::draw() {
    if (WetCamera_window) {
        // //设置窗口的padding为0是图片控件充满窗口
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        //设置窗口的边框大小为0
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        // 改变窗口的背景颜色
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 100, 0.3));

        ImGui::Begin("WetCamera_Window", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);

        if (ImGui::Button(u8"open camera")) {
            startCamera();
            FlagOpenCamera = true;
        }

        ImGui::SameLine();
        if (ImGui::Button(u8"close camera")) {
            capture.release();
            FlagOpenCamera = false;
        }

        if (FlagOpenCamera) {
            CreateCameraTexture(capture);
        }

        ImTextureID image_id = reinterpret_cast<ImTextureID>(cameraTextureId);
        ImGui::Image(image_id, ImGui::GetContentRegionAvail());

        ImGui::End();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    }
}

void ImCamera::startCamera() {
    capture = cv::VideoCapture(0);
    // CreateCameraTexture(capture);
    // 使用GL指令生成贴图，获取贴图ID
    glGenTextures(1, &cameraTextureId);
}

ImCamera::~ImCamera() {
}

ImCamera::ImCamera() {
    fdetect = new dnnfacedetect(ModelBinary, ModelDesc);
    fdetect->initdnnNet();
}
