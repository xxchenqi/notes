# WMS

WindowManagerService管理窗口的创建、更新和删除，显示顺序 等，

是WindowManager这个管理接品的真正的实现类。它运行在System_server 

进程，作为服务端，客户端（应用程序）通过IPC调用和它进行交互



## WMS启动流程分析

WMS通过SystemServer的其他服务startOtherServices方法中进行初始化,

### WindowManagerService.main

```java
public static WindowManagerService main(final Context context, final InputManagerService im,final boolean haveInputMethods, final boolean showBootMsgs, final boolean onlyCore,
WindowManagerPolicy policy) {
    DisplayThread.getHandler().runWithScissors(() ->
            sInstance = new WindowManagerService(context, im, haveInputMethods, showBootMsgs,onlyCore, policy), 0);
    return sInstance;
}
```

在main方法中在display线程中进行**同步**等待WMS初始化成功





### mActivityManagerService.setWindowManager(wm)

AMS和WMS进行关联



### wm.onInitReady()

initPolicy()初始化策略



### wm.displayReady()  

初始化显示尺寸信息，结束后WMS会根据AMS进行一次configure



### wm.systemReady()  

直接调用mPolicy的systemready方法







## 从Activity启动角度看WMS

### Activity.attach

从handleLaunchActivity->...->performLaunchActivity->...->activity.attach



mWindow = new PhoneWindow  创建Window对象



mWindow.setWindowManager 创建WindowManager对象



mWindowManager = mWindow.getWindowManage





### setContentView

PhoneWindow.setContentView

installDecor

generateDecor

class DecorView extends FrameLayout