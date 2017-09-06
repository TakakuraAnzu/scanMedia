package com.illya.scanmedia;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("scanMedia");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView textView = findViewById(R.id.sample_text);
        StringBuffer sb = new StringBuffer();
        List<String> mediaFiles = StartScan(Environment.getExternalStorageDirectory().getPath());
        for (String s : mediaFiles){
            sb.append(s).append("\n");
        }
        textView.setText(sb);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native ArrayList<String> StartScan(String path);
}
