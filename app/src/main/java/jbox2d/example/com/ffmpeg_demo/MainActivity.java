package jbox2d.example.com.ffmpeg_demo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import java.io.File;

import jbox2d.example.com.ffmpeg_demo.utils.JasonPlayer;
import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;
import jbox2d.example.com.ffmpeg_demo.utils.VideoView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("native-lib");
    }

    private Button button;
    private Button button_start;
    private VideoView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }


    private void initView() {
        button = (Button) findViewById(R.id.button);
        button_start = (Button) findViewById(R.id.button_start);
        surfaceView = (VideoView) findViewById(R.id.surfaceView);

        button.setOnClickListener(this);
        button_start.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button:
                // 编码
                mDecode(v);
                break;
            case R.id.button_start:
                // 渲染
                mRender(v);
                break;
        }
    }


    public void mDecode(View btn) {
        String input = new File(Environment.getExternalStorageDirectory(), "小苹果.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "小苹果_out.yuv").getAbsolutePath();
        VideoUtils.decode(input, output);
    }

    public void mRender(View btn) {
        String input = new File(Environment.getExternalStorageDirectory(), "小苹果.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "小苹果_out.yuv").getAbsolutePath();
        JasonPlayer jasonPlayer = new JasonPlayer();
//        jasonPlayer.render(input, surfaceView.getsu);
    }

    public native String stringFromJNI();
}
