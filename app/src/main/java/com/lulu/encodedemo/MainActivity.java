package com.lulu.encodedemo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String s = Codec.hexEncode(new byte[]{0x3c, 0x7c});
        Log.d(TAG, "onCreate: s ==> " + s);
        byte[] bytes = Codec.hexDecode(s);

        for (byte aByte : bytes) {
            Log.d(TAG, "onCreate: byte ==> " + String.format(Locale.CANADA, "%x",aByte ));
        }

    }
}
