# 虚拟机相关

## Dalvik or ART

Android 4.4 以前用的是 Dalvik 虚拟机，Android 4.4 开始引入 ART 虚拟机。

Android 4.4 版本上两种运行时环境共存，可以相互切换。

Android 5.0 之后，Dalvik 虚拟机被彻底丢弃，全部采用 ART。

## JIT

### **概念**

JIT 编译器：Just-In-Time Compiler

JIT（Just-in-time Compilation，即时编译），又称为动态编译，是一种通过在运行时将字节码翻译为机器码的技术，使得程序的执行速度更快

JIT 是在运行时进行编译，是动态编译，并且每次运行程序的时候都需要对 odex 重新进行编译

### **解决了什么问题?**

Dalvik 虚拟机负责解释dex文件为机器码，如果我们不做处理的话，每次执行代码，都需要Dalvik将dex代码翻译为微处理器指令，然后交给系统处理，这样效率不高。

为了解决这个问题，Google在2.2版本添加了JIT编译器，当App运行时，每当遇到一个新类，JIT编译器就会对这个类进行编译，经过编译后的代码，会被优化成相当精简的原生型指令码（即native code），这样在下次执行到相同逻辑的时候，速度就会更快。

### **编译方式**

主流的JIT包含两种字节码编译方式：

- method方式：以函数或方法为单位进行编译。
- trace方式：以trace为单位进行编译。(默认)



### **什么是trace?**

在函数中一般很少是顺序执行代码的，多数的代码都分成了好几条执行路径，其中函数的有些路径在实际运行过程中是很少被执行的，这部分路径被称为“冷路径”，而执行比较频繁的路径被称为“热路径”。采用传统的 method 方式会编译整个方法的代码，这会使得在“冷路径”上浪费很多编译时间，并且耗费更多的内存；

trace方式编译则能够快速地获取“热路径”代码，使用更短的时间与更少的内存来编译代码。



### **优点**	

**安装速度超快 存储空间小**

每次应用在运行时，它实时的将一部分 dex翻译成机器码。在程序的执行过程中，更多的代码被被编译并缓存。由于 JIT 只翻译一部分代码，它消耗的更少的内存，占用的更少的物理存储空间



### **缺点**

**运行时比较耗电，造成电池额外的开销**

JIT中需要解释器，解释器解释的字节码会带来CPU和时间的消耗 由于热点代码的Monitor一直在运行，也会带来电量的损耗

**Android SDK < 21， 安装或者升级更新之后，首次冷启动的耗时漫长**

Multidex加载的时候会非常慢，因为Dalvik 虚拟机只能执行做过 OPT 优化的 DEX 文件，也就是我们常说的 ODEX 文件

由于在Dex加载时会触发dexopt , 导致Multidex加载的时候会非常慢



## AOT

### 概念

AOT（Ahead-of-time）预先编译

AOT 是静态编译，应用在安装的时候会启动 dex2oat 过程把 dex 预编译成 ELF 文件，每次运行程序的时候不用重新编译，是真正意义上的本地应用

Android 5.0引入的。

### 解决了什么问题?

AOT 模式解决了应用启动和运行速度和耗电问题的同时也带来了另外两个问题：

- 应用安装和系统升级之后的应用优化比较耗时
- 优化后的文件会占用额外的存储空间

### **优点**

运行时省电 , 运行速度快

### **缺点**

安装APK时触发dex2oat , 需要编译成native code , 导致安装时间过长
dex2oat生成的文件较大 , 会占用较多的空间



## AOT/JIT 混合编译



### 解决了什么问题?

在 Android 5.x 和 6.x 的机器上，系统每次 OTA 升级完成重启的时候都会有个应用优化的过程，这个过程就是 dex2oat 过程，这个过程比较耗时并且会占用额外的存储空间。



Android 7.0形成 AOT/JIT 混合编译模式，这种混合编译模式的特点是：

- 应用在安装的时候 dex 不会被编译
- 应用在运行时 dex 文件先通过解析器（Interpreter）后会被直接执行（这一步骤跟 Android 2.2 - Android 4.4之前的行为一致），与此同时，热点函数（Hot Code）会被识别并被 JIT 编译后存储在 jit code cache 中并生成 profile 文件以记录热点函数的信息。
- 手机进入 IDLE（空闲） 或者 Charging（充电） 状态的时候，系统会扫描 App 目录下的 profile 文件并执行 AOT 过程进行编译。



### 优点

应用在安装速度加快的同时，运行速度、存储空间和耗电量等指标都得到了优化。	





## dexopt 与 dex2oat 的区别

**dexopt** 

针对 Dalvik 虚拟机。是对 dex 文件 进行 verification 和 optimization 的操作，其对 dex 文件的优化结果变成了 odex 文件，这个文件和 dex 文件很像，只是使用了一些优化操作码（譬如优化调用虚拟指令等）。



**dex2oat** 

针对 Art 虚拟机。是对 dex 文件的 AOT 提前编译操作，其需要一个 dex 文件，然后对其进行编译，结果是一个本地可执行的 ELF 文件，可以直接被本地处理器执行。



