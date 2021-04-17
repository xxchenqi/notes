# Handler常问面试题

### 1.一个线程有几个 Handler？

一个线程可以new出多个handler

### 2.一个线程有几个 Looper？如何保证？

一个线程只有1个Looper,从prepare方法中看出，在初始化时，会将Looper设置到ThreadLocal里，如果多次prepare会抛出异常

```java
private static void prepare(boolean quitAllowed) {
    if (sThreadLocal.get() != null) {
        throw new RuntimeException("Only one Looper may be created per thread");
    }
    sThreadLocal.set(new Looper(quitAllowed));
}
```



### 3.Handler内存泄漏原因？ 为什么其他的内部类没有说过有这个问题？

**原因**:

非静态内部类，或者匿名内部类。使得Handler默认持有外部类的引用。
在Activity销毁时，由于Handler可能有未执行完或正在执行的Message。

Message的target持有Handler，Handler又持有activity引用，进而导致GC无法回收Activity。



**可能造成内存泄漏**:

1.匿名内部类:

```java
//匿名内部类    
Handler handler=new Handler(){
    @Override
    public void handleMessage(Message msg) {
        super.handleMessage(msg);
    }
};
```

2.非静态内部类:

```java
//非静态内部类
protected class AppHandler extends Handler {
    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
        }
    }
}
```

**解决方法**：

1.Activity销毁时，清空Handler中，未执行或正在执行的Callback以及Message。

```java
// 清空消息队列，移除对外部类的引用
@Override
protected void onDestroy() {
    super.onDestroy();
    mHandler.removeCallbacksAndMessages(null);
}


//Handler源码中removeCallbacksAndMessages()注释含义
/**
 * Remove any pending posts of callbacks and sent messages whose
 * <var>obj</var> is <var>token</var>.  If <var>token</var> is null,
 * all callbacks and messages will be removed.
 */
public final void removeCallbacksAndMessages(Object token) {
    mQueue.removeCallbacksAndMessages(this, token);
}
```

2.静态内部类+弱引用

```java
private static class AppHandler extends Handler {
    //弱引用，在垃圾回收时，被回收
    WeakReference<Activity> activity;

    AppHandler(Activity activity){
        this.activity=new WeakReference<Activity>(activity);
    }

    public void handleMessage(Message message){
        switch (message.what){
            //todo
        }
    }
}
```



**RecyclerView Adapter的 ViewHolder为什么不会泄露，因为生命周期一致**



### 4.为何主线程可以new Handler？如果想要在子线程中new Handler 要做些什么准备？

应用启动时,在ActivityThread中的main方法中为我们prepare了。所以可以直接在主线程new Handler。

```java
public static void main(String[] args) {
  	//省略部分代码
    //初始化prepare
    Looper.prepareMainLooper();
    //循环
    Looper.loop();
    throw new RuntimeException("Main thread loop unexpectedly exited");
}
```

```java
public static void prepareMainLooper() {
    prepare(false);
    synchronized (Looper.class) {
        if (sMainLooper != null) {
            throw new IllegalStateException("The main Looper has already been prepared.");
        }
        sMainLooper = myLooper();
    }
}

private static void prepare(boolean quitAllowed) {
    if (sThreadLocal.get() != null) {
        throw new RuntimeException("Only one Looper may be created per thread");
    }
    sThreadLocal.set(new Looper(quitAllowed));
}

private Looper(boolean quitAllowed) {
    mQueue = new MessageQueue(quitAllowed);
    mThread = Thread.currentThread();
}

public static Looper myLooper() {
    return sThreadLocal.get();
}
```

```java
public Handler(Callback callback, boolean async) {
    mLooper = Looper.myLooper();//获取looper
    if (mLooper == null) {
        throw new RuntimeException(
            "Can't create handler inside thread " + Thread.currentThread()
                    + " that has not called Looper.prepare()");
    }
    mQueue = mLooper.mQueue;
    mCallback = callback;
    mAsynchronous = async;
}
```

