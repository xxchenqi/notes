# JVM

## Java运行过程

xxx.java 通过javac编译成 xxx.class

jvm通过类加载器加载xxx.class->解释执行或者jit执行->执行引擎->硬件

解释执行：通过JVM翻译，速度会慢一点

JIT执行(hotspot):

当方法、代码 循环次数达到一定。

java代码 翻译成 汇编码



## 栈帧执行步骤

java字节码指令集

https://cloud.tencent.com/developer/article/1333540



**启动线程会创建**：虚拟机栈 、本地方法栈、程序计数器

**程序计数器**：指向当前线程正在执行的字节码指令的地址

**虚拟机栈**：存储当前线程运行方法所需的数据，指令、返回地址

**每个栈帧中包含**：局部变量表、操作数栈、动态链接、完成出口

​	完成出口:记录方法出口的偏移量(类似于方法进入的行号)

示例代码:

```java
public class Person {
    public  int work()throws Exception{//一个方法对应一个栈帧
        int x =1;// iconst_1 、 istore_1
        int y =2;// iconst_2 、 istore_2
        int z =(x+y)*10;
        return  z;
    }
    public static void main(String[] args) throws Exception{
        Person person = new Person();
        person.work();  //这个  3  字节码的行号（针对 本方法偏移）
        person.hashCode();//方法属于本地方法 ---本地方法栈  4
    }
}
```

通过反汇编工具:

```
javap -p xxx.class
```

```
//程序计数器 count = 0,将1压入操作数栈
0:iconst_1
//程序计数器 count = 1,将操作数栈顶的位置压入局部变量表下标是1的位置(PS：局部变量表位置0是this)
1:istore_1
//程序计数器 count = 2,将2压入操作数栈
2:iconst_2
//程序计数器 count = 3,将操作数栈顶的位置压入局部变量表下标是2的位置
3:istore_2
//程序计数器 count = 4,把局部变量表下标是1的位置压入到操作数栈顶
4:iload_1
//程序计数器 count = 5,把局部变量表下标是2的位置压入到操作数栈顶
5:iload_2
//程序计数器 count = 6,将两个操作数栈上的值,先进行出栈，然后进行加法运算，运算后的结果压入操作数栈顶
6：iadd
//程序计数器 count = 7,将10压入操作数栈
7:bipush	10
//程序计数器 count = 9,将两个操作数栈上的值,先进行出栈，然后进行乘法运算，运算后的结果压入操作数栈顶
9:imul
//程序计数器 count = 10,将操作数栈顶的位置压入局部变量表下标是3的位置
10:istore_3
//程序计数器 count = 11,把局部变量表下标是3的位置压入到操作数栈顶
11:iload_3
//程序计数器 count = 12,将操作数栈的值返回到main栈帧
12:ireturn
```



## 运行时数据区其他区域

- 本地方法栈
- 方法区
- 直接内存
- 堆



示例:

```java
public class ObjectAndClass {
    static int age=18;//todo 静态变量（基本数据类型）  方法区
    final static int sex=1;//todo 常量（基本数据类型） 方法区
    
    private boolean isKing;//todo 成员变量 

    public   static void main(String[] args) {
         int x=18;//todo 局部变量（基本数据类型） 虚拟机栈的栈帧中
         long y=1;//todo 局部变量（基本数据类型） 虚拟机栈的栈帧中
        
         // new ObjectAndClass() 在堆，lobject 在虚拟机栈的局部变量表,
         ObjectAndClass lobject = new ObjectAndClass();//todo 局部变量 引用  （对象）
         lobject.isKing=true;// isKing跟随对象，堆空间
         lobject.hashCode();//方法中调用方法  本地方法（C++语言写  JNI）
         ByteBuffer bb = ByteBuffer.allocateDirect(128*1024*1024);//todo 直接分配128M的直接内存

    }
}
```





## 线程共享和私有

线程共享区：方法区、堆

线程私有区：虚拟机栈、本地方法栈、程序计数器





## JVM中对象创建过程

- 检查加载：
  - 检查这个指令的参数是否能在常量池中定位到一个类的符号引用（引用以一组符号来描述所引用的目标），并且检查类是否已经被加载、解析和初始化过。

- 分配内存
  - 指针碰撞
    - 如果Java堆中内存是绝对规整的，所有用过的内存都放在一边，空闲的内存放在另一边，中间放着一个指针作为分界点的指示器，那所分配内存就仅仅是把那个指针向空闲空间那边挪动一段与对象大小相等的距离，这种分配方式称为“**指针碰撞**”。
  - 空闲列表
    - 如果Java堆中的内存并不是规整的，已使用的内存和空闲的内存相互交错，那就没有办法简单地进行指针碰撞了，虚拟机就必须维护一个列表，记录上哪些内存块是可用的，在分配的时候从列表中找到一块足够大的空间划分给对象实例，并更新列表上的记录，这种分配方式称为"**空闲列表**"。
  - 并发安全：在多线程分配对象会有安全问题
    - CAS机制
    - 分配缓冲
      - 即每个线程在Java堆中预先分配一小块私有内存，也就是本地线程分配缓冲（Thread Local Allocation Buffer,TLAB）
      - TLAB的目的是在为新对象分配内存空间时，让每个Java应用线程能在使用自己专属的分配指针来分配空间，减少同步开销。
      - java8默认使用TLAB

- 内存空间初始化
  - 初始化"零"值
- 设置
  - 设置对象头
- 对象初始化
  - 构造方法



## 判断对象的存活

引用计数

可达性分析