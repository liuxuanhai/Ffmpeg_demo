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
#include "include/libswresample/swresample.h"
#include "include/libavutil/channel_layout.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFmpeg",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFmpeg",FORMAT,##__VA_ARGS__);

// nb_stream  视频文件中存在，音频流，视频流，字幕
#define MAX_STREAM 2

//音频解码 采样率 新版版可达48000 * 4 (32位pcm数据)
#define MAX_AUDIO_FRME_SIZE   48000 * 4


struct Player {
    JavaVM *javaVM;

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

    // ----------------- 音频 start ------------
    //采样格式
    enum AVSampleFormat *in_sample_fmt;
    enum AVSampleFormat *out_sample_fmt;
    // 采样率
    int in_sample_rate;
    int out_sample_rate;
    //输出的声道个数
    int out_channel_nb;

    struct SwsContext *swrCtx;
    // ----------------- 音频 end ------------
    jobject audio_track;
    jmethodID audio_track_write_mid;
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
    // 未赋值 导致异常使用adde2line定位
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

//    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    av_frame_free(&rgb_frame);
}

void jni_audio_prepare(JNIEnv *env, jobject jobj, struct Player *pPlayer) {
    //JNI begin------------------
    jclass cls = (*env)->FindClass(env, "jbox2d.example.com.ffmpeg_demo.utils.AudioUtil");
    jmethodID constructor_mid = (*env)->GetMethodID(env, cls, "<init>", "()V");
    //实例化一个AudioUtil对象(可以在constructor_mid后加参)
    jobject audioutil_obj = (*env)->NewObject(env, cls, constructor_mid);

    // AudioTrack对象
    jmethodID create_audio_track_mid = (*env)->GetMethodID(env, cls, "createAudioTrack",
                                                           "(II)Landroid/media/AudioTrack");
    jobject audio_track = (*env)->CallObjectMethod(env, audioutil_obj, create_audio_track_mid,
                                                   pPlayer->out_sample_rate,
                                                   pPlayer->out_channel_nb);
    // 调用AudioTrack.play方法
    jclass audio_track_class = (*env)->GetObjectClass(env, audio_track);
    jmethodID audio_track_play_mid = (*env)->GetMethodID(env, audio_track_class, "play", "()V");
    (*env)->CallVoidMethod(env, audio_track, audio_track_play_mid);

    // AudioTrack.write
    jmethodID audio_track_write_mid = (*env)->GetMethodID(env, audio_track_class, "write",
                                                          "([BII)I");
    //JNI end------------------
    // 存储数据
    pPlayer->audio_track = (*env)->NewGlobalRef(env,audio_track); // 此处对象必须保存为全局引用，子线程才能使用
    pPlayer->audio_track_write_mid = audio_track_write_mid;
    // 释放
    (*env)->DeleteLocalRef(env, audioutil_obj);
}

/**
 * 音频解码准备
 */
void decode_audio_prepare(struct Player *pPlayer) {
    AVCodecContext *codec_ctx = pPlayer->input_codec_ctx[pPlayer->audio_stream_index];

    //重采样设置参数-------------start
    //frame->16bit  44100 PCM 统一音频采样格式与采样率
    // 格式
    enum AVSampleFormat *in_sample_fmt = codec_ctx->sample_fmt;
    enum AVSampleFormat *out_sample_fmt = AV_SAMPLE_FMT_S16;
    // 采样率
    int in_sample_rate = codec_ctx->sample_rate;
    int out_sample_rate = 44100;

    uint64_t in_ch_layout = codec_ctx->channel_layout;
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    //frame->16bit 44100 PCM 统一音频采样格式与采样率
    struct SwsContext *swrCtx = swr_alloc();
    swr_alloc_set_opts(swrCtx,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       out_ch_layout, out_sample_fmt, out_sample_rate,
                       0, NULL); // 日志级别 ，父日志上下文
    swr_init(swrCtx);

    // 输入输出的声道个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    LOGI("out_count:%d", out_channel_nb);
    //重采样设置参数-------------end

    pPlayer->in_sample_fmt = in_sample_fmt;
    pPlayer->out_sample_fmt = out_sample_fmt;
    pPlayer->in_sample_rate = in_sample_rate;
    pPlayer->out_sample_rate = out_sample_rate;
    pPlayer->out_channel_nb = out_channel_nb;
    pPlayer->swrCtx = swrCtx;
}

