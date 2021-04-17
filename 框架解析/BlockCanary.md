# BlockCanary解析

在Looper的loop方法中:

```java
public static void loop() {
	...
    final Printer logging = me.mLogging;
    if (logging != null) {
        logging.println(">>>>> Dispatching to " + msg.target + " " +
                msg.callback + ": " + msg.what);
    }
    ...
    msg.target.dispatchMessage(msg);//观察方法的耗时
    ...
    if (logging != null) {
    	logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
	}
}
```

message有mLogging字段用来打印日志,

我们可以自定义Printer，来监控方法的耗时时间



```java
public class BlockCanary {
    public static void install() {
        LogMonitor logMonitor = new LogMonitor();
        Looper.getMainLooper().setMessageLogging(logMonitor);
    }
}
```



```java
public class LogMonitor implements Printer {
    
    private StackSampler mStackSampler;
    private boolean mPrintingStarted = false;
    private long mStartTimestamp;
    // 卡顿阈值
    private long mBlockThresholdMillis = 3000;
    //采样频率
    private long mSampleInterval = 1000;

    private Handler mLogHandler;

    public LogMonitor() {
        mStackSampler = new StackSampler(mSampleInterval);
        HandlerThread handlerThread = new HandlerThread("block-canary-io");
        handlerThread.start();
        mLogHandler = new Handler(handlerThread.getLooper());
    }

    @Override
    public void println(String x) {
        //从if到else会执行 dispatchMessage，如果执行耗时超过阈值，输出卡顿信息
        if (!mPrintingStarted) {
            //记录开始时间
            mStartTimestamp = System.currentTimeMillis();
            mPrintingStarted = true;
            mStackSampler.startDump();
        } else {
            final long endTime = System.currentTimeMillis();
            mPrintingStarted = false;
            //出现卡顿
            if (isBlock(endTime)) {
                notifyBlockEvent(endTime);
            }
            mStackSampler.stopDump();
        }
    }

    private void notifyBlockEvent(final long endTime) {
        mLogHandler.post(new Runnable() {
            @Override
            public void run() {
                //获得卡顿时主线程堆栈
                List<String> stacks = mStackSampler.getStacks(mStartTimestamp, endTime);
                for (String stack : stacks) {
                    Log.e("block-canary", stack);
                }
            }
        });
    }

    private boolean isBlock(long endTime) {
        return endTime - mStartTimestamp > mBlockThresholdMillis;
    }
}

```

```java
package com.enjoy.example.jank.blockcanary;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class StackSampler {
    public static final String SEPARATOR = "\r\n";
    public static final SimpleDateFormat TIME_FORMATTER =
            new SimpleDateFormat("MM-dd HH:mm:ss.SSS");


    private Handler mHandler;
    private Map<Long, String> mStackMap = new LinkedHashMap<>();
    private int mMaxCount = 100;
    private long mSampleInterval;
    //是否需要采样
    protected AtomicBoolean mShouldSample = new AtomicBoolean(false);

    public StackSampler(long sampleInterval) {
        mSampleInterval = sampleInterval;
        HandlerThread handlerThread = new HandlerThread("block-canary-sampler");
        handlerThread.start();
        mHandler = new Handler(handlerThread.getLooper());
    }

    /**
     * 开始采样 执行堆栈
     */
    public void startDump() {
        //避免重复开始
        if (mShouldSample.get()) {
            return;
        }
        mShouldSample.set(true);

        mHandler.removeCallbacks(mRunnable);
        mHandler.postDelayed(mRunnable, mSampleInterval);
    }

    public void stopDump() {
        if (!mShouldSample.get()) {
            return;
        }
        mShouldSample.set(false);

        mHandler.removeCallbacks(mRunnable);
    }


    public List<String> getStacks(long startTime, long endTime) {
        ArrayList<String> result = new ArrayList<>();
        synchronized (mStackMap) {
            for (Long entryTime : mStackMap.keySet()) {
                if (startTime < entryTime && entryTime < endTime) {
                    result.add(TIME_FORMATTER.format(entryTime)
                            + SEPARATOR
                            + SEPARATOR
                            + mStackMap.get(entryTime));
                }
            }
        }
        return result;
    }

    private Runnable mRunnable = new Runnable() {
        @Override
        public void run() {
            StringBuilder sb = new StringBuilder();
            StackTraceElement[] stackTrace = Looper.getMainLooper().getThread().getStackTrace();
            for (StackTraceElement s : stackTrace) {
                sb.append(s.toString()).append("\n");
            }
            synchronized (mStackMap) {
                //最多保存100条堆栈信息
                if (mStackMap.size() == mMaxCount) {
                    mStackMap.remove(mStackMap.keySet().iterator().next());
                }
                mStackMap.put(System.currentTimeMillis(), sb.toString());
            }

            if (mShouldSample.get()) {
                mHandler.postDelayed(mRunnable, mSampleInterval);
            }
        }
    };

}
```

