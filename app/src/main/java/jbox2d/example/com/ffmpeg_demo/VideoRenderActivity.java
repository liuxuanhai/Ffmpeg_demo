package jbox2d.example.com.ffmpeg_demo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

import java.io.File;

import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;
import jbox2d.example.com.ffmpeg_demo.utils.VideoView;

public class VideoRenderActivity extends AppCompatActivity implements View.OnClickListener {

    private VideoView surfaceView;
    private Spinner sp_video;
    private Button start;

    private VideoUtils videoUtils;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_render);
        initView();
    }

    private void initView() {
        surfaceView = (VideoView) findViewById(R.id.surfaceView);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder surfaceHolder) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        mRender("小苹果.mp4");
                    }
                }).start();
            }

            @Override
            public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
                surfaceHolder.getSurface().release();
            }
        });


        sp_video = (Spinner) findViewById(R.id.sp_video);
        //多种格式的视频列表
        String[] videoArray = getResources().getStringArray(R.array.video_list);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                android.R.id.text1, videoArray);
        sp_video.setAdapter(adapter);


        start = (Button) findViewById(R.id.start);
        start.setOnClickListener(this);

        videoUtils = new VideoUtils();
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.start:
                mRender(sp_video.getSelectedItem().toString());
                break;
        }
    }


    public void mRender(String path) {
        String input = new File(Environment.getExternalStorageDirectory(), path).getAbsolutePath();
        videoUtils.render(input, surfaceView.getHolder().getSurface());
    }
}
