# Android环境配置



### 环境变量

```
ANDROID_HOME:G:\sdk
ANDROID_SDK_ROOT:G:\sdk
ANDROID_SDK_HOME:D:\Android\

JAVA_HOME:C:\Program Files\Java\jdk1.8.0_202
CLASSPATH:.;%JAVA_HOME%\lib\dt.jar;%JAVA_HOME%\lib\tools.jar;

path:
C:\Program Files\Java\jdk1.8.0_202\bin

G:\sdk\build-tools\29.0.2
G:\sdk\platform-tools
G:\sdk\ndk\17.2.4988734\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin

%ANDROID_HOME%\platform-tools
%ANDROID_HOME%\emulator
%ANDROID_HOME%\tools
%ANDROID_HOME%\tools\bin

G:\flutter\bin
G:\flutter\bin\cache\dart-sdk\bin

```



### AS配置文件修改

.AndroidStudio: 这个文件夹是Android Studio的配置文件夹，主要存放一些AndroidStudio设置和插件和项目的缓存信息

进入Android Studio的安装目录，进入`bin`文件夹，用文本编辑软件打开`idea.properties`，去掉以下两项的注释符号`#`，修改对应的路径为新路径即可。

```
idea.config.path=D:/Android/.AndroidStudio3.5/config
idea.system.path=D:/Android/.AndroidStudio3.5/system
```



.gradle文件夹的修改

在Android Studio的配置选项中修改就行



.android文件夹的修改

需要添加一个系统的环境变量`ANDROID_SDK_HOME`

