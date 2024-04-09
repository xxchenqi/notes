# Matrix

## 初始化

```java
public class MatrixApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
		// 创建 Matrix，传入 application
		Matrix.Builder builder = new Matrix.Builder(this);
		// 设置插件监听
		builder.patchListener(new TestPluginListener(this));
		// 创建插件
		TracePlugin tracePlugin = new TracePlugin(new TraceConfig.Builder()
		        .build());
		// 添加插件
		builder.plugin(tracePlugin);
		// 初始化
		Matrix.init(builder.build());
		// 插件开始工作
		tracePlugin.start();
    }
}

```

### Builder 变量

```java
public static class Builder {
	// 持有 Application 
    private final Application application;
    // 插件工作回调
    private PluginListener pluginListener;
	// 维护插件列表，使用 HashSet 维护，保证插件不会重复添加。
    private HashSet<Plugin> plugins = new HashSet<>();

    public Builder(Application app) {
        if (app == null) {
            throw new RuntimeException("matrix init, application is null");
        }
        this.application = app;
    }
}
```

### PluginListener 监听插件的生命周期

```java
public interface PluginListener {
    void onInit(Plugin plugin);// 初始化

    void onStart(Plugin plugin);// 开始

    void onStop(Plugin plugin);// 结束

    void onDestroy(Plugin plugin);// 销毁

    void onReportIssue(Issue issue);// 提交报告
}
```

### 插件 Plugin接口

```java
public interface IPlugin {

    Application getApplication();

    void init(Application application, PluginListener pluginListener);

    void start();

    void stop();

    void destroy();

    String getTag();

    void onForeground(boolean isForeground);
}
```





## TracePlugin 的主线程监听

### TracePlugin 的初始化

```java
public class TracePlugin extends Plugin {
    private static final String TAG = "Matrix.TracePlugin";

    private final TraceConfig traceConfig;
    private EvilMethodTracer evilMethodTracer;
    private StartupTracer startupTracer;
    private FrameTracer frameTracer;
    private AnrTracer anrTracer;

    public TracePlugin(TraceConfig config) {
        this.traceConfig = config;
    }

    @Override
    public void init(Application app, PluginListener listener) {
        super.init(app, listener);
        MatrixLog.i(TAG, "trace plugin init, trace config: %s", traceConfig.toString());
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            MatrixLog.e(TAG, "[FrameBeat] API is low Build.VERSION_CODES.JELLY_BEAN(16), TracePlugin is not supported");
            unSupportPlugin();
            return;
        }
		// 初始化四个 Tracer
        //ANR 追踪器
        anrTracer = new AnrTracer(traceConfig);
        //帧率追踪器
        frameTracer = new FrameTracer(traceConfig);
        //慢函数追踪器
        evilMethodTracer = new EvilMethodTracer(traceConfig);
        //启动追踪器
        startupTracer = new StartupTracer(traceConfig);
    }
    ...
}

```



> TracePlugin # start

```java
@Override
public void start() {
    super.start();
    if (!isSupported()) {
        MatrixLog.w(TAG, "[start] Plugin is unSupported!");
        return;
    }
    MatrixLog.w(TAG, "start!");
    Runnable runnable = new Runnable() {
        @Override
        public void run() {
            // 1. UI线程监视器初始化
            if (!UIThreadMonitor.getMonitor().isInit()) {
                try {
                    UIThreadMonitor.getMonitor().init(traceConfig);
                } catch (java.lang.RuntimeException e) {
                    MatrixLog.e(TAG, "[start] RuntimeException:%s", e);
                    return;
                }
            }

            AppMethodBeat.getInstance().onStart();
            UIThreadMonitor.getMonitor().onStart();
            // 2. 追踪器开始工作
            anrTracer.onStartTrace();
            frameTracer.onStartTrace();
            evilMethodTracer.onStartTrace();
            startupTracer.onStartTrace();
        }
    };
    // 3. 线程检查
    if (Thread.currentThread() == Looper.getMainLooper().getThread()) {
        runnable.run();
    } else {
        MatrixLog.w(TAG, "start TracePlugin in Thread[%s] but not in mainThread!", Thread.currentThread().getId());
        MatrixHandlerThread.getDefaultMainHandler().post(runnable);
    }
}
```



### UIThreadMonitor

> UIThreadMonitor # init

```java
public void init(TraceConfig config) {
    if (Thread.currentThread() != Looper.getMainLooper().getThread()) {
        throw new AssertionError("must be init in main thread!");
    }
    this.config = config;
    // Choreographer 帧率监听器
    choreographer = Choreographer.getInstance();
    // 下面反射得到一堆 Choreographer 的属性
    callbackQueueLock = ReflectUtils.reflectObject(choreographer, "mLock", new Object());
    j
    callbackQueues = ReflectUtils.reflectObject(choreographer, "mCallbackQueues", null);
    if (null != callbackQueues) {
        addInputQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_INPUT], ADD_CALLBACK, long.class, Object.class, Object.class);
        addAnimationQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_ANIMATION], ADD_CALLBACK, long.class, Object.class, Object.class);
        addTraversalQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_TRAVERSAL], ADD_CALLBACK, long.class, Object.class, Object.class);
    }
    vsyncReceiver = ReflectUtils.reflectObject(choreographer, "mDisplayEventReceiver", null);
    frameIntervalNanos = ReflectUtils.reflectObject(choreographer, "mFrameIntervalNanos", Constants.DEFAULT_FRAME_DURATION);
    // Looper 监听
    LooperMonitor.register(new LooperMonitor.LooperDispatchListener() {
        @Override
        public boolean isValid() {
            return isAlive;
        }

        @Override
        public void dispatchStart() {
            super.dispatchStart();
            UIThreadMonitor.this.dispatchBegin();
        }

        @Override
        public void dispatchEnd() {
            super.dispatchEnd();
            UIThreadMonitor.this.dispatchEnd();
        }

    });
    this.isInit = true;
    MatrixLog.i(TAG, "[UIThreadMonitor] %s %s %s %s %s %s frameIntervalNanos:%s", callbackQueueLock == null, callbackQueues == null,
            addInputQueue == null, addTraversalQueue == null, addAnimationQueue == null, vsyncReceiver == null, frameIntervalNanos);
	...
}
```



### Choreographer

```
TODO
```



### LooperMonitor

```java
private static final LooperMonitor mainMonitor = new LooperMonitor();

private LooperMonitor() {
	 // 传入主线程 Looper
     this(Looper.getMainLooper());
 }
 
public LooperMonitor(Looper looper) {
    Objects.requireNonNull(looper);
    this.looper = looper;
    // 设置 Printer 对象
    resetPrinter();
    // 添加空闲 Handler
    addIdleHandler(looper);
}
```

重新设置 Looper 中的 Printer 对象

```java
private synchronized void resetPrinter() {
    Printer originPrinter = null;
    try {
        if (!isReflectLoggingError) {
        	// 1. 反射获取 Looper 的 mLogging 对象,mLogging 就是一个 Printer 类型的对象。
            originPrinter = ReflectUtils.get(looper.getClass(), "mLogging", looper);
            // 避免重复设置
            if (originPrinter == printer && null != printer) {
                return;
            }
        }
    } catch (Exception e) {
    	// 如果反射设置失败，标记一下下次不再尝试
        isReflectLoggingError = true;
        Log.e(TAG, "[resetPrinter] %s", e);
    }

    if (null != printer) {
        MatrixLog.w(TAG, "maybe thread:%s printer[%s] was replace other[%s]!",
                looper.getThread().getName(), printer, originPrinter);
    }
    // 2. 重新设置 Looper 的 Printer 对象
    looper.setMessageLogging(printer = new LooperPrinter(originPrinter));
    if (null != originPrinter) {
        MatrixLog.i(TAG, "reset printer, originPrinter[%s] in %s", originPrinter, looper.getThread().getName());
    }
}
```



```java
class LooperPrinter implements Printer {
    public Printer origin;
    boolean isHasChecked = false;
    boolean isValid = false;

    LooperPrinter(Printer printer) {
        this.origin = printer;
    }

    @Override
    public void println(String x) {
        if (null != origin) {
            //先是调用原 Printer 对象的 println() 方法打印日志，再进行获取日志前后事件的处理
            //这样做既不影响原 Printer 打印日志的逻辑，又可以拦截到 println() 方法。
            origin.println(x);
            if (origin == this) {
                throw new RuntimeException(TAG + " origin == this");
            }
        }

        if (!isHasChecked) { // 日志信息检查
            isValid = x.charAt(0) == '>' || x.charAt(0) == '<';
            isHasChecked = true;
            if (!isValid) {
                MatrixLog.e(TAG, "[println] Printer is inValid! x:%s", x);
            }
        }

        if (isValid) {
            //事件开始处理打印的是 ">>>>> Dispatching to..."，结束打印的是 "<<<<< Finished to..."，所以可以根据第一个符号来区分是开始还是结束。
            dispatch(x.charAt(0) == '>', x);
        }
    }
}
```



```java
private void dispatch(boolean isBegin, String log) {

    for (LooperDispatchListener listener : listeners) {
        if (listener.isValid()) {
            if (isBegin) {
                if (!listener.isHasDispatchStart) {
                    listener.onDispatchStart(log);
                }
            } else {
                if (listener.isHasDispatchStart) {
                    listener.onDispatchEnd(log);
                }
            }
        // 特殊情况，listener 不可用但是接收到了开始状态
        // 说明 listener 在设置生效前先设置了监听
        // 此时又收到了事件结束的信息，回调结束即可
        } else if (!isBegin && listener.isHasDispatchStart) {
            listener.dispatchEnd();
        }
    }
}
```

添加空闲Handler

```java
private synchronized void addIdleHandler(Looper looper) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        looper.getQueue().addIdleHandler(this);
    } else {
        try {
            //反射添加 IdleHandler，在主线程空闲时（也就是 Looper 没有 Message 可以拿出来使用时）会调用 IdleHandler 的 queueIdle() 方法。
            MessageQueue queue = ReflectUtils.get(looper.getClass(), "mQueue", looper);
            queue.addIdleHandler(this);
        } catch (Exception e) {
            Log.e(TAG, "[removeIdleHandler] %s", e);
        }
    }
}

@Override
public boolean queueIdle() {
	// private static final long CHECK_TIME = 60 * 1000L;
    if (SystemClock.uptimeMillis() - lastCheckPrinterTime >= CHECK_TIME) {
        resetPrinter();
        lastCheckPrinterTime = SystemClock.uptimeMillis();
    }
    //queueIdle() 返回 true 表示会保留该 IdleHandler 供下次空闲时调用，也就是 queueIdle() 会在主线程空闲时不断回调。这样就实现了不占用资源，又保证了 LooperPrinter 的有效性。
    return true;
}
```



### 总结

利用 Choreographer 接收 VSync 信号监听每帧回调；

利用 Looper 中的 Printer 对象获取事件处理前后的信息;(打印的是 “>>>” 开头的，处理后是 “<<<” 结尾的。)



## TracePlugin  卡顿分析

```java
public class UIThreadMonitor implements BeatLifecycle, Runnable {
	
	private static final String ADD_CALLBACK = "addCallbackLocked";
    private Object callbackQueueLock;
    //CALLBACK_INPUT(下标 0)、CALLBACK_ANIMATION(下标 1)、CALLBACK_TRAVERSAL(下标 2)。
    private Object[] callbackQueues;
    // 三种类型添加数据的方法
    private Method addTraversalQueue;
    private Method addInputQueue;
    private Method addAnimationQueue;
    private Choreographer choreographer;
    
    public void init(TraceConfig config) {
        if (Thread.currentThread() != Looper.getMainLooper().getThread()) {
            throw new AssertionError("must be init in main thread!");
        }
 		// getInstance() 是 ThreadLocal 实现，在主线程创建的，所以获取的是主线程的 Choreographer
        choreographer = Choreographer.getInstance();
        // 获取 Choreographer 的对象锁
        callbackQueueLock = ReflectUtils.reflectObject(choreographer, "mLock", new Object());
        // 获取 Choreographer 的 mCallbackQueues 对象，也就是 CallbackQueues 数组
        callbackQueues = ReflectUtils.reflectObject(choreographer, "mCallbackQueues", null);
        if (null != callbackQueues) {
        	// 获取三种类型对象的 addCallbackLocked 方法
            addInputQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_INPUT], ADD_CALLBACK, long.class, Object.class, Object.class);
            addAnimationQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_ANIMATION], ADD_CALLBACK, long.class, Object.class, Object.class);
            addTraversalQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_TRAVERSAL], ADD_CALLBACK, long.class, Object.class, Object.class);
        }
        vsyncReceiver = ReflectUtils.reflectObject(choreographer, "mDisplayEventReceiver", null);
        //获取设备的刷新率，尝试反射获取系统的值。获取不到则使用默认值
        //public static final long DEFAULT_FRAME_DURATION = 16666667L;
        frameIntervalNanos = ReflectUtils.reflectObject(choreographer, "mFrameIntervalNanos", Constants.DEFAULT_FRAME_DURATION);
    }
	
    LooperMonitor.register(new LooperMonitor.LooperDispatchListener() {
         @Override
         public boolean isValid() {
             return isAlive;
         }

         @Override
         public void dispatchStart() {
             super.dispatchStart();
             UIThreadMonitor.this.dispatchBegin();
         }

         @Override
         public void dispatchEnd() {
             super.dispatchEnd();
             UIThreadMonitor.this.dispatchEnd();
         }

     });
     this.isInit = true;
}
```



Choreographer 在接收到 VSync 信号之后会通过内部的 Handler 发送一个异步消息执行 `doFrame()` 方法。由于是主线程的 Choreographer，所以执行 `doFrame()` 方法也是在主线程环境下完成的。

而在 doFrame() 方法中会遍历 CallbackQueue 数组（Choreographer 内部维护的回调队列）并回调它们的 doFrame()/run() 方法，Matrix 当然可以通过 Choreographer 的外部方法向 CallbackQueue 添加监听，但是这样做就只能接收到回调而不能达到监控的目的。

所以 Matrix 利用反射向 CallbackQueue 数组每个下标链表头部添加回调，在每个类型回调之后更新状态并统计耗时。

最后获取它们的 `addCallbackLocked()` 方法，这个方法的作用是往当前链表添加元素。





### 数据容器

```java
public class UIThreadMonitor implements BeatLifecycle, Runnable {
	// 三种回调三个下标
	public static final int CALLBACK_INPUT = 0;
	public static final int CALLBACK_ANIMATION = 1;
	public static final int CALLBACK_TRAVERSAL = 2;
	// 回调的最大值
	private static final int CALLBACK_LAST = CALLBACK_TRAVERSAL;
	// 创建数组存放状态和花费时间
	private int[] queueStatus = new int[CALLBACK_LAST + 1];
	private long[] queueCost = new long[CALLBACK_LAST + 1];
	
	@Override
	public synchronized void onStart() {
	    if (!isInit) {
	        MatrixLog.e(TAG, "[onStart] is never init.");
	        return;
	    }
	    if (!isAlive) {
	        this.isAlive = true;
	        synchronized (this) {
	            MatrixLog.i(TAG, "[onStart] callbackExist:%s %s", Arrays.toString(callbackExist), Utils.getStack());
	            callbackExist = new boolean[CALLBACK_LAST + 1];
	        }
	        // 1.状态数组
	        queueStatus = new int[CALLBACK_LAST + 1];
	        // 2.花费时间数组
	        queueCost = new long[CALLBACK_LAST + 1];
	        addFrameCallback(CALLBACK_INPUT, this, true);
	    }
	}
}
```

关注的回调类型有三种：**CALLBACK_INPUT（输入）**、**CALLBACK_ANIMATION（动画）**、**CALLBACK_TRAVERSAL（绘制）**。



1.数组 `queueStatus`记录每种回调的状态，容量为 3（因为监听三种回调），每个下标可以赋值为固定的状态值：

```java
private static final int DO_QUEUE_BEGIN = 1;
private static final int DO_QUEUE_END = 2;
```

2.记录每种回调所花费的时间，数组 `queueCost`，容量同样为 3。
每个下标第一次记录回调开始的时间，第二次计算出花费的时间并记录。