如果想在子线程中new Handler。需要手动初始化Looper.prepare和调用loop方法

```java
Thread thread = new Thread(new Runnable() {
	Looper looper; 
    @Override 
    public void run() { 
		Looper.prepare(); 
        looper =Looper.myLooper(); 
        Looper.loop(); 
    }
    public Looper getLooper() {
        return looper; 
    }
}); 
thread.start(); 
Handler handler = new Handler(thread.getLooper());//这种写法有问题，只是例子
```



### 5.子线程中维护的Looper，消息队列无消息的时候的处理方案是什么？有什么用？

让looper退出死循环，就是让meesage返回null。

如何返回null:

调用Looper.quit()方法。

```java
//Looper.loop()

if (msg == null) {
    // No message indicates that the message queue is quitting.
    return;
}
```

```java
//MessageQueue.next
if (mQuitting) {
    dispose();
    return null;
}
```

```java
//MessageQueue.quit
void quit(boolean safe) {
    if (!mQuitAllowed) {
        throw new IllegalStateException("Main thread not allowed to quit.");
    }
    synchronized (this) {
        if (mQuitting) {
            return;
        }
        mQuitting = true;
        if (safe) {
            removeAllFutureMessagesLocked();
        } else {
            removeAllMessagesLocked();
        }
        // We can assume mPtr != 0 because mQuitting was previously false.
        nativeWake(mPtr);
    }
}
```





### 6.既然可以存在多个 Handler 往 MessageQueue 中添加数据（发消息时各个 Handler 可能处于不同线程），那它内部是如何确保线程安全的？取消息呢？

Handler机制里面最主要的类MessageQueue，这个类就是所有消息的存储仓库，在这个仓库中，我们如何的管理好 消息，这个就是一个关键点了。消息管理就2点：1）消息入库（enqueueMessage），2）消息出库（next），所以 这两个接口是确保线程安全的主要档口。

```java
boolean enqueueMessage(Message msg, long when) {
    if (msg.target == null) {
        throw new IllegalArgumentException("Message must have a target.");
    }
    if (msg.isInUse()) {
        throw new IllegalStateException(msg + " This message is already in use.");
    }
    //锁开始地方
    synchronized (this) {
        if (mQuitting) {
            IllegalStateException e = new IllegalStateException(
                    msg.target + " sending message to a Handler on a dead thread");
            Log.w(TAG, e.getMessage(), e);
            msg.recycle();
            return false;
        }
        msg.markInUse();
        msg.when = when;
        Message p = mMessages;
        boolean needWake;
        if (p == null || when == 0 || when < p.when) {
            // New head, wake up the event queue if blocked.
            msg.next = p;
            mMessages = msg;
            needWake = mBlocked;
        } else {
            // Inserted within the middle of the queue.  Usually we don't have to wake
            // up the event queue unless there is a barrier at the head of the queue
            // and the message is the earliest asynchronous message in the queue.
            needWake = mBlocked && p.target == null && msg.isAsynchronous();
            Message prev;
            for (;;) {
                prev = p;
                p = p.next;
                if (p == null || when < p.when) {
                    break;
                }
                if (needWake && p.isAsynchronous()) {
                    needWake = false;
                }
            }
            msg.next = p; // invariant: p == prev.next
            prev.next = msg;
        }
        // We can assume mPtr != 0 because mQuitting is false.
        if (needWake) {
            nativeWake(mPtr);
        }
    }
    //锁结束地方
    return true;
}
```

synchronized锁是一个内置锁，也就是由系统控制锁的lock unlock时机的。

```java
synchronized (this)
```

这个锁，说明的是对所有调用同一个MessageQueue对象的线程来说，他们都是互斥的，然而，在我们的Handler里 面，一个线程是对应着一个唯一的Looper对象，而Looper中又只有一个唯一的MessageQueue。所以，我们主线程就只有一个MessageQueue对象，也就是说，所有的子线程向主线程发送消息的时候， 主线程一次都只会处理一个消息，其他的都需要等待，那么这个时候消息队列就不会出现混乱。



