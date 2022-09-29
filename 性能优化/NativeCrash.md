# NativeCrash

## 1.minidump_stackwalk+addr2line

1.通过minidump_stackwalk生成堆栈跟踪log （minidump_stackwalk工具目前只有mac和linux）

```
./minidump_stackwalk dff95a53-6721-4144-e5339fa0-8c74f49c.dmp > x.txt
```

2.生成的log

```
Operating system: Android
                  0.0.0 Linux 4.14.116 #1 SMP PREEMPT Thu Jul 7 23:42:57 CST 2022 aarch64
CPU: arm64
     8 CPUs

GPU: UNKNOWN

Crash reason:  SIGSEGV /SEGV_MAPERR
Crash address: 0x0
Process uptime: not available

Thread 0 (crashed)
 0  libcrash-lib.so + 0x650
     x0 = 0x00000074f3af3ac0    x1 = 0x0000007fdbb20214
     x2 = 0x0000000000000001    x3 = 0x0000000003e103e1
     x4 = 0x12c80c4012c80c40    x5 = 0x00000074f36eda44
     x6 = 0x0000000000000001    x7 = 0x000000000000206e
     x8 = 0x0000000000000000    x9 = 0x0000000000000001
    x10 = 0x0000000000430000   x11 = 0x00000074f39fa000
    x12 = 0x0000007576c64720   x13 = 0x6afcc239af7f2e6d
    x14 = 0x0000000000000006   x15 = 0xffffffffffffffff
    x16 = 0x00000074e727efe8   x17 = 0x00000074e727d63c
    x18 = 0x000000757a5d4000   x19 = 0x00000074f3a10800
    x20 = 0x0000000000000000   x21 = 0x00000074f3a10800
    x22 = 0x0000007fdbb20480   x23 = 0x00000074e2f879c9
    x24 = 0x0000000000000004   x25 = 0x000000757961c020
    x26 = 0x00000074f3a108b0   x27 = 0x0000000000000001
    x28 = 0x0000007fdbb20210    fp = 0x0000007fdbb201e0
     lr = 0x00000074e727d674    sp = 0x0000007fdbb201c0
     pc = 0x00000074e727d650
    Found by: given as instruction pointer in context
 1  libcrash-lib.so + 0x670
     fp = 0x0000007fdbb20210    lr = 0x00000074f3545354
     sp = 0x0000007fdbb201f0    pc = 0x00000074e727d674
    Found by: previous frame's frame pointer
 2  libart.so + 0x150350
     fp = 0x190f689000000001    lr = 0xd9ee20ab6ad348e3
     sp = 0x0000007fdbb20220    pc = 0x00000074f3545354
    Found by: previous frame's frame pointer
```

3.使用 ndk 中提供的addr2line来根据地址进行符号反解

```
路径:$NDK_HOME/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-addr2line
```

```
aarch64-linux-android-addr2line.exe -f -C -e libcrash-lib.so 0x650
```

4.查看结果

```
Crash()
F:\project\project_jk\Chapter01-master\sample\.cxx\cmake\debug\arm64-v8a/../../../../src/main/cpp/crash.cpp:10
```



```
aarch64-linux-android-addr2line.exe -f -C -e libcrash-lib.so 0x670
```

```
Java_com_dodola_breakpad_MainActivity_crash
F:\project\project_jk\Chapter01-master\sample\.cxx\cmake\debug\arm64-v8a/../../../../src/main/cpp/crash.cpp:16
```





## 2.dump_syms+minidump_stackwalk

1.调用 dump_syms 工具获取符号表

```
dump_syms libcrash-lib.so > libcrash-lib.so.sym
```

2.打开生成的 libcrash-lib.so.syms，找到如下编码

```
MODULE Linux arm64 322FCC26DA8ED4D7676BD9A174C299630 libcrash-lib.so
```

3.建立如下结构的目录,**将 libcrash-lib.so.sym 文件复制到该文件夹中**

**注意，目录结构不能有错，否则会导致符号表对应失败。**

```
Symbol/libcrash-lib.so/322FCC26DA8ED4D7676BD9A174C299630/
```

4.执行 minidump_stackwalk 命令解析 Crash 日志

```
minidump_stackwalk crash.dmp ./Symbol > dump.txt
```

