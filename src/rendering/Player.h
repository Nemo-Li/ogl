//
// Created by Nemo li on 2021/6/24.
//

#ifndef OGL_PLAYER_H
#define OGL_PLAYER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <pthread.h>
#include <unistd.h>
#include <SDL2/SDL.h>
}

class Player {
private:
    AVFormatContext *avFormatContext;
    AVCodecContext *avCodecContext;
    AVCodecContext *avAudioCodecContext;
    SwsContext *sws_context;
    SwrContext *swrContext;
    int out_buffer_size;
    uint8_t *out_buffer;
    AVPacket *pkt;
    AVFrame *fra;

    uint8_t *pointers[4];
    int linesizes[4];

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint texture;

    const GLint indices[6] = {
            //前
            0, 2, 4,
            2, 4, 6,
    };

    const float vertices[40] = {
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

            1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 1.0f, 0.0f
    };


public:
    void initVideo();

    void loadShader();

    void play();

    void release();
};


#endif //OGL_PLAYER_H
