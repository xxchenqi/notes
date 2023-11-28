# Xposed

## 简介

Xposed是一款可以在不修改APK的情况下影响程序运行(修改系统)的框架，基于它可以制作出许多功能强大的模块，且在功能不冲突的情况下同时运作。在这个框架下，我们可以编写并加载自己编写的插件App，实现对目标apk的注入、拦截等。



## 项目

地址：https://github.com/rovo89

1、XposedInstaller: 完整AndroidStudio工程，编译生成为apk，用于管理Xposed框架的插件;

2、Xposed: 对Zygote进程定制，能够实现对接下来任何一个app进程的hook;

3、andorid art: 用于支持对java类函数hook而对ART源码进行的一系列定制;

4、XposedBridge: Xposed的java部分，可单独作为AndoridStudio工程进行编译;

5、XposedTools: 用于编译和打包Xposed框架



## 原理

控制zygote进程: 通过替换 /system/bin/app_processs 程序控制zygote进程，使得它在系统启动的过程中会加载Xposed framework的一个jar文件即XposedBridge.jar，从而完成对Zygote进程及其创建的Dalvik/ART虚拟机的劫持，并且能够允许开发者独立的替代任何class，例如framework本身，系统UI又或者随意的一个app。



## 安装

**4.4以下**

1、对需要安装xposed的手机进行root

2、下载并安装xposedinstaller，之后授予其root权限，进入app点击安装即可

很遗憾，当前官网不再维护，导致无法直接通过xposedinstaller下载补丁包



**5.0以上9.0以下**

从Android 5.0开始，谷歌使用ART替换Dalvik，所以xposed安装有点麻烦，分为两个部分: xposed*.zip 和XposedInstaller.apk。zip文件是框架主体，需要进入Recovery后刷入，apk文件是xposed模块管理应用，主要用于下载、激活、是否启用模块等管理功能。

1、完成对手机的root，并刷入recovery (比如twrp)

2、下载对应的zip补丁包，进入recovery刷入

3、重启手机，安装xposedinstaller并授予root权限即可



**9.0以上**

对于Xposed官方不支持的较新的Android版本

相关链接:

https://github.com/ElderDrivers/EdXposed

https://github.com/RikkaApps/Riru/releases

https://github.com/ElderDrivers/EdXposed/releases

https://github.com/ElderDrivers/EdXposedManager



```
1.TWRP刷magisk
注:magisk.apk如果打不卡或报错就把zip改成apk重新安装下
magisk打开提示重新刷magisk就直接在module中在刷magisk.zip
2.magisk 安装 riru
注:riru版本高于26,EdXposed暂时不支持,所以安装25.4.4
3.magisk 安装 EdXposed
4.adb install 安装 EdXposedManager.apk
```



### LSPosed

Android 8.1 ~ 14

https://github.com/LSPosed/LSPosed

```
1.TWRP刷magisk(v24+)
注:magisk.apk如果打不卡或报错就把zip改成apk重新安装下
magisk打开提示重新刷magisk就直接在module中在刷magisk.zip
2.magisk 安装 riru(v26.1.7+)
3.magisk 安装 LSPosed(riru)

注意使用magisk 安装 riru 不是 twrp安装riru
```





## Xposed插件编写

1、拷贝XposedBridgeApi.jar到新建工程的libs目录

2、修改app目录下的build.gradle文件，在AndroidManifest.xml中增加Xposed相关内容

```groovy
// 注：依赖方式改成compileOnly 否则模块会报错
compileOnly fileTree(includes: ['*.jar'], dir: 'libs')
```

```xml
<!--是否配置为 Xposed 插件，设置为 true-->
<meta-data
    android:name="xposedmodule"
    android:value="true" />
<!--模块名称-->
<meta-data
    android:name="xposeddescription"
    android:value="学习插件1" />
<!--最低版本号-->
<meta-data
    android:name="xposedminversion"
    android:value="54" />
```

3、新建hook类，编写hook代码

```java
public class Xposed01 implements IXposedHookLoadPackage {
    // 该方法会在每个软件被启动的时候回调，所以一般需要通过目标包名过滤，内容如下
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {
        Log.e("xxchenqi", "Xposed01: " + loadPackageParam.packageName);
        if (loadPackageParam.packageName.equals("com.example.myapp")) {
            XposedBridge.log("xxchenqi-Xposed01-666");
        }
    }
}
```



4、新建assets文件夹，然后在assets目录下新建文件xposed_init，在里面写上hook类的完整路径

```
com.example.myapp.Xposed01
com.example.myapp.Xposed02
```





## 使用

### hook构造方法