**另外看下next函数**

```java
Message next() {
  	.......
    for (;;) {
        ......
        nativePollOnce(ptr, nextPollTimeoutMillis);
        synchronized (this) {  //锁开始
            // Try to retrieve the next message.  Return if found.
            final long now = SystemClock.uptimeMillis();
            Message prevMsg = null;
            Message msg = mMessages;
            if (msg != null && msg.target == null) {
                // Stalled by a barrier.  Find the next asynchronous message in the queue.
                do {
                    prevMsg = msg;
                    msg = msg.next;
                } while (msg != null && !msg.isAsynchronous());
            }
            if (msg != null) {
                if (now < msg.when) {
                    // Next message is not ready.  Set a timeout to wake up when it is ready.
                    nextPollTimeoutMillis = (int) Math.min(msg.when - now, Integer.MAX_VALUE);
                } else {
                    // Got a message.
                    mBlocked = false;
                    if (prevMsg != null) {
                        prevMsg.next = msg.next;
                    } else {
                        mMessages = msg.next;
                    }
                    msg.next = null;
                    if (DEBUG) Log.v(TAG, "Returning message: " + msg);
                    msg.markInUse();
                    return msg;
                }
            } else {
                // No more messages.
                nextPollTimeoutMillis = -1;
            }
            // Process the quit message now that all pending messages have been handled.
            if (mQuitting) {
                dispose();
                return null;
            }
            // If first time idle, then get the number of idlers to run.
            // Idle handles only run if the queue is empty or if the first message
            // in the queue (possibly a barrier) is due to be handled in the future.
            if (pendingIdleHandlerCount < 0
                    && (mMessages == null || now < mMessages.when)) {
                pendingIdleHandlerCount = mIdleHandlers.size();
            }
            if (pendingIdleHandlerCount <= 0) {
                // No idle handlers to run.  Loop and wait some more.
                mBlocked = true;
                continue;
            }
            if (mPendingIdleHandlers == null) {
                mPendingIdleHandlers = new IdleHandler[Math.max(pendingIdleHandlerCount, 4)];
            }
            mPendingIdleHandlers = mIdleHandlers.toArray(mPendingIdleHandlers);
        }//锁结束
      ......
    }
}
```

next函数很多同学会有疑问：我从线程里面取消息，而且每次都是队列的头部取，那么它加锁是不是没有意义呢？答 案是否定的，我们必须要在next里面加锁，因为，这样由于synchronized（this）作用范围是所有 this正在访问的代 码块都会有保护作用，也就是它可以保证 next函数和 enqueueMessage函数能够实现互斥。这样才能真正的保证多 线程访问的时候messagequeue的有序进行。 

小结： 这个地方是面试官经常问的点，而且他们会基于这个点来拓展问你多线程，所以，这个地方请大家重视。



### 7.我们使用 Message 时应该如何创建它？

Message.obtain() 内存复用机制(享元设计)

如果直接new Message会导致内存抖动



### 8.Looper死循环为什么不会导致应用卡死

ANR是message执行时间超时导致的，和Looper死循环没有关系。

你没有死循环，程序就运行不起来了。



## 扩展:

###  第一个： Handler loop 休眠为什么不会导致ANR？

 ANR是message执行时间超时导致的，和Looper休眠没有关系。



###  第二个： Messagequeue 队列处理机制，在fragment 生命周期管理中的应用，整理

MessageQueue为优先级队列，以时间优先级顺序存储的单链表结构，当有消息进来，会调用enqueueMessage方法将消息以时间顺序入队。

取消息通过next方法将消息取出，当无消息或者有延迟消息通过nativePollOnce阻塞。

当有消息时，enqueueMessage通过nativeWake(mPtr);方法唤醒

