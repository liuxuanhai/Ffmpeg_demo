//
// Created by Administrator on 2018/6/19.
//
#include <jni.h>
#include <string.h>


//解码
//封装格式处理
#include "include/libavformat/avformat.h"
//像素处理
#include "include/libswscale/swscale.h"

#include "include/libavutil/frame.h"
#include "include/libavcodec/avcodec.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFmpeg",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFmpeg",FORMAT,##__VA_ARGS__);


extern "C"
JNIEXPORT void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_JasonPlayer_render(JNIEnv *env, jobject instance,
                                                              jstring input_jstr, jobject surface) {
    //需要转码的视频文件(输入的视频文件)
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);

    // 1. 注册所有组件
    av_register_all();

    // 封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //2. 打开输入视频文件
    if (avformat_open_input(pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGE("%s", "无法打开输入视频文件");
        return;
    }

    // 3. 获取视频文件信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "无法获取视频文件信息");
        return;
    }

    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
    int v_stream_idx = -1;
    int i = 0;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_idx = i;
            break;
        }
    }

    if (-1 == v_stream_idx) {
        LOGE("%s", "找不到视频流\n");
        return;
    }

    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码上下文
    AVCodecContext *pCodecCtx = pFormatCtx->streams[v_stream_idx]->codec;
    //4.根据编解码上下文中的编码id查找对应的解码
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //（迅雷看看，找不到解码器，临时下载一个解码器）
    if (NULL == pCodec) {
        LOGE("%s", "找不到解码器\n");
        return;
    }

    /*   avcodec_register_all();
       AVCodec *pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
       AVCodecContext *pCodecCtx =avcodec_alloc_context3(pCodec);*/

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开\n");
        return;
    }

    // 输出视频信息
    LOGI("视频的文件格式 %s", pFormatCtx->iformat->name);
    LOGI("视频时长 %lld", (pFormatCtx->duration) / 1000000);
    LOGI("视频的宽高：%d,%d", pCodecCtx->width, pCodecCtx->height);
    LOGI("解码器的名称：%s", pCodec->name);
























    avcodec_close(pCodecCtx);
    avformat_close_input(pFormatCtx);
    avformat_free_context(pFormatCtx);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
}