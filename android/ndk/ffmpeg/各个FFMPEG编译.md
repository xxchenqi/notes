# 各个FFMPEG编译

## 一、windows

1.安装MSYS2

2.打开mingw64

3.安装ffmpeg的依赖

```shell
pacman -S make gcc diffutils pkg-config nasm
```

4.编译ffmpeg

```shell
./configure --prefix=./output --enable-ffplay --enable-shared --enable-sdl2
make -j 4
make install
```



问题：

1.ERROR: sdl2 requested but not found

安装sdl2

```shell
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-SDL2
```

2.运行ffmpeg.exe缺少dll

添加path环境变量

E:\msys64\mingw64\bin



## 二、linux/mac

安装依赖

```
brew install sdl2
brew install nasm
```



编译

```shell
./configure --prefix=./output --enable-ffplay --enable-shared --enable-sdl2
make -j 4
make install / sudo make install
```


