# Binder原理

## binder优势:

1. 开辟内存
2. 风险隔离-每一个进程，单独的一个app



## binder与传统IPC对比

性能方面，binder 小于 共享内存  优于其他IPC 进程间通信



binder需要拷贝1次，共享内存无需拷贝，socket拷贝2次

线程共享区域比较难操作，binder属于C/S架构，比较好操作

socket和内存共享身份识别为pid，依赖上层协议，不安全

binder身份识是系统分配的UID更安全 



系统服务  -- 实名 --- getsystemservice（context.activity）

匿名：自定义service



## 进程之间如何调用

内存被系统划分成2块，分别为用户空间和内核空间。

每个app的内核空间的区域是不同的，但是内核空间是共享的(映射区域是一样的)。

用户空间和内核空间指的是虚拟内存，虚拟内存实际上是要映射到物理内存。

每个app的内核空间映射到的都是同一块物理空间。

所以内核空间是共享的，用户控件是独立的。	



## 2次拷贝和1次拷贝：

2次拷贝：

1.发送数据

2.通过系统调用copy_from_user将数据从用户空间拷贝到内核空间（第一次拷贝）

3.通过系统调用copy_to_user将数据从内核空间拷贝到用户空间（第二次拷贝）

4.接收数据



1次拷贝：

1.发送数据

2.通过系统调用copy_from_user将数据从用户空间拷贝到内核空间

3.通过mmap将内核空间和接收方的用户空间映射在同一块物理内存

4.接收数据



共享内存如何实现无需拷贝：

数据发送方，数据接收方和内核地址全都共享一块物理内存



## MMAP:

Linux通过将一个虚拟内存区域与一个磁盘上的对象关联起来，以初始化这个虚拟内存区域的内容，这个过程称为内存映射(memory mapping)。



mmap---能够让虚拟内存和指定的物理内存直接联系起来











## binder的jni方法注册

### 1.zygote启动

#### 1-1.启动zygote进程

zygote是由init进程通过解析 init.zygote.rc 文件而创建的，zygote所对应的可执行程序 

app_process，所对应的源文件是 app_main.cpp ，进程名为zygote。



#### 1-2.执行app_main.cpp 中的main方法

启动zygote的入口函数是 app_main.cpp 中的main方法。



#### 1-3.AndroidRuntime::start

调用startReg方法来完成jni方法的注册





## binder驱动

通过init，创建/dev/binder设备节点

通过open()，获取Binder Driver的文件描述符

通过mmap()，在内核分配一块内存，用于存放数据

通过ioctl()，将IPC数据作为参数传递给Binder Driver



binder_init

1. 分配内存
2. 初始化设备
3. 放入链表  binder_devices

binder_open

1. 创建binder_proc对象
2. 当前进程信息，proc
3. filp->private_data = proc;
4. 添加到binder_procs链表中

binder_mmap

struct vm_struct *area; --- 内核的虚拟内存

vma --- 进程的虚拟内存  --- 4M 驱动定的，1M-8k --- intent  异步  同步



虚拟内存  放入一个东西  

用户空间  = 虚拟内存地址 + 偏移值



分配4kb的物理内存 --- 内核的虚拟空间

默认 同步

tf_oneway



binder_mmap 

1. 通过用户空间的虚拟内存大小 --- 分配一块内核的虚拟内存
2. 分配了一块物理内存  --- 4KB
3. 把这块物理内
4. 存分别映射到    用户空间的虚拟内存和内核的虚拟内存

1M



binder_ioctl

读写操作 --- BINDER_WRITE_READ  --- ioctl(BINDER_WRITE_READ)





## 启动service_manager

1. 打开驱动，内存映射（设置大小 128K）
2. 设置 SM 为大管家 --- sm  作用  为了管理系统服务
   1. 创建 binder_node 结构体对象
   2. proc --》 binder_node  将新创建的 node对象添加到 proc红黑树
   3. 创建  work  和 todo --》类似 messageQueue
3. BC_ENTER_LOOPER 命令
   1. 写入状态Loop
   2. 去读数据：binder_thread_read：ret = wait_event_freezable_exclusive(proc->wait, binder_has_proc_work(proc, thread)); 进入等待

sm获取 --- native这块 --- 





## 获取ServiceManager

获取sm的情况：native 的服务注册和获取的时候都会走这个

1. 注册服务到sm
2. 通过sm去获取服务 ---java

也是服务端

