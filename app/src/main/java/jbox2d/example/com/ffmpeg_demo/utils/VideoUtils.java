package jbox2d.example.com.ffmpeg_demo.utils;

import android.view.Surface;

/**
 * @Author: Ycl
 * @Date: 2018-06-08 19:54
 * @Desc:
 */
public class VideoUtils {
    //视频解码
    public native static void decode(String input, String output);

    //视频播放
    public native void render(String input, Surface surface);

    //音频解码
    public native void audioDecode(String input, String output);

    //音频播放
    public native void audioPlayer(String input );

    //音视频同步播放
    public native void play(String input , Surface surface);


    static {
        System.loadLibrary("avcodec-58");
        System.loadLibrary("avdevice-58");
        System.loadLibrary("avfilter-7");
        System.loadLibrary("avformat-58");
        System.loadLibrary("avutil-56");
        System.loadLibrary("postproc-55");
        System.loadLibrary("swresample-3");
        System.loadLibrary("swscale-5");
        System.loadLibrary("yuv");
    }
}
