# FFMPEG

## 虚拟机环境配置

### ubuntu下载

```
https://cn.ubuntu.com/download
```

### vmware15虚拟机下载

用vmware虚拟机,不要使用Oracle VM VirtualBox （ping不通主机,samba用不了）

vmware15秘钥:

YG5H2-ANZ0H-M8ERY-TXZZZ-YKRV8



### 虚拟机配置:

磁盘大小配置到20G(10G不够用)



### ubuntu安装:

选择英语，最小安装，拔掉网线，不然会下载语言包等!



安装完成,启动命令行:

ctrl+alt+t



### 更新软件源:

Ubuntu系统中，软件源文件地址为：/etc/apt/sources.list

1.备份

sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak

2.修改(删除里面的内容)

sudo gedit /etc/apt/sources.list

```
#添加阿里源
deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse
deb http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
deb-src http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
#添加清华源
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse multiverse
```

更新源

```
sudo apt-get update
```



需要安装的软件

```
samba 共享文件
make ffmpeg脚本需要用到
vim 可选 
ifconfig 查看ip
...
```



### 安装samba

1.安装samba服务

```
sudo apt-get install samba samba-common
```

2.配置共享目录

```
sudo chmod 777 /home/ -R
```

3.添加用户

```
sudo smbpasswd -a cq
```

4.配置samba

先备份sudo cp /etc/samba/smb.conf /etc/samba/smb.conf.bak，以防改错，修改配置文件时建议养成备份的好习惯，改错了还能恢复，sudo vim /etc/samba/smb.conf 修改配置，添加共享，可直接加到文件尾

```
[cq] 
    comment = home directories
    path = /home/ 
    public = yes  
    writable = yes 
    workgroup = WORKGROUP
```

5.重启服务

```
sudo service smbd restart
```

若命令找不到尝试以下命令

```
sudo /etc/init.d/samba restart
sudo /etc/init.d/samba-ad-dc restart
```



windows访问samba

\\\\linux ip



## NDK配置

NDK下载(所有的下载地址)

<https://blog.csdn.net/gyh198/article/details/75036686>

下载NDK20版本

<https://dl.google.com/android/repository/android-ndk-r20b-linux-x86_64.zip>

解压:

```
unzip android-ndk-r20-linux-x86_64.zip -d ndk
```



配置环境变量

1.修改~/.bashrc (根据自己路径修改)

```
vim ~/.bashrc 
# 在文件末尾添加 
export NDKROOT=/home/cq/ndk/android-ndk-r20 
exprot PATH=$NDKROOT:$PATH 
# 保存退出，更新一下环境变量 
source ~/.bashrc
```

2.修改 "/etc/"下面的profile文件 (根据自己路径修改)

```
vim /etc/profile 
# 在文件末尾添加 
export NDKROOT=/home/cq/ndk/android-ndk-r20 
exprot PATH=$NDKROOT:$PATH 
# 保存退出，更新一下环境变量 
source /etc/profile
```

3.可以在shell中输入ndk-build命令来检查你的安装是否成功，如果不 是显示“ndk-build not found”, 则说明你的ndk安装成功



## FFMPEG配置

下载:

<http://ffmpeg.org/>

或者

```
git clone git://source.ffmpeg.org/ffmpeg.git ffmpeg
```



修改configure

(3812行# build settings)

```
SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)' 
LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"' 
SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)' 
SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR) $(SLIBNAME)'
将其修改成： 
SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(S LIBSUF)' LIB_INSTALL_EXTRA_CMD='$$(RANLIB) "$(LIBDIR)/$(LIBNAME)"' SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)' 
SLIB_INSTALL_LINKS='$(SLIBNAME)'
```





在ffmpeg目录下(和configure同级目录)，新建bash.sh文件。 (根据自己路径修改)

```bash
#!/bin/bash
NDK=/home/cq/ndk/android-ndk-r20b
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64/
API=29

function build_android
{
echo "Compiling FFmpeg for $CPU"
	./configure \
		--prefix=$PREFIX \
		--enable-neon \
		--enable-hwaccels \
		--disable-gpl \
		--disable-postproc \
		--enable-shared \
		--enable-jni \
		--enable-mediacodec \
		--enable-decoder=h264_mediacodec \
		--disable-static \
		--disable-doc \
		--enable-ffmpeg \
		--disable-ffplay \
		--disable-ffprobe \
		--enable-avdevice \
		--disable-doc \
		--disable-symver \
		--cross-prefix=$CROSS_PREFIX \
		--target-os=android \
		--arch==$ARCH \
		--cpu=$CPU \
		--cc=$CC
		--cxx=$CXX
		--enable-cross-compile \
		--sysroot=$SYSROOT \
		--extra-cflags="-Os -fpic $OPTIMIZE_CFLAGS" \
		--extra-ldflags="$ADDI_LDFLAGS" \
		$ADDITIONAL_CONFIGURE_FLAG
make clean
make 
make install 
echo "The Compilation of FFmpeg for $CPU is completed"
}


#armv8-a 
ARCH=arm64 
CPU=armv8-a 
CC=$TOOLCHAIN/bin/aarch64-linux-android$API-clang 
CXX=$TOOLCHAIN/bin/aarch64-linux-android$API-clang++ 
SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot 
CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android- PREFIX=$(pwd)/android/$CPU 
OPTIMIZE_CFLAGS="-march=$CPU" 
build_android
```



还可以增加其他CPU

```bash
#armv7-a
ARCH=arm64 
CPU=armv7-a 
CC=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang 
CXX=$TOOLCHAIN/bin/armv7a-linux-androideabi$API-clang++ SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot CROSS_PREFIX=$TOOLCHAIN/bin/arm-linux-androideabi- PREFIX=$(pwd)/android/$CPU OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm - march=$CPU "
build_android

#x86 
ARCH=x86 
CPU=x86 
CC=$TOOLCHAIN/bin/i686-linux-android$API-clang 
CXX=$TOOLCHAIN/bin/i686-linux-android$API-clang++ SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot CROSS_PREFIX=$TOOLCHAIN/bin/i686-linux-android- PREFIX=$(pwd)/android/$CPU OPTIMIZE_CFLAGS="-march=i686 -mtune=intel -mssse3 - mfpmath=sse -m32"
build_android

#x86_64 
ARCH=x86_64 
CPU=x86-64 
CC=$TOOLCHAIN/bin/x86_64-linux-android$API-clang 
CXX=$TOOLCHAIN/bin/x86_64-linux-android$API-clang++ SYSROOT=$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot
CROSS_PREFIX=$TOOLCHAIN/bin/x86_64-linux-android- PREFIX=$(pwd)/android/$CPU OPTIMIZE_CFLAGS="-march=$CPU -msse4.2 -mpopcnt -m64 - mtune=intel" 
build_android
```



执行

```
./bash.sh
```



执行成功会生成android目录

