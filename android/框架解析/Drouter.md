# Drouter

## 跨进程原理分析

使用流程

```kotlin
val service: HelloDService = DRouter.build(HelloDService::class.java)
    .setRemote(Strategy("com.cq.test"))
    .getService()
Log.e("test", "onCreate: ${service.str}")
```



**客户端：**

getService流程

```java
// strategy != null 获取RemoteBridge
IRemoteBridge bridge = DRouter.build(IRemoteBridge.class).getService();
// 通过RemoteBridge获取自定义Service
bridge.getService(strategy, lifecycle, function, alias, feature, constructors);
// 动态代理创建实际service(创建完成后，服务端就经常执行onTransact toStirng )
return (T) Proxy.newProxyInstance(getClass().getClassLoader(),
        new Class[] {serviceClass}, new Handler(serviceClass, alias, feature, constructor));
```

动态代理invoke函数

```java
public Object invoke(Object proxy, Method method, @Nullable Object[] args) {
    final StreamCmd command = new StreamCmd();
	...
	// 执行
    StreamResult result = execute(command);
    if (result != null && StreamResult.SUCCESS.equals(result.state)) {
		return result.result;
    } else {
    	...
    }
}
```

执行自定义服务函数service.getStr

```java
// 动态代理->invoke
execute(command);
// 获取服务端的AIDL服务
IHostService hostService = RemoteProvider.getHostService(strategy.authority);
	// 有缓存
	IHostService hostService = sHostServiceMap.get(authority);
	// 无缓存
	// 获取ContentProvider
	client = DRouter.getContext().getContentResolver()
        .acquireUnstableContentProviderClient(authority);
	bundle = client.call("", "", null); // 调用服务端的call，返回bundle
		
	// 获取的是服务端的 stub
	BinderParcel parcel = bundle.getParcelable(RemoteProvider.FIELD_REMOTE_BINDER);
	// 此处获取的是服务端的进程名
    sProcessMap.put(authority, bundle.getString(FIELD_REMOTE_PROCESS)); 
	// 获取服务
	hostService = IHostService.Stub.asInterface(parcel.getBinder());
	// 缓存服务
	sHostServiceMap.put(authority, hostService);

// 最终调到AIDL生成的文件
result = hostService.call(command);
// transact发送
boolean _status = this.mRemote.transact(1, _data, _reply, 0);
```



服务端：

RemoteProvider call 定义

```java
RemoteProvider extends ContentProvider{
   @Override
    public Bundle call(String method, String arg, Bundle extras) {
        Bundle bundle = new Bundle();
        bundle.putParcelable(FIELD_REMOTE_BINDER, new BinderParcel(stub));
        bundle.putString(FIELD_REMOTE_PROCESS, ProcUtil.getProcessName());
        return bundle;
    }
}
```



stub定义

```java
private static final IHostService.Stub stub = new IHostService.Stub() {
    @Override
    public StreamResult call(StreamCmd command) {
        try {
            ProcChecker.checkApplication();
            return new CmdDispatcher().call(command);
        } catch (RuntimeException e) {
            throw e;
        }
    }
    @Override
    public void callAsync(StreamCmd command) {
        call(command);
    }
};
```

onTransact接收

```java
StreamResult _result = this.call(_arg0);
```

调用到stub的call

```java
return new CmdDispatcher().call(command);
```

call流程

```java
// 自定义服务Loader
ServiceLoader<?> loader = DRouter.build(command.serviceClass);
// 获取自定义服务
Object instance = loader.getService(command.constructorArgs);
	T target = (T) getServiceInstance(meta, constructors);
		// 有缓存
		Object t = sSingletonInstanceMap.get(implClass);
		// 无缓存
		// 调用apt生成的newInstance方法
        meta.getRouterProxy().newInstance(null) : null;	

// 反射调用
result.result = ReflectUtil.invokeMethod(instance, command.methodName, command.methodArgs);

return result;
```



总结：

客户端调用服务端方法：
客户端通过contentprovider获取到服务端的Binder，生成AIDL接口服务
通过跨进程通信，调用服务端的方法，在返回结果





## 各个类的作用

**RemoteBridge作用：**
动态代理创建服务
执行指令，返回结果

如果是跨进程就会通过RemoteBridge来动态代理创建服务,否则就调用APT生成的类来创建服务

**RemoteProvider作用：**
所有自定义ContentProvider 继承自 RemoteProvider作用
跨进程传输binder
创建和缓存主服务(IHostService)
getHostService 获取 IHostService AIDL服务

**IHostService 作用:**
AIDL生成的文件
客户端和服务端通信

**BinderParcel作用:**
IBinder的包装

**Binder的实现：**

```java
private static final IHostService.Stub stub = new IHostService.Stub() {
    @Override
    public StreamResult call(StreamCmd command) {
        return new CmdDispatcher().call(command);
    }
};
```

**ServiceAgent 服务代理 作用：**
获取服务





## apt生成的类参考

```java
// Package com.didi.drouter.loader.host.ServiceLoader.load;
public void ServiceLoader.load(Map p0)	
{

   this.put(HelloDService.class, 
            RouterMeta.build(RouterMeta.SERVICE).assembleService(HelloDServiceImpl.class, 
            new com_edu_check_drouter_HelloDServiceImpl(), 
            "", null, 5, 2), p0);
   return;
}
```

```java
// package: com.didi.drouter.proxy.com_edu_check_drouter_HelloDServiceImpl
class com_edu_check_drouter_HelloDServiceImpl extends Object implements IRouterProxy	
{
	  /*direct methods:*/
      public void <init>();
      /*virtual methods:*/
      public Object callMethod(Object p0,String p1,Object[] p2);
      public Object newInstance(Context p0);
}
```

```java
public Object com_edu_check_drouter_HelloDServiceImpl.callMethod(Object p0,String p1,Object[] p2)	
{
   p0 = new IRouterProxy$RemoteMethodMatchException();
   throw p0;
}
```

```java
public Object com_edu_check_drouter_HelloDServiceImpl.newInstance(Context p0)
{
   p0 = new HelloDServiceImpl();
   return p0;
}
```





aidl
IHostService.aidl
StreamCmd.aidl
StreamResult.aidl



```java
interface IHostService {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    StreamResult call(in StreamCmd command);

    oneway void callAsync(in StreamCmd command);
}
```