```java
ClassLoader classLoader = loadPackageParam.classLoader;
Class clazz = classLoader.loadClass("com.example.myapp.Student");
// 无参hook
XposedHelpers.findAndHookConstructor(clazz, new XC_MethodHook() {
    @Override
    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
        super.beforeHookedMethod(param);
    }
    @Override
    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
        super.afterHookedMethod(param);
    }
});
//有参hook
XposedHelpers.findAndHookConstructor(clazz, String.class, new XC_MethodHook() {
    @Override
    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
        super.beforeHookedMethod(param);
        // 获取参数
        Object[] args = param.args;
        String name = (String) args[0];
        // 修改参数
        args[0] = "xx";
        XposedBridge.log("xxchenqi=" + name);
    }
    @Override
    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
        super.afterHookedMethod(param);
        // 获取当前对象
        Object thisObject = param.thisObject;
        // 获取返回结果
        Object result = param.getResult();
    }
});
```



### 修改属性

静态属性

```java
ClassLoader classLoader = loadPackageParam.classLoader;
Class clazz = classLoader.loadClass("com.example.myapp.Student");

XposedHelpers.setStaticObjectField(stuclass,"name","cq");
XposedHelpers.getStaticObjectField(stuclass, "name");
```

成员属性

```java
@Override
protected void afterHookedMethod(MethodHookParam param) throws Throwable {
    super.afterHookedMethod(param);
    XposedHelpers.setObjectField(param.thisObject,"name", "cq");
	XposedHelpers.getObjectField(param.thisObject, "name");
}
```

### hook函数(普通、静态、私有、内部类函数、JNI函数)

匿名内部类有编号，直接拷贝就行

```java
XposedHelpers.findAndHookMethod(clazz, "methodName", String.class, int.class, new XC_MethodHook() {
    @Override
    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
        super.beforeHookedMethod(param);
    }
    @Override
    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
        super.afterHookedMethod(param);
        // 修改返回值
        param.setResult("cq");
    }
});
```



### 主动调用函数

```java
// 调用静态函数
XposedHelpers.callStaticMethod(stuClass, "staticMethodName", "cq", 100);
// 调用构造函数
Object stuObject = XposedHelpers.newInstance(stuClass, "cq", 18);
// 调用普通函数
XposedHelpers.callMethod(stuObject, "methodName", params, "cq", 100);
```



### 加壳APP处理

思路：一般加壳App在Application的attachBaseContext和onCreate方法中进进行dex解密和ClassLoader修正，所以在onCreate之后获取到ClassLoader进行类的hook。

#### 360

```java
public class XposedHook360 implements IXposedHookLoadPackage {
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {
        if (loadPackageParam.packageName.equals("com.cq.xposedhook01")) {

            XposedHelpers.findAndHookMethod("com.stub.StubApp", loadPackageParam.classLoader, "onCreate", new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(XC_MethodHook.MethodHookParam param) throws Throwable {
                    super.beforeHookedMethod(param);
                }

                @Override
                protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                    super.afterHookedMethod(param);
                    ClassLoader finalClassLoader = getClassloader();
                }
            });
        }
    }

    public static ClassLoader getClassloader() {
        ClassLoader resultClassloader = null;
        Object currentActivityThread = invokeStaticMethod(
                "android.app.ActivityThread", "currentActivityThread",
                new Class[]{}, new Object[]{});
        Object mBoundApplication = getFieldOjbect(
                "android.app.ActivityThread", currentActivityThread,
                "mBoundApplication");
        Application mInitialApplication = (Application) getFieldOjbect("android.app.ActivityThread",
                currentActivityThread, "mInitialApplication");
        Object loadedApkInfo = getFieldOjbect(
                "android.app.ActivityThread$AppBindData",
                mBoundApplication, "info");
        Application mApplication = (Application) getFieldOjbect("android.app.LoadedApk", loadedApkInfo, "mApplication");
        resultClassloader = mApplication.getClassLoader();
        return resultClassloader;
    }


    public static Object invokeStaticMethod(String class_name,
                                            String method_name, Class[] pareTyple, Object[] pareVaules) {

        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getMethod(method_name, pareTyple);
            return method.invoke(null, pareVaules);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object getFieldOjbect(String class_name, Object obj,
                                        String filedName) {
        try {
            Class obj_class = Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        return null;
    }
}

```

#### lebo

```java
public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {
    if (loadPackageParam.packageName.equals("com.f0208.lebo")) {
        XposedHelpers.findAndHookMethod("com.f0208.lebo.MyWrapperProxyApplication", loadPackageParam.classLoader, "onCreate", new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
            }

            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                ClassLoader finalClassLoader=getClassloader();
                XposedHelpers.findAndHookMethod("com.zw.lebo.MainActivity", finalClassLoader, "onCreate", Bundle.class, new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                    }

                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                    }
                });
            }
        });
    }
}
```



### 动态加载dex处理

