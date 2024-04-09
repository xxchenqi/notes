# Android系统启动

## init进程启动过程

init进程是Android系统中用户空间的第一个进程，进程号为1，是Android系统启动流程中一个关键的步骤，作为第一个进程，它被赋予了很多极其重要的工作职责，比如创建Zygote（孵化器）和属性服务等。init进程是由多个源文件共同组成的，这些文件位于源码目录system/core/init中。

### Android系统启动流程的前几步:

1. 启动电源以及系统启动

   当电源按下时引导芯片代码从预定义的地方（固化在ROM）开始执行。加载引导程序BootLoader到RAM中，然后执行。

2. 引导程序BootLoader

   引导程序BootLoader是在Android操作系统开始运行前的一个小程序，它的主要作用是把系统OS拉起来并运行。

3. Linux内核启动

   当内核启动时，设置缓存、被保护存储器、计划列表、加载驱动。在内核完成系统设置后，它首先在系统文件中寻找init.rc文件，并启动init进程。

4. init进程启动

   init进程做的工作比较多，主要用来初始化和启动属性服务，也用来启动Zygote进程。



### init进程的入口函数(main)

1. 挂载了tmpfs、devpts、proc、sysfs和selinuxfs共5种文件系统
2. 属性进行初始化,启动属性服务
3. 子进程信号处理:主要用于防止init进程的子进程成为僵尸进程，为了防止僵尸进程的出现
4. 解析init.rc文件



### 解析init.rc

Action类型语句采用ActionParser来进行解析，Service 类型语句采用ServiceParser来进行解析

```
system\core\rootdir\init.zygote64.rc

service zygote /system/bin/app_process64 -Xzygote /system/bin --zygote --start-system-server
    class main
    priority -20
    user root
    group root readproc reserved_disk
    socket zygote stream 660 root system
    onrestart write /sys/android_power/request_state wake
    onrestart write /sys/power/state on
    onrestart restart audioserver
    onrestart restart cameraserver
    onrestart restart media
    onrestart restart netd
    onrestart restart wificond
    writepid /dev/cpuset/foreground/tasks

```

Service 用于通知init进程创建名为zygote的进程，这个进程执行程序的路径为/system/bin/app_process64，其后面的代码是要传给app_process64的参数。class main指的是Zygote的classname为main



### init启动Zygote

```c
frameworks\base\cmds\app_process\app_main.cpp

int main(int argc, char* const argv[])
{
    ...
    if (zygote) {
        runtime.start("com.android.internal.os.ZygoteInit", args, zygote);
    } else if (className) {
        runtime.start("com.android.internal.os.RuntimeInit", args, zygote);
    } else {
        fprintf(stderr, "Error: no class name or --zygote supplied.\n");
        app_usage();
        LOG_ALWAYS_FATAL("app_process: no class name or --zygote supplied.");
    }
}
```



runtime的start函数启动Zygote，至此Zygote就启动了。



### 属性服务

Windows 平台上有一个注册表管理器，注册表的内容采用键值对的形式来记录用户、软件的一些使用信息。即使系统或者软件重启，其还是能够根据之前注册表中的记录，进行相应的初始化工作。Android也提供了一个类似的机制，叫作属性服务。

init进程启动时会启动属性服务，并为其分配内存，用来存储这些属性，如果需要这些属性直接读取就可以了。

```c
property_init();
start_property_service();
```



### init进程启动总结

init进程启动做了很多的工作，总的来说主要做了以下三件事：

1. 创建和挂载启动所需的文件目录。
2. 初始化和启动属性服务。
3. 解析init.rc配置文件并启动Zygote进程。





## Zygote进程启动过程

### Zygote概述

在Android系统中，DVM（Dalvik虚拟机）和ART、应用程序进程以及运行系统的关键服务的SystemServer进程都是由Zygote进程来创建的，我们也将它称为孵化器。它通过fock（复制进程）的形式来创建应用程序进程和SystemServer进程，由于Zygote进程在启动时会创建DVM或者ART，因此通过fock而创建的应用程序进程和SystemServer进程可以在内部获取一个DVM或者ART的实例副本。



### Zygote进程启动过程介绍

```c
frameworks\base\cmds\app_process\app_main.cpp

int main(int argc, char* const argv[]){
	...
	while (i < argc) {
        const char* arg = argv[i++];
        if (strcmp(arg, "--zygote") == 0) {
            //如果当前运行在zygote进程就将zygote设为true
            zygote = true;
            niceName = ZYGOTE_NICE_NAME;
        } else if (strcmp(arg, "--start-system-server") == 0) {
            //如果当前运行在SystemServer进程就将startSystemServer设为true
            startSystemServer = true;
        }
    }
    ...
        
    //如果运行在zygote进程中就启动
    if (zygote) {
        runtime.start("com.android.internal.os.ZygoteInit", args, zygote);
    } 
}
```



