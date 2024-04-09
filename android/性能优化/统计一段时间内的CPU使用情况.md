# 统计一段时间内的CPU使用情况



## 文章内容

```
// 获取 CPU 核心数
cat /sys/devices/system/cpu/possible  

// 获取某个 CPU 的频率
cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq

卡顿问题分析指标
proc/self/stat:
  utime:       用户时间，反应用户代码执行的耗时  
  stime:       系统时间，反应系统调用执行的耗时
  majorFaults：需要硬盘拷贝的缺页次数
  minorFaults：无需硬盘拷贝的缺页次数
  nice:        在低优先级用户模式下耗时
  system:      系统耗时
  idle:		   空闲时执行任务的耗时
  iowait:	   io等待时间
  irq:         硬中断耗时
  softirq:     软中断耗时


如果 CPU 使用率长期大于 60% ，表示系统处于繁忙状态，就需要进一步分析用户时间和系统时间的比例。对于普通应用程序，系统时间不会长期高于 30%，如果超过这个值，我们就应该进一步检查是 I/O 过多，还是其他的系统调用问题。


查看 CPU 上下文切换次数
proc/self/sched:
  nr_voluntary_switches：     
  主动上下文切换次数，因为线程无法获取所需资源导致上下文切换，最普遍的是IO。    
  nr_involuntary_switches：   
  被动上下文切换次数，线程被系统强制调度导致上下文切换，例如大量线程在抢占CPU。
  se.statistics.iowait_count：IO 等待的次数
  se.statistics.iowait_sum：  IO 等待的时间
  
  
通过 uptime 命令可以检查 CPU 在 1 分钟、5 分钟和 15 分钟内的平均负载。
00:02:39 up 7 days, 46 min,  0 users,  
load average: 13.91, 14.70, 14.32

比如一个 4 核的 CPU，如果当前平均负载是 8，这意味着每个 CPU 上有一个线程在运行，还有一个线程在等待。一般平均负载建议控制在“0.7 × 核数”以内。



当发现应用的某个进程 CPU 使用率比较高的时候，可以通过下面几个文件检查该进程下各个线程的 CPU 使用率，继而统计出该进程各个线程的时间占比。
/proc/[pid]/stat             // 进程CPU使用情况
/proc/[pid]/task/[tid]/stat  // 进程下面各个线程的CPU使用情况
/proc/[pid]/sched            // 进程CPU调度相关
/proc/loadavg                // 系统平均负载，uptime命令对应文件
```



```
CPU核数
可以通过文件/proc/cpuinfo获得

CPU Core: 8

负载
系统通过/proc/loadavg获得

Load Average: 8.74 / 7.74 / 7.36

各线程负载
从/proc/[pid]/stat可以获得进程CPU使用情况
从/proc/[pid]/task/[tid]/stat可以获得进程下面各个线程的CPU使用情况
```







## 监控

