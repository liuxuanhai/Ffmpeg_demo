package jbox2d.example.com.ffmpeg_demo.utils;

import android.view.Surface;

/**
 * @Author: Ycl
 * @Date: 2018/6/19 22:51
 * @Desc: 视频播放的控制器
 */
public class JasonPlayer {
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
    }

}
