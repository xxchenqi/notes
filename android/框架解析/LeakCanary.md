# LeakCanary原理解析



### LeakCanary是如何安装的？

通过ContentProvider特性自动启动。



### LeakCanary检测Activity退出的原理

通过application注册ActivityLifecycleCallbacks，重写onActivityDestroyed，

通过refWatch监控activity



### 如何监控对象是否被回收

Reference对象所引用的对象被GC回收时，该Reference对象将会被加入引用队列中（ReferenceQueue）的队列末尾。

```java
public static void main(String[] args) {
    //引用队列
    ReferenceQueue referenceQueue = new ReferenceQueue();
    Object obj = new Object();
    //把obj放入weakReference，并和一个referenceQueue关联
    //当obj被gc回收后，盛放它的weakReference会被添加与之关联的referenceQueue
    WeakReference weakReference = new WeakReference(obj,referenceQueue);
    System.out.println("盛放obj的weakReference = " + weakReference);
    //把obj置空，让它没有强引用
    obj = null;
    Runtime.getRuntime().gc();//gc，让可以回收的对象回收
    try{
        Thread.sleep(1000);
    }catch (Exception e){}
    Reference findRef = null;
    do{
        findRef = referenceQueue.poll();
        //如果能找到上面的weakReference => 说明它盛放的obj被gc回收了
        System.out.println("findRef = " +findRef + "是否等于上面的weakReference = " + (findRef == weakReference));
    }while(findRef !=null);//把所有放到referenceQueue的引用容器找出来
}
```



### 如何多个对象监控

定义2个Map:

怀疑列表和观察列表。

1. 在onDestroy方法执行watch监控对象。
2. 在没有监控前，先清除观察列表和怀疑列表。
3. 生成唯一的UUID，让观察对象与一个自定义弱引用建立一对一映射关系，并与引用队列queue关联
4. 加入到观察列表
5. 过5秒后去看是否还在观察列表，如果还在，则加入到怀疑列表
6. 如果怀疑列表大于一定个数，启动工作线程调用haha进行可达性分析

















System.gc()：只是发个通知，在合适的时间在gc

Runtime.getRuntime().gc()：立刻gc