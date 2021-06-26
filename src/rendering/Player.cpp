//
// Created by Nemo li on 2021/6/24.
//

#include <glad/glad.h>
#include <helpers/RootDir.h>
#include <string>
#include "Player.h"

Uint8 *myStream = nullptr;
SDL_atomic_t myLen = {0};
SDL_atomic_t hasAudioAtomic = {0};

void sdlAudioCallback(void *userData, Uint8 *stream, int len) {
    if (!SDL_AtomicGet(&hasAudioAtomic)) {
        memset(stream, 0, len);
    } else {
        myStream = stream;
        SDL_AtomicSet(&myLen, len);
    }
}

void Player::initVideo() {
    //网络流视频初始化
    avformat_network_init();
    avFormatContext = avformat_alloc_context();

    std::string path = "res/other/killer.mp4";
    if (avformat_open_input(&avFormatContext,
//                            (ROOT_DIR + path).c_str(),
                            "http://mytianh5.oss-cn-beijing.aliyuncs.com/website/video/zaojiaoji.mp4",
                            nullptr, nullptr)) {
        return;
    }
    if (avformat_find_stream_info(avFormatContext, nullptr) < 0) {
        return;
    }
    //定义video和audio的参数变量
    AVCodecParameters *videoCodecParameters = nullptr;
    AVRational videoAVRational = {1, 50};
    AVCodecParameters *audioCodecParmeters = nullptr;

    //遍历取出video的codecParameter 以及audio的codecParameters
    for (int index = 0; index < avFormatContext->nb_streams; index++) {
        AVCodecParameters *codecParmeters = avFormatContext->streams[index]->codecpar;
        if (codecParmeters->codec_type == AVMEDIA_TYPE_VIDEO
            && nullptr == videoCodecParameters) {
            videoCodecParameters = codecParmeters;
            videoAVRational = avFormatContext->streams[index]->time_base;
        } else if (codecParmeters->codec_type == AVMEDIA_TYPE_AUDIO
                   && nullptr == audioCodecParmeters) {
            audioCodecParmeters = codecParmeters;
        }
    }

    if (nullptr == videoCodecParameters || nullptr == audioCodecParmeters) {
        return;
    }
    //找出video和audio 的解码器
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecParameters->codec_id);
    AVCodec *audioCodec = avcodec_find_decoder(audioCodecParmeters->codec_id);
    //分配上下文的空间
    avCodecContext = avcodec_alloc_context3(videoCodec);
    avAudioCodecContext = avcodec_alloc_context3(audioCodec);
    //parameters转化为上下文
    avcodec_parameters_to_context(avAudioCodecContext, audioCodecParmeters);
    avcodec_parameters_to_context(avCodecContext, videoCodecParameters);

    sws_context = sws_getContext(avCodecContext->width,
                                 avCodecContext->height,
                                 avCodecContext->pix_fmt,
                                 avCodecContext->width,
                                 avCodecContext->height, AV_PIX_FMT_RGB24,
                                 SWS_BILINEAR, nullptr, nullptr, nullptr);

    swrContext = swr_alloc();
    swr_alloc_set_opts(swrContext, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16,
                       avAudioCodecContext->sample_rate,
                       av_get_default_channel_layout(avAudioCodecContext->channels),
                       avAudioCodecContext->sample_fmt, avAudioCodecContext->sample_rate, 0,
                       nullptr);
    swr_init(swrContext);
    out_buffer_size = av_samples_get_buffer_size(nullptr, av_get_channel_layout_nb_channels(
            AV_CH_LAYOUT_MONO), 1024, AV_SAMPLE_FMT_S16, 1);
    out_buffer = (uint8_t *) av_malloc(out_buffer_size);
    SDL_AudioSpec audioSpec;
    audioSpec.freq = avAudioCodecContext->sample_rate;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
    audioSpec.samples = 1024;
    audioSpec.callback = sdlAudioCallback;
    SDL_OpenAudio(&audioSpec, nullptr);
    SDL_PauseAudio(0);
    if (avcodec_open2(avCodecContext, videoCodec, nullptr) ||
        avcodec_open2(avAudioCodecContext, audioCodec, nullptr)) {
        return;
    }
    pkt = av_packet_alloc();
    fra = av_frame_alloc();
    av_image_alloc(pointers, linesizes, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGB24, 1);

}

void Player::loadShader() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenTextures(1, &texture);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex texture
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    // vertex position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindVertexArray(0);
}

void Player::play() {
    if (!av_read_frame(avFormatContext, pkt)) {
        if (pkt->stream_index == 0) {
            if (!avcodec_send_packet(avCodecContext, pkt)) {
                while (!avcodec_receive_frame(avCodecContext, fra)) {
                    sws_scale(sws_context, (const uint8_t *const *) fra->data,
                              fra->linesize, 0, fra->height, pointers, linesizes);
//                    glBindTexture(GL_TEXTURE_2D, texture);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fra->width, fra->height, 0,
                                 GL_RGB, GL_UNSIGNED_BYTE, pointers[0]);
                    av_frame_unref(fra);

                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) 0);
                }
            } else {
                if (!avcodec_send_packet(avAudioCodecContext, pkt)) {
                    while (!avcodec_receive_frame(avAudioCodecContext, fra)) {
                        swr_convert(swrContext, &out_buffer, out_buffer_size,
                                    (const uint8_t **) fra->data, fra->nb_samples);
                        av_frame_unref(fra);
                        SDL_AtomicSet(&hasAudioAtomic, 1);
                        while (0 >= SDL_AtomicGet(&myLen)) {

                        }
                        memcpy(myStream, out_buffer, SDL_AtomicGet(&myLen));
                        SDL_AtomicSet(&myLen, 0);
                    }
                }
            }
            av_packet_unref(pkt);
        }
    }
}

void Player::release() {
    av_free(pointers[0]);
    av_packet_free(&pkt);
    av_frame_free(&fra);
    avcodec_close(avCodecContext);
    avcodec_free_context(&avCodecContext);
    avcodec_close(avAudioCodecContext);
    avcodec_free_context(&avAudioCodecContext);
    sws_freeContext(sws_context);
    av_free(out_buffer);
    swr_free(&swrContext);
    avformat_free_context(avFormatContext);
}
