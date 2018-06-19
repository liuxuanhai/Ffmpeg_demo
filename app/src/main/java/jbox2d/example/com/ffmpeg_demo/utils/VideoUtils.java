package jbox2d.example.com.ffmpeg_demo.utils;

/**
 * @Author: Ycl
 * @Date: 2018-06-08 19:54
 * @Desc:
 */
public class VideoUtils {

    public native static void decode(String input, String output);

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
    }
}