原理:hook DexClassLoader的构造方法

 ```java
 XposedHelpers.findAndHookConstructor(DexClassLoader.class, String.class, String.class, String.class, ClassLoader.class, new XC_MethodHook() {
     @Override
     protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
         super.beforeHookedMethod(param);
     }
 
     @Override
     protected void afterHookedMethod(MethodHookParam param) throws Throwable {
         super.afterHookedMethod(param);
         DexClassLoader classLoader = (DexClassLoader) param.thisObject;
         // hook ....
     }
 });
 ```



### hook so

**so被加载后会依次执行以下函数:**

```c++
extern "C" void _init(void){}

__attribute__ ((constructor(10), visibility ("hidden"))) void initarray_4(void)

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
```





#### 32位(inline-hook)

https://github.com/ele7enxxh/Android-Inline-Hook

思路：hook Runtime.loadLibary方法，加载自己的so库，此so内部进行inline hook，hook so中的函数(记得修改777权限)

为什么不hook System.loadLibrary?  因为ClassLoader不对,获取到的是Xposed的ClassLoader



**注：7.0开始 hook的函数是Runtime.loadLibrary0,参数也有变化**

```java
// 7.0之前
XposedHelpers.findAndHookMethod("java.lang.Runtime", loadPackageParam.classLoader, "loadLibrary", String.class,ClassLoader.class, new XC_MethodHook() {
}
                                
// 7.0之后
XposedHelpers.findAndHookMethod("java.lang.Runtime", loadPackageParam.classLoader, "loadLibrary0", ClassLoader.class, String.class, new XC_MethodHook() {
}
```





```c
#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>

extern "C" {
#include "include/inlineHook.h"
}

// 定义指针保存原来的函数
void *(*old_strstr)(char *, char *) = nullptr;

// 替换的函数
void *new_strstr(char *arg0, char *arg1) {
    if (strcmp(arg1, "hookso") == 0) {
        int a = 1;
        return &a;
    } else {
        void *result = old_strstr(arg0, arg1);
        return result;
    };
}

void starthooklibc() {
    void *libchandle = dlopen("libc.so", RTLD_NOW);
    void *strstr_addr = dlsym(libchandle, "strstr");
    if (registerInlineHook((uint32_t) strstr_addr, (uint32_t) new_strstr,  (uint32_t **) &old_strstr) !=
        ELE7EN_OK) {
        return;
    }
    if (inlineHook((uint32_t) strstr_addr) == ELE7EN_OK) {
        __android_log_print(4, "hookso", "hook libc.so->strstr success!");
        //return -1;
    }
}

// so加载最先运行的函数
extern "C" void _init(void) {
    starthooklibc();
}
```





#### 32/64位(SandHook)

https://github.com/asLody/SandHook

说明：SandHook不能放在_init，应该放在jniLoad，因为SandHook初始化是晚于init方法

```c++
void *(*old_strstr)(char *, char *) = nullptr;

void *new_strstr(char *arg0, char *arg1) {
    if (strcmp(arg1, "hookso") == 0) {
        int a = 1;
        return &a;
    } else {
        void *result = old_strstr(arg0, arg1);
        return result;
    };
}

void starthooklibc() {
    if (sizeof(void *) == 8) { // 64位
        const char *libcpath = "/system/lib64/libc.so";
        old_strstr = reinterpret_cast<void *(*)(char *, char *)>(
            SandInlineHookSym(libcpath, "strstr", reinterpret_cast<void *>(new_strstr)));
    } else { // 32位
        const char *libcpath = "/system/lib/libc.so";
        old_strstr = reinterpret_cast<void *(*)(char *, char *)>(
            SandInlineHookSym(libcpath, "strstr", reinterpret_cast<void *>(new_strstr)));
    }
}

extern "C" jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("go into JNI_OnLoad");
    starthooklibc();
    return JNI_VERSION_1_6;
}
```



#### so函数主动调用(获取函数地址)

```java
public class XposedHookSo implements IXposedHookLoadPackage {
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam loadPackageParam) throws Throwable {
        Log.i("XposedHookSo", loadPackageParam.packageName);
        XposedBridge.log("XposedHookSo->app packagename" + loadPackageParam.packageName);
        if (loadPackageParam.packageName.equals("com.cq.test")) {
            XposedBridge.log("XposedHookSo " + loadPackageParam.packageName);

           XposedHelpers.findAndHookMethod("java.lang.Runtime", loadPackageParam.classLoader, "loadLibrary", String.class,ClassLoader.class, new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                    super.beforeHookedMethod(param);
                    String soname= (String) param.args[0];
                    XposedBridge.log("beforeHookedMethod Runtime.load("+soname+","+param.args[1]+")");
                }

                @Override
                protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                    super.afterHookedMethod(param);
                    String soname= (String) param.args[0];
                    XposedBridge.log("afterHookedMethod Runtime.load("+soname+")");
                    if(soname.contains("native-lib")){
                        System.load("/data/data/com.cq.test/files/hookso.so");
                    }
                }
            });
        }
    }
}
```