```java
// mJiffyMillis = 10

//开机到现在的时间(不包括CPU休眠时间)
long nowUptime = SystemClock.uptimeMillis();
//开机到现在的时间(包括CPU休眠时间)
long nowRealtime = SystemClock.elapsedRealtime();
//当前时间
long nowWallTime = System.currentTimeMillis();
//查看CPU使用情况,只读1行
String[] sysCpu = readProcFile("/proc/stat")
/*
cpu  1322 174 2247 222834 517 2 191 0 0 0

1322 = SYSTEM_STATS_USER_TIME
174 = SYSTEM_STATS_NICE_TIME
2247 = SYSTEM_STATS_SYS_TIME
222834 = SYSTEM_STATS_IDLE_TIME
517 = SYSTEM_STATS_IOWAIT_TIME
2 = SYSTEM_STATS_IRQ_TIME
191 = SYSTEM_STATS_SOFT_IRQ_TIME


cpu0 363 38 620 55583 181 0 26 0 0 0
cpu1 382 39 548 55731 136 0 34 0 0 0
cpu2 285 58 549 55896 78 0 28 0 0 0
cpu3 292 39 530 55624 122 2 103 0 0 0
intr 182190 17 1 0 0 0 0 0 0 1 0 0 0 1 0 0 0 457 0 0 694 629 142 9844 33 8661 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 365849
btime 1662563226
processes 4003
procs_running 4
procs_blocked 0
softirq 44717 3 12169 0 3474 8308 0 467 10932 86 9278
*/
    
long usertime = (SYSTEM_STATS_USER_TIME + SYSTEM_STATS_NICE_TIME) * mJiffyMillis;
long systemtime = SYSTEM_STATS_SYS_TIME * mJiffyMillis;
long idletime = SYSTEM_STATS_IDLE_TIME * mJiffyMillis;
long iowaittime = SYSTEM_STATS_IOWAIT_TIME * mJiffyMillis;
long irqtime = SYSTEM_STATS_IRQ_TIME * mJiffyMillis;
long softirqtime = SYSTEM_STATS_SOFT_IRQ_TIME * mJiffyMillis;

mRelUserTime = (int) (usertime - mBaseUserTime);
mRelSystemTime = (int) (systemtime - mBaseSystemTime);
mRelIoWaitTime = (int) (iowaittime - mBaseIoWaitTime);
mRelIrqTime = (int) (irqtime - mBaseIrqTime);
mRelSoftIrqTime = (int) (softirqtime - mBaseSoftIrqTime);
mRelIdleTime = (int) (idletime - mBaseIdleTime);
mRelStatsAreGood = true;
mBaseUserTime = usertime;
mBaseSystemTime = systemtime;
mBaseIoWaitTime = iowaittime;
mBaseIrqTime = irqtime;
mBaseSoftIrqTime = softirqtime;
mBaseIdleTime = idletime;

//最近一次采样时间
mLastSampleTime = mCurrentSampleTime;
//当前采样时间
mCurrentSampleTime = nowUptime;
mLastSampleRealTime = mCurrentSampleRealTime;
mCurrentSampleRealTime = nowRealtime;
mLastSampleWallTime = mCurrentSampleWallTime;
mCurrentSampleWallTime = nowWallTime;

//设置名称  cat /proc/4232/cmdline -> com.sample.processtracker
getName(mCurrentProcStat, mCurrentProcStat.cmdlineFile);
//收集进程状态
collectProcsStats("/proc/self/stat", mCurrentProcStat);

//收集线程信息 ls /proc/4232/task -> 4232  4235  4237  4239  4240  4241  4242  4243  4244  4245  4246  4248  4252  4253
File[] threadsProcFiles = new File(mCurrentProcStat.threadsDir).listFiles();
for (File thread : threadsProcFiles) {
    //获取线程id
    int threadID = Integer.parseInt(thread.getName());
    
    Stats threadStat = findThreadStat(threadID, mCurrentProcStat.workingThreads);
    if (threadStat == null) {
        threadStat = new Stats(threadID, true);
        //获取线程名称 /proc/self/task/4232/comm
        getName(threadStat, threadStat.cmdlineFile);
        mCurrentProcStat.workingThreads.add(threadStat);
    }
    //收集线程状态信息 cat /proc/self/task/4232/stat
    collectProcsStats(threadStat.statFile, threadStat);
}
//排序 
/*
int ta = sta.rel_utime + sta.rel_stime;
int tb = stb.rel_utime + stb.rel_stime;
if (ta != tb) {
    return ta > tb ? -1 : 1;
}
*/
Collections.sort(mCurrentProcStat.workingThreads, sLoadComparator);

// 查看平均负载 cat /proc/loadavg
// 0.00 0.01 0.05 2/864 4357
final String[] loadAverages = readProcFile("/proc/loadavg");
if (loadAverages != null) {
    //平均1分钟负载
    float load1 = 0.00;
    //平均5分钟负载
    float load5 = 0.01;
    //平均15分钟负载
    float load15 = 0.05;
    if (load1 != mLoad1 || load5 != mLoad5 || load15 != mLoad15) {
        mLoad1 = load1;
        mLoad5 = load5;
        mLoad15 = load15;
    }
}

```