### FrameTracer 添加监听

> Tracer # onStartTrace()

```java
@Override
final synchronized public void onStartTrace() {
    if (!isAlive) {
        this.isAlive = true;
        onAlive();
    }
}
```

> FrameTracer # onAlive()

```java
@Override
public void onAlive() {
    super.onAlive();
    UIThreadMonitor.getMonitor().addObserver(this);
}
```

> UIThreadMonitor # addObserver

```java
public void addObserver(LooperObserver observer) {
    if (!isAlive) {
        onStart();
    }
    synchronized (observers) {
        observers.add(observer);
    }
}
```



### UIThreadMonitor 监听帧率

#### 监听系统 VSync 信号

> UIThreadMonitor # onStart()

```java
@Override
public synchronized void onStart() {
    if (!isInit) {
        MatrixLog.e(TAG, "[onStart] is never init.");
        return;
    }
    if (!isAlive) {
        this.isAlive = true;
        synchronized (this) {
            MatrixLog.i(TAG, "[onStart] callbackExist:%s %s", Arrays.toString(callbackExist), Utils.getStack());
            callbackExist = new boolean[CALLBACK_LAST + 1];
        }
        queueStatus = new int[CALLBACK_LAST + 1];
        queueCost = new long[CALLBACK_LAST + 1];
        // 添加系统信号回调, 第一个参数为 CALLBACK_INPUT ,第二个参数Runnable, 第三个 true 表示添加到队首
        addFrameCallback(CALLBACK_INPUT, this, true);
    }
}
```

> UIThreadMonitor # addFrameCallback()

```java
private synchronized void addFrameCallback(int type, Runnable callback, boolean isAddHeader) {
    if (callbackExist[type]) {
        MatrixLog.w(TAG, "[addFrameCallback] this type %s callback has exist! isAddHeader:%s", type, isAddHeader);
        return;
    }

    if (!isAlive && type == CALLBACK_INPUT) {
        MatrixLog.w(TAG, "[addFrameCallback] UIThreadMonitor is not alive!");
        return;
    }
    try {
        synchronized (callbackQueueLock) {
            Method method = null;
            // 1. 根据添加类型得到要调用的对象方法
            switch (type) {
                case CALLBACK_INPUT:
                    method = addInputQueue; 
                    break;
                case CALLBACK_ANIMATION:
                    method = addAnimationQueue;
                    break;
                case CALLBACK_TRAVERSAL:
                    method = addTraversalQueue;
                    break;
            }
            if (null != method) {
            	// 2. 调用相应对象添加元素的方法
                method.invoke(callbackQueues[type], !isAddHeader ? SystemClock.uptimeMillis() : -1, callback, null);
                callbackExist[type] = true;
            }
        }
    } catch (Exception e) {
        MatrixLog.e(TAG, e.toString());
    }
}

```



#### 接收到信号之后

> UIThreadMonitor # run()

```java
@Override
public void run() {
    final long start = System.nanoTime();
    try {
    	// 1.标记回调垂直同步
        doFrameBegin(token);
        // 2.记录时间
        doQueueBegin(CALLBACK_INPUT);
		// 3.添加动画类型回调
        addFrameCallback(CALLBACK_ANIMATION, new Runnable() {

            @Override
            public void run() {
            	// 3.1CALLBACK_INPUT结束、CALLBACK_ANIMATION开始
                doQueueEnd(CALLBACK_INPUT);
                doQueueBegin(CALLBACK_ANIMATION);
            }
        }, true);
		// 4.添加绘制类型回调
        addFrameCallback(CALLBACK_TRAVERSAL, new Runnable() {

            @Override
            public void run() {
            	// 4.1CALLBACK_ANIMATION结束、CALLBACK_TRAVERSAL开始
                doQueueEnd(CALLBACK_ANIMATION);
                doQueueBegin(CALLBACK_TRAVERSAL);
            }
        }, true);

    } finally {
        if (config.isDevEnv()) {
            MatrixLog.d(TAG, "[UIThreadMonitor#run] inner cost:%sns", System.nanoTime() - start);
        }
    }
}

private void doFrameBegin(long token) {
	// 垂直同步标记
    this.isVsyncFrame = true;
}

private void doQueueBegin(int type) {
	// 记录状态
    queueStatus[type] = DO_QUEUE_BEGIN;
    // 记录时间
    queueCost[type] = System.nanoTime();
}

private void doQueueEnd(int type) {
	// 更新状态
    queueStatus[type] = DO_QUEUE_END;
    // 当前时间 - 开始时记录的时间 = 处理 type 类型花费的时间
    queueCost[type] = System.nanoTime() - queueCost[type];
    synchronized (this) {
        callbackExist[type] = false;
    }
}
```



### FrameTracer 接收数据

在 `doFrame()` 事件结束之后，给 LooperMonitor 设置的监听会回调 `dispatchEnd()` 方法执行 UIThreadMonitor 的 `dispatchEnd()` 方法：

> UIThreadMonitor # dispatchEnd()

```java
private void dispatchEnd() {
    long traceBegin = 0;
    long startNs = token;
    long intendedFrameTimeNs = startNs;
    if (isVsyncFrame) {
    	// 1.结束回调
        doFrameEnd(token);
        intendedFrameTimeNs = getIntendedFrameTimeNs(startNs);
    }
    // 2.通知监听
    long endNs = System.nanoTime();
    synchronized (observers) {
         for (LooperObserver observer : observers) {
             if (observer.isDispatchBegin()) {
                 observer.doFrame(AppMethodBeat.getVisibleScene(), startNs, endNs, isVsyncFrame, intendedFrameTimeNs, queueCost[CALLBACK_INPUT], queueCost[CALLBACK_ANIMATION], queueCost[CALLBACK_TRAVERSAL]);
             }
         }
    }
    this.isVsyncFrame = false;
}


private void doFrameEnd(long token) {
	// 标记结束，统计耗时
    doQueueEnd(CALLBACK_TRAVERSAL);
    queueStatus = new int[CALLBACK_LAST + 1];
	// 设置新一轮的监听
    addFrameCallback(CALLBACK_INPUT, this, true);
}

```



> FrameTracer # doFrame()

```java
@Override
public void doFrame(String focusedActivity, long startNs, long endNs, boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
    if (isForeground()) {
        notifyListener(focusedActivity, startNs, endNs, isVsyncFrame, intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
    }
}
```

### 总结

FrameTracer 为了接收三种事件处理的时间和帧率，向 UIThreadMonitor 添加监听；
UIThreadMonitor 内部获取主线程 Choreographer 用于接收垂直同步信号，同时反射获取向 Choreographer 数组添加回调方法的对象；
UIThreadMonitor 初始化时向 Choreographer 数组添加回调，等 Choreographer 回调数组之后记录每种回调的状态和耗时；
最后 UIThreadMonitor 将回调得来的数据返回给监听者，其中就包含 FrameTracer。





## TracePlugin 丢帧分析

### FrameTracer 收到帧率回调

> FrameTracer .notifyListener  中遍历通知所有监听。

```java
private void notifyListener(final String focusedActivity, final long startNs, final long endNs, final boolean isVsyncFrame,
                            final long intendedFrameTimeNs, final long inputCostNs, final long animationCostNs, final long traversalCostNs) {
    long traceBegin = System.currentTimeMillis();
    try {
        // 一次刷新处理的时间
        final long jitter = endNs - intendedFrameTimeNs;
        // 掉帧数： 实际上一帧的耗时 / 理论每一帧的时间
        // 如果一次刷新耗时 16ms，这台设备 16ms 刷新一次，得出刚好丢失 1 帧。但是如果耗时不足 16ms，得出 0 说明不会丢帧。
        final int dropFrame = (int) (jitter / frameIntervalNs);
        if (dropFrameListener != null) {
            //触发掉帧数阈值，则分发掉帧数回调
            if (dropFrame > dropFrameListenerThreshold) {
                try {
                    if (MatrixUtil.getTopActivityName() != null) {
                        long lastResumeTime = lastResumeTimeMap.get(MatrixUtil.getTopActivityName());
                        dropFrameListener.dropFrame(dropFrame, jitter, MatrixUtil.getTopActivityName(), lastResumeTime);
                    }
                } catch (Exception e) {
                    MatrixLog.e(TAG, "dropFrameListener error e:" + e.getMessage());
                }
            }
        }
		//累计掉帧数
        droppedSum += dropFrame;
        //累计掉帧时间
        durationSum += Math.max(jitter, frameIntervalNs);

        synchronized (listeners) {
            /**
                0 = {FrameDecorator@13864} 
                1 = {TestFpsActivity$2@13882} 
                2 = {FrameTracer$FPSCollector@13884} 
            */
            for (final IDoFrameListener listener : listeners) {
                if (config.isDevEnv()) {
                    listener.time = SystemClock.uptimeMillis();
                }
                if (null != listener.getExecutor()) {
                    if (listener.getIntervalFrameReplay() > 0) {
                        // 收集
                        listener.collect(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                                intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                    } else {
                        // 异步收集、分析
                        listener.getExecutor().execute(new Runnable() {
                            @Override
                            public void run() {
                                listener.doFrameAsync(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                                        intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                            }
                        });
                    }
                } else {
                    // 同步收集、分析
                    listener.doFrameSync(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                            intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                }

                if (config.isDevEnv()) {
                    listener.time = SystemClock.uptimeMillis() - listener.time;
                    MatrixLog.d(TAG, "[notifyListener] cost:%sms listener:%s", listener.time, listener);
                }
            }
        }
    } finally {
        long cost = System.currentTimeMillis() - traceBegin;
        if (config.isDebug() && cost > frameIntervalNs) {
            MatrixLog.w(TAG, "[notifyListener] warm! maybe do heavy work in doFrameSync! size:%s cost:%sms", listeners.size(), cost);
        }
    }
}
```



### 数据的收集和报告

> IDoFrameListener.collect 收集

```java

public void collect(String focusedActivity, long startNs, long endNs, int dropFrame, boolean isVsyncFrame,
                    long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
    //创建FrameReplay 进行赋值
    FrameReplay replay = FrameReplay.create();
    replay.focusedActivity = focusedActivity;
    replay.startNs = startNs;
    replay.endNs = endNs;
    replay.dropFrame = dropFrame;
    replay.isVsyncFrame = isVsyncFrame;
    replay.intendedFrameTimeNs = intendedFrameTimeNs;
    replay.inputCostNs = inputCostNs;
    replay.animationCostNs = animationCostNs;
    replay.traversalCostNs = traversalCostNs;
    list.add(replay);
    if (list.size() >= intervalFrame && getExecutor() != null) {
        // 拷贝list
        final List<FrameReplay> copy = new LinkedList<>(list);
        list.clear();
        getExecutor().execute(new Runnable() {
            @Override
            public void run() {
                //进入此方法
                doReplay(copy);
                for (FrameReplay record : copy) {
                    //回收复用
                    record.recycle();
                }
            }
        });
    }
}
```



> FrameTracer.FPSCollector.doReplay

```java
public void doReplay(List<FrameReplay> list) {
    super.doReplay(list);
    // 遍历list
    for (FrameReplay replay : list) {
        doReplayInner(replay.focusedActivity, replay.startNs, replay.endNs, replay.dropFrame, replay.isVsyncFrame,
                replay.intendedFrameTimeNs, replay.inputCostNs, replay.animationCostNs, replay.traversalCostNs);
    }
}

public void doReplayInner(String visibleScene, long startNs, long endNs, int droppedFrames,
                          boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs,
                          long animationCostNs, long traversalCostNs) {
    if (Utils.isEmpty(visibleScene)) return;
    // isVsyncFrame 确定是否是UI渲染，过滤掉一些不是UI渲染的消息,排除无效帧
    if (!isVsyncFrame) return;
    // 从页面维度，把每一帧和当前的页面关联起来
    FrameCollectItem item = map.get(visibleScene);
    if (null == item) {
        item = new FrameCollectItem(visibleScene);
        map.put(visibleScene, item);
    }
    //对每个页面的所属帧信息进行收集
    item.collect(droppedFrames);
    //某activity收集到所有帧的总时长超过10秒：这个指的是页面在有刷新的10秒时间，当页面静止没有刷新时，时间不记录在内。
    if (item.sumFrameCost >= timeSliceMs) {
        map.remove(visibleScene);
        //上报
        item.report();
    }
}

```



```java
private class FrameCollectItem {
    String visibleScene;
    long sumFrameCost;
    int sumFrame = 0;
    int sumDroppedFrames;
    // record the level of frames dropped each time
    int[] dropLevel = new int[DropStatus.values().length];
    int[] dropSum = new int[DropStatus.values().length];

    FrameCollectItem(String visibleScene) {
        this.visibleScene = visibleScene;
    }

    void collect(int droppedFrames) {
        float frameIntervalCost = 1f * FrameTracer.this.frameIntervalNs
                / Constants.TIME_MILLIS_TO_NANO;
        sumFrameCost += (droppedFrames + 1) * frameIntervalCost;
        sumDroppedFrames += droppedFrames;
        sumFrame++;
        if (droppedFrames >= frozenThreshold) {
            dropLevel[DropStatus.DROPPED_FROZEN.index]++;
            dropSum[DropStatus.DROPPED_FROZEN.index] += droppedFrames;
        } else if (droppedFrames >= highThreshold) {
            dropLevel[DropStatus.DROPPED_HIGH.index]++;
            dropSum[DropStatus.DROPPED_HIGH.index] += droppedFrames;
        } else if (droppedFrames >= middleThreshold) {
            dropLevel[DropStatus.DROPPED_MIDDLE.index]++;
            dropSum[DropStatus.DROPPED_MIDDLE.index] += droppedFrames;
        } else if (droppedFrames >= normalThreshold) {
            dropLevel[DropStatus.DROPPED_NORMAL.index]++;
            dropSum[DropStatus.DROPPED_NORMAL.index] += droppedFrames;
        } else {
            dropLevel[DropStatus.DROPPED_BEST.index]++;
            dropSum[DropStatus.DROPPED_BEST.index] += Math.max(droppedFrames, 0);
        }
    }

    void report() {
        //上报该页面10s内的fps
        float fps = Math.min(refreshRate, 1000.f * sumFrame / sumFrameCost);
        MatrixLog.i(TAG, "[report] FPS:%s %s", fps, toString());
        try {
            TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
            if (null == plugin) {
                return;
            }
            JSONObject dropLevelObject = new JSONObject();
            dropLevelObject.put(DropStatus.DROPPED_FROZEN.name(), dropLevel[DropStatus.DROPPED_FROZEN.index]);
            dropLevelObject.put(DropStatus.DROPPED_HIGH.name(), dropLevel[DropStatus.DROPPED_HIGH.index]);
            dropLevelObject.put(DropStatus.DROPPED_MIDDLE.name(), dropLevel[DropStatus.DROPPED_MIDDLE.index]);
            dropLevelObject.put(DropStatus.DROPPED_NORMAL.name(), dropLevel[DropStatus.DROPPED_NORMAL.index]);
            dropLevelObject.put(DropStatus.DROPPED_BEST.name(), dropLevel[DropStatus.DROPPED_BEST.index]);

            JSONObject dropSumObject = new JSONObject();
            dropSumObject.put(DropStatus.DROPPED_FROZEN.name(), dropSum[DropStatus.DROPPED_FROZEN.index]);
            dropSumObject.put(DropStatus.DROPPED_HIGH.name(), dropSum[DropStatus.DROPPED_HIGH.index]);
            dropSumObject.put(DropStatus.DROPPED_MIDDLE.name(), dropSum[DropStatus.DROPPED_MIDDLE.index]);
            dropSumObject.put(DropStatus.DROPPED_NORMAL.name(), dropSum[DropStatus.DROPPED_NORMAL.index]);
            dropSumObject.put(DropStatus.DROPPED_BEST.name(), dropSum[DropStatus.DROPPED_BEST.index]);

            JSONObject resultObject = new JSONObject();
            resultObject = DeviceUtil.getDeviceInfo(resultObject, plugin.getApplication());

            resultObject.put(SharePluginInfo.ISSUE_SCENE, visibleScene);
            resultObject.put(SharePluginInfo.ISSUE_DROP_LEVEL, dropLevelObject);
            resultObject.put(SharePluginInfo.ISSUE_DROP_SUM, dropSumObject);
            resultObject.put(SharePluginInfo.ISSUE_FPS, fps);

            Issue issue = new Issue();
            issue.setTag(SharePluginInfo.TAG_PLUGIN_FPS);
            issue.setContent(resultObject);
            //回调给插件的onDetectIssue
            plugin.onDetectIssue(issue);
        } catch (JSONException e) {
            MatrixLog.e(TAG, "json error", e);
        } finally {
            sumFrame = 0;
            sumDroppedFrames = 0;
            sumFrameCost = 0;
        }
    }
}
```