```c++
void activecalltesthook() {
    typedef int (*testhook)(const char *a1);

    testhook testhookfunction = nullptr;
    //得到函数地址
/*    extern "C"
    EXPORT void* SandGetModuleBase(const char* so);*/

    void *libnativebase = SandGetModuleBase("libnative-lib.so");
    // thumb指令集，地址是奇数，所以要+1
    if(sizeof(void*)==4){
        unsigned long tmpaddr = (unsigned long) libnativebase + 0x8B4C + 1;
        void *testhookaddr = reinterpret_cast<void *>(tmpaddr);
        testhookfunction = reinterpret_cast<testhook>(testhookaddr);
    }else{
        unsigned long tmpaddr = (unsigned long) libnativebase + 0xf67c;
        void *testhookaddr = reinterpret_cast<void *>(tmpaddr);
        testhookfunction = reinterpret_cast<testhook>(testhookaddr);
        
        LOGD("libnative-lib.so base:%p,testfuncaddr:%p",libnativebase,(void*)tmpaddr);
    }

    int result1 = testhookfunction("aaaaahookso");
    LOGD("testhookfunction(\"aaaaahookso\"); return:%d", result1);
    int result2 = testhookfunction("aaaabbbbb");
    LOGD("testhookfunction(\"aaaabbbbb\"); return:%d", result2);
}
```





### dumpDex

#### 方式1(8.0之前可用)

原理：通过java.lang.Class.getDex方法获取到Dex，然后通过com.android.dex.Dex.getBytes获取到Dex的数据

```java
public class XposedFdex implements IXposedHookLoadPackage {
    Class Dex;
    Method Dex_getBytes;
    Method getDex;
    String packagename;

    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        XposedBridge.log("目标包名：" + lpparam.packageName);
        if (lpparam.packageName.equals("com.example.cq")) {
            initReflect();
            packagename = lpparam.packageName;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(10 * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    // 针对加固获取classLoader
                    ClassLoader classLoader = getClassloader();
                    try {
                        Class MainActivityClass = classLoader.loadClass("com.example.cq.MainActivity");
                        dumpdexbyclass(MainActivityClass);
                    } catch (ClassNotFoundException e) {
                        e.printStackTrace();
                    } catch (IllegalAccessException e) {
                        e.printStackTrace();
                    } catch (InvocationTargetException e) {
                        e.printStackTrace();
                    }


                }
            }).start();
        }
    }

    public void dumpdexbyclass(Class cls) throws InvocationTargetException, IllegalAccessException {
        String name = cls.getName();
        XposedBridge.log("当前类名：" + name);
        Object dexObj = getDex.invoke(cls);
        byte[] bArr = (byte[]) Dex_getBytes.invoke(dexObj);
        if (bArr == null) {
            XposedBridge.log("数据为空：返回");
            return;
        }
        XposedBridge.log("开始写数据");
        String dex_path = "/data/data/" + packagename + "/" + packagename + "_" + bArr.length + ".dex";
        XposedBridge.log(dex_path);
        File file = new File(dex_path);
        if (file.exists()) return;
        writeByte(bArr, file.getAbsolutePath());
    }

    public void initReflect() {
        try {
            Dex = Class.forName("com.android.dex.Dex");
            Dex_getBytes = Dex.getDeclaredMethod("getBytes");
            getDex = Class.forName("java.lang.Class").getDeclaredMethod("getDex");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    public void writeByte(byte[] bArr, String str) {
        try {
            XposedBridge.log("write dex to:" + str);
            OutputStream outputStream = new FileOutputStream(str);
            outputStream.write(bArr);
            outputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
            XposedBridge.log("文件写出失败");
        }
    }

    public static Field getClassField(ClassLoader classloader, String class_name, String filedName) {

        try {
            Class obj_class = classloader.loadClass(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field;
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static Object getClassFieldObject(ClassLoader classloader, String class_name, Object obj,
                                             String filedName) {

        try {
            Class obj_class = classloader.loadClass(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            Object result = null;
            result = field.get(obj);
            return result;
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object invokeStaticMethod(String class_name,
                                            String method_name, Class[] pareTyple, Object[] pareVaules) {

        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getMethod(method_name, pareTyple);
            return method.invoke(null, pareVaules);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object getFieldOjbect(String class_name, Object obj,
                                        String filedName) {
        try {
            Class obj_class = Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static ClassLoader getClassloader() {
        ClassLoader resultClassloader = null;
        Object currentActivityThread = invokeStaticMethod(
                "android.app.ActivityThread", "currentActivityThread",
                new Class[]{}, new Object[]{});
        Object mBoundApplication = getFieldOjbect(
                "android.app.ActivityThread", currentActivityThread,
                "mBoundApplication");
        Application mInitialApplication = (Application) getFieldOjbect("android.app.ActivityThread",
                currentActivityThread, "mInitialApplication");
        Object loadedApkInfo = getFieldOjbect(
                "android.app.ActivityThread$AppBindData",
                mBoundApplication, "info");
        Application mApplication = (Application) getFieldOjbect("android.app.LoadedApk", loadedApkInfo, "mApplication");
        resultClassloader = mApplication.getClassLoader();
        return resultClassloader;
    }
}
```