```java
collectProcsStats("/proc/self/stat", mCurrentProcStat);
/*
4232 (.processtracker)
S 1885 1885 0 0 -1 1077944640 3678 0 0 0 13 22 0 0 10 -10 14 0 73898 991551488 23055 4294967295 3615051776 3615065588 3754974960 3754958744 3614366912 0 4612 0 34040 4294967295 0 0 17 1 0 0 0 0 0 3615071564 3615072256 3636305920 3754982448 
3754982524 3754982524 3754983396 0
*/
//[0]
String status = S;
//[7]
long minfaults = 3678 ;
//[9]
long majfaults = 0;
//[11] * mJiffyMillis
long utime = 13 * mJiffyMillis;
//[12] * mJiffyMillis
long stime = 22 * mJiffyMillis;

//开机到现在的时间(不包括CPU休眠时间)
long uptime = SystemClock.uptimeMillis();
st.rel_uptime = uptime - st.base_uptime;
st.base_uptime = uptime;
st.rel_utime = (int) (utime - st.base_utime);
st.rel_stime = (int) (stime - st.base_stime);
st.base_utime = utime;
st.base_stime = stime;
st.rel_minfaults = (int) (minfaults - st.base_minfaults);
st.rel_majfaults = (int) (majfaults - st.base_majfaults);
st.base_minfaults = minfaults;
st.base_majfaults = majfaults;
st.status = status;
```









## 日志

```
CPU usage from 225ms to 7ms ago (2022-09-06 23:39:58.174 to 2022-09-06 23:39:58.391):
    129% 3983/com.sample.processtracker(R): 32% user + 96% kernel / faults: 12628 minor
    thread stats:
    64% 3994/HeapTaskDaemon(S): 13% user + 50% kernel / faults: 1983 minor
    59% 3983/.processtracker(R): 13% user + 46% kernel / faults: 10315 minor
    0% 4000/RenderThread(S): 0% user + 0% kernel
    0% 3987/Jit thread pool(S): 0% user + 0% kernel / faults: 319 minor
    0% 3988/Signal Catcher(S): 0% user + 0% kernel
    0% 3989/JDWP(S): 0% user + 0% kernel
    0% 3991/ReferenceQueueD(S): 0% user + 0% kernel
    0% 3992/FinalizerDaemon(S): 0% user + 0% kernel / faults: 6 minor
    0% 3993/FinalizerWatchd(S): 0% user + 0% kernel
    0% 3996/Binder:3983_1(S): 0% user + 0% kernel
    0% 3997/Binder:3983_2(S): 0% user + 0% kernel
    0% 3998/Profile Saver(S): 0% user + 0% kernel
    0% 4004/hwuiTask1(S): 0% user + 0% kernel
    0% 4005/hwuiTask2(S): 0% user + 0% kernel
    0% 4006/Binder:3983_3(S): 0% user + 0% kernel
    100% TOTAL(): 6% user + 24% kernel + 1.2% softirq + 68% idle
    Load: 1.37 / 0.36 / 0.13
```



```
CPU usage from 225ms to 7ms ago (2022-09-06 23:39:58.174 to 2022-09-06 23:39:58.391):
225ms = 系统到现在的启动时间 - 最后采样的时间
7ms = 系统到现在的启动时间 - 当前采样的时间
```



```
129% 3983/com.sample.processtracker(R): 32% user + 96% kernel / faults: 12628 minor

pid = st.pid
status = st.status
totalTime = st.rel_uptime
user = st.rel_utime
system = st.rel_stime
iowait = 0
irq = 0
softIrq = 0 
idle = 0
minFaults = st.rel_minfaults
majFaults = st.rel_majfaults

129% =  (user + system + iowait + irq + softIrq + idle) * 1000 / totalTime / 10
3983 是 进程id
(R) 是 状态

32% user = user * 1000 / totalTime / 10
96% kernel = system * 1000 / totalTime / 10

12628 minor = minFaults

```



```
64% 3994/HeapTaskDaemon(S): 13% user + 50% kernel / faults: 1983 minor
59% 3983/.processtracker(R): 13% user + 46% kernel / faults: 10315 minor
0% 4000/RenderThread(S): 0% user + 0% kernel
...

计算过程和上面一样

```



```
100% TOTAL(): 6% user + 24% kernel + 1.2% softirq + 68% idle

totalTime = totalTime
user = mRelUserTime
system = mRelSystemTime
iowait = mRelIoWaitTime
irq = mRelIrqTime
softIrq = mRelSoftIrqTime
idle = mRelIdleTime
minFaults = 0
majFaults = 0

计算过程和上面一样

1.2% softirq = softirq * 1000 / totalTime / 10
68% idle = idle * 1000 / totalTime / 10
```



```
Load: 1.37 / 0.36 / 0.13

1.37 = mLoad1
0.36 = mLoad5
0.13 = mLoad15
```

