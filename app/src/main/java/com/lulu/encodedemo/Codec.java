package com.lulu.encodedemo;

/**
 * Created by Lulu on 2016/10/29.
 */

public class Codec {

    static {
        //一定不要忘记调用!!!!
        System.loadLibrary("codec");
    }
    public static native String hexEncode(byte[] data);
    public static native byte[] hexDecode(String data);
}