#### 方式2(inlinehook)

原理：通过inlinehook，hook相关函数(有dex的起始地址和size的函数)，获取到dex的begin和size，然后dump。

此方法6.0测试没问题

```c++
#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

extern "C" {
#include "include/inlineHook.h"
}

struct DexFile{
    void* vfn;
    void* dexfilebegin;
    uint32_t size;
};

void *(*old_ResolveType)(void*a,void*b,void*c,void*d,void*e) = nullptr;

void dumpdex(void* begin,uint32_t size){
    char *dexfilepath=(char*)malloc(sizeof(char)*1000);
    if(dexfilepath==nullptr)
    {
       return;
    }
    int result=0;
    memset(dexfilepath,0,1000);
    int fcmdline =-1;
    char szCmdline[64]= {0};
    char szProcName[256] = {0};
    int procid = getpid();
    sprintf(szCmdline,"/proc/%d/cmdline", procid);
    fcmdline = open(szCmdline, O_RDONLY,0644);
    if(fcmdline >0)
    {
        result=read(fcmdline, szProcName,256);
        close(fcmdline);

    }

    if(szProcName[0])
    {
        const uint8_t* begin_= reinterpret_cast<const uint8_t *>(begin);
        size_t size_=size;
        memset(dexfilepath,0,1000);
        int size_int_=(int)size_;

        memset(dexfilepath,0,1000);
        sprintf(dexfilepath,"/data/data/%s/%d_dexfile_Resolve.dex",szProcName,size_int_);
        int dexfilefp=open(dexfilepath,O_RDONLY,0666);
        if(dexfilefp>0){
            close(dexfilefp);
            dexfilefp=0;

        }else{
            int fp=open(dexfilepath,O_CREAT|O_APPEND|O_RDWR,0666);
            if(fp>0)
            {
                result=write(fp,(void*)begin_,size_);
                fsync(fp);
                close(fp);
            }
        }
    }
    if(dexfilepath!=nullptr)
    {
        free(dexfilepath);
        dexfilepath=nullptr;
    }
}
void *new_ResolveType(void*a,void*b,void*c,void*d,void*e) {
    __android_log_print(4, "hookso", "go into new_resolveType");
    DexFile* dexfile = reinterpret_cast<DexFile *>(b);
    void* dexfilebegin=dexfile->dexfilebegin;
    uint32_t dexfilesize=dexfile->size;
    dumpdex(dexfilebegin,dexfilesize);
    __android_log_print(4, "hookso", "go into new_resolveType,begin:%p,size:%d",dexfilebegin,dexfilesize);
    void *result = old_ResolveType(a,b,c,d,e);
    return result;

}
void hooklibart(){
    void* handle=dlopen("libart.so",RTLD_NOW);
   //ResolveType(art::DexFile const&, unsigned short, art::Handle<art::mirror::DexCache>, art::Handle<art::mirror::ClassLoader>)
    void* funcaddr=dlsym(handle,"_ZN3art11ClassLinker11ResolveTypeERKNS_7DexFileEtNS_6HandleINS_6mirror8DexCacheEEENS4_INS5_11ClassLoaderEEE");

    if (registerInlineHook((uint32_t) funcaddr, (uint32_t) new_ResolveType,
                           (uint32_t **) &old_ResolveType) !=
        ELE7EN_OK) {
        return;
    }
    if (inlineHook((uint32_t) funcaddr) == ELE7EN_OK) {
        __android_log_print(4, "hookso", "hook libc.so->Resolve success!");
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    jint result = -1;
    hooklibart();
    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        printf("err!!");
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
```





### method补全

原理：加固导致方法抽空，通过加载每个类让方法还原

