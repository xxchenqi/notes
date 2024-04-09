# Eventbus原理解析

![](.\assets\eventbus原理.png)







```java
//key 事件类型
//value 订阅对象集合(订阅对象包含了订阅者和订阅方法)
private final Map<Class<?>, CopyOnWriteArrayList<Subscription>> subscriptionsByEventType;


//key 订阅者
//value 事件类型集合
private final Map<Object, List<Class<?>>> typesBySubscriber;
```



注册

1.通过反射找出这个类的所有订阅方法，保存集合中

2.创建订阅对象将订阅的这个类对象和订阅方法绑定在一起，根据事件类型将订阅对象保存到map里来完成订阅。

然后在通过订阅的这个类对象作为key，事件类型集合作为value存到另一个map，用来注销遍历的。

3.如果是黏性事件，则取出黏性事件对应的事件类型，发送给订阅者





2.发送事件

找到所有事件对应的类型集合，根据事件类型取出订阅对象集合，进行处理，

如果当前是主线程，直接反射调用方法

如果不是，则将事件加入到主线程队列里通过handler，切换到主线程执行。





3.注销

通过订阅对象找到事件类型集合，遍历集合，通过事件类型获取到订阅对象集合，如果相等则移除。

最后在移除该订阅者对应的事件类型集合