**Art 虚拟机的 dex2oat 是提前编译所有 dex 字节码，而 Dalvik 虚拟机只编译使用启发式检测中最频繁执行的热点字节码**





## 各产物区别

### dex

dex（Dalvik VM Excutors）：Dalvik 虚拟机执行程序，执行前需要优化

### vdex

Android 8.0(Android O) 在 odex 的基础上又引入了 vdex 机制，目的是为了避免不必要的验证 dex 文件合法性以降低 dex2oat时间

### odex

在Android N 之前，Dalvik虚拟机执行程序 dex文件前，系统会对dex文件做优化，生成可执行文件 odex，保存到 data/dalvik-cache 目录，最后把apk文件中的dex文件删除。

### .art

odex 进行优化生成的可执行二进制码文件，主要是apk 启动的常用函数相关地址的记录，方便寻址相关； 通常会在 data/dalvik-cache/ 保存常用的jar包的相关地址记录。

### oat

ART虚拟机使用的是 oat文件，oat 文件是一种Android私有 ELF文件格式，它不仅包含有从DEX文件翻译而来的本地机器指令，还包含有原来的DEX文件内容。

APK在安装的过程中，会通过 dex2oat 工具生成一个OAT文件。对于apk来说，oat文件实际上就是对odex文件的包装，即 oat=odex











## odex、oat验证

**如何使用dex2oat**

工具路径： /system/bin/dex2oat

命令：

```shell
dex2oat \
  --dex-file=test.apk \
  --oat-file=test.oat \
  --instruction-set=arm \
  --compiler-filter=speed
```

如果是64位就改成arm64



**验证odex、oat是否包含多个dex**

odex生成目录(6.0):/data/app/[pkg]/oat/arm/base.odex

**方式一:**

用010打开，搜索classes1.dex、classes2.dex....

**方式二：**

dextra工具解析

http://newandroidbook.com/tools/dextra.html

https://cloud.tencent.com/developer/article/1193488

```shell
# 查看头信息
./dextra.armv7 -h test.oat

# 从oat中抽取dex
./dextra.armv7 -dextract test.oat
```

**方式三：**

代码解析

https://blog.csdn.net/seaaseesa/article/details/54837370

```java
package dex;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

public class DEODEX {
    /**
     * 从内存中搜索数据，并返回在内存中的偏移
     ***/
    public static int findPos(MappedByteBuffer data, int offset, byte[] found) {
        for (int i = offset; i < data.capacity(); i++) {
            boolean bFound = true;
            for (int j = 0; j < found.length; j++) {
                bFound = bFound && data.get(i + j) == found[j];
            }
            if (bFound) {
                return i;
            }
        }
        return -1;
    }

    private static void write_to_file(MappedByteBuffer buffer, int offset, int dexsize, int file_count, String out)
            throws IOException {
        RandomAccessFile file = new RandomAccessFile(String.format("%s%02d.dex", out, file_count), "rw");
        FileChannel channel = file.getChannel();
        byte[] data = new byte[dexsize];
        buffer.position(offset);
        buffer.get(data);
        ByteBuffer bw = ByteBuffer.wrap(data);
        channel.write(bw);
        channel.close();
        file.close();
    }

    public static void oat2dex(String in, String out) throws IOException {
        RandomAccessFile raf = new RandomAccessFile(in, "rw");
        // 内存映射文件
        FileChannel channel = raf.getChannel();
        MappedByteBuffer buffer = channel.map(FileChannel.MapMode.READ_WRITE, 0, raf.length());
        int file_count = 0;
        int length = (int) raf.length();
        //dex头
        byte[] magic = {0x64, 0x65, 0x78, 0x0A, 0x30, 0x33, 0x35, 0x00};

        int offset = 0;
        while (offset != -1) {
            offset = findPos(buffer, offset + 8, magic);

            if (offset == -1) { // 未发现dex magic
                break;
            }

            int dexsize = (buffer.get(offset + 35) << 24) | (buffer.get(offset + 34) & 0xff) << 16
                    | (buffer.get(offset + 33) & 0xff) << 8 | (buffer.get(offset + 32) & 0xff);

            if (offset + dexsize > length)
                continue;
            write_to_file(buffer, offset, dexsize, ++file_count, out);
        }
        channel.close();
        raf.close();
    }

    public static void main(String[] args) {
        try {
            DEODEX.oat2dex("D:\\oat_test\\test.odex", "D:\\oat_test\\cq\\cc");
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
```



**其他问题：**

具体代码：

http://androidxref.com/5.0.0_r2/xref/frameworks/native/cmds/installd/commands.c#863



android5.0安装apk时，apk是多dex，预编译时是编译的所有dex吗？

是的。如果不是的话那不就又回到了jit吗？



baksmali-2.5.2验证下来无法将odex抽取多个dex，只有一个dex



## 参考文献

https://blog.51cto.com/u_11864323/5784496

https://huanle19891345.github.io/en/



