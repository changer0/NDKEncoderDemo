//
// Created by Lulu on 2016/10/29.
//

#include "com_lulu_encodedemo_Codec.h"
#include <stdio.h>


// 代表可以被 Java 调用
JNIEXPORT
// 返回值类型
jstring
//声明遵守JNI Java 调用C的规则
JNICALL
Java_com_lulu_encodedemo_Codec_hexEncode(JNIEnv *env, jclass clazz, jbyteArray array) {
    // 1. 数组长度；2. new StringBuilder(); or char[len * 2] 3. char[] -> jstring
    jstring ret = NULL;
    if (array != NULL) {
        //得到数组的长度
        jsize len = (*env)->GetArrayLength(env, array);
        if (len > 0) {
            //存储编码后的字符, +1的原因是考虑到\0
            char chs[len * 2 + 1];
            jboolean b = JNI_FALSE;
            //得到数据的原始数据 此处注意要取b的地址!
            jbyte *data = (*env)->GetByteArrayElements(env, array, &b);
            int index;
            for (index = 0; index < len; index++) {
                jbyte bc = data[index];
                //拆分成高位, 低位
                jbyte h = (jbyte) ((bc >> 4) & 0x0f);
                jbyte l = (jbyte) (bc & 0x0f);
                //把高位和地位转换成字符
                jchar ch;
                jchar cl;

                if (h > 9) {
                    ch = (jchar) ('A' + (h - 10));
                } else {
                    ch = (jchar) ('0' + h);
                }

                if (l > 9) {
                    cl = (jchar) ('A' + (l - 10));
                } else {
                    cl = (jchar) ('0' + l);
                }
                //转换之后拼接
                chs[index * 2] = (char) ch;
                chs[index * 2 + 1] = (char) cl;
            }
            //最后一位置为0
            chs[len * 2] = 0;
            //释放数组
            (*env)->ReleaseByteArrayElements(env, array, data, JNI_ABORT);
            ret = (*env)->NewStringUTF(env, chs);
        }

    }
    return ret;
}

JNIEXPORT jbyteArray JNICALL
Java_com_lulu_encodedemo_Codec_hexDecode(JNIEnv *env, jclass type, jstring str) {
    jbyteArray ret = NULL;

    if (str != NULL) {
        // TODO
        jsize len = (*env)->GetStringLength(env, str);
        //判断只有在长度为偶数的情况下才继续
        if (len % 2 == 0) {
            jsize dLen = len >> 1;
            jbyte data[dLen];
            jboolean b = JNI_FALSE;
            const jchar *chs = (*env)->GetStringChars(env, str, &b);
            int index;
            for (index = 0; index < dLen; index++) {
                //获取到单个字符
                jchar ch = chs[index * 2];
                jchar cl = chs[index * 2 + 1];
                jint h = 0;
                jint l = 0;
                //得到高位和低位的 ascii
                if (ch >= 'A') {
                    h = ch - 'A' + 10;
                } else if (ch >= 'a') {
                    h = ch - 'a' + 10;
                } else if(ch >= '0') {
                    h = ch - '0';
                }
                if (cl >= 'A') {
                    l = cl - 'A' + 10;
                } else if (cl >= 'a') {
                    l = cl - 'a' + 10;
                } else if(cl >= '0'){
                    l = cl - '0';
                }
                //高位和地位拼接
                data[index] = (jbyte) ((h << 4) | l);
            }
            //释放
            (*env)->ReleaseStringChars(env, str, chs);
            //创建新的字节数组
            ret = (*env)->NewByteArray(env, dLen);
            //给新创建的数组设置数值
            (*env)->SetByteArrayRegion(env, ret, 0,dLen, data);
        }
    }
    return ret;
}