```java
public class FART implements IXcustomHookLoadPackage {
    static String packagename = "com.android.";
    static String system_server = "system_server";

    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        if (!lpparam.packageName.startsWith(packagename) && !lpparam.packageName.equals(system_server)) {
            HookDexClassLoader(lpparam);
            HookPathDexClassLoader(lpparam);
            fartthread();
        }
    }

    public static void HookPathDexClassLoader(XC_LoadPackage.LoadPackageParam lpparam) {
        final Class<?> PathClassLoaderclazz = XcustomHelpers.findClass("dalvik.system.PathClassLoader", lpparam.classLoader);
        XcustomHelpers.findAndHookConstructor(PathClassLoaderclazz, String.class, ClassLoader.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
            }

            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                try {
                    ClassLoader PathClassLoaderObj = (ClassLoader) param.getResult();
                    XcustomBridge.log("PathClassLoaderObj->afterHookedMethod:getResult->" + PathClassLoaderObj);
                    //TestClassloader((ClassLoader) BaseDexClassLoaderObj);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                final ClassLoader PathClassLoaderObj = (ClassLoader) param.thisObject;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            XcustomBridge.log("PathClassLoader->afterHookedMethod:thisObject->" + PathClassLoaderObj + "--start unpack,sleep....");
                            Thread.sleep(60000);
                            XcustomBridge.log("BaseDexClassLoader->afterHookedMethod:thisObject->" + PathClassLoaderObj + "--start unpack,sleep over");
                            TestClassloader(PathClassLoaderObj);
                            XcustomBridge.log("PathClassLoader->afterHookedMethod:thisObject->" + PathClassLoaderObj + "--unpack over");
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();


            }
        });
        //public PathClassLoader(String dexPath, String libraryPath,ClassLoader parent)
        XcustomHelpers.findAndHookConstructor(PathClassLoaderclazz, String.class, String.class, ClassLoader.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
            }

            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                final ClassLoader PathClassLoaderObj = (ClassLoader) param.thisObject;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            XcustomBridge.log("PathClassLoader->afterHookedMethod:String libraryPath,thisObject->" + PathClassLoaderObj + "--start unpack,sleep....");
                            Thread.sleep(60000);
                            XcustomBridge.log("BaseDexClassLoader->afterHookedMethod:String libraryPath,thisObject->" + PathClassLoaderObj + "--start unpack,sleep over");
                            TestClassloader(PathClassLoaderObj);
                            XcustomBridge.log("PathClassLoader->afterHookedMethod:String libraryPath,thisObject->" + PathClassLoaderObj + "--unpack over");
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();


            }
        });

    }

    public static void HookDexClassLoader(XC_LoadPackage.LoadPackageParam lpparam) {
        XcustomBridge.log("go into HookDexClassLoader");
        //public DexClassLoader(String dexPath, String optimizedDirectory,String libraryPath, ClassLoader parent)
        final Class<?> DexClassLoaderclazz = XcustomHelpers.findClass("dalvik.system.DexClassLoader", lpparam.classLoader);
        //Hook有参构造函数,获取返回的实例
        XcustomBridge.log("Xcustom Get DexClassLoader->" + DexClassLoaderclazz);
        XcustomHelpers.findAndHookConstructor(DexClassLoaderclazz, String.class, String.class, String.class, ClassLoader.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                //param.args[0] = "Haha, HookDemo(str) are hooked";
                String dexpath = (String) param.args[0];
                String optimizedDirectory = (String) param.args[1];
                String libraryPath = (String) param.args[2];
                ClassLoader parent = (ClassLoader) param.args[3];
                try {
                    XcustomBridge.log("[DexClassLoader]->beforeHookedMethod(String dexPath, File optimizedDirectory,String libraryPath, ClassLoader parent)--dexpath:"
                            + dexpath);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                try {
                    XcustomBridge.log("[DexClassLoader]->beforeHookedMethod(String dexPath, File optimizedDirectory,String libraryPath, ClassLoader parent)--optimizedDirectory:"
                            + optimizedDirectory);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                try {
                    XcustomBridge.log("[DexClassLoader]->beforeHookedMethod(String dexPath, File optimizedDirectory,String libraryPath, ClassLoader parent)--libraryPath:"
                            + libraryPath);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                try {
                    XcustomBridge.log("[DexClassLoader]->beforeHookedMethod(String dexPath, File optimizedDirectory,String libraryPath, ClassLoader parent)--ClassLoader:"
                            + parent);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                super.beforeHookedMethod(param);
            }

            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                try {
                    ClassLoader DexClassLoaderObj = (ClassLoader) param.getResult();
                    XcustomBridge.log("DexClassLoader->afterHookedMethod:getResult->" + DexClassLoaderObj);
                    //TestClassloader((ClassLoader) BaseDexClassLoaderObj);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                final ClassLoader DexClassLoaderObj = (ClassLoader) param.thisObject;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            XcustomBridge.log("DexClassLoader->afterHookedMethod:thisObject->" + DexClassLoaderObj + "--start unpack,start sleep....");
                            Thread.sleep(60000);
                            XcustomBridge.log("DexClassLoader->afterHookedMethod:thisObject->" + DexClassLoaderObj + "--start unpack,sleep over");
                            TestClassloader(DexClassLoaderObj);
                            XcustomBridge.log("DexClassLoader->afterHookedMethod:thisObject->" + DexClassLoaderObj + "--unpack over");
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();


            }
        });

    }

    //add
    public static Field getClassField(ClassLoader classloader, String class_name,
                                      String filedName) {

        try {
            Class obj_class = classloader.loadClass(class_name);//Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field;
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object getClassFieldObject(ClassLoader classloader, String class_name, Object obj,
                                             String filedName) {

        try {
            Class obj_class = classloader.loadClass(class_name);//Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            Object result = null;
            result = field.get(obj);
            return result;
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object invokeStaticMethod(String class_name,
                                            String method_name, Class[] pareTyple, Object[] pareVaules) {

        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getMethod(method_name, pareTyple);
            return method.invoke(null, pareVaules);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static Object getFieldOjbect(String class_name, Object obj,
                                        String filedName) {
        try {
            Class obj_class = Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        return null;

    }

    public static ClassLoader getClassloader() {
        ClassLoader resultClassloader = null;
        try {
            Object currentActivityThread = invokeStaticMethod(
                    "android.app.ActivityThread", "currentActivityThread",
                    new Class[]{}, new Object[]{});
            Object mBoundApplication = getFieldOjbect(
                    "android.app.ActivityThread", currentActivityThread,
                    "mBoundApplication");
            Application mInitialApplication = (Application) getFieldOjbect("android.app.ActivityThread",
                    currentActivityThread, "mInitialApplication");
            Object loadedApkInfo = getFieldOjbect(
                    "android.app.ActivityThread$AppBindData",
                    mBoundApplication, "info");
            Application mApplication = (Application) getFieldOjbect("android.app.LoadedApk", loadedApkInfo, "mApplication");
            XcustomBridge.log("Applicatoin->" + mApplication);
            resultClassloader = mApplication.getClassLoader();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return resultClassloader;
    }

    public static void loadClassAndInvoke(ClassLoader appClassloader, String eachclassname, Method dumpMethodCode_method) {
        Class resultclass = null;
        Log.i("ActivityThread", "go into loadClassAndInvoke->" + "classname:" + eachclassname);
        try {
            resultclass = appClassloader.loadClass(eachclassname);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        } catch (Error e) {
            e.printStackTrace();
            return;
        }

        if (resultclass != null) {
            try {
                Constructor<?> cons[] = resultclass.getDeclaredConstructors();
                for (Constructor<?> constructor : cons) {
                    if (dumpMethodCode_method != null) {
                        try {
                            dumpMethodCode_method.invoke(null, constructor);
                        } catch (Exception e) {
                            e.printStackTrace();
                            continue;
                        } catch (Error e) {
                            e.printStackTrace();
                            continue;
                        }
                    } else {
                        Log.e("ActivityThread", "dumpMethodCode_method is null ");
                    }

                }
            } catch (Exception e) {
                e.printStackTrace();
            } catch (Error e) {
                e.printStackTrace();
            }
            try {
                Method[] methods = resultclass.getDeclaredMethods();
                if (methods != null) {
                    for (Method m : methods) {
                        if (dumpMethodCode_method != null) {
                            try {
                                dumpMethodCode_method.invoke(null, m);
                            } catch (Exception e) {
                                e.printStackTrace();
                                continue;
                            } catch (Error e) {
                                e.printStackTrace();
                                continue;
                            }
                        } else {
                            Log.e("ActivityThread", "dumpMethodCode_method is null ");
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } catch (Error e) {
                e.printStackTrace();
            }
        }
    }

    public static void fart() {
        ClassLoader appClassloader = getClassloader();
        if (appClassloader == null) {
            return;
        }
        ClassLoader tmpClassloader = appClassloader;
        ClassLoader parentClassloader = appClassloader.getParent();


        if (appClassloader.toString().indexOf("java.lang.BootClassLoader") == -1) {
            TestClassloader(appClassloader);
        }
        while (parentClassloader != null) {
            if (parentClassloader.toString().indexOf("java.lang.BootClassLoader") == -1) {
                TestClassloader(parentClassloader);
            }
            tmpClassloader = parentClassloader;
            parentClassloader = parentClassloader.getParent();
        }
    }

    public static void TestClassloader(ClassLoader appClassloader) {
        Field pathList_Field = (Field) getClassField(appClassloader, "dalvik.system.BaseDexClassLoader", "pathList");
        Object pathList_object = getFieldOjbect("dalvik.system.BaseDexClassLoader", appClassloader, "pathList");
        Object[] ElementsArray = (Object[]) getFieldOjbect("dalvik.system.DexPathList", pathList_object, "dexElements");
        Field dexFile_fileField = null;
        try {
            dexFile_fileField = (Field) getClassField(appClassloader, "dalvik.system.DexPathList$Element", "dexFile");
        } catch (Exception e) {
            e.printStackTrace();
        } catch (Error e) {
            e.printStackTrace();
        }
        Class DexFileClazz = null;
        try {
            DexFileClazz = appClassloader.loadClass("dalvik.system.DexFile");
        } catch (Exception e) {
            e.printStackTrace();
        } catch (Error e) {
            e.printStackTrace();
        }
        Method getClassNameList_method = null;
        Method XcustomMethodCode_method = null;
        for (Method field : DexFileClazz.getDeclaredMethods()) {
            if (field.getName().equals("getClassNameList")) {
                getClassNameList_method = field;
                getClassNameList_method.setAccessible(true);
            }
            if (field.getName().equals("XcustomMethodCode")) {
                XcustomMethodCode_method = field;
                XcustomMethodCode_method.setAccessible(true);
            }
        }
        Field mCookiefield = getClassField(appClassloader, "dalvik.system.DexFile", "mCookie");
        Log.v("ActivityThread->methods", "dalvik.system.DexPathList.ElementsArray.length:" + ElementsArray.length);//5个
        for (int j = 0; j < ElementsArray.length; j++) {
            Object element = ElementsArray[j];
            Object dexfile = null;
            try {
                dexfile = (Object) dexFile_fileField.get(element);
            } catch (Exception e) {
                e.printStackTrace();
            } catch (Error e) {
                e.printStackTrace();
            }
            if (dexfile == null) {
                Log.e("ActivityThread", "dexfile is null");
                continue;
            }
            if (dexfile != null) {


                Object mcookie = getClassFieldObject(appClassloader, "dalvik.system.DexFile", dexfile, "mCookie");
                if (mcookie == null) {
                    Object mInternalCookie = getClassFieldObject(appClassloader, "dalvik.system.DexFile", dexfile, "mInternalCookie");
                    if (mInternalCookie != null) {
                        mcookie = mInternalCookie;
                    } else {
                        Log.v("ActivityThread->err", "get mInternalCookie is null");
                        continue;
                    }

                }
                String[] classnames = null;
                try {
                    classnames = (String[]) getClassNameList_method.invoke(dexfile, mcookie);
                } catch (Exception e) {
                    e.printStackTrace();
                    continue;
                } catch (Error e) {
                    e.printStackTrace();
                    continue;
                }
                if (classnames != null) {
                    for (String eachclassname : classnames) {
                        XcustomBridge.log("BaseDexClassLoader->TestClassLoader:classname->" + eachclassname);
                        loadClassAndInvoke(appClassloader, eachclassname, XcustomMethodCode_method);
                    }
                }

            }
        }
        return;
    }

    public static void fartthread() {
        new Thread(new Runnable() {

            @Override
            public void run() {
                try {
                    Log.e("ActivityThread", "start sleep......");
                    Thread.sleep(1 * 30 * 1000);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                Log.e("ActivityThread", "sleep over and start fart");
                fart();
                Log.e("ActivityThread", "fart run over");

            }
        }).start();
    }
}
```