> Plugin.onDetectIssue
>

```java
@Override
public void onDetectIssue(Issue issue) {
    if (issue.getTag() == null) {
        // set default tag
        issue.setTag(getTag());
    }
    issue.setPlugin(this);
    JSONObject content = issue.getContent();
    // add tag and type for default
    try {
        if (issue.getTag() != null) {
            content.put(Issue.ISSUE_REPORT_TAG, issue.getTag());
        }
        if (issue.getType() != 0) {
            content.put(Issue.ISSUE_REPORT_TYPE, issue.getType());
        }
        content.put(Issue.ISSUE_REPORT_PROCESS, MatrixUtil.getProcessName(application));
        content.put(Issue.ISSUE_REPORT_TIME, System.currentTimeMillis());
    } catch (JSONException e) {
        MatrixLog.e(TAG, "json error", e);
    }
    //MatrixLog.e(TAG, "detect issue:%s", issue);
    //回调插件监听器进行报告问题收集(用户自定义的监听器)
    pluginListener.onReportIssue(issue);
}
```







### 掉帧等级分布

掉帧数的区间分布

| Best  | Normal | Middle | High    | Frozen   |
| ----- | ------ | ------ | ------- | -------- |
| [0,3) | [3,9)  | [9,24) | [24,42) | [42,...) |

每一帧的渲染时间基本维持

| Best   | Normal   | Middle    | High      | Frozen    |
| ------ | -------- | --------- | --------- | --------- |
| [0,48] | [48,144] | [144,388] | [388,672] | [672,...] |



dropLevel： 丢帧等级，Matrix 把丢帧分为四个等级：

- DROPPED_FROZEN： 丢帧严重；
- DROPPED_HIGH： 高度丢帧；
- DROPPED_MIDDLE： 中度丢帧；
- DROPPED_NORMAL： 普通丢帧；
- DROPPED_BEST： 低丢帧，最佳状态；

```java
public enum DropStatus {
    DROPPED_FROZEN(4), DROPPED_HIGH(3), DROPPED_MIDDLE(2), DROPPED_NORMAL(1), DROPPED_BEST(0);
}
```









## 扩展



### FrameTracer 帧率检测

- 大于等于8.0系统：采用FrameMetrics来检测。
- 小于8.0 && 大于等于4.1：利用looper.print和choreographer来检测。



整体原理:

matrix通过不断postcallback到choreographer的队列中，

按顺序分别在input、animation、traversal三个阶段的开始和结束打点，然后统计每个阶段的耗时，存起来用作后面的上报。

在Looper.print方法结束时,计算每个阶段的以及当前帧的耗时,进行页面维度的统计



> TracePlugin.init

```java
public void init(Application app, PluginListener listener) {
    super.init(app, listener);
    MatrixLog.i(TAG, "trace plugin init, trace config: %s", traceConfig.toString());
    int sdkInt = Build.VERSION.SDK_INT;
    if (sdkInt < Build.VERSION_CODES.JELLY_BEAN) {
        MatrixLog.e(TAG, "[FrameBeat] API is low Build.VERSION_CODES.JELLY_BEAN(16), TracePlugin is not supported");
        unSupportPlugin();
        return;
    } else if (sdkInt >= Build.VERSION_CODES.O) {
        //sdk大于等于8.0 设置为 true
        supportFrameMetrics = true;
    }

    looperAnrTracer = new LooperAnrTracer(traceConfig);

    frameTracer = new FrameTracer(traceConfig, supportFrameMetrics);

    evilMethodTracer = new EvilMethodTracer(traceConfig);

    startupTracer = new StartupTracer(traceConfig);
}
```



> FrameTracer构造方法

```java
public FrameTracer(TraceConfig config, boolean supportFrameMetrics) {
    //赋值给 useFrameMetrics
    useFrameMetrics = supportFrameMetrics;
    this.config = config;
    this.frameIntervalNs = UIThreadMonitor.getMonitor().getFrameIntervalNanos();
    this.timeSliceMs = config.getTimeSliceMs();
    this.isFPSEnable = config.isFPSEnable();
    this.frozenThreshold = config.getFrozenThreshold();
    this.highThreshold = config.getHighThreshold();
    this.normalThreshold = config.getNormalThreshold();
    this.middleThreshold = config.getMiddleThreshold();

    MatrixLog.i(TAG, "[init] frameIntervalMs:%s isFPSEnable:%s", frameIntervalNs, isFPSEnable);
    if (isFPSEnable) {
        addListener(new FPSCollector());
    }
}
```



> FrameTracer.onAlive

```java
public void onAlive() {
    super.onAlive();
    if (isFPSEnable) {
        //sdk >= 8.0  useFrameMetrics 为 true
        if (!useFrameMetrics) {
            UIThreadMonitor.getMonitor().addObserver(this);
        }
        //生命周期监听
        Matrix.with().getApplication().registerActivityLifecycleCallbacks(this);
    }
}
```

> onActivityResumed 生命周期回调
>

```java
public void onActivityResumed(Activity activity) {
    lastResumeTimeMap.put(activity.getClass().getName(), System.currentTimeMillis());

    if (useFrameMetrics) {
        if (frameListenerMap.containsKey(activity.hashCode())) {
            return;
        }
        this.refreshRate = (int) activity.getWindowManager().getDefaultDisplay().getRefreshRate();
        this.frameIntervalNs = Constants.TIME_SECOND_TO_NANO / (long) refreshRate;
        Window.OnFrameMetricsAvailableListener onFrameMetricsAvailableListener = new Window.OnFrameMetricsAvailableListener() {
            @RequiresApi(api = Build.VERSION_CODES.O)
            @Override
            public void onFrameMetricsAvailable(Window window, FrameMetrics frameMetrics, int dropCountSinceLastInvocation) {
                FrameMetrics frameMetricsCopy = new FrameMetrics(frameMetrics);
                //获取这一帧CPU的绘制时间 endNs
                long vsynTime = frameMetricsCopy.getMetric(FrameMetrics.VSYNC_TIMESTAMP);
                //获取这一帧vsync到来的时间 startNs
                long intendedVsyncTime = frameMetricsCopy.getMetric(FrameMetrics.INTENDED_VSYNC_TIMESTAMP);
                frameMetricsCopy.getMetric(FrameMetrics.DRAW_DURATION);
                //notifyListener
                notifyListener(ProcessUILifecycleOwner.INSTANCE.getVisibleScene(), intendedVsyncTime, vsynTime, true, intendedVsyncTime, 0, 0, 0);
            }
        };
        this.frameListenerMap.put(activity.hashCode(), onFrameMetricsAvailableListener);
        // 通过 addOnFrameMetricsAvailableListener 监听帧率的变化
        activity.getWindow().addOnFrameMetricsAvailableListener(onFrameMetricsAvailableListener, new Handler());
    }
}
```



```java
private void notifyListener(final String focusedActivity, final long startNs, final long endNs, final boolean isVsyncFrame,
                            final long intendedFrameTimeNs, final long inputCostNs, final long animationCostNs, final long traversalCostNs) {
    long traceBegin = System.currentTimeMillis();
    try {
        final long jitter = endNs - intendedFrameTimeNs;
        // 掉帧数： 实际上一帧的耗时/理论每一帧的时间
        final int dropFrame = (int) (jitter / frameIntervalNs);
        if (dropFrameListener != null) {
            //触发掉帧数阈值，则分发掉帧数回调
            if (dropFrame > dropFrameListenerThreshold) {
                try {
                    if (MatrixUtil.getTopActivityName() != null) {
                        long lastResumeTime = lastResumeTimeMap.get(MatrixUtil.getTopActivityName());
                        dropFrameListener.dropFrame(dropFrame, jitter, MatrixUtil.getTopActivityName(), lastResumeTime);
                    }
                } catch (Exception e) {
                    MatrixLog.e(TAG, "dropFrameListener error e:" + e.getMessage());
                }
            }
        }
		//累计掉帧数
        droppedSum += dropFrame;
        //累计掉帧时间
        durationSum += Math.max(jitter, frameIntervalNs);

        synchronized (listeners) {
            /**
                0 = {FrameDecorator@13864} 
                1 = {TestFpsActivity$2@13882} 
                2 = {FrameTracer$FPSCollector@13884} 
            */
            for (final IDoFrameListener listener : listeners) {
                if (config.isDevEnv()) {
                    listener.time = SystemClock.uptimeMillis();
                }
                if (null != listener.getExecutor()) {
                    if (listener.getIntervalFrameReplay() > 0) {
                        // 收集
                        listener.collect(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                                intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                    } else {
                        // 异步收集、分析
                        listener.getExecutor().execute(new Runnable() {
                            @Override
                            public void run() {
                                listener.doFrameAsync(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                                        intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                            }
                        });
                    }
                } else {
                    // 同步收集、分析
                    listener.doFrameSync(focusedActivity, startNs, endNs, dropFrame, isVsyncFrame,
                            intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                }

                if (config.isDevEnv()) {
                    listener.time = SystemClock.uptimeMillis() - listener.time;
                    MatrixLog.d(TAG, "[notifyListener] cost:%sms listener:%s", listener.time, listener);
                }
            }
        }
    } finally {
        long cost = System.currentTimeMillis() - traceBegin;
        if (config.isDebug() && cost > frameIntervalNs) {
            MatrixLog.w(TAG, "[notifyListener] warm! maybe do heavy work in doFrameSync! size:%s cost:%sms", listeners.size(), cost);
        }
    }
}
```



```java
//IDoFrameListener.collect
public void collect(String focusedActivity, long startNs, long endNs, int dropFrame, boolean isVsyncFrame,
                    long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
    FrameReplay replay = FrameReplay.create();
    replay.focusedActivity = focusedActivity;
    replay.startNs = startNs;
    replay.endNs = endNs;
    replay.dropFrame = dropFrame;
    replay.isVsyncFrame = isVsyncFrame;
    replay.intendedFrameTimeNs = intendedFrameTimeNs;
    replay.inputCostNs = inputCostNs;
    replay.animationCostNs = animationCostNs;
    replay.traversalCostNs = traversalCostNs;
    list.add(replay);
    if (list.size() >= intervalFrame && getExecutor() != null) {
        final List<FrameReplay> copy = new LinkedList<>(list);
        list.clear();
        getExecutor().execute(new Runnable() {
            @Override
            public void run() {
                //FrameTracer.FPSCollector.doReplay
                doReplay(copy);
                for (FrameReplay record : copy) {
                    //回收复用
                    record.recycle();
                }
            }
        });
    }
}
```



```java
//FPSCollector extends IDoFrameListener

//FrameTracer.FPSCollector.doReplay
public void doReplay(List<FrameReplay> list) {
    super.doReplay(list);
    for (FrameReplay replay : list) {
        doReplayInner(replay.focusedActivity, replay.startNs, replay.endNs, replay.dropFrame, replay.isVsyncFrame,
                replay.intendedFrameTimeNs, replay.inputCostNs, replay.animationCostNs, replay.traversalCostNs);
    }
}

//FrameTracer.FPSCollector.doReplayInner
public void doReplayInner(String visibleScene, long startNs, long endNs, int droppedFrames,
                          boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs,
                          long animationCostNs, long traversalCostNs) {
    if (Utils.isEmpty(visibleScene)) return;
    // isVsyncFrame 确定是否是UI渲染，过滤掉一些不是UI渲染的消息,排除无效帧
    if (!isVsyncFrame) return;
    // 从页面维度，把每一帧和当前的页面关联起来
    FrameCollectItem item = map.get(visibleScene);
    if (null == item) {
        item = new FrameCollectItem(visibleScene);
        map.put(visibleScene, item);
    }
    //对每个页面的所属帧信息进行收集
    item.collect(droppedFrames);
    //某activity收集到所有帧的总时长超过10秒：这个指的是页面在有刷新的10秒时间，当页面静止没有刷新时，时间不记录在内。
    if (item.sumFrameCost >= timeSliceMs) { // report
        map.remove(visibleScene);
        item.report();
    }
}

//FrameCollectItem.collect
//掉帧数收集
void collect(int droppedFrames) {
    float frameIntervalCost = 1f * FrameTracer.this.frameIntervalNs
            / Constants.TIME_MILLIS_TO_NANO;
    sumFrameCost += (droppedFrames + 1) * frameIntervalCost;
    sumDroppedFrames += droppedFrames;
    sumFrame++;
    if (droppedFrames >= frozenThreshold) {
        dropLevel[DropStatus.DROPPED_FROZEN.index]++;
        dropSum[DropStatus.DROPPED_FROZEN.index] += droppedFrames;
    } else if (droppedFrames >= highThreshold) {
        dropLevel[DropStatus.DROPPED_HIGH.index]++;
        dropSum[DropStatus.DROPPED_HIGH.index] += droppedFrames;
    } else if (droppedFrames >= middleThreshold) {
        dropLevel[DropStatus.DROPPED_MIDDLE.index]++;
        dropSum[DropStatus.DROPPED_MIDDLE.index] += droppedFrames;
    } else if (droppedFrames >= normalThreshold) {
        dropLevel[DropStatus.DROPPED_NORMAL.index]++;
        dropSum[DropStatus.DROPPED_NORMAL.index] += droppedFrames;
    } else {
        dropLevel[DropStatus.DROPPED_BEST.index]++;
        dropSum[DropStatus.DROPPED_BEST.index] += Math.max(droppedFrames, 0);
    }
}

void report() {
    //上报该页面10s内的fps
    float fps = Math.min(refreshRate, 1000.f * sumFrame / sumFrameCost);
    MatrixLog.i(TAG, "[report] FPS:%s %s", fps, toString());
    try {
        TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
        if (null == plugin) {
            return;
        }
        JSONObject dropLevelObject = new JSONObject();
        dropLevelObject.put(DropStatus.DROPPED_FROZEN.name(), dropLevel[DropStatus.DROPPED_FROZEN.index]);
        dropLevelObject.put(DropStatus.DROPPED_HIGH.name(), dropLevel[DropStatus.DROPPED_HIGH.index]);
        dropLevelObject.put(DropStatus.DROPPED_MIDDLE.name(), dropLevel[DropStatus.DROPPED_MIDDLE.index]);
        dropLevelObject.put(DropStatus.DROPPED_NORMAL.name(), dropLevel[DropStatus.DROPPED_NORMAL.index]);
        dropLevelObject.put(DropStatus.DROPPED_BEST.name(), dropLevel[DropStatus.DROPPED_BEST.index]);
        JSONObject dropSumObject = new JSONObject();
        dropSumObject.put(DropStatus.DROPPED_FROZEN.name(), dropSum[DropStatus.DROPPED_FROZEN.index]);
        dropSumObject.put(DropStatus.DROPPED_HIGH.name(), dropSum[DropStatus.DROPPED_HIGH.index]);
        dropSumObject.put(DropStatus.DROPPED_MIDDLE.name(), dropSum[DropStatus.DROPPED_MIDDLE.index]);
        dropSumObject.put(DropStatus.DROPPED_NORMAL.name(), dropSum[DropStatus.DROPPED_NORMAL.index]);
        dropSumObject.put(DropStatus.DROPPED_BEST.name(), dropSum[DropStatus.DROPPED_BEST.index]);
        JSONObject resultObject = new JSONObject();
        resultObject = DeviceUtil.getDeviceInfo(resultObject, plugin.getApplication());
        resultObject.put(SharePluginInfo.ISSUE_SCENE, visibleScene);
        resultObject.put(SharePluginInfo.ISSUE_DROP_LEVEL, dropLevelObject);
        resultObject.put(SharePluginInfo.ISSUE_DROP_SUM, dropSumObject);
        resultObject.put(SharePluginInfo.ISSUE_FPS, fps);
        Issue issue = new Issue();
        issue.setTag(SharePluginInfo.TAG_PLUGIN_FPS);
        issue.setContent(resultObject);
        //onDetectIssue
        plugin.onDetectIssue(issue);
    } catch (JSONException e) {
        MatrixLog.e(TAG, "json error", e);
    } finally {
        sumFrame = 0;
        sumDroppedFrames = 0;
        sumFrameCost = 0;
    }
}
```







