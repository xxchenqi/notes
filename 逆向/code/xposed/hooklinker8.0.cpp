#include <jni.h>
#include <string>
#include "sandhook_native.h"
#include "utils/log.h"
#include <android/log.h>
#include <unistd.h>
#include <cassert>
#include <cstdlib>

#include "includes/elf.h"

class instance;

void *(*old_strstr)(char *, char *) = nullptr;

void *new_strstr(char *arg0, char *arg1) {
    __android_log_print(4, "hooksoarm64", "strstr is called,arg1:%s,arg2:%s", arg0, arg1);
    if (strcmp(arg1, "hookso") == 0) {
        int a = 1;
        return &a;
    } else {
        void *result = old_strstr(arg0, arg1);
        return result;
    };
}

void starthooklibc() {
    if (sizeof(void *) == 8) {
        const char *libcpath = "/system/lib64/libc.so";
        old_strstr = reinterpret_cast<void *(*)(char *, char *)>(SandInlineHookSym(libcpath,
                                                                                   "strstr",
                                                                                   reinterpret_cast<void *>(new_strstr)));
    } else {
        const char *libcpath = "/system/lib/libc.so";
        old_strstr = reinterpret_cast<void *(*)(char *, char *)>(SandInlineHookSym(libcpath,
                                                                                   "strstr",
                                                                                   reinterpret_cast<void *>(new_strstr)));
    }

}

bool testhook(const char *content) {

    if (strstr(content, "hookso") != nullptr) {
        __android_log_print(4, "xposedhookso", "find hookso");
        return true;
    } else {
        __android_log_print(4, "xposedhookso", "not find hookso");
    }
    return false;

}

int (*testhookfunctin)(const char *) = nullptr;

void activecalltesthook() {
    typedef int (*testhook)(const char *a1);

    testhook testhookfunction = nullptr;
    //得到函数地址
/*    extern "C"
    EXPORT void* SandGetModuleBase(const char* so);*/

    void *libnativebase = SandGetModuleBase("libnative-lib.so");
    //thumb
    if (sizeof(void *) == 4) {
        unsigned long tmpaddr = (unsigned long) libnativebase + 0x8B4C + 1;
        void *testhookaddr = reinterpret_cast<void *>(tmpaddr);
        testhookfunction = reinterpret_cast<testhook>(testhookaddr);
    } else {
        unsigned long tmpaddr = (unsigned long) libnativebase + 0xf67c;
        void *testhookaddr = reinterpret_cast<void *>(tmpaddr);
        testhookfunction = reinterpret_cast<testhook>(testhookaddr);
        LOGD("libnative-lib.so base:%p,testfuncaddr:%p", libnativebase, (void *) tmpaddr);
    }

    int result1 = testhookfunction("aaaaahookso");

    LOGD("testhookfunction(\"aaaaahookso\"); return:%d", result1);

    int result2 = testhookfunction("aaaabbbbb");
    LOGD("testhookfunction(\"aaaabbbbb\"); return:%d", result2);


}

void *(*old_init_arrayfunction)(void) = nullptr;

void *new_init_arrayfunction(void) {
    LOGD("go into new_init_arrayfunction,addr:%p", old_init_arrayfunction);
    return old_init_arrayfunction();
}

void *(*old_call_function)(void *, void *, void *) = nullptr;
void *new_call_function(void *arg0, void *arg1, void *arg2) {
    //init_array void initarray_4(void)
    __android_log_print(4, "hooklinker", "call_function is called,type:%s,addr:%p,path:%s", arg0,
                        arg1, arg2);
    const char* libname="libnative-lib.so";
    //"libnative-lib.so"
    if (arg2 != nullptr)
        if (strstr((char *) arg2, libname) != nullptr) {
            if (arg1 != nullptr && strstr((char *) arg0, "function") != nullptr) {
                LOGD("start inline hook DT_INIT:%p", arg1);
                old_init_arrayfunction = reinterpret_cast<void *(*)(void)>(SandInlineHook(arg1,
                                                                                          reinterpret_cast<void *>(new_init_arrayfunction)));
                LOGD("end inline hook DT_INIT:%p", arg1);
            }
        }

    void *result = old_call_function(arg0, arg1, arg2);
    return result;
}
//hook android 8 linker
void hooklinker() {
    LOGD("go into hooklinker");
    //void *libnativebase = SandGetModuleBase("libnative-lib.so");

    //8.0
    void *call_function_addr = SandGetSym("/system/bin/linker",
                                          "__dl__ZL13call_functionPKcPFviPPcS2_ES0_");
    LOGD("call_function_addr:%p", call_function_addr);
    old_call_function = reinterpret_cast<void *(*)(void *, void *, void *)>(SandInlineHook(
            call_function_addr,
            reinterpret_cast<void *>(new_call_function)));


    //__dl__ZL13call_functionPKcPFviPPcS2_ES0_
}

extern "C" void _init(void) {
    LOGD("go into _init");
    //starthooklibc();
}
extern "C" jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("go into JNI_OnLoad");
    //starthooklibc();
    //activecalltesthook();
    hooklinker();
    return JNI_VERSION_1_6;
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_kanxue_xposeehooksobysandhook_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