```
Operating system: Android
                  0.0.0 Linux 4.14.116 #1 SMP PREEMPT Thu Jul 7 23:42:57 CST 2022 aarch64
CPU: arm64
     8 CPUs

GPU: UNKNOWN

Crash reason:  SIGSEGV /SEGV_MAPERR
Crash address: 0x0
Process uptime: not available

Thread 0 (crashed)
 0  libcrash-lib.so!Crash() [crash.cpp : 10 + 0x0]
     x0 = 0x00000074f3af3ac0    x1 = 0x0000007fdbb20214
     x2 = 0x0000000000000001    x3 = 0x0000000003e103e1
     x4 = 0x12c80c4012c80c40    x5 = 0x00000074f36eda44
     x6 = 0x0000000000000001    x7 = 0x000000000000206e
     x8 = 0x0000000000000000    x9 = 0x0000000000000001
    x10 = 0x0000000000430000   x11 = 0x00000074f39fa000
    x12 = 0x0000007576c64720   x13 = 0x6afcc239af7f2e6d
    x14 = 0x0000000000000006   x15 = 0xffffffffffffffff
    x16 = 0x00000074e727efe8   x17 = 0x00000074e727d63c
    x18 = 0x000000757a5d4000   x19 = 0x00000074f3a10800
    x20 = 0x0000000000000000   x21 = 0x00000074f3a10800
    x22 = 0x0000007fdbb20480   x23 = 0x00000074e2f879c9
    x24 = 0x0000000000000004   x25 = 0x000000757961c020
    x26 = 0x00000074f3a108b0   x27 = 0x0000000000000001
    x28 = 0x0000007fdbb20210    fp = 0x0000007fdbb201e0
     lr = 0x00000074e727d674    sp = 0x0000007fdbb201c0
     pc = 0x00000074e727d650
    Found by: given as instruction pointer in context
 1  libcrash-lib.so!Java_com_dodola_breakpad_MainActivity_crash [crash.cpp : 16 + 0x0]
    x19 = 0x00000074f3a10800   x20 = 0x0000000000000000
    x21 = 0x00000074f3a10800   x22 = 0x0000007fdbb20480
    x23 = 0x00000074e2f879c9   x24 = 0x0000000000000004
    x25 = 0x000000757961c020   x26 = 0x00000074f3a108b0
    x27 = 0x0000000000000001   x28 = 0x0000007fdbb20210
     fp = 0x0000007fdbb201e0    sp = 0x0000007fdbb201d0
     pc = 0x00000074e727d674
    Found by: call frame info
 2  libart.so + 0x150350
    x19 = 0x00000074f3a10800   x20 = 0x0000000000000000
    x21 = 0x00000074f3a10800   x22 = 0x0000007fdbb20480
    x23 = 0x00000074e2f879c9   x24 = 0x0000000000000004
    x25 = 0x000000757961c020   x26 = 0x00000074f3a108b0
    x27 = 0x0000000000000001   x28 = 0x0000007fdbb20210
     fp = 0x0000007fdbb20210    sp = 0x0000007fdbb201f0
     pc = 0x00000074f3545354
    Found by: call frame info
```





## 3.ndk-stack(强烈推荐)

注:此方式不需要引入breakpad，引入的情况是捕获不到堆栈信息

```
adb logcat | $NDK/ndk-stack -sym xxx/obj/local/armeabi (so文件目录)

adb logcat | G:\sdk\ndk\16.1.4479499\ndk-stack.cmd -sym xxx\build\intermediates\cmake\debug\obj\arm64-v8a
```



查看日志

```

********** Crash dump: **********
Build fingerprint: 'HONOR/DUK-AL20/HWDUK:8.0.0/HUAWEIDUK-AL20/360(C00GT):user/release-keys'
#00 0x0000000000000650 /data/app/com.dodola.breakpad-Dbs0MzoQ2QMvJqcTpZylwQ==/lib/arm64/libcrash-lib.so (_Z5Crashv+20)
                                                                                                         Crash()
                                                                                                         E:\project\project_android\Chapter01-master\sample\.cxx\cmake\debug\arm64-v8a\../../../../src/main/cpp\crash.cpp:10:8
#01 0x0000000000000670 /data/app/com.dodola.breakpad-Dbs0MzoQ2QMvJqcTpZylwQ==/lib/arm64/libcrash-lib.so (Java_com_dodola_breakpad_MainActivity_crash+20)
                                                                                                         Java_com_dodola_breakpad_MainActivity_crash
                                                                                                         E:\project\project_android\Chapter01-master\sample\.cxx\cmake\debug\arm64-v8a\../../../../src/main/cpp\crash.cpp:16:5
#02 0x000000000000909c /data/app/com.dodola.breakpad-Dbs0MzoQ2QMvJqcTpZylwQ==/oat/arm64/base.odex (offset 0x9000)
Crash dump is completed
```





## 其他

dump_syms 和 minidump_stackwalk 需要通过breakpad编译出来
