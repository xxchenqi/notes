# NDK

```cpp
/**
	JNIEXPORT: 代表当前函数符号需要导出,与之对应为hidden隐藏符号信息
	
	extern “c” : 由于C++支持重载，C++类中的函数会有name mangling过程(函数名修改)
	主要作用是告诉编译器保留函数名
	注：当你需要使用到其他c库时，也需要加上 extern “c” {}
*/
extern "C" JNIEXPORT:
jstring JNICALL
Java_com_test_ndk_MainActivity_stringFromJNI(
	JNIEnv *env,
    jobject // 普通函数是jobject，静态函数是jclass
) {
    std::string hello = "hello";
    return env->NewStringUTF(hello.c_str());
}
```



JNIEXPORT

默认为导出

```
__attribute__ ((visibility ("default")))
```

修改隐藏(这样通过IDA查看就看不到函数名称了)

```
__attribute__ ((visibility ("hidden")))
```



cmake定义

```
add_definitions(-fvisibility=hidden)
```







## java函数运行模式

1.纯解释模式下执行

2.JTI模式

3.经过dex2oat编译后再quick模式下运行

注：7.0开始结合AOT、即时(JIT)编译、配置文件引导型编译。

因此一个java函数可能运行在解释模式、JIT或者quick模式



ART 编译详解:

https://source.android.com/docs/core/runtime/configure?hl=zh-cn



oatdump工具： 用于反编译dex2oat编译后生成的oat文件

```
oatdump --oat-file=base.odex >> /sdcard/oatdump.txt
```

```
CODE:
  NO CODE! //说明还未编译，如果是7.0以下版本，此处就会有编译后的指令
```

所以，在Android 6.0，会直接运行在quick模式下，而不会在去解释执行原有的java代码



## 性能对比

```
for (int i = 0; i <= 1000000; i++) {
	result = result +i;
}
```

4.4 davilk 纯解释模式下执行同一段代码，jni时间花销比java时间花销性能提升接近5倍。

6.0 art quick模式下执行同一段代码，此时jni执行和java执行的时间花销相差就不是很大，接近1倍。



## JavaVM和JNIEnv

### JavaVM

JavaVM 是虚拟机在 JNI 层的代表，一个进程只有一个 JavaVM，所有的线程共用一个 JavaVM。



### JNIEnv

JNIEnv 表示 Java 调用 native 语言的环境，是一个封装了几乎全部 JNI 方法的指针。

JNIEnv 只在创建它的线程生效，不能跨线程传递，不同线程的 JNIEnv 彼此独立。

native 环境中创建的线程，如果需要访问 JNI，必须要调用 AttachCurrentThread 关联，并使用DetachCurrentThread 解除链接

JNIEnv是当前Java线程的执行环境，一个JVM对应一个JavaVM结构，而一个JVM中可能创建多个Java线程，每个线程对应一个JNIEnv结构，它们保存在线程本地存储中。因此，不同的线程的JNIEnv是不同，也不能相互共享使用。

子线程和主线程的JNIEnv并不是都在调用方的ClassLoader当中，因此，需要特别注意env->FindClass的用法



### JNIEnv在C和C++中区别:

C++

```
env-> funcname(arg0,arg1,...);
```

C

```
(*env)-> funcname(env,arg0, arg1,...); 
```

在C的定义中,env是一个两级指针,而在C++的定义中,env是个一级指针



两种代码风格（C/C++）：
JavaVM 和 JNIEnv 在 C 语言环境下和 C++ 环境下调用是有区别的，主要表现在：
C风格：(*env)->NewStringUTF(env, “Hellow World!”);
C++风格：env->NewStringUTF(“Hellow World!”)



### 子线程如何获取类

1.将ClassLoader缓存起来

```cpp
jobject gClassLoader = nullprt;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = getEnv();
    
	jboject localClassLoader = env->CallObjectMethod(...)
    gClassLoader = env->NewGlobalRef(localClassLoader);
    
    return JNI_VERSION_1_6;
}

jclass loadClass(JNIEnv *env, const char *name) {
    jclass result = nullptr;
    if (env) {
        result = env->FindClass(name);
        jthrowable exception = env->ExceptionOccurred();//清除异常
        if (exception) {
            env->ExceptionClear();
            jclass ClassLoaderClass = env->FindClass("java/lang/ClassLoader");
            jmethodID loadClassMethod = env->GetMethodID(ClassLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            return static_cast<jclass>(env->CallObjectMethod(gClassLoader, loadClassMethod, env->NewStringUTF(name)));
        }
    }
    return result;
}
```



2.globalVM->GetEnv

```cpp
JavaVM *globalVM = nullptr;
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    globalVM = vm;
    pthread_t thread;
    pthread_create(&thread, nullptr, threadtest, nullptr);
    pthread_join(thread, nullptr);
}

void *threadtest(void *args) {
    JNIEnv *threadenv = nullptr;
    if (globalVM->GetEnv((void **) &threadenv, JNI_VERSION_1_6) == JNI_OK) {
    }
    int result = globalVM->AttachCurrentThread(&threadenv, nullptr);

    
    //threadenv->ExceptionDescribe();
    //threadenv->ExceptionClear();

    globalVM->DetachCurrentThread();

    return JNI_VERSION_1_6;
}
```







