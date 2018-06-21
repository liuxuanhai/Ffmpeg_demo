package jbox2d.example.com.ffmpeg_demo.utils;

import android.view.Surface;

/**
 * @Author: Ycl
 * @Date: 2018-06-08 19:54
 * @Desc:
 */
public class VideoUtils {

    public native static void decode(String input, String output);

    /**
     * 1.lock window
     * 2. 缓冲器赋值
     * 3.unlock window
     */
    public native void render(String input, Surface surface);

    static {
        System.loadLibrary("libavcodec-56");
        System.loadLibrary("libavdevice-56");
        System.loadLibrary("libavfilter-5");
        System.loadLibrary("libavformat-56");
        System.loadLibrary("libavutil-54");
        System.loadLibrary("libmyffmpeg");
        System.loadLibrary("libpostproc-53");
        System.loadLibrary("libswresample-1");
        System.loadLibrary("libswscale-3");
        System.loadLibrary("libyuv");
    }
}
