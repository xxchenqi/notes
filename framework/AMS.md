# AMS

### 应用程序进程启动过程分析

点击桌面APP->Launcher进程请求AMS启动根activity->AMS通过Zygote进程请求创建app进程(socket)->创建完成后在通过AMS启动根activity



### AMS请求Zygote过程分析

AMS->Process->ZygoteProcess->ZygoteState



### Zygote进程fork进程分析

Zygote通过反射调用到ActivityThread.main()方法



### 应用程序创建后细节一

AMS->通过Handler创建Activity



AMS如何被创建出来:

SystemServer.main->SystemServer.run->startBootstrapServices->





ActivityThread.performLaunchActivity方法解析:

1．从ActivityCIientRecord中获取待启动的Activity的组件信息

2．通过Instrumentation的newActivity方法使用类加载器创建Activity对象

3．通过LoadedApk的makeAppIication方法来尝试创建AppIication对象

4．创建ContextImpI对象并通过Activity的attach方法来完成一些重要数据的初始化

5．调用Activity的onCreate方法





问题:

Zygote->ActivityThread.main->启动activity 这么联系起来







# 各流程解析

## Android系统启动

Boot Rom->Boot Loader->kernel->init(pid=1)->Zygote->System Server->Launcher app  AMS





## Zygote启动流程

zygote是由init进程通过解析 init.zygote.rc 文件而创建的，

解析到的源文件是 app_main.cpp ，进程名为zygote。最终会fork出zygote进程



## SystemServer启动流程

SystemServer是由ZygoteInit这个类的入口方法中去创建的，是有Zygote进程fork出来。

相关代码:Zygote.forkSystemServer



SystemServer会启动各种服务:

1.startBootstrapServices 启动引导服务

2.startCoreServices 启动核心服务

3.startOtherServices 启动其他服务







## AMS启动流程

AMS是由SystemServer中的startBootstrapServices 引导服务去创建的,

内部由SystemServiceManager反射创建ActivityManagerService.Lifecycle,

ActivityManagerService.Lifecycle的构造方法里会初始化AMS

ASM初始化里会创建:

1.创建"android.ui"的线程

2.创建ActiveServices

2.创建ActivityStackSupervisor对象





## App进程启动流程

当Zygote收到AMS请求创建一个新app进程后，会调用到ActivityThread.main

main方法：

1.初始化ActivityThread

2.mAppThread = new ApplicationThread() 说明：class ApplicationThread extends IApplicationThread.Stub

3.ActivityThread.attach

4.Looper启动



attach解析:

通过H(handler)类发送消息BIND_APPLICATION做一些处理:

1.创建一个Instrumentation

2.获取LoadedApk对象（创建LoadedApk对象,并将将新创建的LoadedApk加入到mPackages. 也就是说每个app都会创建唯一的LoadedApk对象）

3.ContextImpl.createAppContext  创建ContextImpl上下文

4.data.info.makeApplication 通过反射创建目标应用Application对象

5.调用Application的onCreate生命周期

mInstrumentation.callApplicationOnCreate

app.onCreate 常见的application的oncreate就是这里调用的





## startActivity流程

调用startActivity会调用到startActivityForResult，

->Instrumentation.execStartActivity

*Instrumentation相当于一个钩子，提供各种流程控制方式。*



->AMS.startActivity

->ActivityStarter.startActivityMayWait

->ActivityStarter.startActivity 这里有多重调用

->ActivityStarter.startActivityUnchecked



startActivityUnchecked解析:

1.根据启动标志位和Activity启动模式来决定如何启动一个Activity以及是否要调用deliverNewIntent方法通知Activity有一个Intent试图重新启动它

2.

App进程已启动：->ASS.realStartActivityLocked

通过ApplicationThread发送消息到H再到ActivityThread去调用Instrumentation的

callActivityOnCreate方法执行Activity的onCreate生命周期



App进程未启动：

通过Zygote fork出APP进程





## Application启动流程

App进程启动流程里的attach里已分析



## AndroidManifest.xml是在哪儿解析的？ 

PKMS 等待补全



## 启动模式在哪里判断

在startActivity流程已分析







