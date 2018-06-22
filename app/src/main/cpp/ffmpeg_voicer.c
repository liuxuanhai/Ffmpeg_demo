//
// Created by Administrator on 2018/6/22.
//
#include <jni.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
//#include <android/native_window_jni.h>
//#include <android/native_window.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
#include "include/libswscale/swscale.h"
#include "include/libswresample/swresample.h"
#include "include/libavutil/frame.h"


#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFmpeg",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFmpeg",FORMAT,##__VA_ARGS__);


extern "C"
JNIEXPORT void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_VideoUtils_audioDecode(JNIEnv *env,
                                                                  jobject jobj,
                                                                  jstring input_jstr,
                                                                  jstring output_jstr) {

    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    const char *output_cstr = (*env)->GetStringUTFChars(env, output_jstr, NULL);

    //注册组件
    av_register_all();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //打开音频文件
    if (avformat_open_input(pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGI("%s", "无法打开音频文件");
        return;
    }
    //获取输入文件信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGI("%s", "无法获取输入文件信息");
        return;
    }
    //获取音频流索引位置
    int audio_stream_idx = 0;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = i;
            break;
        }
    }
    if (audio_stream_idx == -1) {
        LOGI("%s", "找不到音频流");
        return;
    }
    //获取解码器
    AVCodecContext *pCodeCtx = pFormatCtx->streams[audio_stream_idx]->codec;
    AVCodec *codec = avcodec_find_decoder(pCodeCtx->codec_id);
    if (codec == NULL) {
        LOGI("%s", "无法获取加码器");
        return;
    }
    if (avcodec_open2(pCodeCtx, codec, NULL) < 0) {
        LOGI("%s", "无法打开解码器");
        return;
    }


    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));


    struct SwsContext *swrCtx = swr_alloc();
    //frame->16bit  44100 PCM 统一音频采样格式与采样率
    AVFrame *frame = av_frame_alloc();
    //重采样设置参数 ----------  start
    //输入采样率格式
    enum AVSampleFormat in_sample_fmt = pCodeCtx->sample_fmt;
    //输出采样率格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    int in_sample_rate = pCodeCtx->sample_rate;
    //输出采样率
    int out_sample_rate = 44100;
    // 获取输入的声音布局
    //根据声道个数获取默认的声道布局(2个声道，默认立体声)
    uint64_t in_ch_layout = pCodeCtx->channel_layout;
    //输出的声道布局
    uint64_t out_ch_layout = AV_CODEC_ID_LAGARITH;

    swr_alloc_set_opts(swrCtx,
                       out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);
    swr_init(swrCtx);

    int out_channel_nb=av_get_channel_layout_nb_channels(out_ch_layout);
    LOGI("out_count:%d", out_channel_nb);
    //重采样设置参数--------------end







    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_cstr);
}