### Choreographer

只做记录,只要发生了UI绘制，那么就肯定会走looper的消息结束接口，然后在计算每个阶段的以及当前帧的耗时。

> UIThreadMonitor.getMonitor().onStart();

```java
public synchronized void onStart() {
    if (!isInit) {
        MatrixLog.e(TAG, "[onStart] is never init.");
        return;
    }
    if (!isAlive) {
        this.isAlive = true;
        synchronized (this) {
            MatrixLog.i(TAG, "[onStart] callbackExist:%s %s", Arrays.toString(callbackExist), Utils.getStack());
            callbackExist = new boolean[CALLBACK_LAST + 1];
        }
        if (!useFrameMetrics) {
            //这个int[]数组用来保存UI渲染一帧中的三个阶段(input、animation、traversal)的开始和结束状态。
            queueStatus = new int[CALLBACK_LAST + 1];
            //这个long[]数组则是保存每一帧的三个阶段每个阶段对应的执行耗时
            queueCost = new long[CALLBACK_LAST + 1];
            //往input类型的链表中加入一个input阶段的callback。
            addFrameCallback(CALLBACK_INPUT, this, true);
        }
    }
}

private synchronized void addFrameCallback(int type, Runnable callback, boolean isAddHeader) {
    if (callbackExist[type]) {
        MatrixLog.w(TAG, "[addFrameCallback] this type %s callback has exist! isAddHeader:%s", type, isAddHeader);
        return;
    }

    if (!isAlive && type == CALLBACK_INPUT) {
        MatrixLog.w(TAG, "[addFrameCallback] UIThreadMonitor is not alive!");
        return;
    }
    try {
        //callbackQueueLock是在初始化时，通过反射拿到的choreographer中锁对象。
        synchronized (callbackQueueLock) {
            Method method = null;
            switch (type) {
                case CALLBACK_INPUT:
                    method = addInputQueue;
                    break;
                case CALLBACK_ANIMATION:
                    method = addAnimationQueue;
                    break;
                case CALLBACK_TRAVERSAL:
                    method = addTraversalQueue;
                    break;
            }
            // 最终method就是 CallbackQueue的 addCallbackLocked 方法, 反射调用callback加入type对应的链表结构中
            if (null != method) {
                method.invoke(callbackQueues[type], !isAddHeader ? SystemClock.uptimeMillis() : -1, callback, null);
                callbackExist[type] = true;
            }
        }
    } catch (Exception e) {
        MatrixLog.e(TAG, e.toString());
    }
}
```

```java
//按顺序分别在input、animation、traversal三个阶段的开始和结束打点，然后统计每个阶段的耗时，存起来用作后面的上报。
public void run() {
    final long start = System.nanoTime();
    try {
        doFrameBegin(token);
        doQueueBegin(CALLBACK_INPUT);

        addFrameCallback(CALLBACK_ANIMATION, new Runnable() {

            @Override
            public void run() {
                doQueueEnd(CALLBACK_INPUT);
                doQueueBegin(CALLBACK_ANIMATION);
            }
        }, true);

        addFrameCallback(CALLBACK_TRAVERSAL, new Runnable() {

            @Override
            public void run() {
                doQueueEnd(CALLBACK_ANIMATION);
                doQueueBegin(CALLBACK_TRAVERSAL);
            }
        }, true);

    } finally {
        if (config.isDevEnv()) {
            MatrixLog.d(TAG, "[UIThreadMonitor#run] inner cost:%sns", System.nanoTime() - start);
        }
    }
}
```



```java
private void doQueueBegin(int type) {
    queueStatus[type] = DO_QUEUE_BEGIN;
    queueCost[type] = System.nanoTime();
}

private void doQueueEnd(int type) {
    queueStatus[type] = DO_QUEUE_END;
    queueCost[type] = System.nanoTime() - queueCost[type];
    synchronized (this) {
        callbackExist[type] = false;
    }
}
```



### Looper.print

> UIThreadMonitor.getMonitor().init(traceConfig, supportFrameMetrics);

```java
public void init(TraceConfig config, boolean supportFrameMetrics) {
    this.config = config;
    useFrameMetrics = supportFrameMetrics;

    if (Thread.currentThread() != Looper.getMainLooper().getThread()) {
        throw new AssertionError("must be init in main thread!");
    }

    boolean historyMsgRecorder = config.historyMsgRecorder;
    boolean denseMsgTracer = config.denseMsgTracer;
	//注册
    LooperMonitor.register(new LooperMonitor.LooperDispatchListener(historyMsgRecorder, denseMsgTracer) {
        @Override
        public boolean isValid() {
            return isAlive;
        }

        @Override
        public void dispatchStart() {
            super.dispatchStart();
            UIThreadMonitor.this.dispatchBegin();
        }

        @Override
        public void dispatchEnd() {
            super.dispatchEnd();
            UIThreadMonitor.this.dispatchEnd();
        }

    });
    this.isInit = true;
    frameIntervalNanos = ReflectUtils.reflectObject(choreographer, "mFrameIntervalNanos", Constants.DEFAULT_FRAME_DURATION);
    if (!useFrameMetrics) {
        // Choreographer创建
        choreographer = Choreographer.getInstance();
        callbackQueueLock = ReflectUtils.reflectObject(choreographer, "mLock", new Object());
        callbackQueues = ReflectUtils.reflectObject(choreographer, "mCallbackQueues", null);
        if (null != callbackQueues) {
            addInputQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_INPUT], ADD_CALLBACK, long.class, Object.class, Object.class);
            addAnimationQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_ANIMATION], ADD_CALLBACK, long.class, Object.class, Object.class);
            addTraversalQueue = ReflectUtils.reflectMethod(callbackQueues[CALLBACK_TRAVERSAL], ADD_CALLBACK, long.class, Object.class, Object.class);
        }
        vsyncReceiver = ReflectUtils.reflectObject(choreographer, "mDisplayEventReceiver", null);

        MatrixLog.i(TAG, "[UIThreadMonitor] %s %s %s %s %s %s frameIntervalNanos:%s", callbackQueueLock == null, callbackQueues == null,
                addInputQueue == null, addTraversalQueue == null, addAnimationQueue == null, vsyncReceiver == null, frameIntervalNanos);

        if (config.isDevEnv()) {
            addObserver(new LooperObserver() {
                @Override
                public void doFrame(String focusedActivity, long startNs, long endNs, boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
                    MatrixLog.i(TAG, "focusedActivity[%s] frame cost:%sms isVsyncFrame=%s intendedFrameTimeNs=%s [%s|%s|%s]ns",
                            focusedActivity, (endNs - startNs) / Constants.TIME_MILLIS_TO_NANO, isVsyncFrame, intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
                }
            });
        }
    }
}
```

> LooperMonitor.class
>

```java
//创建LooperMonitor
private static final LooperMonitor sMainMonitor = LooperMonitor.of(Looper.getMainLooper());

private final HashSet<LooperDispatchListener> listeners = new HashSet<>();


//注册
static void register(LooperDispatchListener listener) {
    sMainMonitor.addListener(listener);
}
//添加监听器
public void addListener(LooperDispatchListener listener) {
    synchronized (listeners) {
        listeners.add(listener);
    }
}


public static LooperMonitor of(@NonNull Looper looper) {
    LooperMonitor looperMonitor = sLooperMonitorMap.get(looper);
    if (looperMonitor == null) {
        // 创建LooperMonitor
        looperMonitor = new LooperMonitor(looper);
        sLooperMonitorMap.put(looper, looperMonitor);
    }
    return looperMonitor;
}

private LooperMonitor(Looper looper) {
    Objects.requireNonNull(looper);
    this.looper = looper;
    resetPrinter();
    addIdleHandler(looper);
}

private synchronized void resetPrinter() {
    Printer originPrinter = null;
    try {
        if (!isReflectLoggingError) {
            originPrinter = ReflectUtils.get(looper.getClass(), "mLogging", looper);
            if (originPrinter == printer && null != printer) {
                return;
            }
            // Fix issues that printer loaded by different classloader
            if (originPrinter != null && printer != null) {
                if (originPrinter.getClass().getName().equals(printer.getClass().getName())) {
                    MatrixLog.w(TAG, "LooperPrinter might be loaded by different classloader"
                            + ", my = " + printer.getClass().getClassLoader()
                            + ", other = " + originPrinter.getClass().getClassLoader());
                    return;
                }
            }
        }
    } catch (Exception e) {
        isReflectLoggingError = true;
        Log.e(TAG, "[resetPrinter] %s", e);
    }

    if (null != printer) {
        MatrixLog.w(TAG, "maybe thread:%s printer[%s] was replace other[%s]!",
                looper.getThread().getName(), printer, originPrinter);
    }
    //设置打印器
    looper.setMessageLogging(printer = new LooperPrinter(originPrinter));
    if (null != originPrinter) {
        MatrixLog.i(TAG, "reset printer, originPrinter[%s] in %s", originPrinter, looper.getThread().getName());
    }
}
```



```java
class LooperPrinter implements Printer {
    public Printer origin;
    boolean isHasChecked = false;
    boolean isValid = false;

    LooperPrinter(Printer printer) {
        this.origin = printer;
    }

    @Override
    public void println(String x) {
        if (null != origin) {
            origin.println(x);
            if (origin == this) {
                throw new RuntimeException(TAG + " origin == this");
            }
        }

        if (!isHasChecked) {
            isValid = x.charAt(0) == '>' || x.charAt(0) == '<';
            isHasChecked = true;
            if (!isValid) {
                MatrixLog.e(TAG, "[println] Printer is inValid! x:%s", x);
            }
        }

        
        if (isValid) {

            dispatch(x.charAt(0) == '>', x);
        }

    }
}
```





```java
private void dispatch(boolean isBegin, String log) {
    synchronized (listeners) {
        //遍历 dispatch 分发
        for (LooperDispatchListener listener : listeners) {
            if (listener.isValid()) {
                if (isBegin) {
                    if (!listener.isHasDispatchStart) {
                        if (listener.historyMsgRecorder) {
                            messageStartTime = System.currentTimeMillis();
                            latestMsgLog = log;
                            recentMCount++;
                        }
                        listener.onDispatchStart(log);
                    }
                } else {
                    if (listener.isHasDispatchStart) {
                        if (listener.historyMsgRecorder) {
                            recordMsg(log, System.currentTimeMillis() - messageStartTime, listener.denseMsgTracer);
                        }
                        listener.onDispatchEnd(log);
                    }
                }
            } else if (!isBegin && listener.isHasDispatchStart) {
                listener.dispatchEnd();
            }
        }
    }
}
```



```java
public abstract static class LooperDispatchListener {

        boolean isHasDispatchStart = false;
        boolean historyMsgRecorder = false;
        boolean denseMsgTracer = false;

        public LooperDispatchListener(boolean historyMsgRecorder, boolean denseMsgTracer) {
            this.historyMsgRecorder = historyMsgRecorder;
            this.denseMsgTracer = denseMsgTracer;
        }

        public LooperDispatchListener() {

        }

        public boolean isValid() {
            return false;
        }


        public void dispatchStart() {
			//UIThreadMonitor里LooperMonitor.register方法重写了
        }

        @CallSuper
        public void onDispatchStart(String x) {
            this.isHasDispatchStart = true;
            dispatchStart();
        }

        @CallSuper
        public void onDispatchEnd(String x) {
            this.isHasDispatchStart = false;
            dispatchEnd();
        }


        public void dispatchEnd() {
            //UIThreadMonitor里LooperMonitor.register方法重写了
        }
    }

```



```java
LooperMonitor.register(new LooperMonitor.LooperDispatchListener(historyMsgRecorder, denseMsgTracer) {
    @Override
    public boolean isValid() {
        return isAlive;
    }
    @Override
    public void dispatchStart() {
        super.dispatchStart();
        UIThreadMonitor.this.dispatchBegin();
    }
    @Override
    public void dispatchEnd() {
        super.dispatchEnd();
        UIThreadMonitor.this.dispatchEnd();
    }
});
```



```java
//UIThreadMonitor.class
private void dispatchBegin() {
    token = dispatchTimeMs[0] = System.nanoTime();
    dispatchTimeMs[2] = SystemClock.currentThreadTimeMillis();
    if (config.isAppMethodBeatEnable()) {
        AppMethodBeat.i(AppMethodBeat.METHOD_ID_DISPATCH);
    }
    synchronized (observers) {
        //遍历observers进行分发
        for (LooperObserver observer : observers) {
            if (!observer.isDispatchBegin()) {
                observer.dispatchBegin(dispatchTimeMs[0], dispatchTimeMs[2], token);
            }
        }
    }
    if (config.isDevEnv()) {
        MatrixLog.d(TAG, "[dispatchBegin#run] inner cost:%sns", System.nanoTime() - token);
    }
}

private void dispatchEnd() {
    long traceBegin = 0;
    if (config.isDevEnv()) {
        traceBegin = System.nanoTime();
    }

    if (config.isFPSEnable() && !useFrameMetrics) {
        long startNs = token;
        long intendedFrameTimeNs = startNs;
        if (isVsyncFrame) {
            // postcallback到choreographer的队列中
            doFrameEnd(token);
            intendedFrameTimeNs = getIntendedFrameTimeNs(startNs);
        }

        long endNs = System.nanoTime();

        synchronized (observers) {
            //遍历observers进行分发
            for (LooperObserver observer : observers) {
                if (observer.isDispatchBegin()) {
                    //回调doFrame
                    observer.doFrame(AppActiveMatrixDelegate.INSTANCE.getVisibleScene(), startNs, endNs, isVsyncFrame, intendedFrameTimeNs, queueCost[CALLBACK_INPUT], queueCost[CALLBACK_ANIMATION], queueCost[CALLBACK_TRAVERSAL]);
                }
            }
        }
    }

    if (config.isEvilMethodTraceEnable() || config.isDevEnv()) {
        dispatchTimeMs[3] = SystemClock.currentThreadTimeMillis();
        dispatchTimeMs[1] = System.nanoTime();
    }

    AppMethodBeat.o(AppMethodBeat.METHOD_ID_DISPATCH);

    synchronized (observers) {
        //遍历observers进行分发
        for (LooperObserver observer : observers) {
            if (observer.isDispatchBegin()) {
                observer.dispatchEnd(dispatchTimeMs[0], dispatchTimeMs[2], dispatchTimeMs[1], dispatchTimeMs[3], token, isVsyncFrame);
            }
        }
    }

    this.isVsyncFrame = false;

    if (config.isDevEnv()) {
        MatrixLog.d(TAG, "[dispatchEnd#run] inner cost:%sns", System.nanoTime() - traceBegin);
    }
}
```



```java
private void doFrameEnd(long token) {

    doQueueEnd(CALLBACK_TRAVERSAL);

    for (int i : queueStatus) {
        if (i != DO_QUEUE_END) {
            queueCost[i] = DO_QUEUE_END_ERROR;
            if (config.isDevEnv) {
                throw new RuntimeException(String.format("UIThreadMonitor happens type[%s] != DO_QUEUE_END", i));
            }
        }
    }
    queueStatus = new int[CALLBACK_LAST + 1];

    addFrameCallback(CALLBACK_INPUT, this, true);

}
```



```java
//FrameTracer.doFrame
public void doFrame(String focusedActivity, long startNs, long endNs, boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
    if (isForeground()) {
        // 省略...
        notifyListener(focusedActivity, startNs, endNs, isVsyncFrame, intendedFrameTimeNs, inputCostNs, animationCostNs, traversalCostNs);
    }
}
```





