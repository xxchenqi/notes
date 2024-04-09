IPC框架设计思路

1.服务端提供接口与服务实现,通过注解提供serviceId

2.服务端只需要暴露服务接口给其他进程使用，所以服务端只需要调用框架的注册接口，对服务实现进行注册。(***注册的是服务实现，而不是服务接口***)

通过反射获得Class上的`ServiceId`即可记录**服务表**。同时利用反射获得Class中所有的public Method即可记录**方法表**。

3.框架内部提供了多个预留的Service，通信Service会返回一个AIDL生成的Binder类对象，客户端使用`send`方法向服务端发起请求。

4.客户端需要先与服务端建立连接，因此框架中提供了`connect`方法，内部封装`bindService`实现与服务端通信Service（`IPCService`）的绑定。

5.绑定成功后，客户端就可以获得服务端通信Service提供的`IIPCService`对象，客户端调用`IIPCService#send`发起请求

当我们需要获得`Location`。则应该调用`LocationManager.getDefault().getLocation()`。这句调用会需要执行`LocationManager`的两个方法：`getDefault`与`getLocation`。

服务端执行完`getDefault()`之后，框架会根据`ServiceId`保存这个单例对象。
当执行`getLocation`时，就可以根据`ServiceId`获得这个单例对象

我们可以利用动态代理，在客户端创建一个**"假的"**服务接口对象（代理）。

当我们执行这个代理对象的方法(`getLocation`)时，会回调`IPCInvocationHandler#invoke`方法，在这个方法中框架会向服务端发起请求：`IIPCService#send`

而`getLocation`会返回一个`Location`记录定位信息的对象，这个对象会被服务端json序列化发送过来，因此，客户端只需要在此处获得`Method`的返回类型并反序列化即可。



![1594368615162](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1594368615162.png)