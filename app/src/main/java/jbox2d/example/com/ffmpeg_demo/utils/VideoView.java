package jbox2d.example.com.ffmpeg_demo.utils;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * @Author: Ycl
 * @Date: 2018/6/19 22:49
 * @Desc: 视频绘制的画布
 */
public class VideoView extends SurfaceView {
    public VideoView(Context context) {
        super(context);
        init();
    }

    public VideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        // 初始化，SurfaceView 绘制的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);



    }
}