### LooperAnrTrace

原理:

在Looper.print方法开始和结束监听，

方法开始时，分别postDelayed2秒和5秒的延时任务

方法结束时，分别移除这2个任务

如果方法执行大于2秒或者5秒，就会执行lagTask或anrTask，进行收集和上报



```java
@Override
public void onAlive() {
    super.onAlive();
    if (isAnrTraceEnable) {
        UIThreadMonitor.getMonitor().addObserver(this);
        this.anrHandler = new Handler(MatrixHandlerThread.getDefaultHandler().getLooper());
        this.lagHandler = new Handler(MatrixHandlerThread.getDefaultHandler().getLooper());
    }
}
```



```java
@Override
public void dispatchBegin(long beginNs, long cpuBeginMs, long token) {
    super.dispatchBegin(beginNs, cpuBeginMs, token);
    //记录方法开始的indexRecord
    anrTask.beginRecord = AppMethodBeat.getInstance().maskIndex("AnrTracer#dispatchBegin");
    anrTask.token = token;
    if (traceConfig.isDevEnv()) {
        MatrixLog.v(TAG, "* [dispatchBegin] token:%s index:%s", token, anrTask.beginRecord.index);
    }
    long cost = (System.nanoTime() - token) / Constants.TIME_MILLIS_TO_NANO;
    // DEFAULT_ANR = 5 * 1000;
    anrHandler.postDelayed(anrTask, Constants.DEFAULT_ANR - cost);
    // DEFAULT_NORMAL_LAG = 2 * 1000;
    lagHandler.postDelayed(lagTask, Constants.DEFAULT_NORMAL_LAG - cost);
}


@Override
public void dispatchEnd(long beginNs, long cpuBeginMs, long endNs, long cpuEndMs, long token, boolean isBelongFrame) {
    super.dispatchEnd(beginNs, cpuBeginMs, endNs, cpuEndMs, token, isBelongFrame);
    if (traceConfig.isDevEnv()) {
        long cost = (endNs - beginNs) / Constants.TIME_MILLIS_TO_NANO;
        MatrixLog.v(TAG, "[dispatchEnd] token:%s cost:%sms cpu:%sms usage:%s",
                token, cost,
                cpuEndMs - cpuBeginMs, Utils.calculateCpuUsage(cpuEndMs - cpuBeginMs, cost));
    }
    anrTask.getBeginRecord().release();
    anrHandler.removeCallbacks(anrTask);
    lagHandler.removeCallbacks(lagTask);
}
```



```java
class AnrHandleTask implements Runnable {

    AppMethodBeat.IndexRecord beginRecord;
    long token;

    public AppMethodBeat.IndexRecord getBeginRecord() {
        return beginRecord;
    }

    AnrHandleTask() {
    }

    AnrHandleTask(AppMethodBeat.IndexRecord record, long token) {
        this.beginRecord = record;
        this.token = token;
    }

    @Override
    public void run() {
        long curTime = SystemClock.uptimeMillis();
        boolean isForeground = isForeground();
        // process
        int[] processStat = Utils.getProcessPriority(Process.myPid());
        //方法的入口和出口都会将 ++index
		//拷贝数据：开始的indexRecord ~ 结束的indexRecord(sIndex - 1)
        long[] data = AppMethodBeat.getInstance().copyData(beginRecord);
        beginRecord.release();
        String scene = AppActiveMatrixDelegate.INSTANCE.getVisibleScene();

        // memory
        long[] memoryInfo = dumpMemory();

        // Thread state
        Thread.State status = Looper.getMainLooper().getThread().getState();
        StackTraceElement[] stackTrace = Looper.getMainLooper().getThread().getStackTrace();
        String dumpStack;
        if (traceConfig.getLooperPrinterStackStyle() == TraceConfig.STACK_STYLE_WHOLE) {
            dumpStack = Utils.getWholeStack(stackTrace, "|*\t\t");
        } else {
            dumpStack = Utils.getStack(stackTrace, "|*\t\t", 12);
        }


        // frame
        UIThreadMonitor monitor = UIThreadMonitor.getMonitor();
        long inputCost = monitor.getQueueCost(UIThreadMonitor.CALLBACK_INPUT, token);
        long animationCost = monitor.getQueueCost(UIThreadMonitor.CALLBACK_ANIMATION, token);
        long traversalCost = monitor.getQueueCost(UIThreadMonitor.CALLBACK_TRAVERSAL, token);

        // trace
        LinkedList<MethodItem> stack = new LinkedList();
        if (data.length > 0) {
            //将data转换为MethodItem(深度，方法id，数量，耗时)存储进list 
            TraceDataUtils.structuredDataToStack(data, stack, true, curTime);
            /*
            	裁剪:
            	list 超过 targetCount 进行裁剪：
            		小于5ms的方法移除
            	还是超过 targetCount 进行 fallback
            		只留前 x 条数据，其他都删除
            		这里的数量可以自定义，你觉得不够，那就多设置点,反之就减少
            */
            TraceDataUtils.trimStack(stack, Constants.TARGET_EVIL_METHOD_STACK, new TraceDataUtils.IStructuredDataFilter() {
                @Override
                public boolean isFilter(long during, int filterCount) {
                    return during < filterCount * Constants.TIME_UPDATE_CYCLE_MS;
                }

                @Override
                public int getFilterMaxCount() {
                    return Constants.FILTER_STACK_MAX_COUNT;
                }

                @Override
                public void fallback(List<MethodItem> stack, int size) {
                    MatrixLog.w(TAG, "[fallback] size:%s targetSize:%s stack:%s", size, Constants.TARGET_EVIL_METHOD_STACK, stack);
                    Iterator iterator = stack.listIterator(Math.min(size, Constants.TARGET_EVIL_METHOD_STACK));
                    while (iterator.hasNext()) {
                        iterator.next();
                        iterator.remove();
                    }
                }
            });
        }
		//0,1048574,1,2296
        StringBuilder reportBuilder = new StringBuilder();
        /*
        	|*		TraceStack:
            |*		[id count cost]
            |*		1048574 1 2296
        */
        StringBuilder logcatBuilder = new StringBuilder();
        
        //1.赋值 reportBuilder，logcatBuilder
		//2.计算方法的耗时
        long stackCost = Math.max(Constants.DEFAULT_ANR, TraceDataUtils.stackToString(stack, reportBuilder, logcatBuilder));

        // 获取到stack里的唯一标识 (方法id | 方法id)
        // 过滤栈耗时的百分比( >= stackCost * 0.3 则不删除)
        String stackKey = TraceDataUtils.getTreeKey(stack, stackCost);
        MatrixLog.w(TAG, "%s \npostTime:%s curTime:%s",
                printAnr(scene, processStat, memoryInfo, status, logcatBuilder, isForeground, stack.size(),
                        stackKey, dumpStack, inputCost, animationCost, traversalCost, stackCost),
                token / Constants.TIME_MILLIS_TO_NANO, curTime); // for logcat

        if (stackCost >= Constants.DEFAULT_ANR_INVALID) {
            MatrixLog.w(TAG, "The checked anr task was not executed on time. "
                    + "The possible reason is that the current process has a low priority. just pass this report");
            return;
        }
        // report
        try {
            TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
            if (null == plugin) {
                return;
            }
            JSONObject jsonObject = new JSONObject();
            jsonObject = DeviceUtil.getDeviceInfo(jsonObject, Matrix.with().getApplication());
            jsonObject.put(SharePluginInfo.ISSUE_STACK_TYPE, Constants.Type.ANR);
            jsonObject.put(SharePluginInfo.ISSUE_COST, stackCost);
            jsonObject.put(SharePluginInfo.ISSUE_STACK_KEY, stackKey);
            jsonObject.put(SharePluginInfo.ISSUE_SCENE, scene);
            jsonObject.put(SharePluginInfo.ISSUE_TRACE_STACK, reportBuilder.toString());
            if (traceConfig.getLooperPrinterStackStyle() == TraceConfig.STACK_STYLE_WHOLE) {
                jsonObject.put(SharePluginInfo.ISSUE_THREAD_STACK, Utils.getWholeStack(stackTrace));
            } else {
                jsonObject.put(SharePluginInfo.ISSUE_THREAD_STACK, Utils.getStack(stackTrace));
            }
            jsonObject.put(SharePluginInfo.ISSUE_PROCESS_PRIORITY, processStat[0]);
            jsonObject.put(SharePluginInfo.ISSUE_PROCESS_NICE, processStat[1]);
            jsonObject.put(SharePluginInfo.ISSUE_PROCESS_FOREGROUND, isForeground);
            // memory info
            JSONObject memJsonObject = new JSONObject();
            memJsonObject.put(SharePluginInfo.ISSUE_MEMORY_DALVIK, memoryInfo[0]);
            memJsonObject.put(SharePluginInfo.ISSUE_MEMORY_NATIVE, memoryInfo[1]);
            memJsonObject.put(SharePluginInfo.ISSUE_MEMORY_VM_SIZE, memoryInfo[2]);
            jsonObject.put(SharePluginInfo.ISSUE_MEMORY, memJsonObject);

            Issue issue = new Issue();
            issue.setKey(token + "");
            issue.setTag(SharePluginInfo.TAG_PLUGIN_EVIL_METHOD);
            issue.setContent(jsonObject);
            plugin.onDetectIssue(issue);

        } catch (JSONException e) {
            MatrixLog.e(TAG, "[JSONException error: %s", e);
        }

    }
}
```

```java
class LagHandleTask implements Runnable {

        @Override
        public void run() {
            String scene = AppActiveMatrixDelegate.INSTANCE.getVisibleScene();
            boolean isForeground = isForeground();
            try {
                TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
                if (null == plugin) {
                    return;
                }

                StackTraceElement[] stackTrace = Looper.getMainLooper().getThread().getStackTrace();
                String dumpStack = Utils.getWholeStack(stackTrace);

                JSONObject jsonObject = new JSONObject();
                jsonObject = DeviceUtil.getDeviceInfo(jsonObject, Matrix.with().getApplication());
                jsonObject.put(SharePluginInfo.ISSUE_STACK_TYPE, Constants.Type.LAG);
                jsonObject.put(SharePluginInfo.ISSUE_SCENE, scene);
                jsonObject.put(SharePluginInfo.ISSUE_THREAD_STACK, dumpStack);
                jsonObject.put(SharePluginInfo.ISSUE_PROCESS_FOREGROUND, isForeground);

                Issue issue = new Issue();
                issue.setTag(SharePluginInfo.TAG_PLUGIN_EVIL_METHOD);
                issue.setContent(jsonObject);
                plugin.onDetectIssue(issue);
                MatrixLog.e(TAG, "happens lag : %s, scene : %s ", dumpStack, scene);

            } catch (JSONException e) {
                MatrixLog.e(TAG, "[JSONException error: %s", e);
            }


        }
    }
```



### EvilMethodTracer

慢方法检测,方法耗时大于等于700ms,进行上报处理

```java
public void onAlive() {
    super.onAlive();
    if (isEvilMethodTraceEnable) {
        //添加监听
        UIThreadMonitor.getMonitor().addObserver(this);
    }
}

public void onDead() {
    super.onDead();
    if (isEvilMethodTraceEnable) {
        UIThreadMonitor.getMonitor().removeObserver(this);
    }
}
```



```java
public void dispatchBegin(long beginNs, long cpuBeginMs, long token) {
    super.dispatchBegin(beginNs, cpuBeginMs, token);
    indexRecord = AppMethodBeat.getInstance().maskIndex("EvilMethodTracer#dispatchBegin");
}

public void doFrame(String focusedActivity, long startNs, long endNs, boolean isVsyncFrame, long intendedFrameTimeNs, long inputCostNs, long animationCostNs, long traversalCostNs) {
    queueTypeCosts[0] = inputCostNs;
    queueTypeCosts[1] = animationCostNs;
    queueTypeCosts[2] = traversalCostNs;
}

public void dispatchEnd(long beginNs, long cpuBeginMs, long endNs, long cpuEndMs, long token, boolean isVsyncFrame) {
    super.dispatchEnd(beginNs, cpuBeginMs, endNs, cpuEndMs, token, isVsyncFrame);
    long start = config.isDevEnv() ? System.currentTimeMillis() : 0;
    long dispatchCost = (endNs - beginNs) / Constants.TIME_MILLIS_TO_NANO;
    try {
        // dispatchCost >= 700
        if (dispatchCost >= evilThresholdMs) {
            long[] data = AppMethodBeat.getInstance().copyData(indexRecord);
            long[] queueCosts = new long[3];
            System.arraycopy(queueTypeCosts, 0, queueCosts, 0, 3);
            String scene = AppActiveMatrixDelegate.INSTANCE.getVisibleScene();
            // 执行AnalyseTask
            MatrixHandlerThread.getDefaultHandler().post(new AnalyseTask(isForeground(), scene, data, queueCosts, cpuEndMs - cpuBeginMs, dispatchCost, endNs / Constants.TIME_MILLIS_TO_NANO));
        }
    } finally {
        indexRecord.release();
        if (config.isDevEnv()) {
            String usage = Utils.calculateCpuUsage(cpuEndMs - cpuBeginMs, dispatchCost);
            MatrixLog.v(TAG, "[dispatchEnd] token:%s cost:%sms cpu:%sms usage:%s innerCost:%s",
                    token, dispatchCost, cpuEndMs - cpuBeginMs, usage, System.currentTimeMillis() - start);
        }
    }
}
```



```java
private class AnalyseTask implements Runnable {
    long[] queueCost;
    long[] data;
    long cpuCost;
    long cost;
    long endMs;
    String scene;
    boolean isForeground;

    AnalyseTask(boolean isForeground, String scene, long[] data, long[] queueCost, long cpuCost, long cost, long endMs) {
        this.isForeground = isForeground;
        this.scene = scene;
        this.cost = cost;
        this.cpuCost = cpuCost;
        this.data = data;
        this.queueCost = queueCost;
        this.endMs = endMs;
    }

    void analyse() {

        // process
        int[] processStat = Utils.getProcessPriority(Process.myPid());
        String usage = Utils.calculateCpuUsage(cpuCost, cost);
        //此处逻辑和AnrTrace类似
        LinkedList<MethodItem> stack = new LinkedList();
        if (data.length > 0) {
            TraceDataUtils.structuredDataToStack(data, stack, true, endMs);
            TraceDataUtils.trimStack(stack, Constants.TARGET_EVIL_METHOD_STACK, new TraceDataUtils.IStructuredDataFilter() {
                @Override
                public boolean isFilter(long during, int filterCount) {
                    return during < filterCount * Constants.TIME_UPDATE_CYCLE_MS;
                }

                @Override
                public int getFilterMaxCount() {
                    return Constants.FILTER_STACK_MAX_COUNT;
                }

                @Override
                public void fallback(List<MethodItem> stack, int size) {
                    MatrixLog.w(TAG, "[fallback] size:%s targetSize:%s stack:%s", size, Constants.TARGET_EVIL_METHOD_STACK, stack);
                    Iterator iterator = stack.listIterator(Math.min(size, Constants.TARGET_EVIL_METHOD_STACK));
                    while (iterator.hasNext()) {
                        iterator.next();
                        iterator.remove();
                    }
                }
            });
        }


        StringBuilder reportBuilder = new StringBuilder();
        StringBuilder logcatBuilder = new StringBuilder();
        long stackCost = Math.max(cost, TraceDataUtils.stackToString(stack, reportBuilder, logcatBuilder));
        String stackKey = TraceDataUtils.getTreeKey(stack, stackCost);

        MatrixLog.w(TAG, "%s", printEvil(scene, processStat, isForeground, logcatBuilder, stack.size(), stackKey, usage, queueCost[0], queueCost[1], queueCost[2], cost)); // for logcat

        // report
        try {
            TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
            if (null == plugin) {
                return;
            }
            JSONObject jsonObject = new JSONObject();
            jsonObject = DeviceUtil.getDeviceInfo(jsonObject, Matrix.with().getApplication());

            jsonObject.put(SharePluginInfo.ISSUE_STACK_TYPE, Constants.Type.NORMAL);
            jsonObject.put(SharePluginInfo.ISSUE_COST, stackCost);
            jsonObject.put(SharePluginInfo.ISSUE_CPU_USAGE, usage);
            jsonObject.put(SharePluginInfo.ISSUE_SCENE, scene);
            jsonObject.put(SharePluginInfo.ISSUE_TRACE_STACK, reportBuilder.toString());
            jsonObject.put(SharePluginInfo.ISSUE_STACK_KEY, stackKey);

            Issue issue = new Issue();
            issue.setTag(SharePluginInfo.TAG_PLUGIN_EVIL_METHOD);
            issue.setContent(jsonObject);
            plugin.onDetectIssue(issue);

        } catch (JSONException e) {
            MatrixLog.e(TAG, "[JSONException error: %s", e);
        }

    }

    @Override
    public void run() {
        analyse();
    }

    private String printEvil(String scene, int[] processStat, boolean isForeground, StringBuilder stack, long stackSize, String stackKey, String usage, long inputCost,
                             long animationCost, long traversalCost, long allCost) {
        StringBuilder print = new StringBuilder();
        print.append(String.format("-\n>>>>>>>>>>>>>>>>>>>>> maybe happens Jankiness!(%sms) <<<<<<<<<<<<<<<<<<<<<\n", allCost));
        print.append("|* [Status]").append("\n");
        print.append("|*\t\tScene: ").append(scene).append("\n");
        print.append("|*\t\tForeground: ").append(isForeground).append("\n");
        print.append("|*\t\tPriority: ").append(processStat[0]).append("\tNice: ").append(processStat[1]).append("\n");
        print.append("|*\t\tis64BitRuntime: ").append(DeviceUtil.is64BitRuntime()).append("\n");
        print.append("|*\t\tCPU: ").append(usage).append("\n");
        print.append("|* [doFrame]").append("\n");
        print.append("|*\t\tinputCost:animationCost:traversalCost").append("\n");
        print.append("|*\t\t").append(inputCost).append(":").append(animationCost).append(":").append(traversalCost).append("\n");
        if (stackSize > 0) {
            print.append("|*\t\tStackKey: ").append(stackKey).append("\n");
            print.append(stack.toString());
        } else {
            print.append(String.format("AppMethodBeat is close[%s].", AppMethodBeat.getInstance().isAlive())).append("\n");
        }

        print.append("=========================================================================");
        return print.toString();
    }
}
```



