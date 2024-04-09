# FrameWork

1.BootLoader引导开机

2.kernel

加载硬件驱动 

HAL：摄像头驱动，显示器驱动，输入驱动，多媒体驱动等（解决多家厂商标准不统一问题）

3.C++ FrameWork

init进程，adbd，logd

4.Android FrameWork

init进程创建完成会创建Zygote进程->创建完成会固定加载ZygoteInit.java（JNI）->反射调用ActivityThread(App进程)

ZygoteInit：系统资源预加载，创建SystemServer系统服务进程，创建socket服务进入阻塞状态

SystemServer：创建各种系统服务（引导、核心、其他服务）

引导：AMS,PM...

核心：Battery，GPU

其他：网络状态，时钟，电话，输入，蓝牙



所有服务启动完成会注册到SystemServiceManager里去管理，

所有系统服务启动完成后，会调用AMS调用systemReady去启动Launcher



5.Apps

创建Launcher ...

![image-20210210221849779](.\assets\image-20210210221849779.png)

![image-20210210221929738](.\assets\image-20210210221929738.png)

![image-20210210221941297](.\assets\image-20210210221941297.png)

![image-20210210225017977](.\assets\image-20210210225017977.png)