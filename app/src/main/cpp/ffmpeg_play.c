//
// Created by yanchunlan on 2018/6/29.
//
#include <jni.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
// native_window
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "libyuv/libyuv.h"


//解码
//封装格式处理
#include "include/libavformat/avformat.h"
//像素处理
#include "include/libswscale/swscale.h"

#include "include/libavutil/frame.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/imgutils.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFmpeg",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFmpeg",FORMAT,##__VA_ARGS__);

// nb_stream  视频文件中存在，音频流，视频流，字幕
#define MAX_STREAM 2

struct Player {
    //封装格式上下文
    AVFormatContext *input_format_ctx;
    //音频视频流索引位置
    int video_stream_index;
    int audio_stream_index;
    AVCodecContext *input_codec_ctx[MAX_STREAM];
};


/**
 * 初始化封装格式上下文，获取音频视频流的索引位置
 */
void init_input_format_ctx(struct Player *pPlayer, const char *input_cstr) {
    // 1. 注册所有组件
    av_register_all();

    // 封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVFormatContext *input_format_ctx = avformat_alloc_context();

    //2. 打开输入视频文件
    if (avformat_open_input(&input_format_ctx, input_cstr, NULL, NULL) != 0) {
        LOGE("%s", "无法打开输入视频文件");
        return;
    }

    // 3. 获取视频文件信息
    if (avformat_find_stream_info(input_format_ctx, NULL) < 0) {
        LOGE("%s", "无法获取视频文件信息");
        return;
    }

    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
    int i;
    for (i = 0; i < input_format_ctx->nb_streams; ++i) {
        if (input_format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            pPlayer->video_stream_index = i;
        } else if (input_format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            pPlayer->audio_stream_index = i;
        }
    }

    pPlayer->input_format_ctx = input_format_ctx;
}

/**
 * 初始化解码器上下文
 */
void init_code_ctx(struct Player *pPlayer, int stream_idx) {

    AVFormatContext *input_format_ctx = pPlayer->input_format_ctx;

    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码上下文
    AVCodecContext *codec_ctxCtx = input_format_ctx->streams[stream_idx]->codec;
    //4.根据编解码上下文中的编码id查找对应的解码
    AVCodec *codec_ctx = avcodec_find_decoder(codec_ctxCtx->codec_id);
    //（迅雷看看，找不到解码器，临时下载一个解码器）
    if (NULL == codec_ctx) {
        LOGE("%s", "找不到解码器\n");
        return;
    }

    /*   avcodec_register_all();
       AVCodec *codec_ctx = avcodec_find_decoder(AV_CODEC_ID_H264);
       AVCodecContext *codec_ctxCtx =avcodec_alloc_context3(codec_ctx);*/

    if (avcodec_open2(codec_ctxCtx, codec_ctx, NULL) < 0) {
        LOGE("%s", "解码器无法打开\n");
        return;
    }

    pPlayer->input_codec_ctx[stream_idx] = codec_ctx;
}

/**
 * 输出打印信息
 */
void log_video_audio_info(struct Player *pPlayer) {
    // 输出视频信息
    LOGI("视频的文件格式 %s", pPlayer->input_format_ctx->iformat->name);
    LOGI("视频时长 %lld", (pPlayer->input_format_ctx->duration) / 1000000);
    LOGI("视频的宽高：%d,%d", pPlayer->input_codec_ctx[pPlayer->video_stream_index]->width,
         pPlayer->input_codec_ctx[pPlayer->video_stream_index]->height);
//    LOGI("解码器的名称：%s",  pPlayer->input_codec_ctx[stream_idx]->name);
}