### IdleHandlerLagTracer

```java
@Override
public void onAlive() {
    super.onAlive();
    if (traceConfig.isIdleHandlerTraceEnable()) {
        idleHandlerLagHandlerThread = new HandlerThread("IdleHandlerLagThread");
        idleHandlerLagRunnable = new IdleHandlerLagRunable();
        detectIdleHandler();
    }
}
@Override
public void onDead() {
    super.onDead();
    if (traceConfig.isIdleHandlerTraceEnable()) {
        idleHandlerLagHandler.removeCallbacksAndMessages(null);
    }
}

private static void detectIdleHandler() {
    try {
        if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.M) {
            return;
        }
        MessageQueue mainQueue = Looper.getMainLooper().getQueue();
        Field field = MessageQueue.class.getDeclaredField("mIdleHandlers");
        field.setAccessible(true);
        //创建 MyArrayList
        MyArrayList<MessageQueue.IdleHandler> myIdleHandlerArrayList = new MyArrayList<>();
        //替换原IdleHandler
        field.set(mainQueue, myIdleHandlerArrayList);
        //启动handlerThread
        idleHandlerLagHandlerThread.start();
        idleHandlerLagHandler = new Handler(idleHandlerLagHandlerThread.getLooper());
    } catch (Throwable t) {
        MatrixLog.e(TAG, "reflect idle handler error = " + t.getMessage());
    }
}
```



```java
static class MyArrayList<T> extends ArrayList {
    Map<MessageQueue.IdleHandler, MyIdleHandler> map = new HashMap<>();
    @Override
    public boolean add(Object o) {
        if (o instanceof MessageQueue.IdleHandler) {
            //创建 MyIdleHandler 装在原始IdleHandler
            MyIdleHandler myIdleHandler = new MyIdleHandler((MessageQueue.IdleHandler) o);
			//存储进map            
            map.put((MessageQueue.IdleHandler) o, myIdleHandler);
            return super.add(myIdleHandler);
        }
        return super.add(o);
    }
    @Override
    public boolean remove(@Nullable Object o) {
        if (o instanceof MyIdleHandler) {
            MessageQueue.IdleHandler idleHandler = ((MyIdleHandler) o).idleHandler;
            map.remove(idleHandler);
            return super.remove(o);
        } else {
            MyIdleHandler myIdleHandler = map.remove(o);
            if (myIdleHandler != null) {
                return super.remove(myIdleHandler);
            }
            return super.remove(o);
        }
    }
}
```





```java
static class MyIdleHandler implements MessageQueue.IdleHandler {
    private final MessageQueue.IdleHandler idleHandler;

    MyIdleHandler(MessageQueue.IdleHandler idleHandler) {
        this.idleHandler = idleHandler;
    }

    @Override
    public boolean queueIdle() {
        // 2s后执行 idleHandlerLagRunnable 进行分析上报
        idleHandlerLagHandler.postDelayed(idleHandlerLagRunnable 进行分析上报, traceConfig.idleHandlerLagThreshold);
        //执行原始的idleHandler.queueIdle()任务
        boolean ret = this.idleHandler.queueIdle();
        //如果任务在2秒内执行完成,就移除上报任务
        idleHandlerLagHandler.removeCallbacks(idleHandlerLagRunnable);
        return ret;
    }
}
```



```java
static class IdleHandlerLagRunable implements Runnable {
    @Override
    public void run() {
        try {
            TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
            if (null == plugin) {
                return;
            }

            String stackTrace = Utils.getMainThreadJavaStackTrace();
            //TODO
            boolean currentForeground = AppForegroundUtil.isInterestingToUser();
            String scene = AppActiveMatrixDelegate.INSTANCE.getVisibleScene();

            JSONObject jsonObject = new JSONObject();
            jsonObject = DeviceUtil.getDeviceInfo(jsonObject, Matrix.with().getApplication());
            jsonObject.put(SharePluginInfo.ISSUE_STACK_TYPE, Constants.Type.LAG_IDLE_HANDLER);
            jsonObject.put(SharePluginInfo.ISSUE_SCENE, scene);
            jsonObject.put(SharePluginInfo.ISSUE_THREAD_STACK, stackTrace);
            jsonObject.put(SharePluginInfo.ISSUE_PROCESS_FOREGROUND, currentForeground);

            Issue issue = new Issue();
            issue.setTag(SharePluginInfo.TAG_PLUGIN_EVIL_METHOD);
            issue.setContent(jsonObject);
            plugin.onDetectIssue(issue);
            MatrixLog.e(TAG, "happens idle handler Lag : %s ", jsonObject.toString());
            
        } catch (Throwable t) {
            MatrixLog.e(TAG, "Matrix error, error = " + t.getMessage());
        }

    }
}
```







### SignalAnrTracer



### ThreadPriorityTracer



### TouchEventLagTracer



### StartupTracer

> 变量

```JAVA
private final TraceConfig config;
//首屏耗时
private long firstScreenCost = 0;
//冷启动花费时间
private long coldCost = 0;
//activity数量
private int activeActivityCount;
//是否是温启动
private boolean isWarmStartUp;
//是否存在闪屏页
private boolean hasShowSplashActivity;
//是否开启启动耗时检测
private boolean isStartupEnable;
private Set<String> splashActivities;
//冷启动阈值
private long coldStartupThresholdMs;
//文启动阈值
private long warmStartupThresholdMs;
private boolean isHasActivity;

//最后1个activity的创建时间，用来计算温启动耗时
private long lastCreateActivity = 0L;
private HashMap<String, Long> createdTimeMap = new HashMap<>();
private boolean isShouldRecordCreateTime = true;
```



> 构造方法

```java
public StartupTracer(TraceConfig config) {
    this.config = config;
    this.isStartupEnable = config.isStartupEnable();
    this.splashActivities = config.getSplashActivities();
    //DEFAULT_STARTUP_THRESHOLD_MS_COLD = 10 * 1000
    this.coldStartupThresholdMs = config.getColdStartupThresholdMs();
    //DEFAULT_STARTUP_THRESHOLD_MS_WARM = 4 * 1000
    this.warmStartupThresholdMs = config.getWarmStartupThresholdMs();
    this.isHasActivity = config.isHasActivity();
    // 监听 onApplicationCreateEnd 回调
    ActivityThreadHacker.addListener(this);
}
```



```java
protected void onAlive() {
    super.onAlive();
    MatrixLog.i(TAG, "[onAlive] isStartupEnable:%s", isStartupEnable);
    if (isStartupEnable) {
        //添加 onActivityFocused 监听 
        AppMethodBeat.getInstance().addListener(this);
        //注册activity生命周期回调
        Matrix.with().getApplication().registerActivityLifecycleCallbacks(this);
    }
}

protected void onDead() {
    super.onDead();
    if (isStartupEnable) {
        AppMethodBeat.getInstance().removeListener(this);
        Matrix.with().getApplication().unregisterActivityLifecycleCallbacks(this);
    }
}
```



> 冷启动和热启动判断

```java
private boolean isColdStartup() {
    return coldCost == 0;
}
private boolean isWarmStartUp() {
    return isWarmStartUp;
}
```



> application创建结束监听

```java
public void onApplicationCreateEnd() {
	// 是否有Activity,默认为true  ？？？
    if (!isHasActivity) {
		//TODO 创建Application耗时        
        long applicationCost = ActivityThreadHacker.getApplicationCost();
        MatrixLog.i(TAG, "onApplicationCreateEnd, applicationCost:%d", applicationCost);
        //创建application耗时分析
        analyse(applicationCost, 0, applicationCost, false);
    }
}

```



> onActivityFocused 监听

```java
public void onActivityFocused(Activity activity) {
    if (ActivityThreadHacker.sApplicationCreateScene == Integer.MIN_VALUE) {
        Log.w(TAG, "start up from unknown scene");
        return;
    }

    String activityName = activity.getClass().getName();
    //冷启动
    if (isColdStartup()) {
        // 是否创建了LaunchActivity
        boolean isCreatedByLaunchActivity = ActivityThreadHacker.isCreatedByLaunchActivity();
        String key = activityName + "@" + activity.hashCode();
        Long createdTime = createdTimeMap.get(key);
        if (createdTime == null) {
            createdTime = 0L;
        }
        createdTimeMap.put(key, uptimeMillis() - createdTime);

        // 计算首屏耗时
        if (firstScreenCost == 0) {
            // 首屏耗时 = 现在时间 - 创建Application的开始时间(破蛋时间)
            this.firstScreenCost = uptimeMillis() - ActivityThreadHacker.getEggBrokenTime();
        }
        // 是否有闪屏页
        if (hasShowSplashActivity) {
            coldCost = uptimeMillis() - ActivityThreadHacker.getEggBrokenTime();
        } else {
            //是否存在闪屏页
            if (splashActivities.contains(activityName)) {
                hasShowSplashActivity = true;
            } else if (splashActivities.isEmpty()) { //process which is has activity but not main UI process
                // 非UI主进程,计算冷启动耗时
                if (isCreatedByLaunchActivity) {
                    coldCost = firstScreenCost;
                } else {
                    firstScreenCost = 0;
                    coldCost = ActivityThreadHacker.getApplicationCost();
                }
            } else {
                //有创建LaunchActivity,冷启动耗时等于首屏耗时,没有创建，就等于application创建耗时
                if (isCreatedByLaunchActivity) {
                    coldCost = firstScreenCost;
                } else {
                    firstScreenCost = 0;
                    coldCost = ActivityThreadHacker.getApplicationCost();
                }
            }
        }
        // coldCost > 0 进行冷启动分析
        if (coldCost > 0) {
            Long betweenCost = createdTimeMap.get(key);
            if (null != betweenCost && betweenCost >= 30 * 1000) {
                MatrixLog.e(TAG, "%s cost too much time[%s] between activity create and onActivityFocused, "
                        + "just throw it.(createTime:%s) ", key, uptimeMillis() - createdTime, createdTime);
                return;
            }
            analyse(ActivityThreadHacker.getApplicationCost(), firstScreenCost, coldCost, false);
        }

    } else if (isWarmStartUp()) { //热启动
        isWarmStartUp = false;
        // 温启动耗时 = 现在时间 - 最后一个activity的启动时间
        long warmCost = uptimeMillis() - lastCreateActivity;
        if (warmCost > 0) {
            //进行热启动分析
            analyse(0, 0, warmCost, true);
        }
    }
}
```



> 生命周期监听

```java
public void onActivityCreated(Activity activity, Bundle savedInstanceState) {
    MatrixLog.i(TAG, "activeActivityCount:%d, coldCost:%d", activeActivityCount, coldCost);
    // activity数量为0,冷启动大于0 代表是温启动
    if (activeActivityCount == 0 && coldCost > 0) {
        lastCreateActivity = uptimeMillis();
        MatrixLog.i(TAG, "lastCreateActivity:%d, activity:%s", lastCreateActivity, activity.getClass().getName());
        isWarmStartUp = true;
    }
    // 用来判断是否是温启动
    activeActivityCount++;
    if (isShouldRecordCreateTime) {
        createdTimeMap.put(activity.getClass().getName() + "@" + activity.hashCode(), uptimeMillis());
    }
}


public void onActivityDestroyed(Activity activity) {
    MatrixLog.i(TAG, "activeActivityCount:%d", activeActivityCount);
    activeActivityCount--;
}
```



> analyse

```java
private void analyse(long applicationCost, long firstScreenCost, long allCost, boolean isWarmStartUp) {
    MatrixLog.i(TAG, "[report] applicationCost:%s firstScreenCost:%s allCost:%s isWarmStartUp:%s, createScene:%d",
            applicationCost, firstScreenCost, allCost, isWarmStartUp, ActivityThreadHacker.sApplicationCreateScene);
    long[] data = new long[0];
    if (!isWarmStartUp && allCost >= coldStartupThresholdMs) { // for cold startup
        data = AppMethodBeat.getInstance().copyData(ActivityThreadHacker.sApplicationCreateBeginMethodIndex);
        ActivityThreadHacker.sApplicationCreateBeginMethodIndex.release();

    } else if (isWarmStartUp && allCost >= warmStartupThresholdMs) {
        data = AppMethodBeat.getInstance().copyData(ActivityThreadHacker.sLastLaunchActivityMethodIndex);
        ActivityThreadHacker.sLastLaunchActivityMethodIndex.release();
    }
	//分析冷启动耗时
    MatrixHandlerThread.getDefaultHandler().post(new AnalyseTask(data, applicationCost, firstScreenCost, allCost, isWarmStartUp, ActivityThreadHacker.sApplicationCreateScene));

}
```



略，大致流程和之前一样

