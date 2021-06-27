# FFMPEG编译



## 下载安装

### NDK

```
1.下载：在浏览器，下载中心，复制链接地址： 
https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip? hl=zh_cn

2.wget 去下载操作： 
wget https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip? hl=zh_cn

3.修改名称： 
mv android-ndk-r17c-linux-x86_64.zip\?hl\=zh_cn android-ndk-r17c-linux- x86_64.zip

4.解压： 
unzip android-ndk-r17c-linux-x86_64.zip
```

### FFMPEG

http://www.ffmpeg.org/releases/ （ffmpeg历史版本下载）

```
wget http://www.ffmpeg.org/releases/ffmpeg-4.0.2.tar.bz2 
解压
tar -xjf ffmpeg-4.0.2.tar.bz2
```







## 编译

build.sh脚本编写

NDK_ROOT此变量需要设置为自己的NDK目录



```shell
#!/bin/bash

# 首先定义一个NDK目录的变量 NDK_ROOT
NDK_ROOT=/root/cq/Tools/android-ndk-r17c

# 此变量执行ndk中的交叉编译gcc所在目录
TOOLCHAIN=$NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64

#以下命令可以参考 externalNativeBuild/xxx/build.ninja
FLAGS="-isystem $NDK_ROOT/sysroot/usr/include/arm-linux-androideabi -D__ANDROID_API__=21 -g -DANDROID -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -Wa,--noexecstack -Wformat -Werror=format-security  -O0 -fPIC"
INCLUDES=" -isystem $NDK_ROOT/sources/android/support/include"

# 1.定义编译后，所存放的目录
PREFIX=./android/arm

# 2.--enable-small 优化大小 非常重要，必须优化才行的哦
# 3.--disable-programs 不编译ffmpeg程序（命令行工具），我们是需要获取静态、动态库
# 4.--disable-avdevice 关闭avdevice模块，此模块在android中无用
# 5.--disable-encoders 关闭所有编码器（播放不需要编码）
# 6.--disable-muxers 关闭所有复用器（封装器），不需要生成mp4这样的文件，所有关闭
# 7.--disable-filters 关闭所有滤镜
# 8.--enable-cross-compile 开启交叉编译（ffmpeg是跨平台的，注意：并不是所有库都有这么happy的选项）
# 9.--cross-prefix 看右边的值就知道是干嘛的，gcc的前缀..
# 10.disable-shared / enable-static 这个不写也可以，默认就是这样的，（代表关闭动态库，开启静态库）
# 11.--sysroot
# 12.--extra-cflags 会传给gcc的参数
# 13.--arch  --target-os 

./configure \
--prefix=$PREFIX \
--enable-small \
--disable-programs \
--disable-avdevice \
--disable-encoders \
--disable-muxers \
--disable-filters \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--disable-shared \
--enable-static \
--sysroot=$NDK_ROOT/platforms/android-21/arch-arm \
--extra-cflags="$FLAGS $INCLUDES" \
--extra-cflags="-isysroot $NDK_ROOT/sysroot/" \
--arch=arm \
--target-os=android

make clean

make install
```

执行脚本

sh build.sh

执行成功会在指定目录生成

include  lib  share

include：头文件

lib ： 库文件(.a)

share ： 例子









## 使用

1.创建AS项目

...

2.将头文件导入

3.将库文件导入

4.CMakeLists

```cmake
cmake_minimum_required(VERSION 3.4.1)

file(GLOB allCpp *.cpp)

add_library(
        native-lib
        SHARED
        ${allCpp})

find_library(
        log-lib
        log)

# 引入FFmpeg的头文件
include_directories(${CMAKE_SOURCE_DIR}/include)

# 这里主要指定你的库文件的位置
# 引入FFmpeg的库文件，设置内部的方式引入，指定库的目录是 -L  指定具体的库-l
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/${CMAKE_ANDROID_ARCH_ABI}")

target_link_libraries(
        native-lib

        # 具体的库文件，链接到总库
        # 这样写会报错，有坑
        # avcodec avfilter avformat avutil swresample swscale

        # 先把有依赖的库，先依赖进来
        avformat avcodec avfilter avutil swresample swscale

        ${log-lib})
```



5.调用

```cpp
#include <jni.h>
#include <string>

// 有坑，会报错，必须混合编译
// #include <libavutil/avutil.h>

extern "C" {
    #include <libavutil/avutil.h>
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_kevin_ndk14_1code_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "当前的FFmpeg的版本是：";
    hello.append(av_version_info());
    return env->NewStringUTF(hello.c_str());
}

```





