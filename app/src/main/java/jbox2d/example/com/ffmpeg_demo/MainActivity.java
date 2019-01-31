package jbox2d.example.com.ffmpeg_demo;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;

import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button mBtnDecode;
    private Button mBtnRender;
    private Button mBtnAudioDecode;
    private Button mBtnAudioPlayer;
    private Button mBtnPlay;
    private Button mBtnTranscodingCompress;
    private Button mBtnAddWatermark;

    private VideoUtils mVideoUtils;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        initData();
    }

    private void initData() {
        mVideoUtils = new VideoUtils();
    }

    private void initView() {
        mBtnDecode = (Button) findViewById(R.id.btn_decode);
        mBtnRender = (Button) findViewById(R.id.btn_render);
        mBtnAudioDecode = (Button) findViewById(R.id.btn_audio_decode);
        mBtnAudioPlayer = (Button) findViewById(R.id.btn_audio_player);
        mBtnPlay = (Button) findViewById(R.id.btn_play);
        mBtnTranscodingCompress = (Button) findViewById(R.id.btn_transcoding_compress);
        mBtnAddWatermark = (Button) findViewById(R.id.btn_addWatermark);

        mBtnDecode.setOnClickListener(this);
        mBtnRender.setOnClickListener(this);
        mBtnAudioDecode.setOnClickListener(this);
        mBtnAudioPlayer.setOnClickListener(this);
        mBtnPlay.setOnClickListener(this);
        mBtnTranscodingCompress.setOnClickListener(this);
        mBtnAddWatermark.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_decode:
                videoDecode(v);
                break;
            case R.id.btn_render:
                startActivity(new Intent(this, VideoRenderActivity.class));
                break;
            case R.id.btn_audio_decode:
                audioDecode(v);
                break;
            case R.id.btn_audio_player:
                audioPlayer(v);
                break;
            case R.id.btn_play:
                startActivity(new Intent(this, VideoPlayActivity.class));
                break;
            case R.id.btn_transcoding_compress:

                break;
            case R.id.btn_addWatermark:

                break;
        }
    }

    //  音频播放
    private void audioPlayer(View v) {
        String input = new File(Environment.getExternalStorageDirectory(), "说散就散.mp3").getAbsolutePath();
        mVideoUtils.audioPlayer(input);
        Log.d("Main", "正在播放");
    }

    //  音频解码
    private void audioDecode(View v) {
        String input = new File(Environment.getExternalStorageDirectory(), "说散就散.mp3").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "说散就散.pcm").getAbsolutePath();
        mVideoUtils.audioDecode(input, output);
        Toast.makeText(this, "正在音频解码...", Toast.LENGTH_SHORT).show();
    }

    //  视频解码
    public void videoDecode(View btn) {
        String input = new File(Environment.getExternalStorageDirectory(), "小苹果.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "小苹果_out.yuv").getAbsolutePath();
        VideoUtils.decode(input, output);
        Toast.makeText(this, "正在视频解码...", Toast.LENGTH_SHORT).show();
    }

    // --------------------------   native-lib  --------------------------------------
    static {
        System.loadLibrary("native-lib");
    }

    public native String stringFromJNI();
}
