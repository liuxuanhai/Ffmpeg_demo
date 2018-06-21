package jbox2d.example.com.ffmpeg_demo;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;

import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button btn_decode;
    private Button btn_render;
    private Button btn_audio_decode;
    private Button btn_audio_player;
    private Button btn_play;
    private Button btn_transcoding_compress;
    private Button btn_addWatermark;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }

    private void initView() {
        btn_decode = (Button) findViewById(R.id.btn_decode);
        btn_render = (Button) findViewById(R.id.btn_render);
        btn_audio_decode = (Button) findViewById(R.id.btn_audio_decode);
        btn_audio_player = (Button) findViewById(R.id.btn_audio_player);
        btn_play = (Button) findViewById(R.id.btn_play);
        btn_transcoding_compress = (Button) findViewById(R.id.btn_transcoding_compress);
        btn_addWatermark = (Button) findViewById(R.id.btn_addWatermark);

        btn_decode.setOnClickListener(this);
        btn_render.setOnClickListener(this);
        btn_audio_decode.setOnClickListener(this);
        btn_audio_player.setOnClickListener(this);
        btn_play.setOnClickListener(this);
        btn_transcoding_compress.setOnClickListener(this);
        btn_addWatermark.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_decode:
                mDecode(v);
                break;
            case R.id.btn_render:
                startActivity(new Intent(this, VideoRenderActivity.class));
                break;
            case R.id.btn_audio_decode:

                break;
            case R.id.btn_audio_player:

                break;
            case R.id.btn_play:

                break;
            case R.id.btn_transcoding_compress:

                break;
            case R.id.btn_addWatermark:

                break;
        }
    }

    //  视频解码
    public void mDecode(View btn) {
        String input = new File(Environment.getExternalStorageDirectory(), "小苹果.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "小苹果_out.yuv").getAbsolutePath();
        VideoUtils.decode(input, output);
        Toast.makeText(this,"正在解码...",Toast.LENGTH_SHORT).show();
    }

    // --------------------------   native-lib  --------------------------------------
    static {
        System.loadLibrary("native-lib");
    }

    public native String stringFromJNI();
}
