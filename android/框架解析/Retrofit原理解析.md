# Retrofit原理解析



## 初始化:

调用retorfit.build会去创建retorfit，初始化，赋值，创建主线程池等

retorfit.create(API.class)，创建API接口的动态代理对象，

调用API里的方法就会调用到动态代理对象的invoke方法里。





## invoke方法:

1.获取调用方法

获取adapterFactories适配器工厂(转换对象的工厂)

通过addCallAdapterFactory方法会添加到adapterFactories里

```
.addCallAdapterFactory(RxJava2CallAdapterFactory.createWithScheduler(Schedulers.io()))
```

无添加的话默认会调用ExecutorCallAdapterFactory.get()

get()->创建newCallAdapter,里面主要有2个方法

(1)返回真实数据类型

(2)创建ExecutorCallbackCall,将call回调到ui线程



2.解析方法里的地址参数等信息



## enqueue方法:

执行enqueue会去调用到ExecutorCallbackCall的enqueue方法，然后会调用到OkHttpCall的enqueue，

返回成功后解析返回数据，将返回数据转换成特定的数据(初始化时定义的转换器)

