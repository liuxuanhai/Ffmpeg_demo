package jbox2d.example.com.ffmpeg_demo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

import jbox2d.example.com.ffmpeg_demo.utils.VideoUtils;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());


        // 编码
        Button bt = (Button) findViewById(R.id.button);
        bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mDecode(view);
            }
        });
    }


    public void mDecode(View btn) {
        String input = new File(Environment.getExternalStorageDirectory(), "小苹果.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "小苹果_out.yuv").getAbsolutePath();
        VideoUtils.decode(input, output);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