JNIEXPORT  void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_VideoUtils_play(JNIEnv *env, jobject instance,
                                                           jstring input_jstr, jobject surface) {
    //需要转码的视频文件(输入的视频文件)
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);

    //初始化封装格式上下文
    struct Player *player = (struct Player *) malloc(sizeof(struct Player));
    init_input_format_ctx(player, input_cstr);

    int video_stream_index = player->video_stream_index;
    int audio_stream_index = player->audio_stream_index;
    if (-1 == video_stream_index) {
        LOGE("%s", "找不到视频流\n");
        return;
    }
    if (-1 == audio_stream_index) {
        LOGE("%s", "找不到音频流\n");
        return;
    }
    // 获取音视频解码器，并打开
    init_code_ctx(player, video_stream_index);
    init_code_ctx(player, audio_stream_index);
    //  输出打印信息
    log_video_audio_info(player);


    //准备读取
    //AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
   /* AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    //AVFrame用于存储解码后的像素数据(YUV)
    //内存分配
    AVFrame *pFrame = av_frame_alloc();
    // 后面的数据读取之后，解码之后需要转换格式，YUV-RGBA
    // 渲染部分
    AVFrame *pFrame_RGBA = av_frame_alloc();
    if (pFrame_RGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return;
    }

    // native绘制
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = codec_ctxCtx->width;
    int videoHeight = codec_ctxCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    // 贞的缓冲区与其一致，操作缓冲区域就ok了
    ANativeWindow_Buffer outBuffer;



    // buffer 中的数据就是用于渲染的，且格式为RGBA
    //    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, codec_ctxCtx->width, codec_ctxCtx->height, 1);
    //    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    *//*struct SwsContext *swsContext=sws_getContext(codec_ctxCtx->width,
                                                 codec_ctxCtx->height,
                                                 codec_ctxCtx->pix_fmt,
                                                 codec_ctxCtx->width,
                                                 codec_ctxCtx->height,
                                                 AV_PIX_FMT_RGBA,
                                                 SWS_BILINEAR,
                                                 NULL,
                                                 NULL,
                                                 NULL);*//*


    // argus
    int got_picture, ret, frameCount = 0;

    // 6 . 一帧一帧的读取压缩数据
    while (av_read_frame(input_format_ctx, packet) >= 0) {

        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == v_stream_idx) {

            //7.解码一帧视频压缩数据，得到视频像素数据
            ret = avcodec_decode_video2(codec_ctxCtx, pFrame, &got_picture, packet);
            if (ret < 0) {
                LOGE("%s", "解码错误");
                return;
            }
            // 为0说明解码完成，非0正在解码
            if (got_picture) {
                LOGI("%s", frameCount++);

                *//**
                * 1.lock window
                * 2. 缓冲器赋值
                * 3.unlock window
                *//*
                // lock
                ANativeWindow_lock(nativeWindow, &outBuffer, 0);

                //pFrame_RGBA 设置其属性（像素格式，宽高）和缓冲区
                avpicture_fill((AVPicture *) pFrame_RGBA, outBuffer.bits, AV_PIX_FMT_RGBA,
                               codec_ctxCtx->width, codec_ctxCtx->height);

                // fix buffer  格式转换
                *//*     sws_scale(swsContext,(uint8_t const *const *)pFrame->data,
                               pFrame->linesize,0,codec_ctxCtx->height,
                               pFrame_RGBA->data,pFrame_RGBA->linesize);*//*

                //设置rgb_frame的属性（像素格式、宽高）和缓冲区
                //rgb_frame缓冲区与outBuffer.bits是同一块内存
                //  https://chromium.googlesource.com/external/libyuv
                I420ToARGB(pFrame->data[0], pFrame->linesize[0],
                           pFrame->data[2], pFrame->linesize[2],
                           pFrame->data[1], pFrame->linesize[1],
                           pFrame_RGBA->data[0], pFrame_RGBA->linesize[0],
                           codec_ctxCtx->width, codec_ctxCtx->height);

                //unlock  绘制上去了
                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);  // 不能一直播放，还是需要休眠的，防止频率太高
            }
        }
        //释放资源
        av_free_packet(packet);
        //        av_packet_unref(&packet);
    }

    ANativeWindow_release(nativeWindow);

    av_frame_free(&pFrame);// 释放内存
    avcodec_close(codec_ctxCtx);
    avformat_close_input(input_format_ctx);
    avformat_free_context(input_format_ctx);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    free(player);*/
}