```java
private class AnalyseTask implements Runnable {

    long[] data;
    long applicationCost;
    long firstScreenCost;
    long allCost;
    boolean isWarmStartUp;
    int scene;

    AnalyseTask(long[] data, long applicationCost, long firstScreenCost, long allCost, boolean isWarmStartUp, int scene) {
        this.data = data;
        this.scene = scene;
        this.applicationCost = applicationCost;
        this.firstScreenCost = firstScreenCost;
        this.allCost = allCost;
        this.isWarmStartUp = isWarmStartUp;
    }

    @Override
    public void run() {
        LinkedList<MethodItem> stack = new LinkedList();
        if (data.length > 0) {
            TraceDataUtils.structuredDataToStack(data, stack, false, -1);
            TraceDataUtils.trimStack(stack, Constants.TARGET_EVIL_METHOD_STACK, new TraceDataUtils.IStructuredDataFilter() {
                @Override
                public boolean isFilter(long during, int filterCount) {
                    return during < filterCount * Constants.TIME_UPDATE_CYCLE_MS;
                }

                @Override
                public int getFilterMaxCount() {
                    return Constants.FILTER_STACK_MAX_COUNT;
                }

                @Override
                public void fallback(List<MethodItem> stack, int size) {
                    MatrixLog.w(TAG, "[fallback] size:%s targetSize:%s stack:%s", size, Constants.TARGET_EVIL_METHOD_STACK, stack);
                    Iterator iterator = stack.listIterator(Math.min(size, Constants.TARGET_EVIL_METHOD_STACK));
                    while (iterator.hasNext()) {
                        iterator.next();
                        iterator.remove();
                    }

                }
            });
        }

        StringBuilder reportBuilder = new StringBuilder();
        StringBuilder logcatBuilder = new StringBuilder();
        long stackCost = Math.max(allCost, TraceDataUtils.stackToString(stack, reportBuilder, logcatBuilder));
        String stackKey = TraceDataUtils.getTreeKey(stack, stackCost);

        // for logcat
        if ((allCost > coldStartupThresholdMs && !isWarmStartUp)
                || (allCost > warmStartupThresholdMs && isWarmStartUp)) {
            MatrixLog.w(TAG, "stackKey:%s \n%s", stackKey, logcatBuilder.toString());
        }

        // report
        report(applicationCost, firstScreenCost, reportBuilder, stackKey, stackCost, isWarmStartUp, scene);
    }

    private void report(long applicationCost, long firstScreenCost, StringBuilder reportBuilder, String stackKey,
                        long allCost, boolean isWarmStartUp, int scene) {

        TracePlugin plugin = Matrix.with().getPluginByClass(TracePlugin.class);
        if (null == plugin) {
            return;
        }
        try {
            JSONObject costObject = new JSONObject();
            costObject = DeviceUtil.getDeviceInfo(costObject, Matrix.with().getApplication());
            costObject.put(SharePluginInfo.STAGE_APPLICATION_CREATE, applicationCost);
            costObject.put(SharePluginInfo.STAGE_APPLICATION_CREATE_SCENE, scene);
            costObject.put(SharePluginInfo.STAGE_FIRST_ACTIVITY_CREATE, firstScreenCost);
            costObject.put(SharePluginInfo.STAGE_STARTUP_DURATION, allCost);
            costObject.put(SharePluginInfo.ISSUE_IS_WARM_START_UP, isWarmStartUp);
            Issue issue = new Issue();
            issue.setTag(SharePluginInfo.TAG_PLUGIN_STARTUP);
            issue.setContent(costObject);
            plugin.onDetectIssue(issue);
        } catch (JSONException e) {
            MatrixLog.e(TAG, "[JSONException for StartUpReportTask error: %s", e);
        }


        if ((allCost > coldStartupThresholdMs && !isWarmStartUp)
                || (allCost > warmStartupThresholdMs && isWarmStartUp)) {

            try {
                JSONObject jsonObject = new JSONObject();
                jsonObject = DeviceUtil.getDeviceInfo(jsonObject, Matrix.with().getApplication());
                jsonObject.put(SharePluginInfo.ISSUE_STACK_TYPE, Constants.Type.STARTUP);
                jsonObject.put(SharePluginInfo.ISSUE_COST, allCost);
                jsonObject.put(SharePluginInfo.ISSUE_TRACE_STACK, reportBuilder.toString());
                jsonObject.put(SharePluginInfo.ISSUE_STACK_KEY, stackKey);
                jsonObject.put(SharePluginInfo.ISSUE_SUB_TYPE, isWarmStartUp ? 2 : 1);
                Issue issue = new Issue();
                issue.setTag(SharePluginInfo.TAG_PLUGIN_EVIL_METHOD);
                issue.setContent(jsonObject);
                plugin.onDetectIssue(issue);

            } catch (JSONException e) {
                MatrixLog.e(TAG, "[JSONException error: %s", e);
            }
        }
    }
}
```





#### ActivityThreadHacker

```java
public class ActivityThreadHacker {
    private static final String TAG = "Matrix.ActivityThreadHacker";
    private static long sApplicationCreateBeginTime = 0L;
    private static long sApplicationCreateEndTime = 0L;
    public static AppMethodBeat.IndexRecord sLastLaunchActivityMethodIndex = new AppMethodBeat.IndexRecord();
    public static AppMethodBeat.IndexRecord sApplicationCreateBeginMethodIndex = new AppMethodBeat.IndexRecord();
    public static int sApplicationCreateScene = Integer.MIN_VALUE;
    private static final HashSet<IApplicationCreateListener> listeners = new HashSet<>();
    private static boolean sIsCreatedByLaunchActivity = false;

    public static void addListener(IApplicationCreateListener listener) {
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    public static void removeListener(IApplicationCreateListener listener) {
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    public interface IApplicationCreateListener {
        void onApplicationCreateEnd();
    }
	
    //替换HandlerCallback
    public static void hackSysHandlerCallback() {
        try {
            // 设置application创建开始时间
            sApplicationCreateBeginTime = SystemClock.uptimeMillis();
            sApplicationCreateBeginMethodIndex = AppMethodBeat.getInstance().maskIndex("ApplicationCreateBeginMethodIndex");
            Class<?> forName = Class.forName("android.app.ActivityThread");
            Field field = forName.getDeclaredField("sCurrentActivityThread");
            field.setAccessible(true);
            Object activityThreadValue = field.get(forName);
            Field mH = forName.getDeclaredField("mH");
            mH.setAccessible(true);
            Object handler = mH.get(activityThreadValue);
            Class<?> handlerClass = handler.getClass().getSuperclass();
            if (null != handlerClass) {
                Field callbackField = handlerClass.getDeclaredField("mCallback");
                callbackField.setAccessible(true);
                //获取原始callback
                Handler.Callback originalCallback = (Handler.Callback) callbackField.get(handler);
                //创建自定义callback,再将原callback传递进去
                HackCallback callback = new HackCallback(originalCallback);
                //设置自定义callback
                callbackField.set(handler, callback);
            }

            MatrixLog.i(TAG, "hook system handler completed. start:%s SDK_INT:%s", sApplicationCreateBeginTime, Build.VERSION.SDK_INT);
        } catch (Exception e) {
            MatrixLog.e(TAG, "hook system handler err! %s", e.getCause().toString());
        }
    }

    public static long getApplicationCost() {
        return ActivityThreadHacker.sApplicationCreateEndTime
                - ActivityThreadHacker.sApplicationCreateBeginTime;
    }

    public static long getEggBrokenTime() {
        return ActivityThreadHacker.sApplicationCreateBeginTime;
    }

    public static boolean isCreatedByLaunchActivity() {
        return sIsCreatedByLaunchActivity;
    }
	//...
}
```



```java
private final static class HackCallback implements Handler.Callback {
    private static final int LAUNCH_ACTIVITY = 100;
    private static final int CREATE_SERVICE = 114;
    private static final int RELAUNCH_ACTIVITY = 126;
    private static final int RECEIVER = 113;
    private static final int EXECUTE_TRANSACTION = 159; // for Android 9.0
    private static boolean isCreated = false;
    private static int hasPrint = Integer.MAX_VALUE;

    private final Handler.Callback mOriginalCallback;

    private static final int SERIVCE_ARGS = 115;
    private static final int STOP_SERVICE = 116;
    private static final int STOP_ACTIVITY_SHOW = 103;
    private static final int STOP_ACTIVITY_HIDE = 104;
    private static final int SLEEPING = 137;

    HackCallback(Handler.Callback callback) {
        this.mOriginalCallback = callback;
    }

    @Override
    public boolean handleMessage(Message msg) {
        if (IssueFixConfig.getsInstance().isEnableFixSpApply()) {
            //SDK 21~25 之间 ,需要清除sPendingWorkFinishers ???
            if (Build.VERSION.SDK_INT >= 21 && Build.VERSION.SDK_INT <= 25) {
                if (msg.what == SERIVCE_ARGS || msg.what == STOP_SERVICE
                        || msg.what == STOP_ACTIVITY_SHOW || msg.what == STOP_ACTIVITY_HIDE
                        || msg.what == SLEEPING) {
                        MatrixLog.i(TAG, "Fix SP ANR is enabled");
                        fix();
                    }
            }
        }

        if (!AppMethodBeat.isRealTrace()) {
            return null != mOriginalCallback && mOriginalCallback.handleMessage(msg);
        }
		
        //判断是否是LauncherActivity
        boolean isLaunchActivity = isLaunchActivity(msg);

        if (hasPrint > 0) {
            MatrixLog.i(TAG, "[handleMessage] msg.what:%s begin:%s isLaunchActivity:%s SDK_INT=%s", msg.what, SystemClock.uptimeMillis(), isLaunchActivity, Build.VERSION.SDK_INT);
            hasPrint--;
        }
		
        // 默认为false
        if (!isCreated) {
            //是LaunchActivity || Service创建 || RECEIVER
            if (isLaunchActivity || msg.what == CREATE_SERVICE
                    || msg.what == RECEIVER) { // todo for provider
                //设置application 创建结束时间
                ActivityThreadHacker.sApplicationCreateEndTime = SystemClock.uptimeMillis();
                ActivityThreadHacker.sApplicationCreateScene = msg.what;
                isCreated = true;
                sIsCreatedByLaunchActivity = isLaunchActivity;
                MatrixLog.i(TAG, "application create end, sApplicationCreateScene:%d, isLaunchActivity:%s", msg.what, isLaunchActivity);
                synchronized (listeners) {
                    for (IApplicationCreateListener listener : listeners) {
                        listener.onApplicationCreateEnd();
                    }
                }
            }
        }
        return null != mOriginalCallback && mOriginalCallback.handleMessage(msg);
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void fix() {
        try {
            Class cls = Class.forName("android.app.QueuedWork");
            Field field = cls.getDeclaredField("sPendingWorkFinishers");
            if (field != null) {
                field.setAccessible(true);
                ConcurrentLinkedQueue<Runnable> runnables = (ConcurrentLinkedQueue<Runnable>) field.get(null);
                runnables.clear();
                MatrixLog.i(TAG, "Fix SP ANR sPendingWorkFinishers.clear successful");
            }
        } catch (ClassNotFoundException e) {
            MatrixLog.e(TAG,
                    "Fix SP ANR ClassNotFoundException = " + e.getMessage());
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            MatrixLog.e(TAG, "Fix SP ANR IllegalAccessException =" + e.getMessage());
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            MatrixLog.e(TAG, "Fix SP ANR NoSuchFieldException = " + e.getMessage());
            e.printStackTrace();
        } catch (Exception e) {
            MatrixLog.e(TAG, "Fix SP ANR Exception = " + e.getMessage());
            e.printStackTrace();
        }

    }

    private Method method = null;

    private boolean isLaunchActivity(Message msg) {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.O_MR1) {
            if (msg.what == EXECUTE_TRANSACTION && msg.obj != null) {
                try {
                    if (null == method) {
                        Class clazz = Class.forName("android.app.servertransaction.ClientTransaction");
                        method = clazz.getDeclaredMethod("getCallbacks");
                        method.setAccessible(true);
                    }
                    List list = (List) method.invoke(msg.obj);
                    if (!list.isEmpty()) {
                        return list.get(0).getClass().getName().endsWith(".LaunchActivityItem");
                    }
                } catch (Exception e) {
                    MatrixLog.e(TAG, "[isLaunchActivity] %s", e);
                }
            }
            return msg.what == LAUNCH_ACTIVITY;
        } else {
            return msg.what == LAUNCH_ACTIVITY || msg.what == RELAUNCH_ACTIVITY;
        }
    }
}
```







### AppMethodBeat

主要负责方法调用耗时的统计和存储、堆栈的获取等。

通过字节码插桩,方法入口和出口分别调用i()和o()



#### 初始化入口 realExecute

该方法是在`i()`方法中触发的。肯定比`onStart`方法先走。

因为当应用的Application对象创建的时候会调用realExecute()。

**Application的创建过程：**

1. ActivityThread.handleBindApplication->
2. LoadApk.makeApplication->
3. AppComponentFactory.instantiateApplication

```java
public static void i(int methodId) {
    if (status <= STATUS_STOPPED) {
        return;
    }
    if (methodId >= METHOD_ID_MAX) {
        return;
    }
    if (status == STATUS_DEFAULT) {
        synchronized (statusLock) {
            if (status == STATUS_DEFAULT) {
                //初始化
                realExecute();
                //修改状态
                status = STATUS_READY;
            }
        }
    }
}
```



```java
private static void realExecute() {
    MatrixLog.i(TAG, "[realExecute] timestamp:%s", System.currentTimeMillis());
    //更新耗时偏移
    sCurrentDiffTime = SystemClock.uptimeMillis() - sDiffTime;
	//移除静态代码块中的第一次自毁task
    sHandler.removeCallbacksAndMessages(null);
    //每5ms更新时间戳
    sHandler.postDelayed(sUpdateDiffTimeRunnable, Constants.TIME_UPDATE_CYCLE_MS);
    //10s内如果app没有启用AppMethodBeat功能，进行状态变更
    sHandler.postDelayed(checkStartExpiredRunnable = new Runnable() {
        @Override
        public void run() {
            synchronized (statusLock) {
                MatrixLog.i(TAG, "[startExpired] timestamp:%s status:%s", System.currentTimeMillis(), status);
                if (status == STATUS_DEFAULT || status == STATUS_READY) {
                    status = STATUS_EXPIRED_START;
                }
            }
        }
    }, Constants.DEFAULT_RELEASE_BUFFER_DELAY);
	//hook 系统的handlerCallback ，处理自己的逻辑
    ActivityThreadHacker.hackSysHandlerCallback();
    //注册looper的回调监听
    LooperMonitor.register(looperMonitorListener);
}
```



#####  更新耗时偏移量

基本原理：

平时我们计算某个函数的耗时，就是在方法执行前后两次调用`SystemClock.uptimeMillis()`，计算差值即可。但是`SystemClock.uptimeMillis()`本身就存在耗时，matrix存在大量的计算耗时逻辑。如果直接调用势必会影响到调用链底层的方法耗时统计，造成误差。

因此，matrix做了如下优化： 通过维护基准计时变量`sDiffTime`（这个值是不会变的）。然后在通过开启一个子线程每隔`5ms`去更新这个值 `sCurrentDiffTime = SystemClock.uptimeMillis() - sDiffTime`。所以，随着程序的运行，这个值会慢慢变大。

举个例子：

当某个方法A进入`i`方法时，我们存入sCurrentDiffTime值如：`50ms`。当A方法执行完毕时调用`o`方法，由于sCurrentDiffTime一直会被更新，此时为`500ms`。那么，我们直接做一次差值就知道A方法的耗时为： `450ms`。这样虽然有`5ms`左右的误差，但是对于卡顿耗时来说并不影响。确实不错呀！



```java
private static Runnable sUpdateDiffTimeRunnable = new Runnable() {
    @Override
    public void run() {
        try {
            while (true) {
                while (!isPauseUpdateTime && status > STATUS_STOPPED) {
                    sCurrentDiffTime = SystemClock.uptimeMillis() - sDiffTime;
                    //5ms后在更新
                    SystemClock.sleep(Constants.TIME_UPDATE_CYCLE_MS);
                }
                
                //不需要更新时间偏移量，进入wait状态，不要浪费CPU。当消息分发，调用dispatchBegin，会进行唤醒
                //进入等待状态：释放锁、释放CPU，等待其他线程唤醒
                synchronized (updateTimeLock) {
                    updateTimeLock.wait();
                }
            }
        } catch (Exception e) {
            MatrixLog.e(TAG, "" + e.toString());
        }
    }
};

private static void dispatchBegin() {
    sCurrentDiffTime = SystemClock.uptimeMillis() - sDiffTime;
    isPauseUpdateTime = false;
    synchronized (updateTimeLock) {
        //唤醒
        updateTimeLock.notify();
    }
}

private static void dispatchEnd() {
    //改变标志位后进行等待,释放CPU
    isPauseUpdateTime = true;
}
```



##### hook 系统的handlerCallback

