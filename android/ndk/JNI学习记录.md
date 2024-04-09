# JNI学习记录

### 1.预处理器（宏函数）

| 预处理器 | 描述 |
| -------- | ---------- |
| #include | 导入头文件   |
| #if     | if判断操作  【if的范畴 必须endif】   |
| #elif   | else if   |
| #else   | else   |
| #endif  | 结束if   |
| #define | 定义一个宏   |
| #ifdef  | 如果定义了这个宏 【if的范畴 必须endif】   |
| #ifndef | 如果没有定义这个宏 【if的范畴 必须endif】   |
| #undef  | 取消宏定义   |
| #pragma | 设定编译器的状态	|



**优点**：

文本替换，不会造成函数的调用开销(开辟栈空间，形参压栈，函数弹栈释放 ..)


**缺点**：

会导致代码体积增大



**PS：在开发中，宏都是大写。**



例1：

ifndef -> define 这类操作主要用来防止重复定义问题

if前缀操作必须要使用endif来结束

```C
#ifndef CLIONCPPPROJECT_T2_H // 如果没有定义这个宏  解决循环拷贝的问题
#define CLIONCPPPROJECT_T2_H // 定义这个宏

// 只有第一次能进来,之后就无法进入,解决循环拷贝的问题

#ifndef isRelease // 如果没有isRelease这个宏
#define isRelease 1 // 定义isRelease

#if isRelease == true
#define RELEASE // 正式环境下 定义RELEASE宏

#elif isRelease == false
#define DEBUG // 测试环境下  定义DEBUG宏

#endif // 结束里面的if
#endif // 结束里面的if

#endif // 结束外面的if
```

```C
#include <iostream>
#include "test.h"

using namespace std;

int main() {

#ifdef DEBUG // 是否定义了DEBUG这个宏
    cout << "在测试环境下" << endl;

#else RELEASE
    cout << "在正式环境下" << endl;

#endif // 结束IF

}
```



例2：

宏函数参数列表无需类型，返回值看不到

复杂的宏函数需要在末尾加上反斜杠\，但是末尾不需要加反斜杠

```C
#include <iostream>

using namespace std;

#define SHOW(V) cout << V << endl; // 参数列表 无需类型  返回值 看不到
#define ADD(n1, n2) n1 + n2
#define CHE(n1, n2) n1 * n2 // 故意制作问题 ，宏函数的注意事项

// 复杂的宏函数需要在末尾加上反斜杠
#define LOGIN(V) if(V==1) {             \
    cout << "YES:" << V << endl;        \
} else {                                \
    cout << "NO:" << V << endl;         \
} // 这个是结尾，不需要加 \


int main() {
    SHOW(666);

    int r = ADD(1, 2);
    cout << r << endl;

    r = ADD(1 + 1, 2 + 2);
    cout << r << endl;

//    r = 1 + 1 * 2 + 2; // 文本替换：1+1 * 2+2  先算乘法  最终等于 5
    r = CHE(1 + 1, 2 + 2);
    cout << r << endl;
    LOGIN(0);
    return 0;
}


```



### 2.动态库和静态库

静态库的代码在编译过程中已经被载入可执行程序,因此体积比较大；

动态库(共享库)的代码在可执行程序运行时才载入内存，在编译过程中仅简单的引用，因此代码体积比较小。



### 3.system.load()/system.loadLibrary()区别

#### System.load

```
System.load 参数必须为库文件的绝对路径，可以是任意路径，例如： 
System.load("C:\Documents and Settings\TestJNI.dll"); //Windows
System.load("/usr/lib/TestJNI.so"); //Linux
```

#### System.loadLibrary

```
System.loadLibrary 参数为库文件名，不包含库文件的扩展名。
System.loadLibrary ("TestJNI"); //加载Windows下的TestJNI.dll本地库
System.loadLibrary ("TestJNI"); //加载Linux下的libTestJNI.so本地库
```





###  4.Java和C/C++中的基本类型的映射关系


