package jbox2d.example.com.ffmpeg_demo.utils;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

/**
 * @Author: Ycl
 * @Date: 2018/6/23 15:08
 * @Desc: 创建播放器
 * 签名
 * public class jbox2d.example.com.ffmpeg_demo.utils.AudioUtil {
 * public jbox2d.example.com.ffmpeg_demo.utils.AudioUtil();
 * descriptor: ()V
 * <p>
 * public int write(byte[], int, int);
 * descriptor: ([BII)I
 * <p>
 * public android.media.AudioTrack createAudioTrack(int, int);
 * descriptor: (II)Landroid/media/AudioTrack;
 * }
 */
public class AudioUtil {

    //Todo 生成JNI签名后删除 该声明
    //Todo 该声明 来源于 audioTrack.write()
    public int write(byte[] audioData, int offsetInByte, int sizeInByte) {
        return 0;
    }

    /**
     * 创建一个AudioTrack对象，用于播放
     *
     * @param sampleRateInHz
     * @param nb_channels
     * @return
     */
    public AudioTrack createAudioTrack(int sampleRateInHz, int nb_channels) {
        Log.i("AudioUtil", "sampleRateInHz: " + sampleRateInHz + " nb_channels: " + nb_channels);

        //固定格式的音频码流
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;

        //声道个数 影响声道的布局
        int channelConfig;
        if (nb_channels == 1) {
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (nb_channels == 2) {
            channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        }

        // 缓冲区
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz,
                channelConfig, audioFormat);
      /*  int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
        int bufferSizeInBytes, int mode*/
        AudioTrack audioTrack = new AudioTrack(
                AudioManager.STREAM_MUSIC,
                sampleRateInHz, channelConfig,
                audioFormat,
                bufferSizeInBytes, AudioTrack.MODE_STREAM
        );

        //播放
        //audioTrack.play();
        //写入PCM
        //audioTrack.write(audioData, offsetInBytes, sizeInBytes);
        return audioTrack;
    }
}