AppRuntime的start函数:

```c
void AndroidRuntime::start(const char* className, const Vector<String8>& options, bool zygote)
{
	JNIEnv* env;
    //启动JAVA虚拟机
    if (startVm(&mJavaVM, &env, zygote) != 0) {
        return;
    }
    onVmCreated(env);

    //注册JNI
    if (startReg(env) < 0) {
        ALOGE("Unable to register all android natives\n");
        return;
    }

    ...
    char* slashClassName = toSlashClassName(className != NULL ? className : "");
    //找到zygoteInit
    jclass startClass = env->FindClass(slashClassName);
    if (startClass == NULL) {
        ALOGE("JavaVM unable to locate class '%s'\n", slashClassName);
        /* keep going */
    } else {
        //找到main方法
    	jmethodID startMeth = env->GetStaticMethodID(startClass, "main",
        "([Ljava/lang/String;)V");
    if (startMeth == NULL) {
        ALOGE("JavaVM unable to find main() in '%s'\n", className);
    } else {
        //调用main方法
        env->CallStaticVoidMethod(startClass, startMeth, strArray);

}
```





1. 创建Java虚拟机

2. Java虚拟机注册JNI方法

   为何使用jni:

   因为ZygoteInit的main方法是由Java语言编写的，当前的运行逻辑在Native中，这就需要通过JNI来调用Java。这样Zygote就从Native层进入了Java框架层。



通过JNI调用ZygoteInit的main方法后

```java
public static void main(String argv[]) {
   		...
		//创建一个server端的socket，socketName为zygote
        zygoteServer.registerServerSocketFromEnv(socketName);
        if (!enableLazyPreload) {
            bootTimingsTraceLog.traceBegin("ZygotePreload");
            EventLog.writeEvent(LOG_BOOT_PROGRESS_PRELOAD_START,
                SystemClock.uptimeMillis());
            //预加载资源
            preload(bootTimingsTraceLog);
            EventLog.writeEvent(LOG_BOOT_PROGRESS_PRELOAD_END,
                SystemClock.uptimeMillis());
            bootTimingsTraceLog.traceEnd(); // ZygotePreload
        } else {
            Zygote.resetNicePriority();
        }
		
    	//启动SystemServer
        if (startSystemServer) {
            Runnable r = forkSystemServer(abiList, socketName, zygoteServer);
            if (r != null) {
                r.run();
                return;
            }
        }

        Log.i(TAG, "Accepting command socket connections");

        //等待AMS请求
        caller = zygoteServer.runSelectLoop(abiList);
    } catch (Throwable ex) {
        Log.e(TAG, "System zygote died with exception", ex);
        throw ex;
    } finally {
        zygoteServer.closeServerSocket();
    }

    if (caller != null) {
        caller.run();
    }
}
```



1. 创建一个Server端的Socket。
2. 预加载类和资源。
3. 启动SystemServer进程。
4. 等待AMS请求创建新的应用程序进程。





### 1．registerZygoteSocket

创建LocalServerSocket，并将文件操作符作为参数传进去。在Zygote进程将SystemServer进程启动后，就会在这个服务器端的Socket上等待AMS请求Zygote进程来创建新的应用程序进程。



### 2．启动SystemServer进程

调用Zygote的forkSystemServer方法，其内部会调用nativeForkSystemServer 这个Native 方法，nativeForkSystemServer方法最终会通过fork函数在当前进程创建一个子进程，也就是SystemServer 进程，如果forkSystemServer方法返回的pid的值为0，就表示当前的代码运行在新创建的子进程中。



### 3．runSelectLoop

pollFds进行遍历，如果i==0，说明服务器端Socket 与客户端连接上了，换句话说就是，当前Zygote进程与AMS建立了连接，通过acceptCommandPeer方法得到ZygoteConnection类并添加到Socket连接列表peers中，接着将该ZygoteConnection的fd添加到fd列表fds中，以便可以接收到AMS发送过来的请求。

如果i的值不等于0，则说明AMS向Zygote进程发送了一个创建应用进程的请求，则调用ZygoteConnection的runOnce函数来创建一个新的应用程序进程，并在成功创建后将这个连接从Socket连接列表peers和fd列表fds中清除。



### Zygote进程启动总结

Zygote进程启动共做了如下几件事：

1. 创建AppRuntime并调用其start方法，启动Zygote进程。
2. 创建Java虚拟机并为Java虚拟机注册JNI方法。
3. 通过JNI调用ZygoteInit的main函数进入Zygote的Java框架层。
4. 通过registerZygoteSocket方法创建服务器端Socket，并通过runSelectLoop方法等待AMS的请求来创建新的应用程序进程。
5. 启动SystemServer进程。