### 注意事项

#### dlopen

```
经测试
6.0可以直接写 dlopen("libnative-lib.so", RTLD_NOW);
但是在9.0需要写具体路径
void *handle = dlopen("/data/app/com.kanxue.test2-kHmupHW2cTdpH9WuZHNENA==/lib/arm64/libnative-lib.so", RTLD_NOW);
其他版本还未测试
```

#### 导出函数

```
导出函数可以直接通过dlopen+dlsym调用相关函数。
未导出函数需要通过IDA或者Frida找到so的地址+函数的偏移地址，才能调用。32位的thumb指令集，地址是奇数，所以偏移地址要+1,64位不需要(这个解释暂不太确定)
```



#### 爆破

在暴力破解写多层嵌套循环打日志时，部分日志可能丢失是正常现象，代码还是正常会执行的。



## 其他

debug：BDopener



抓包：https://github.com/Fuzion24/JustTrustMe



Xposed framework下载地址：

https://androidfilehost.com/?w=files&flid=227007

https://web.archive.org/web/20220508105706/https://dl-xda.xposed.info/framework/





获取类列表

```java
public void GetClassLoaderClasslist(ClassLoader classLoader) {
    XposedBridge.log("start dealwith classloader:" + classLoader);
    Object pathListObj = XposedHelpers.getObjectField(classLoader, "pathList");

    Object[] dexElementsObj = (Object[]) XposedHelpers.getObjectField(pathListObj, "dexElements");
    for (Object i : dexElementsObj) {
        Object dexFileObj = XposedHelpers.getObjectField(i, "dexFile");
        Object mCookieObj = XposedHelpers.getObjectField(dexFileObj, "mCookie");
        Class DexFileClass = XposedHelpers.findClass("dalvik.system.DexFile", classLoader);
        String[] classlist = (String[]) XposedHelpers.callStaticMethod(DexFileClass, "getClassNameList", mCookieObj);
        for (String classname : classlist) {
            XposedBridge.log(dexFileObj + "---" + classname);
        }
    }
    XposedBridge.log("end dealwith classloader:" + classLoader);
}

```





https://github.com/w568w/XposedChecker

https://github.com/smartdone/dexdump

https://github.com/halfkiss/ZjDroid

https://github.com/heartbee/Va_Fdex2



有导出函数和无导出函数：

有导出可以直接通过dlopen，无导出函数需要找函数地址