## 内存管理

### 全局引用

new、malloc，需要手动释放

### 局部引用

NewLocalRef和各种JNI接口创建(FindClass、NewObject、GetObjectClass、NewCharArray)，函数返回后局部引用对象会被JVM自动释放,或调用DeleteLocalRef手动释放。

因此局部引用不能跨函数，不能跨线程使用。

### 弱全局引用

NewWeakGlobalRef创建，可以跨方法、跨线程使用。当内存紧张的时候会被释放，或调用DelWeakGlobalRef手动释放



### 局部应用表

java层函数调用jni代码时，会维护一个局部引用表(非无限)，一般函数调用结束后，ART会释放这个引用。



jni提供一系列函数来管理局部引用的生命周期：

EnsureLocalCapacity、NewLocalRef、PushLocalFrame、PopLocalFrame、DeleteLocalRef



EnsureLocalCapacity：确保在当前线程中创建指定数量的局部引用，如果创建成功则返回0，否则创建失败，并抛出OOM

PushLocalFrame：为当前函数需要用到的局部引用创建一个引用堆栈；

PopLocalFrame：销毁栈中所有的引用





## Dalvik动态注册原理(4.4)

[Jni.cpp](http://androidxref.com/4.4.4_r1/xref/dalvik/vm/Jni.cpp)

```cpp
static jint RegisterNatives(JNIEnv* env, jclass jclazz,
    const JNINativeMethod* methods, jint nMethods)
{
    ClassObject* clazz = (ClassObject*) dvmDecodeIndirectRef(ts.self(), jclazz);

    for (int i = 0; i < nMethods; i++) {
        if (!dvmRegisterJNIMethod(clazz, methods[i].name,
                methods[i].signature, methods[i].fnPtr))
        {
            return JNI_ERR;
        }
    }
    return JNI_OK;
}
```

```cpp
static bool dvmRegisterJNIMethod(ClassObject* clazz, const char* methodName,
    const char* signature, void* fnPtr)
{

    Method* method = dvmFindDirectMethodByDescriptor(clazz, methodName, signature);
    if (method == NULL) {
        method = dvmFindVirtualMethodByDescriptor(clazz, methodName, signature);
    }
    if (method == NULL) {
        dumpCandidateMethods(clazz, methodName, signature);
        throwNoSuchMethodError(clazz, methodName, signature, "static or non-static");
        return false;
    }

    
	// jni已经注册过了
    if (method->nativeFunc != dvmResolveNativeMethod) {
        /* this is allowed, but unusual */
        ALOGV("Note: %s.%s:%s was already registered", clazz->descriptor, methodName, signature);
    }

    method->fastJni = fastJni;
    
    dvmUseJNIBridge(method, fnPtr);

    return true;
}
```

```cpp
void dvmUseJNIBridge(Method* method, void* func) {
    method->shouldTrace = shouldTrace(method);

    // Does the method take any reference arguments?
    method->noRef = true;
    const char* cp = method->shorty;
    while (*++cp != '\0') { // Pre-increment to skip return type.
        if (*cp == 'L') {
            method->noRef = false;
            break;
        }
    }

    DalvikBridgeFunc bridge = gDvmJni.useCheckJni ? dvmCheckCallJNIMethod : dvmCallJNIMethod;
    dvmSetNativeFunc(method, bridge, (const u2*) func);
}
```

[Class.cpp](http://androidxref.com/4.4.4_r1/xref/dalvik/vm/oo/Class.cpp)

```cpp
void dvmSetNativeFunc(Method* method, DalvikBridgeFunc func, const u2* insns)
{
    ClassObject* clazz = method->clazz;

    assert(func != NULL);

    /* just open up both; easier that way */
    dvmLinearReadWrite(clazz->classLoader, clazz->virtualMethods);
    dvmLinearReadWrite(clazz->classLoader, clazz->directMethods);

    if (insns != NULL) {
        /* update both, ensuring that "insns" is observed first */
        method->insns = insns;
        android_atomic_release_store((int32_t) func,
            (volatile int32_t*)(void*) &method->nativeFunc);
    } else {
        /* only update nativeFunc */
        // 地址绑定
        method->nativeFunc = func;
    }

    dvmLinearReadOnly(clazz->classLoader, clazz->virtualMethods);
    dvmLinearReadOnly(clazz->classLoader, clazz->directMethods);
}
```



## ART动态注册原理(8.0)

[jni_internal.cc](http://androidxref.com/4.4.4_r1/xref/art/runtime/jni_internal.cc)

```cpp
  static jint RegisterNatives(JNIEnv* env, jclass java_class, const JNINativeMethod* methods, jint method_count) {
    return RegisterNativeMethods(env, java_class, methods, method_count, true);
  }

void RegisterNativeMethods(JNIEnv* env, const char* jni_class_name, const JNINativeMethod* methods, jint method_count) {
  ScopedLocalRef<jclass> c(env, env->FindClass(jni_class_name));
  if (c.get() == nullptr) {
    LOG(FATAL) << "Couldn't find class: " << jni_class_name;
  }
  JNI::RegisterNativeMethods(env, c.get(), methods, method_count, false);
}


  static jint RegisterNativeMethods(JNIEnv* env, jclass java_class, const JNINativeMethod* methods,
                                    jint method_count, bool return_errors) {
    if (UNLIKELY(method_count < 0)) {
      JavaVmExtFromEnv(env)->JniAbortF("RegisterNatives", "negative method count: %d",
                                       method_count);
      return JNI_ERR;  // Not reached except in unit tests.
    }
    CHECK_NON_NULL_ARGUMENT_FN_NAME("RegisterNatives", java_class, JNI_ERR);
    ScopedObjectAccess soa(env);
    StackHandleScope<1> hs(soa.Self());
    Handle<mirror::Class> c = hs.NewHandle(soa.Decode<mirror::Class>(java_class));
    if (UNLIKELY(method_count == 0)) {
      return JNI_OK;
    }
    CHECK_NON_NULL_ARGUMENT_FN_NAME("RegisterNatives", methods, JNI_ERR);
    for (jint i = 0; i < method_count; ++i) {
      const char* name = methods[i].name;
      const char* sig = methods[i].signature;
      const void* fnPtr = methods[i].fnPtr;
      if (UNLIKELY(name == nullptr)) {
        ReportInvalidJNINativeMethod(soa, c.Get(), "method name", i, return_errors);
        return JNI_ERR;
      } else if (UNLIKELY(sig == nullptr)) {
        ReportInvalidJNINativeMethod(soa, c.Get(), "method signature", i, return_errors);
        return JNI_ERR;
      } else if (UNLIKELY(fnPtr == nullptr)) {
        ReportInvalidJNINativeMethod(soa, c.Get(), "native function", i, return_errors);
        return JNI_ERR;
      }
      bool is_fast = false;

      if (*sig == '!') {
        is_fast = true;
        ++sig;
      }
	  
      // 获取 ArtMethod
      ArtMethod* m = nullptr;
      bool warn_on_going_to_parent = down_cast<JNIEnvExt*>(env)->vm->IsCheckJniEnabled();
      for (ObjPtr<mirror::Class> current_class = c.Get();
           current_class != nullptr;
           current_class = current_class->GetSuperClass()) {
        // Search first only comparing methods which are native.
        m = FindMethod<true>(current_class.Ptr(), name, sig);
        if (m != nullptr) {
          break;
        }

        // Search again comparing to all methods, to find non-native methods that match.
        m = FindMethod<false>(current_class.Ptr(), name, sig);
        if (m != nullptr) {
          break;
        }


      }

      const void* final_function_ptr = m->RegisterNative(fnPtr, is_fast);
      UNUSED(final_function_ptr);
    }
    return JNI_OK;
  }

```

[art_method.cc](http://androidxref.com/8.0.0_r4/xref/art/runtime/art_method.cc)

```cpp
const void* ArtMethod::RegisterNative(const void* native_method, bool is_fast) {
  // PrettyMethod 获取当前函数名
  CHECK(IsNative()) << PrettyMethod();
  CHECK(!IsFastNative()) << PrettyMethod();
  CHECK(native_method != nullptr) << PrettyMethod();
  if (is_fast) {
    AddAccessFlags(kAccFastNative);
  }
  void* new_native_method = nullptr;
  Runtime::Current()->GetRuntimeCallbacks()->RegisterNativeMethod(this, native_method, 	&new_native_method);
  SetEntryPointFromJni(new_native_method);
  return new_native_method;
}
```

```cpp
  void SetEntryPointFromJni(const void* entrypoint) {
    DCHECK(IsNative());
    // kRuntimePointerSize 表示32/64位模式
    SetEntryPointFromJniPtrSize(entrypoint, kRuntimePointerSize);
  }

  ALWAYS_INLINE void SetEntryPointFromJniPtrSize(const void* entrypoint, PointerSize pointer_size) {
    SetDataPtrSize(entrypoint, pointer_size);
  }

  ALWAYS_INLINE void SetDataPtrSize(const void* data, PointerSize pointer_size) {
    DCHECK(IsImagePointerSize(pointer_size));
    SetNativePointer(DataOffset(pointer_size), data, pointer_size);
  }

  // new_value 设置绑定的函数地址
  template<typename T>
  ALWAYS_INLINE void SetNativePointer(MemberOffset offset, T new_value, PointerSize pointer_size) {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");
    const auto addr = reinterpret_cast<uintptr_t>(this) + offset.Uint32Value();
    if (pointer_size == PointerSize::k32) {
      uintptr_t ptr = reinterpret_cast<uintptr_t>(new_value);
      *reinterpret_cast<uint32_t*>(addr) = dchecked_integral_cast<uint32_t>(ptr);
    } else {
      *reinterpret_cast<uint64_t*>(addr) = reinterpret_cast<uintptr_t>(new_value);
    }
  }
```



### 早于JNI_OnLoad的函数



### 解析so

```
ndk\16.1.4479499\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\arm-linux-androideabi\bin\readelf -d xx.so
```

