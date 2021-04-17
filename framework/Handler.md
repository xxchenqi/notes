# Handler

## Handler 机制的相关类

Hanlder：发送和接收消息 

Looper：用于轮询消息队列，一个线程只能有一个Looper 

Message： 消息实体 

MessageQueue： 消息队列用于存储消息和管理消息 



## 创建Looper

**创建Looper的方法是调用Looper.prepare() 方法**

在ActivityThread中的main方法中为我们prepare了 

```java
public static void main(String[] args) { 	
    Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, "ActivityThreadMain");
    //其他代码省略...
    Looper.prepareMainLooper(); //初始化Looper以及MessageQueue

    ActivityThread thread = new ActivityThread(); thread.attach(false);

    if (sMainThreadHandler == null) {
        sMainThreadHandler = thread.getHandler();
    }

    if (false) {
   	 	Looper.myLooper().setMessageLogging(new LogPrinter(Log.DEBUG, "ActivityThread"));
    }

    // End of event ActivityThreadMain. 
    Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    Looper.loop(); //开始轮循操作

    throw new RuntimeException("Main thread loop unexpectedly exited");
}
```

Looper.prepareMainLooper();

```java
public static void prepareMainLooper() {
    prepare(false);//消息队列不可以quit
    synchronized (Looper.class) {
        if (sMainLooper != null) {
            throw new IllegalStateException("The main Looper has already been prepared.");
        }
        sMainLooper = myLooper();
    }
}
```

prepare有两个重载的方法，主要看 prepare(boolean quitAllowed) quitAllowed的作用是在创建MessageQueue时 

标识消息队列是否可以销毁， **主线程不可被销毁** 下面有介绍

```java
public static void prepare() {
    prepare(true);//消息队列可以quit
}

//quitAllowed 主要
private static void prepare(boolean quitAllowed) {
    if (sThreadLocal.get() != null) {//不为空表示当前线程已经创建了Looper
        throw new RuntimeException("Only one Looper may be created per thread");
        //每个线程只能创建一个Looper
    }
    sThreadLocal.set(new Looper(quitAllowed));//创建Looper并设置给sThreadLocal，这样get的 时候就不会为null了
}	
```

### 创建MessageQueue以及Looper与当前线程的绑定

```java
private Looper(boolean quitAllowed) {
    mQueue = new MessageQueue(quitAllowed);//创建了MessageQueue 
    mThread = Thread.currentThread(); //当前线程的绑定
}			
```



MessageQueue的构造方法

```java
MessageQueue(boolean quitAllowed) {
    //mQuitAllowed决定队列是否可以销毁 主线程的队列不可以被销毁需要传入false, 在MessageQueue的quit()方法 就不贴源码了
    mQuitAllowed = quitAllowed;
    mPtr = nativeInit();
}
```

Looper.loop()



