```java
// 1.如果nextPollTimeoutMillis=-1，一直阻塞不会超时。 
// 2.如果nextPollTimeoutMillis=0，不会阻塞，立即返回。 
// 3.如果nextPollTimeoutMillis>0，最长阻塞nextPollTimeoutMillis毫秒(超时)
nativePollOnce(ptr, nextPollTimeoutMillis);
```



在fragment 生命周期管理中的应用，整理:

FragmentActivity： AMS控制Activity的生命周期，FragmentActivity在一系列的dispatchXXX
调用FragmentController来控制Fragment的生命周期

FragmentController：是一个代理

FragmentHostCallback： 被FragmentController持有，有一个FragmentManagerImpl，真正的进行fragment操作

FragmentContainer： ：view容器

FragmentTransaction： 
 Fragment操作的事务，每个FragmentTransaction对象，代表着系列Fragment操作的事务。
 * 操作Fragment的添加，删除，隐藏，显示时，都要使用FragmentTransaction。
 * FragmentTransaction其实是个抽象类，真正的实现类为BackStackRecord

BackStackRecord： BackStackRecord实现了FragmentTransaction，每一个BackStackRecord对象，都代表了一次Fragment操作事务。

```java
//获取FragmentManager
FragmentManager fragmentManager = getSupportFragmentManager();
//FragmentTransaction->BackStackRecord
FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
//commit->commitInternal->FragmentManagerImpl.enqueueAction->scheduleCommit
//->mHost.getHandler().post(mExecCommit);
fragmentTransaction.add().commit();
```







### 第三个： 同步屏障研究一下原理

#### **同步屏障是什么**

**同步屏障** 就是一个空的消息(Message )，该Message 的 target 为 null，

而同步消息的target就是 Handler，这是因为 Handler 发送消息的时候都会调用如下方法：

```java
private boolean enqueueMessage(MessageQueue queue, Message msg, long uptimeMillis) {
    msg.target = this; //this 指的就是Handler对象
    if (mAsynchronous) {//同步屏障的判断标记
        msg.setAsynchronous(true);//是否开启异步消息
    }
    return queue.enqueueMessage(msg, uptimeMillis);
}

```

为什么说 **同步屏障**是一个空的消息呢？

```java
public int postSyncBarrier() {
    return postSyncBarrier(SystemClock.uptimeMillis());
}

//该方法会返回一个 token，需要根据这个 token 来删除这个 同步屏障
private int postSyncBarrier(long when) {
        synchronized (this) {
        //这个token 就是用来删除的 同步屏障 的
            final int token = mNextBarrierToken++; 
            //从全局消息池中获取一个消息，这样做的目的是避免分配更多的内存，从消息池中获取的消息 target 已经置为 null 了。
            final Message msg = Message.obtain();
            msg.markInUse();//标记该消息正在使用
            msg.when = when;
            msg.arg1 = token;

            Message prev = null;
            Message p = mMessages;
            //按照消息的时间来把这条消息插入链表中
            if (when != 0) {
                while (p != null && p.when <= when) {
                    prev = p;
                    p = p.next;
                }
            }
            if (prev != null) { // invariant: p == prev.next
                msg.next = p;
                prev.next = msg;
            } else {
                msg.next = p;
                mMessages = msg;
            }
            return token;
        }
    }

```



#### **同步消息和异步消息**:

我们平时使用Hander 来发送的消息大多数都是同步消息

handler.sendMessage(message)

```java
public Handler() {
 	// 这里的false就表示这是一个同步消息， 是用来给变量 mAsynchronous 赋值的，
    //上面的 Handler 的 enqueueMessage 中就判断了 mAsynchronous 为 true, 就 msg.setAsynchronous(true);
	this(null, false);
}

public Handler(boolean async) {
    this(null, async);
}
```



#### **同步屏障有什么用处**？

1.异步消息 必须结合 同步屏障 使用，也就是说就算有异步消息，但是没有设置 同步屏障，也是没效果的

