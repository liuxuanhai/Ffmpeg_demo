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
#include <pthread.h>
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
    // 绘制window
    ANativeWindow *nativeWindow;
    //解码线程pid
    pthread_t decode_threads[MAX_STREAM];
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

void decode_video_prepare(JNIEnv *env, struct Player *pPlayer, jobject surface) {
    // native绘制
    pPlayer->nativeWindow = ANativeWindow_fromSurface(env, surface);
}

/**
 * 解码视频
 */
void decode_video(struct Player *pPlayer, AVPacket *pPacket) {
    //像素数据（解码数据）
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    if (rgb_frame == NULL || yuv_frame == NULL) {
        LOGE("Could not allocate video frame.");
        return;
    }

    AVCodecContext *codec_ctx = pPlayer->input_codec_ctx[pPlayer->video_stream_index];
    ANativeWindow_Buffer outBuffer;

    int got_picture, ret;
    //7.解码一帧视频压缩数据 ,解码AVPacket->AVFrame
    ret = avcodec_decode_video2(codec_ctx, yuv_frame, &got_picture, pPacket);
    if (ret < 0) {
        LOGE("%s", "解码错误");
        return;
    }
    // 为0说明解码完成，非0正在解码
    if (got_picture) {
        //lock
        //设置缓冲区的属性（宽、高、像素格式）
        ANativeWindow_setBuffersGeometry(pPlayer->nativeWindow,
                                         codec_ctx->width,
                                         codec_ctx->height,
                                         WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_lock(pPlayer->nativeWindow, &outBuffer, 0);

        //设置rgb_frame的属性（像素格式、宽高）和缓冲区
        //rgb_frame缓冲区与outBuffer.bits是同一块内存
        avpicture_fill((AVPicture *) rgb_frame, outBuffer.bits, AV_PIX_FMT_RGBA,
                       codec_ctx->width, codec_ctx->height);
        //YUV->RGBA_8888
        I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                   yuv_frame->data[2], yuv_frame->linesize[2],
                   yuv_frame->data[1], yuv_frame->linesize[1],
                   rgb_frame->data[0], rgb_frame->linesize[0],
                   codec_ctx->width, codec_ctx->height);

        //unlock
        ANativeWindow_unlockAndPost(pPlayer->nativeWindow);
        usleep(1000 * 16);
    }
}

/**
 * 解码子线程函数
 */
void *decode_data(void *arg) {
    struct Player *player = (struct Player *) arg;
    AVFormatContext *input_format_ctx = player->input_format_ctx;

    //准备读取
    //AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    int video_frame_count = 0;
    // 6 . 一帧一帧的读取压缩数据
    while (av_read_frame(input_format_ctx, packet) >= 0) {

        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == player->video_stream_index) {
            decode_video(player, packet);
            LOGI("video_frame_count:%d", video_frame_count++);
        }
        //释放资源
        av_free_packet(packet);
    }
}


JNIEXPORT void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_VideoUtils_play(JNIEnv *env, jobject instance,
                                                           jstring input_jstr,
                                                           jobject surface) {
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

    decode_video_prepare(env, player, surface);

    //创建子线程解码
    pthread_create(player->decode_threads[player->video_stream_index], NULL, decode_data,
                   (void *) player);

    // 还未释放资源
    /* ANativeWindow_release(nativeWindow);
     av_frame_free(&pFrame);// 释放内存
     avcodec_close(codec_ctxCtx);
     avformat_close_input(input_format_ctx);
     avformat_free_context(input_format_ctx);
     */
    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    free(player);
}