## 开始之前

> 本篇博文承接上篇[Android NDK开发(一) 入门](http://www.jianshu.com/p/0261e6cceb3e), 如果大家没有接触过NDK开发请移至上篇
> 本篇主要通过HEX的编码和解码案例来进一步了解NDK的开发.

### 什么是HEX ?

> 通俗讲就是16进制

### 什么是HEX的编码和解码 ?

> 编码: 就是将字节数组每个字节使用16进制的可见字符串来显示 bytes --> String

> 解码: 就是将16进制的可见字符串转换为原来的字节数组 String --> bytes

> 来张图说明一下:

![HEX.png](http://upload-images.jianshu.io/upload_images/3118842-7820b7d5aa39aa19.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 开始开发

> 今天我们不采用上一篇中创建工程时直接勾选出 Include C++ Support, 而是创建一个普通的工程, 后期如何扩展为NDK的工程

### 创建工程

1. 创建普通工程, 修改local.properties文件, 添加ndk.dir=XXX;

2. 手动在app目录下创建CMakeLists.txt文件, 内容和注释如下:
```
# CMake的编译脚本配置文件

# 1. 标注需要支持的CMake最小版本

cmake_minimum_required(VERSION 3.4.1)

# 2. add_library 定义需要编译的代码库 名称, 类型, 包含的源码

add_library(
            # Sets the name of the library.
            codec

            # Sets the library as a shared library.
            SHARED

            src/main/cpp/com_lulu_encodedemo_Codec.c
)

# 3. find_library 定义当前代码库需要依赖的系统或者第三方库文件

find_library(

        log_lib # 指定要查找的系统库, 给一个名字
        log     # 真正要查找的liblog.so或者liblog.a

)

# 可以写多个 find_library

# 4. target_link_libraries设置最终编译的目标代码库

target_link_libraries(
     codec  # add_library 生成的

     ${log_lib} # find_library 找到的系统库
)

```

3. 修改app目录下的build.gradle文件, 在android便签下添加
```
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
```

4. 在src/main目录下创建cpp文件夹. 至此工程创建完毕

### 编写业务逻辑

#### 创建Codec.java类, 用于绑定C代码
``` java

public class Codec {

    static {
        //一定不要忘记调用!!!!
        System.loadLibrary("codec");
    }
    public static native String hexEncode(byte[] data);
    public static native byte[] hexDecode(String data);
}

```
> Note : 给大家忠告, System.loadLibrary("codec");一定不要忘记调用，否则会报一下错误
![Paste_Image.png](http://upload-images.jianshu.io/upload_images/3118842-8f7a2c3258a58b6c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


#### 实现C语言核心部分

> 对应Codec类有两个方法分别用于编码和解码, 代码中已经做了详细的注释加以说明

1. 编码方法:
``` C
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

```

2. 解码方法：
``` C

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
```

### 代码测试:

``` java

String s = Codec.hexEncode(new byte[]{0x3c, 0x7c});
Log.d(TAG, "onCreate: s ==> " + s);
byte[] bytes = Codec.hexDecode(s);

for (byte aByte : bytes) {
    Log.d(TAG, "onCreate: byte ==> " + String.format(Locale.CANADA, "%x",aByte ));
}

```

> 测试结果:

```
D/MainActivity: onCreate: s ==> 3C7C
D/MainActivity: onCreate: byte ==> 3c
D/MainActivity: onCreate: byte ==> 7c

```

## 完整代码

代码已上传值[Github](https://github.com/changer0/NDKEncoderDemo)上, 欢迎大家Clone

## 总结

> 至此NDK的博客完成了, 本人也是菜鸟一枚, 希望更多的是抛砖引玉, 让各位大神们指出修改意见!
> 写博客不易, 期待大家的鼓励和支持, 谢谢^v^