2.如果使用了同步屏障，那么异步消息将会优先执行，如果没有删除 同步屏障，那么同步消息将不会得到执行。



假设有5个消息:

1:同步屏障

2:异步消息

3-5:同步消息

```java
 Message next() {
        final long ptr = mPtr;
        if (ptr == 0) {
            return null;
        }

        int pendingIdleHandlerCount = -1; // -1 only during first iteration
        int nextPollTimeoutMillis = 0;
        for (;;) {
            if (nextPollTimeoutMillis != 0) {
                Binder.flushPendingCommands();
            }

// nextPollTimeoutMillis = 0， 功能类似于 Object.wait(0), 但是要复杂和强大的多，nativePollOnce（ptr, -1）表示要一直等到需要 nativeWake唤醒
            nativePollOnce(ptr, nextPollTimeoutMillis);

            synchronized (this) {
                final long now = SystemClock.uptimeMillis();
                Message prevMsg = null;
                Message msg = mMessages;
                //msg.target == null，表示这是一个同步屏障，我们上图的消息1 就是一个同步屏障，
                if (msg != null && msg.target == null) {
               	//通过循环找到下一个异步消息，如果没有异步消息，那么就会一个个循环，直到msg = null;
                    do {
                        prevMsg = msg;
                        msg = msg.next;
                    } while (msg != null && !msg.isAsynchronous());
                }
                //根据时间来判断消息是否到时见了，没到则阻塞等待剩余时间
                if (msg != null) {
                    if (now < msg.when) {
                        nextPollTimeoutMillis = (int) Math.min(msg.when - now, Integer.MAX_VALUE);
                    } else {
                        mBlocked = false;
                        //有异步消息，则将异步消息的链断开掉这个异步消息
                        if (prevMsg != null) {
                            prevMsg.next = msg.next;
                        } else {//不是异步消息，则头结点执行下一个消息
                            mMessages = msg.next;
                        }
                        msg.next = null;
                        if (DEBUG) Log.v(TAG, "Returning message: " + msg);
                        msg.markInUse();
                        return msg;
                    }
                } else {
                    //没有消息了，等有消息的时候来唤醒；或者有同步屏障且没有异步消息，nextPollTimeoutMillis 置为 -1 ，表示要无限等待下去，需要删除同步屏障来唤醒。这也就是为什么 没有删除同步屏障，剩下的同步消息就无法执行的原因。
                    nextPollTimeoutMillis = -1;
                }
                .....省略部分代码
           }

```



我们分析下上面图中的消息；

消息1 是同步屏障， 所以 if (msg != null && msg.target == null) 成立，走到里面的循环，
prevMsg = msg; // prevMsg 指向第一个消息
msg = msg.next; // msg 指向下一条消息；

由于消息2 是异步消息 !msg.isAsynchronous() = false,

所以循环结束， 由于 msg 指向指向了 消息2 即异步消息，所以if (msg != null) 成立， 走到下面这里：

```java
	mBlocked = false;
	if (prevMsg != null) {  //1 
	    prevMsg.next = msg.next;
	} else {
	    mMessages = msg.next;
	}
	msg.next = null;
	if (DEBUG) Log.v(TAG, "Returning message: " + msg);
	msg.markInUse();
	return msg;
```

prevMsg 指向了消息1 ， 语句1 成立，执行 prevMsg.next = msg.next 就是让消息1 的执行消息3了，msg.next = null; 就是断开 指向异步消息的 msg；然后返回这条消息；

然后下次 再次 调用next获取消息的时候，由于以及没有异步消息了，但是同步屏障没有移除，所以 if (msg != null && msg.target == null) 成立，然后走do …while 直到 msg = null; 由于 msg = null ，所以走到 nextPollTimeoutMillis = -1; 接着for循环 调用 nativePollOnce(ptr, -1);
一直阻塞，直到移除 同步屏障 时 调用 nativeWake 来唤醒。