1. ProcessState::self()->getContextObject(NULL)、
   1. ProcessState::self()
      1. 打开驱动：binder
      2. 设置线程最大数目：15个
      3. mmap  -- 设置共享内存大小 --- （1M-8K） 普通服务的大小
   2. getContextObject
      1. 创建一个BpBinder --- 客户端的对象
2. interface_cast
   1. new BpServiceManager(new BpBinder) ==》 new Proxy(binder==BinderProxy)
   2. remote.transact -->远程调用  
   3. remote == BpBinder
3. java 层 --- ServiceManager.addService
   1. new ServiceManagerProxy(new BinderProxy)
   2. mRemote == BinderProxy
   3. BinderProxy.mObject == BpBinder
   4. mRemote.transact == BpBinder.transact





## 服务的注册和获取，线程池管理

从java --- AMS 

如何注册到SM中

getIServiceManager().addService(name, service, false);

- getIServiceManager --- new ServiceManagerProxy(new BinderProxy())
  - ServiceManagerNative.asInterface(BinderInternal.getContextObject())
    - BinderInternal.getContextObject  --- 返回 BinderProxy 对象
      - ProcessState::self()->getContextObject：创建一个BpBinder
      - javaObjectForIBinder -- BinderProxy 和 BpBinder 互相绑定
    - ServiceManagerNative.asInterface
      - 返回 ServiceManagerProxy
- addService
  - data.writeStrongBinder(service); -- service == AMS --- 将AMS 放入 data中
  - mRemote.transact --- mRemote == BinderProxy
    - 获取BpBinder  --- IPCThreadState::transact
      - 1.writeTransactionData --- out 写入命令 --write  --- cmd == BC_TRANSACTION
      - 2.waitForResponse
        - talkWithDriver -- 非常重要 --- 代码非常长
          - binder_transaction
            - handle == 0 --》 sm
            - 1. target_node
              2. proc
              3. todo,wait
              4. 创建t，tcomplete，
              5. 数据拷贝
              6. binder_transaction_binder --> handle
              7. thread->transaction_stack = t; ---> 方便sm找到client
              8. t->work.type = BINDER_WORK_TRANSACTION; -- 给sm -- 做事
              9. tcomplete->type = BINDER_WORK_TRANSACTION_COMPLETE; -- 给client--挂起
              10. wake_up_interruptible 唤醒sm
      - client挂起
        - BR_NOOP ，BR_TRANSACTION_COMPLETE
        - wait_event_freezable --- 挂起
      - sm处理添加服务
        - BINDER_WORK_TRANSACTION --- 要处理 cmd == BR_TRANSACTION
        - 1. reply初始化
          2. res = func(bs, txn, &msg, &reply); --- 函数指针 --- svcmgr_handler作用：获取或者添加 service
             1. sm是用 svclist  保存所有服务的
          3. binder_send_reply --- bc_reply
          4. t->work.type = BINDER_WORK_TRANSACTION; --- 给Client
             list_add_tail(&t->work.entry, target_list);
                	tcomplete->type = BINDER_WORK_TRANSACTION_COMPLETE; -- 给SM --- 被挂起
                	list_add_tail(&tcomplete->entry, &thread->todo);
          5. wake_up_interruptible(target_wait); -- 唤醒 Client
      - client 被唤醒
        - BINDER_WORK_TRANSACTION --- cmd = BR_REPLY;

SM 处理 onTransact

- IPCThreadState::executeCommand

  - error = reinterpret_cast<BBinder*>(tr.cookie)->transact(tr.code, buffer,
        &reply, tr.flags);
  - JavaBBinder.onTransact --- C++
  - jboolean res = env->CallBooleanMethod(mObject, gBinderOffsets.mExecTransact,
        code, reinterpret_cast<jlong>(&data), reinterpret_cast<jlong>(reply), flags); -- Binder.java.execTransact 方法

    

binder_node  --- binder对象

binder_ref -- binder引用



线程池管理

- 主线程 -- 不会退出，非主线程 
- 线程最大数  --- 15个 --- 非主线程
- 主线程有一个  -- 不算这在线程最大数
- 线程真正最大数 ： 15 + 1 + 其他线程



























================================================

面试题

1.Binder有什么优势--字节跳动

2.Binder是如何做到一次拷贝的--腾讯

3.MMAP的原理讲解--腾讯

4.讲解 AIDL 生成的代码  --- 字节跳动的面试题 





Intent传输为什么不能超过1M：

intent通过binder传输的，binder设置的时候是1M-8K

同步是返回所有，异步是返回一半



binder_mmap为什么分配4K物理内存：

没通信时，只分配4K，等到要用的时候，要用多少分配多少。防止内存浪费。