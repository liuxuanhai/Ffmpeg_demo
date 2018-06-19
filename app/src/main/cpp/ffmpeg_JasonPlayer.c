//
// Created by Administrator on 2018/6/19.
//
#include <jni.h>


extern "C"
JNIEXPORT void JNICALL
Java_jbox2d_example_com_ffmpeg_1demo_utils_JasonPlayer_render(JNIEnv *env, jobject instance,
                                                              jstring input_, jobject surface) {
    const char *input = (*env)->GetStringUTFChars(env, input_, 0);


    (*env)->ReleaseStringUTFChars(env, input_, input);
}