| java类型 | jni类型    | 描述             |
| :------- | :--------- | ---------------- |
| boolean  | jboolean   | unsigned 8 bits  |
| byte     | jbyte      | signed 8 bits    |
| char     | jchar      | unsigned 16 bits |
| short    | jshort     | signed 16 bits   |
| int      | jint       | signed 32 bits   |
| long     | jlong      | signed 64 bits   |
| float    | jfloat     | signed 32bit     |
| double   | jdouble    | signed 64 bits   |
| Class    | jclass     | class类对象      |
| String   | jstring    | 字符串对象       |
| Object   | jobject    | 任何java对象     |
| byte[]   | jbyteArray | byte数组         |



### 5.java字段的签名

| 数据类型 | 签名                                                         |
| :------- | :----------------------------------------------------------- |
| boolean  | Z                                                            |
| byte     | B                                                            |
| char     | C                                                            |
| short    | S                                                            |
| int      | I                                                            |
| long     | J                                                            |
| float    | F                                                            |
| double   | D                                                            |
| void     | V                                                            |
| Object   | L开头，然后以/分割包完整卡类型，最后加上;分号<br />例如：String签名就是 Ljava/lang/String; |
| Array    | [开头，在加上元素类型签名<br />例如：<br />int[]签名[I<br />int[] []签名[II<br />object数组签名[Ljava/lang/Object; |

#### 函数签名：

(ILjava/util/Date;[I)I



#### 查看签名：

使用javap命令:

javap -s -p JniTes.class

-s 输出xxxx.class的所有属性和方法的签名，   -p 忽略私有公开的所有属性方法全部输出



### 6.静态注册

生成头文件命令(需要在包名根目录执行该命令)：javah 包名.类名



优点：开发简单
缺点：
1.JNI函数名非常长
2.捆绑 上层 包名 + 类名
3.运行期 才会去 匹配JNI函数，性能上 低于 动态注册



头文件

```C
#include <jni.h>
#include <string>

// 解决循环Copy的问题 第二次就进不来了
#ifndef _Included_com_cq_as_jni_project_MainActivity // 如果没有定义这个宏
#define _Included_com_cq_as_jni_project_MainActivity // 定义这个宏

#ifdef __cplusplus // 如果是C++环境,增加 extern "C" {
extern "C" { // 全部采用C的方式，因为不允许函数重载
#endif

// 函数的声明
// 注意：包名中.会改成_,_会改成_1
JNIEXPORT jstring JNICALL Java_com_cq_as_1jni_1project_MainActivity_getStringPwd
  (JNIEnv *, jobject);


#ifdef __cplusplus //如果是C++环境,增加结尾花括号}
}
#endif

#endif
```

实现:

```C
#include "com_cq_as_jni_project_MainActivity.h"

// NDK工具链里面的 log 库 引入过来
#include <android/log.h>

// __VA_ARGS__ 代表 ...的可变参数
#define TAG "xxchenqi"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)


extern "C" // 必须采用C的编译方式
JNIEXPORT // 标记该方法可以被外部调用
jstring // Java <---> native 转换用的
JNICALL // JNI标记，可以不需要(最好加上)
// Java_包名_类名_方法名
// JNIEnv * env  JNI的桥梁环境
// jobject jobj 谁调用，就是谁的实例  MainActivity this
// jclass clazz 谁调用，就是谁的class MainActivity.class
Java_com_cq_as_1jni_1project_MainActivity_getStringPwd
        (JNIEnv *env, jobject jobj) {
}

// 静态函数
extern "C"
JNIEXPORT jstring JNICALL
Java_com_cq_as_1jni_1project_MainActivity_getStringPwd2(JNIEnv *env, jclass clazz) {
    // TODO: implement getStringPwd2()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cq_as_1jni_1project_MainActivity_changeName(JNIEnv *env, jobject thiz) {
    // 获取class
    jclass j_cls = env->GetObjectClass(thiz);

    // 获取属性
    // jfieldID GetFieldID(MainActivity.class, 属性名, 属性的签名)
    jfieldID j_fid = env->GetFieldID(j_cls, "name", "Ljava/lang/String;");

    // jfieldID 转换 jstring
    jstring j_str = static_cast<jstring>(env->GetObjectField(thiz, j_fid));

    // jstring 转换 char *
    char *c_str = const_cast<char *>(env->GetStringUTFChars(j_str, NULL));
    LOGE("native : %s\n", c_str);

    // 修改字段属性
    jstring jName = env->NewStringUTF("cq");
    env->SetObjectField(thiz, j_fid, jName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cq_as_1jni_1project_MainActivity_changeAge(JNIEnv *env, jclass jcls) {
    //获取静态的字段
    jfieldID j_fid = env->GetStaticFieldID(jcls, "age", "I");
    //jfieldID 转成 jint
    jint age = env->GetStaticIntField(jcls, j_fid);
    age += 10;
    //修改jfieldID
    env->SetStaticIntField(jcls, j_fid, age);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cq_as_1jni_1project_MainActivity_callAddMethod(JNIEnv *env, jobject job) {
    //获取class
    jclass mainActivityClass = env->GetObjectClass(job);

    // 获取方法id   GetMethodID(MainActivity.class, 方法名, 方法的签名)
    jmethodID j_mid = env->GetMethodID(mainActivityClass, "add", "(II)I");

    // 调用 Java的方法
    jint sum = env->CallIntMethod(job, j_mid, 3, 3);
    LOGE("sum result:%d", sum);
}
```



### 7.动态注册

```cpp
#include <jni.h>
#include <string>
#include <android/log.h>

#define TAG "xxchenqi"
// __VA_ARGS__ 代表 ...的可变参数
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);

//如果用不到  JNIEnv jobject ，可以不用写
void dynamicJavaMethod01() {
    LOGD("我是动态注册的函数 dynamicMethod01...");
}

const char *mainActivityClassName = "com/cq/ndkapp/MainActivity";
/*
     typedef struct {
        const char* name;       // 函数名
        const char* signature; // 函数的签名
        void*       fnPtr;     // 函数指针
     } JNINativeMethod;
*/
static const JNINativeMethod methods[] = {
        {"dynamicJavaMethod01", "()V", (void *) dynamicJavaMethod01}
};

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *javaVm, void *) {
    JNIEnv *jniEnv = nullptr;
    //获得jniEnv jint GetEnv(void** env, jint version)
    int result = javaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (result != JNI_OK) {
        return -1;
    }

    LOGE("JNI Load init");

    jclass mainActivityClass = jniEnv->FindClass(mainActivityClassName);
    //注册 jint RegisterNatives(Class, 我们的数组==methods，注册的数量)
    jniEnv->RegisterNatives(mainActivityClass, methods, sizeof(methods) / sizeof(JNINativeMethod));
    return JNI_VERSION_1_6;
}
```





### 8.extern

用extern来声明字段或者函数，在其他文件中去实现，这样可以让当前文件不会那么的臃肿。

但是不能有多个相同的方法

```
extern void show(); // 声明show函数 
extern int age; // 声明age
```



### 9.native 和 java 的数据转换

```C
//jint本质是int，所以可以用int接收
int countInt = count; 

//jstring 转 const char *
const char *textInfo = env->GetStringUTFChars(text_info, NULL);

//释放jstring
env->ReleaseStringUTFChars(text_info, textInfo);

//jintArray 转 int * 
int *jintArray = env->GetIntArrayElements(ints, NULL);

//获取数组长度
jsize size = env->GetArrayLength(ints);

//打印
for (int i = 0; i < size; ++i) {
    *(jintArray + i) += 100; // C++的修改，影响不了Java层
    LOGI("int[]:%d\n", *jintArray + i);
}

/**
 * 刷新Java数组，释放jintArray
 * 0:           刷新Java数组，并 释放C++层数组
 * JNI_COMMIT:  只提交 只刷新Java数组，不释放C++层数组
 * JNI_ABORT:   只释放C++层数组
 */
env->ReleaseIntArrayElements(ints, jintArray, 0);

//jobjectArray的某一元素 转成 jstring
jstring jstr = static_cast<jstring>(env->GetObjectArrayElement(strs, i));


//string(类) 转 jclass
jclass studentClass = env->FindClass("com/cq/as_jni_project/Student"); // 第一种
//jobject 转 jclass
jclass studentClass = env->GetObjectClass(student); // 第二种

//获取jmethodID
jmethodID setName = env->GetMethodID(studentClass, "setName", "(Ljava/lang/String;)V");
jmethodID getName = env->GetMethodID(studentClass, "getName", "()Ljava/lang/String;");
//获取静态jmethodID
jmethodID showInfo = env->GetStaticMethodID(studentClass, "showInfo", "(Ljava/lang/String;)V");

//调用Method
jstring value = env->NewStringUTF("AAAA");
env->CallVoidMethod(student, setName, value);

jstring getNameResult = static_cast<jstring>(env->CallObjectMethod(student, getName));
const char *getNameValue = env->GetStringUTFChars(getNameResult, NULL);

//调用静态Method
jstring jstringValue = env->NewStringUTF("静态方法");
env->CallStaticVoidMethod(studentClass, showInfo, jstringValue);

//实例化对象,不会调用构造函数 
jobject studentObj = env->AllocObject(studentClass); // AllocObject 只实例化对象，不会调用对象的构造函数

//释放引用
env->DeleteLocalRef(studentClass);

//局部引用： jobject jclass jstring ...  【函数结束后，会自动释放】

//获取构造函数jmethodID
jmethodID init = env->GetMethodID(dogClass, "<init>", "()V");
//实例化对象,会调用构造函数 
jobject dog = env->NewObject(dogClass, init);

//全局引用
studentClass = static_cast<jclass>(env->NewGlobalRef(temp));

//释放全局引用
env->DeleteGlobalRef(studentClass);
studentClass = null;//防止悬空指针
```



### 10.gradle配置

```groovy
android {

    defaultConfig {

        externalNativeBuild {
            cmake {
                // cppFlags ""  // 默认五大平台
                
                // 指定CPU架构，Cmake的本地库， 例如：native-lib ---> armeabi-v7a
                abiFilters "armeabi-v7a"
            }
        }
        ndk {
            // 指定CPU架构，打入APK lib/CPU平台
            abiFilters "armeabi-v7a"
        }
    }

    
    externalNativeBuild {
        // cmake 文件路径
        cmake {
            path "src/main/cpp/CMakeLists.txt"
            version "3.10.2"
        }
    }
    
}
```



### 11.线程

注意：

JNIEnv和jobject 不能跨线程，

jobject : 不能跨越线程，不能跨越函数，需要提升全局引用

JNIEnv : 不能跨越线程，可以跨越函数，需要附加当前异步线程后，得到一个子线程专用env

JavaVM ：全局的，能够跨越线程，能够跨越函数





例：

```CPP
#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>


#define TAG "xxchenqi"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);


JavaVM *jVm = nullptr;

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *javaVm, void *) {
    jVm = javaVm;
    // ...
    return JNI_VERSION_1_6;
}

class MyContext {
public:
    JNIEnv *jniEnv = nullptr;  // 不能跨线程 ，会奔溃
    jobject instance = nullptr; // 不能跨线程 ，会奔溃
    char *message = nullptr; 
};


void *taskAction(void *pVoid) {
    MyContext *myContext = static_cast<MyContext *>(pVoid);

    JNIEnv *jniEnv = nullptr; // 异步线程里面操作JNIEnv
    // 由于JNIEnv无法跨线程操作，所以需要附加当前异步线程后，会得到一个全新的 env，此env相当于是子线程专用env
    jint attachResult = jVm->AttachCurrentThread(&jniEnv, nullptr);
    if (attachResult != JNI_OK) {
        LOGE("附加失败")
        return 0;
    }
    // 1.拿到class
    // myContext->jniEnv->FindClass(mainActivityClassName); // 不能跨线程 ，会奔溃
    // jniEnv->FindClass(mainActivityClassName); // 还是会崩溃,估计jniEnv跨线程无法找到主线程的类?
    jclass mainActivityClass = jniEnv->GetObjectClass(myContext->instance);
    // 2.拿到方法
    jmethodID showToast = jniEnv->GetMethodID(mainActivityClass, "showToast",
                                              "(Ljava/lang/String;)V");

    jstring str = jniEnv->NewStringUTF(myContext->message);
    // 3.调用
    jniEnv->CallVoidMethod(myContext->instance, showToast, str);

    jVm->DetachCurrentThread(); // 必须解除附加，否则报错
    LOGE("C++ 异步线程OK")
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cq_ndkapp_MainActivity_nativeThread(JNIEnv *env, jobject thiz, jstring message) {
    pthread_t pid;
    const char *char_message = env->GetStringUTFChars(message, nullptr);

    MyContext *myContext = new MyContext;
    myContext->jniEnv = env;
//    myContext->instance = thiz; // 默认是局部引用，会奔溃
    myContext->instance = env->NewGlobalRef(thiz); // 提升全局引用
    myContext->message = const_cast<char *>(char_message);

//    pthread_create(&pid, nullptr, taskAction, (void *)char_message);
    pthread_create(&pid, nullptr, taskAction, myContext);
    pthread_join(pid, nullptr);
}
```





### 12.静态缓存写法

```CPP
static jfieldID f_name1_id = nullptr;


// 先缓存
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity2_initStaticCache(JNIEnv *env, jclass clazz) {
    f_name1_id = env->GetStaticFieldID(clazz, "name1", "Ljava/lang/String;");
}

// 使用
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity2_staticCache(JNIEnv *env, jclass clazz, jstring name) {
    env->SetStaticObjectField(clazz, f_name1_id, name);
}

// 清除
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity2_clearStaticCache(JNIEnv *env, jclass clazz) {
    f_name1_id = nullptr;
}

```



### 13.异常

```cpp
//方式1 补救措施
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity3_exception(JNIEnv *env, jclass clazz) {
    // 假设没有xxx就会在native层奔溃掉
    jfieldID f_id = env->GetStaticFieldID(clazz, "xxx", "Ljava/lang/String;");
	
	 // 监测本次执行，到底有没有异常
    jthrowable thr =  env->ExceptionOccurred();

    if(thr) { // 非0 进去，监测到有异常
        LOGD("C++层有异常 监测到了");

        env->ExceptionClear(); // 此异常被清除，就不会崩溃了

        // 开始 补救措施
        jfieldID f_id = env->GetStaticFieldID(clazz, "qqq", "Ljava/lang/String;");
    }
}



// 方式2 往Java层抛,让java层处理
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity3_exception2(JNIEnv *env, jclass clazz) {
    // 假设没有xxx就会在native层奔溃掉
    jfieldID f_id = env->GetStaticFieldID(clazz, "XXX", "Ljava/lang/String;");
    
	// 监测本次执行，到底有没有异常   JNI函数里面代码有问题
    jthrowable jthrowable = env->ExceptionOccurred(); 

    if(jthrowable) { // 非0 进去，监测到有异常
        LOGD("C++层有异常 监测到了");

        env->ExceptionClear(); // 此异常被清除

        // Throw抛一个 Java的对象
        jclass clz = env->FindClass("java/lang/NoSuchFieldException");
        env->ThrowNew(clz, "NoSuchFieldException 无法找到XXX方法");
    }
}


// 异常3 【native层被动干的异常  被动 我是Java方法坑了】
extern "C"
JNIEXPORT void JNICALL
Java_com_derry_as_1jni_1project_MainActivity3_exception3(JNIEnv *env, jclass clazz) {
    //java中的show方法会主动抛出异常
    jmethodID showID = env->GetStaticMethodID(clazz, "show", "()V");
    env->CallStaticVoidMethod(clazz, showID);

    // 不会立即奔溃
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe(); // 输出描述 信息
        env->ExceptionClear(); // 此异常被清除    业务逻辑控制
    }

    
    /*
        // 奔溃后，下面的语句，照样打印
        LOGI("C++层>>>>>>>>>>>>>>>>>>>>>>>>>>>>1");

        // 如果创建局部引用放在异常清除前，那么可能还是会崩溃，这里不是很确定，因为我的手机上没有崩溃
        env->NewStringUTF("AAAA"); // 局部引用 奔溃给磨平
    */
}
```