/**
 * 解码音频
 */
void decode_audio(struct Player *pPlayer, AVPacket *pPacket) {
    AVCodecContext *codec_ctx = pPlayer->input_codec_ctx[pPlayer->audio_stream_index];

    //解压缩数据
    AVFrame *frame = av_frame_alloc();
    int got_frame = 0, ret;
    ret = avcodec_decode_audio4(codec_ctx, frame, &got_frame, pPacket);
    if (ret < 0) {
        LOGI("%s", "解码完成");
        return;
    }

    // 16bit 44100 PCM 数据（重采样缓冲区）
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);
    //非0，正在解码
    if (got_frame > 0) {
        // 重新采样
        swr_convert(pPlayer->swrCtx, &out_buffer, MAX_AUDIO_FRME_SIZE,
                    (const uint8_t **) frame->data, frame->nb_samples);
        // 获取sample的size
        int out_buffer_size = av_samples_get_buffer_size(NULL,
                                                         pPlayer->out_channel_nb, frame->nb_samples,
                                                         pPlayer->out_sample_fmt, 1);
        // 关联当前线程的JNIenv
        JavaVM *javaVM = pPlayer->javaVM;
        JNIEnv *env;
        (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);


        // out_buffer缓冲区的数据，转换成bute数组
        jbyteArray audio_sample_array = (*env)->NewByteArray(env, out_buffer_size);
        jbyte *sample_byte = (*env)->GetByteArrayElements(env, audio_sample_array, NULL);
        // 将out_buffer的数据复制到sample_byte
        memcpy(sample_byte, out_buffer, out_buffer_size);

        (*env)->ReleaseByteArrayElements(env, audio_sample_array, sample_byte, 0);
        (*env)->CallIntMethod(env, pPlayer->audio_track, pPlayer->audio_track_write_mid,
                              audio_sample_array,
                              0, out_buffer_size);
        //释放引用
        (*env)->DeleteLocalRef(env, audio_sample_array);
//        (*env)->DeleteGlobalRef(env,  pPlayer->audio_track);

        (*javaVM)->DetachCurrentThread(javaVM);

        usleep(1000 * 16); //16ms
    }
    av_frame_free(&frame);
//    av_free(out_buffer);
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
        } else if (packet->stream_index == player->audio_stream_index) {
            decode_audio(player, packet);
            LOGI("video_frame_count:%d", video_frame_count++);
        }
        //释放资源
        av_free_packet(packet);
    }

    // 释放资源
    ANativeWindow_release(player->nativeWindow);
    avcodec_close(player->input_codec_ctx);
    avformat_close_input(input_format_ctx);
    avformat_free_context(input_format_ctx);
}


JNIEXPORT void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_VideoUtils_play(JNIEnv *env, jobject jobj,
                                                           jstring input_jstr,
                                                           jobject surface) {
    //需要转码的视频文件(输入的视频文件)
    const char *input_cstr = (*env)->GetStringUTFChars(env, input_jstr, NULL);

    //初始化封装格式上下文
    struct Player *player = (struct Player *) malloc(sizeof(struct Player));
    (*env)->GetJavaVM(env, &(player->javaVM));

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
    decode_audio_prepare(player);
    jni_audio_prepare(env, jobj, player);

    //创建子线程解码视频
    pthread_create(&(player->decode_threads[video_stream_index]), NULL, decode_data,
                   (void *) player);
    //创建子线程解码音频
    pthread_create(&(player->decode_threads[audio_stream_index]), NULL, decode_data,
                   (void *) player);


//    (*env)->ReleaseStringUTFChars(env, input_jstr, input_cstr);
//    free(player);
}