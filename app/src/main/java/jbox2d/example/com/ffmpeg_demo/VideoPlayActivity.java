package jbox2d.example.com.ffmpeg_demo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;
import java.util.logging.Logger;

import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;

public class VideoPlayActivity extends AppCompatActivity {
    private static final String TAG = "VideoPlayActivity";
    private SurfaceView video_view;
    private VideoUtils player;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_play);
        init();
        initView();
    }

    private void init() {
        player = new VideoUtils();
    }

    private void initView() {
        video_view = (SurfaceView) findViewById(R.id.video_view);
        video_view.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(final SurfaceHolder surfaceHolder) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        String input = new File(Environment.getExternalStorageDirectory(),"告白气球.avi").getAbsolutePath();
                        Log.i(TAG, "run: "+input);
                        player.play(input, surfaceHolder.getSurface());
                    }
                }).start();
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

            }
        });
    }
}