```java
public static void hackSysHandlerCallback() {
    try {
        sApplicationCreateBeginTime = SystemClock.uptimeMillis();
        sApplicationCreateBeginMethodIndex = AppMethodBeat.getInstance().maskIndex("ApplicationCreateBeginMethodIndex");
        Class<?> forName = Class.forName("android.app.ActivityThread");
        Field field = forName.getDeclaredField("sCurrentActivityThread");
        field.setAccessible(true);
        Object activityThreadValue = field.get(forName);
        Field mH = forName.getDeclaredField("mH");
        mH.setAccessible(true);
        Object handler = mH.get(activityThreadValue);
        Class<?> handlerClass = handler.getClass().getSuperclass();
        if (null != handlerClass) {
            Field callbackField = handlerClass.getDeclaredField("mCallback");
            callbackField.setAccessible(true);
            Handler.Callback originalCallback = (Handler.Callback) callbackField.get(handler);
            //创建自定义callback,再将原callback传递进去
            HackCallback callback = new HackCallback(originalCallback);
            //设置自定义callback
            callbackField.set(handler, callback);
        }
        MatrixLog.i(TAG, "hook system handler completed. start:%s SDK_INT:%s", sApplicationCreateBeginTime, Build.VERSION.SDK_INT);
    } catch (Exception e) {
        MatrixLog.e(TAG, "hook system handler err! %s", e.getCause().toString());
    }
}
```



#### i()方法

```java
public static void i(int methodId) {
    if (status <= STATUS_STOPPED) {
        return;
    }
    if (methodId >= METHOD_ID_MAX) {
        return;
    }
    if (status == STATUS_DEFAULT) {
        synchronized (statusLock) {
            if (status == STATUS_DEFAULT) {
                //初始化 (详情请见上文)
                realExecute();
                status = STATUS_READY;
            }
        }
    }
    long threadId = Thread.currentThread().getId();
    if (sMethodEnterListener != null) {
        sMethodEnterListener.enter(methodId, threadId);
    }
    // 主线程才命中
    if (threadId == sMainThreadId) {
        if (assertIn) {
            android.util.Log.e(TAG, "ERROR!!! AppMethodBeat.i Recursive calls!!!");
            return;
        }
        assertIn = true;
        if (sIndex < Constants.BUFFER_SIZE) {
            //记录函数进入时候的方法id和耗时，封装为一个long数据存入到buffer数组中。然后index自增。
            mergeData(methodId, sIndex, true);
        } else {
            //index超过数组长度，则重新从0开始
            sIndex = 0;
            mergeData(methodId, sIndex, true);
        }
        ++sIndex;
        assertIn = false;
    }
}
```





> mergeData

```java
private static void mergeData(int methodId, int index, boolean isIn) {
    //消息分发前后都会调用到这里。传入的都是METHOD_ID_DISPATCH。作为一个消息消息帧。
    //在每个帧中间或者两帧之间的方法会被记录下来。
    if (methodId == AppMethodBeat.METHOD_ID_DISPATCH) {
        sCurrentDiffTime = SystemClock.uptimeMillis() - sDiffTime;
    }
    try {
        long trueId = 0L;
        if (isIn) {
            //如果是方法开始：，那么就是在long的高63位设置1。
            trueId |= 1L << 63;
        }
        // 43~62位 是methodId的存储区间，共 20位
        trueId |= (long) methodId << 43;
        //0~42位 则是耗时存储的位置。 共 43位
	    //& 0x7FFFFFFFFFFL ,得到是原值：sCurrentDiffTime。
		//去除高位的冗余信息
        trueId |= sCurrentDiffTime & 0x7FFFFFFFFFFL;
        //把以上三个参数组成一个完整的long数据，存入下标中。
        sBuffer[index] = trueId;
        //标记链表数据为无效，
        checkPileup(index);
        sLastIndex = index;
    } catch (Throwable t) {
        MatrixLog.e(TAG, t.getMessage());
    }
}
```

> checkPileup

```java
//避免重复堆积
private static void checkPileup(int index) {
    IndexRecord indexRecord = sIndexRecordHead;
    while (indexRecord != null) {
        if (indexRecord.index == index || (indexRecord.index == -1 && sLastIndex == Constants.BUFFER_SIZE - 1)) {
            //标记无效
            indexRecord.isValid = false;
            MatrixLog.w(TAG, "[checkPileup] %s", indexRecord.toString());
            //删除当前节点，更新头结点。继续遍历
            sIndexRecordHead = indexRecord = indexRecord.next;
        } else {
            //直到，不等则退出循环。至此，链表已经排除掉了旧数据。
            break;
        }
    }
}
```

总结： `sIndexRecordHead`代表链表的头结点。如果一直不获取链表中数据（也就是不发生卡顿），那么链表势必会变长，此时`buffer`数组中存储还是旧数据，当然此时数据也就没有用了。同时`index`当超过buffer的`size`后又会重新开始计数。所以当新的`index`下标处要插入新数据时候，必须要删除之前跟`index`绑定的链表节点，避免数据冗余。



#### o()方法

同i()方法

```java
public static void o(int methodId) {
    if (status <= STATUS_STOPPED) {
        return;
    }
    if (methodId >= METHOD_ID_MAX) {
        return;
    }
    if (Thread.currentThread().getId() == sMainThreadId) {
        if (sIndex < Constants.BUFFER_SIZE) {
            mergeData(methodId, sIndex, false);
        } else {
            sIndex = 0;
            mergeData(methodId, sIndex, false);
        }
        ++sIndex;
    }
}
```



#### at()方法

在activity的onWindowFocusChange时候回调。

```java
public static void at(Activity activity, boolean isFocus) {
    String activityName = activity.getClass().getName();
    if (isFocus) {
        if (sFocusActivitySet.add(activityName)) {
            synchronized (listeners) {
                for (IAppMethodBeatListener listener : listeners) {
                     //回调给startupTrace用来计算activity的启动耗时
                    listener.onActivityFocused(activity);
                }
            }
            MatrixLog.i(TAG, "[at] visibleScene[%s] has %s focus!", getVisibleScene(), "attach");
        }
    } else {
        if (sFocusActivitySet.remove(activityName)) {
            MatrixLog.i(TAG, "[at] visibleScene[%s] has %s focus!", getVisibleScene(), "detach");
        }
    }
}
```







#### IndexRecord 内部类

```java
public static final class IndexRecord {
    public int index;
    private IndexRecord next; // 链表
    public boolean isValid = true;
    public String source;
}
```



#### maskIndex()方法

```java
public IndexRecord maskIndex(String source) {
    // 链表为空，则新建一个节点。application调用的第一次肯定是null
    if (sIndexRecordHead == null) {
        //index是数组中的下标。
        sIndexRecordHead = new IndexRecord(sIndex - 1);
        sIndexRecordHead.source = source;
        return sIndexRecordHead;
    } else {
        IndexRecord indexRecord = new IndexRecord(sIndex - 1);
        indexRecord.source = source;
        IndexRecord record = sIndexRecordHead;
        IndexRecord last = null;
        while (record != null) {
            if (indexRecord.index <= record.index) {
                if (null == last) {
                    // 新节点插到链表头部
                    IndexRecord tmp = sIndexRecordHead;
                    sIndexRecordHead = indexRecord;
                    indexRecord.next = tmp;
                } else {
                    // 把新节点插到last和 next节点的中间。
                    IndexRecord tmp = last.next;
                    last.next = indexRecord;
                    indexRecord.next = tmp;
                }
                return indexRecord;
            }
            last = record;
            record = record.next;
        }
        last.next = indexRecord;
        return indexRecord;
    }
}
```



#### copyData()方法

```java
public long[] copyData(IndexRecord startRecord) {
    // startRecord：之前标记的indexRecord。
    // endRecord: 当前indexRecord。
    return copyData(startRecord, new IndexRecord(sIndex - 1));
}

// 通过开始节点和结束节点，从buffer数组中获取调用链信息。
private long[] copyData(IndexRecord startRecord, IndexRecord endRecord) {
    long current = System.currentTimeMillis();
    long[] data = new long[0];
    try {
        if (startRecord.isValid && endRecord.isValid) {
            int length;
            int start = Math.max(0, startRecord.index);
            int end = Math.max(0, endRecord.index);

            if (end > start) {
                length = end - start + 1;
                data = new long[length];
                System.arraycopy(sBuffer, start, data, 0, length);
            } else if (end < start) {
                length = 1 + end + (sBuffer.length - start);
                data = new long[length];
                System.arraycopy(sBuffer, start, data, 0, sBuffer.length - start);
                System.arraycopy(sBuffer, 0, data, sBuffer.length - start, end + 1);
            }
            return data;
        }
        return data;
    } catch (Throwable t) {
        MatrixLog.e(TAG, t.toString());
        return data;
    } finally {
        MatrixLog.i(TAG, "[copyData] [%s:%s] length:%s cost:%sms", Math.max(0, startRecord.index), endRecord.index, data.length, System.currentTimeMillis() - current);
    }
}
```







### TraceDataUtils

TODO

> structuredDataToStack

```java
public static void structuredDataToStack(long[] buffer, LinkedList<MethodItem> result, boolean isStrict, long endTime) {
    long lastInId = 0L;
    int depth = 0;
    LinkedList<Long> rawData = new LinkedList<>();
    boolean isBegin = !isStrict;

    for (long trueId : buffer) {
        if (0 == trueId) {
            continue;
        }
        if (isStrict) {
            if (isIn(trueId) && AppMethodBeat.METHOD_ID_DISPATCH == getMethodId(trueId)) {
                isBegin = true;
            }

            if (!isBegin) {
                MatrixLog.d(TAG, "never begin! pass this method[%s]", getMethodId(trueId));
                continue;
            }

        }
        if (isIn(trueId)) {
            lastInId = getMethodId(trueId);
            if (lastInId == AppMethodBeat.METHOD_ID_DISPATCH) {
                depth = 0;
            }
            depth++;
            rawData.push(trueId);
        } else {
            int outMethodId = getMethodId(trueId);
            if (!rawData.isEmpty()) {
                long in = rawData.pop();
                depth--;
                int inMethodId;
                LinkedList<Long> tmp = new LinkedList<>();
                tmp.add(in);
                while ((inMethodId = getMethodId(in)) != outMethodId && !rawData.isEmpty()) {
                    MatrixLog.w(TAG, "pop inMethodId[%s] to continue match ouMethodId[%s]", inMethodId, outMethodId);
                    in = rawData.pop();
                    depth--;
                    tmp.add(in);
                }

                if (inMethodId != outMethodId
                        && inMethodId == AppMethodBeat.METHOD_ID_DISPATCH) {
                    MatrixLog.e(TAG, "inMethodId[%s] != outMethodId[%s] throw this outMethodId!", inMethodId, outMethodId);
                    rawData.addAll(tmp);
                    depth += rawData.size();
                    continue;
                }

                long outTime = getTime(trueId);
                long inTime = getTime(in);
                long during = outTime - inTime;
                if (during < 0) {
                    MatrixLog.e(TAG, "[structuredDataToStack] trace during invalid:%d", during);
                    rawData.clear();
                    result.clear();
                    return;
                }
                MethodItem methodItem = new MethodItem(outMethodId, (int) during, depth);
                addMethodItem(result, methodItem);
            } else {
                MatrixLog.w(TAG, "[structuredDataToStack] method[%s] not found in! ", outMethodId);
            }
        }
    }

    while (!rawData.isEmpty() && isStrict) {
        long trueId = rawData.pop();
        int methodId = getMethodId(trueId);
        boolean isIn = isIn(trueId);
        long inTime = getTime(trueId) + AppMethodBeat.getDiffTime();
        MatrixLog.w(TAG, "[structuredDataToStack] has never out method[%s], isIn:%s, inTime:%s, endTime:%s,rawData size:%s",
                methodId, isIn, inTime, endTime, rawData.size());
        if (!isIn) {
            MatrixLog.e(TAG, "[structuredDataToStack] why has out Method[%s]? is wrong! ", methodId);
            continue;
        }
        MethodItem methodItem = new MethodItem(methodId, (int) (endTime
                - inTime), rawData.size());
        addMethodItem(result, methodItem);
    }
    TreeNode root = new TreeNode(null, null);
    int count = stackToTree(result, root);
    MatrixLog.i(TAG, "stackToTree: count=%s", count);
    result.clear();
    treeToStack(root, result);
}
```







## 方案:

从监控主线程的实现原理上，主要分为两种：

- 依赖主线程 Looper，监控每次 dispatchMessage 的执行耗时。（BlockCanary）
- 依赖 Choreographer 模块，监控相邻两次 Vsync 事件通知的时间差。（ArgusAPM、LogMonitor）



第一种方案

```java
public static void loop() {
    ...
    for (;;) {
        ...
        // This must be in a local variable, in case a UI event sets the logger
        Printer logging = me.mLogging;
        if (logging != null) {
            logging.println(">>>>> Dispatching to " + msg.target + " " +
                    msg.callback + ": " + msg.what);
        }
        msg.target.dispatchMessage(msg);
        if (logging != null) {
            logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
        }
        ...
    }
}
```



第二种方案，利用系统 Choreographer 模块，向该模块注册一个 FrameCallback 监听对象，同时通过另外一条线程循环记录主线程堆栈信息，并在每次 Vsync 事件 doFrame 通知回来时，循环注册该监听对象，间接统计两次 Vsync 事件的时间间隔，当超出阈值时，取出记录的堆栈进行分析上报。

```java
Choreographer.getInstance().postFrameCallback(new Choreographer.FrameCallback() {
    @Override    
    public void doFrame(long frameTimeNanos) {
        if(frameTimeNanos - mLastFrameNanos > 100) {
            ...
        }
        mLastFrameNanos = frameTimeNanos;
        Choreographer.getInstance().postFrameCallback(this);
    }
});
```





这两种方案，可以较方便的捕捉到卡顿的堆栈，但其最大的不足在于，无法获取到各个函数的执行耗时，对于稍微复杂一点的堆栈，很难找出可能耗时的函数，也就很难找到卡顿的原因。另外，通过其他线程循环获取主线程的堆栈，如果稍微处理不及时，很容易导致获取的堆栈有所偏移，不够准确，加上没有耗时信息，卡顿也就不好定位。

所以我们希望寻求一种可以在线上准确地捕捉卡顿堆栈，又能计算出各个函数执行耗时的方案。而要计算函数的执行耗时，最关键的点在于如何对执行过程中的函数进行打点监控。

这里介绍两种方式：

1. 在应用启动时，默认打开 Trace 功能（Debug.startMethodTracing），应用内所有函数在执行前后将会经过该函数（dalvik 上 dvmMethodTraceAdd 函数 或  art 上 Trace::LogMethodTraceEvent 函数），通过hack手段代理该函数，在每个执行方法前后进行打点记录。
2. 修改字节码的方式，在编译期修改所有 class 文件中的函数字节码，对所有函数前后进行打点插桩。

第一种方案，最大的好处是能统计到包括系统函数在内的所有函数出入口，对代码或字节码不用做任何修改，所以对apk包的大小没有影响，但由于方式比较hack，在兼容性和安全性上存在一定的风险。

第二种方案，利用 Java 字节码修改工具（如 BCEL、ASM、Javassis等），在编译期间收集所有生成的 class 文件，扫描文件内的方法指令进行统一的打点插桩，同样也可以高效的记录函数执行过程中的信息，相比第一种方案，除了无法统计系统内执行的函数，其它应用内实现的函数都可以覆盖到。而往往造成卡顿的函数并不是系统内执行的函数，一般都是我们应用开发实现的函数，所以这里无法统计系统内执行的函数对卡顿的定位影响不大。此方案无需 hook 任何函数，所以在兼容性方面会比第一个方案更可靠。

在这考虑上，我们最终选择了修改字节码的方案，来实现 Matrix-TraceCannary 模块，解决其它方案中卡顿堆栈无耗时信息的主要问题，来帮助开发者发现及定位卡顿问题。





## 参考资料:

https://cloud.tencent.com/developer/article/1382771







## 问题:

stackkey是什么 ，唯一标识

为什么要用SystemClock

SystemClock.currentThreadTimeMillis()

该方法会返回当前线程处于running状态的毫秒时间。

SystemClock.uptimeMillis()



如何获取方法的开始节点和结束节点

