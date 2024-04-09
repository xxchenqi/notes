# ANR

### ANR类型

#### KeyDispatchTimeout（常见）

input事件在5S内没有处理完成发生了ANR。
logcat日志关键字：Input event dispatching timed out

#### BroadcastTimeout

前台Broadcast：onReceiver在10S内没有处理完成发生ANR。
后台Broadcast：onReceiver在60s内没有处理完成发生ANR。
logcat日志关键字：Timeout of broadcast BroadcastRecord

根据发送广播sendBroadcast(Intent intent)中的intent的flflags是否包含FLAG_RECEIVER_FOREGROUND来决定把该 广播是放入前台广播队列或者后台广播队列，默认情况下广播是放入后台广播队列，除非指明加上FLAG_RECEIVER_FOREGROUND标识。 



#### ServiceTimeout

前台Service：onCreate，onStart，onBind等生命周期在20s内没有处理完成发生ANR。
后台Service：onCreate，onStart，onBind等生命周期在200s内没有处理完成发生ANR
logcat日志关键字：Timeout executing service

#### ContentProviderTimeout

ContentProvider 在10S内没有处理完成发生ANR。 
logcat日志关键字：timeout publishing content providers



### anr目录:

/data/anr/traces.txt

文件:

traces_*.txt   :一般是firstPid，即发生anr的pid

traces_SystemServer_WDT.txt(watchdog)：Watchdog中实现，system_server进程

traces.txt（dalvik.vm.stack-trace-file）：系统定义的默认trace文件路径
    





### ANR监控方案1-watchdog 

创建自定义线程，循环检测，方法执行时间是否超时或者未完成

循环里post一个anr检测任务到主线程的消息队列，post之前记录开始时间和完成标志

然后调用wait方法传入等待的时长，让线程处于等待的状态

等待完成后，检测anr任务执行情况，执行时间是否超时和完成标志的状态。

如果有异常，打印日志，上传，如果没有就继续循环。

需要任务结束，发送interrupt



```java
public class ANRWatchDog extends Thread {

    private static final String TAG = "ANR";
    private int timeout = 5000;
    private boolean ignoreDebugger = true;

    static ANRWatchDog sWatchdog;

    private Handler mainHandler = new Handler(Looper.getMainLooper());


    private class ANRChecker implements Runnable {

        private boolean mCompleted;
        private long mStartTime;
        private long executeTime = SystemClock.uptimeMillis();

        @Override
        public void run() {
            synchronized (ANRWatchDog.this) {
                mCompleted = true;
                executeTime = SystemClock.uptimeMillis();
            }
        }

        void schedule() {
            mCompleted = false;
            mStartTime = SystemClock.uptimeMillis();
            mainHandler.postAtFrontOfQueue(this);
        }

        boolean isBlocked() {
            return !mCompleted || executeTime - mStartTime >= 5000;
        }
    }

    public interface ANRListener {
        void onAnrHappened(String stackTraceInfo);
    }

    private ANRChecker anrChecker = new ANRChecker();

    private ANRListener anrListener;

    public void addANRListener(ANRListener listener){
        this.anrListener = listener;
    }

    public static ANRWatchDog getInstance(){
        if(sWatchdog == null){
            sWatchdog = new ANRWatchDog();
        }
        return sWatchdog;
    }

    private ANRWatchDog(){
        super("ANR-WatchDog-Thread");
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    @Override
    public void run() {
        Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND); // 设置为后台线程
        while(true){
            while (!isInterrupted()) {
                synchronized (this) {
                    anrChecker.schedule();
                    long waitTime = timeout;
                    long start = SystemClock.uptimeMillis();
                    while (waitTime > 0) {//预防假唤醒
                        try {
                            wait(waitTime);
                        } catch (InterruptedException e) {
                            Log.w(TAG, e.toString());
                        }
                        waitTime = timeout - (SystemClock.uptimeMillis() - start);
                    }
                    if (!anrChecker.isBlocked()) {
                        continue;
                    }
                }
                if (!ignoreDebugger && Debug.isDebuggerConnected()) {
                    continue;
                }
                String stackTraceInfo = getStackTraceInfo();
                if (anrListener != null) {
                    anrListener.onAnrHappened(stackTraceInfo);
                }
            }
            anrListener = null;
        }
    }

    private String getStackTraceInfo() {
        StringBuilder stringBuilder = new StringBuilder();
        for (StackTraceElement stackTraceElement : Looper.getMainLooper().getThread().getStackTrace()) {
            stringBuilder
                    .append(stackTraceElement.toString())
                    .append("\r\n");
        }
        return stringBuilder.toString();
    }
}
```





### ANR监控方案2-FileObserver

Android系统在此基础上封装了一个FileObserver类来方便使用Inotify机制。FileObserver是一个抽象类，需要定义子类实现该类的onEvent抽象方法，当被监控的文件或者目录发生变更事件时，将回调FileObserver的onEvent()函数来处理文件或目录的变更事件



FileObserver在5.0之后会被selinux挡住



### trace解析

如果 CPU 使用量接近 100%，说明当前设备很忙，有可能是 CPU 饥饿导致了 ANR

如果 CPU 使用量很少，说明主线程被 BLOCK 了

如果 IOwait 很高，说明 ANR 有可能是主线程在进行 I/O 操作造成的



案例 1：关键词:ContentResolver in AsyncTask onPostExecute, high iowait

**100%TOTAL: 6.9% user + 8.2% kernel +84%iowait**

原因：IOWait 很高，说明当前系统在忙于 I/O，因此数据库操作被阻塞



案例 2：关键词:在 UI 线程进行网络数据的读写

关于网络连接，在设计的时候可以设置个 timeout 的时间或者放入独立的线程来处理。



案例 3：
关键词：Memoryleak/Thread leak

分析：
atdalvik.system.VMRuntime.trackExternalAllocation(NativeMethod)内存不足导致 block
在创建 bitmap 上
**MEMINFO in pid 1360 [android.process.acore] **
native dalvik other total
size: 17036 23111 N/A 40147
allocated: 16484 20675 N/A 37159
free: 296 2436 N/A 2732
解决：如果机器的内存族，可以修改虚拟机的内存为 36M 或更大，不过最好是复查代码，查看哪些内存没有释放



内存不足的时候不一定第一时间导致oom,因为当发现内存不足的时候，首先产生的现象是导致gc频率增大，从而可能导致anr