下面看看 移除同步屏障 是怎么实现的。



#### 同步屏障怎么删除

```java
 public void removeSyncBarrier(int token) {
        synchronized (this) {
            Message prev = null;
            Message p = mMessages;//指向消息队列的第一条消息
            //循环找到 对应的 同步屏障
            while (p != null && (p.target != null || p.arg1 != token)) {
                prev = p;
                p = p.next;
            }
            //p 为空 表示 该同步屏障已被移除或该同步屏障就不存在
            if (p == null) {
                throw new IllegalStateException("The specified message queue synchronization " + " barrier token has not been posted or has already been removed.");
            }
            final boolean needWake;
            //下面就是 结点的移除操作 和 唤醒操作啦
            if (prev != null) {
                prev.next = p.next;
                needWake = false;
            } else {
                mMessages = p.next;
                needWake = mMessages == null || mMessages.target != null;
            }
            p.recycleUnchecked();

            if (needWake && !mQuitting) {
                nativeWake(mPtr); //相当于 notify 唤醒 调用了 nativePollOnce(ptr, nextPollTimeoutMillis);
            }
        }
    }
```

#### **总结：**

1. 异步消息 要结合 同步屏障使用才有效果，不然就和同步消息是一样的
2. 使用同步屏障 后 能确保异步消息 能优先于 同步消息 执行
3. 使用同步屏障后要删除同步屏障，否则消息队列中的同步消息将得不到执行。





## HandlerThread

**HandlerThread**是Thread的子类，严格意义上来说就是一个线程，只是它在自己的线程里面帮我们创建了Looper

**HandlerThread** **存在的意义如下：**

1） 方便使用：a. 方便初始化，b，方便获取线程looper 

2）保证了线程安全



问题:

创建子线程后，start后，

获取Looper会有概率获取不到，HandlerThread就帮我们解决了这个问题

HandlerThread内部通过加锁+notifyAll方式解决此问题





## IntentServices

IntentService是一种特殊的Service，可用于执行后台耗时的任务，当任务执行后它会自动停止，它的优先级比单纯的线程要高很多，所以IntentService比较适合执行一些高优先级的后台任务，因为它优先级高不容易被系统杀死。



第一次启动时候，onCreate被调用

```java
@Override
public void onCreate() {
    super.onCreate();
    HandlerThread thread = new HandlerThread("IntentService[" + mName + "]");
    thread.start();

    mServiceLooper = thread.getLooper();
    mServiceHandler = new ServiceHandler(mServiceLooper);
}
```

onCreate方法会创建一个HandlerThread，然后使用它的Looper来构造一个Handler对象mServiceHandler，这样通过mServiceHandler发送的消息最终都会在HandlerThread中执行。

每次启动IntentService都会调用onStartCommand，onStartCommand调用了onStart。

```java
@Override
public void onStart(@Nullable Intent intent, int startId) {
    Message msg = mServiceHandler.obtainMessage();
    msg.arg1 = startId;
    msg.obj = intent;
    mServiceHandler.sendMessage(msg);
}
```

IntentService仅仅是通过mServiceHandler发送了一个消息，这个消息会在HandlerThread中被处理。



```java
 private final class ServiceHandler extends Handler {
     public ServiceHandler(Looper looper) {
         super(looper);
     }
     @Override
     public void handleMessage(Message msg) {
         onHandleIntent((Intent)msg.obj);
         stopSelf(msg.arg1);
     }
 }
```

ServiceHandler收到消息后会调用onHandleIntent抽象方法，如果目前只存在一个后台任务，执行完成会调用stopSelf(intstartId)就会直接停止服务；如果目前存在多个后台任务，那么当onHandleIntent方法执行完最后一个任务时，stopSelf(int startId)才会直接停止服务。

特点：当有多个后台任务同时存在时，这些后台任务会按照外界发起的顺序排队执行。