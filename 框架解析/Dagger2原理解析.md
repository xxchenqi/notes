# Dagger2原理解析

![](..\res\Dagger2.png)





Dagger2自动生成3个辅助类

DaggerActivityComponent:为程序入口和桥梁，负责初始化 WatchModule_ProvideWatchFactory和Dagger2Activity_MemberInject，并将它们串联起来

WatchModule_ProvideWatchFactory:来生成 Watch 实例

MainActivity_MembersInjector: 将 Watch 实例赋值 给 MainActivity 的成员变量





定义:

module ->提供初始化对象

component->初始化入口桥梁，提供inject注入方法

使用:

DaggerActivityComponent.create().inject(this);



编译后会生成3个辅助类







## 单例原理

```java
public final class DoubleCheck<T> implements Provider<T>, Lazy<T> {

  private static final Object UNINITIALIZED = new Object();
  private volatile Provider<T> provider;
  private volatile Object instance = UNINITIALIZED;

  private DoubleCheck(Provider<T> provider) {
    assert provider != null;
    this.provider = provider;
  }

  @Override
  public T get() {
    Object result = instance;
    if (result == UNINITIALIZED) {
      synchronized (this) {
        result = instance;
        if (result == UNINITIALIZED) {
          result = provider.get();
          instance = reentrantCheck(instance, result);
          provider = null;
        }
      }
    }
    return (T) result;
  }

  public static Object reentrantCheck(Object currentInstance, Object newInstance) {
    boolean isReentrant = !(currentInstance == UNINITIALIZED || currentInstance instanceof MemoizedSentinel);
    if (isReentrant && currentInstance != newInstance) {
      throw new IllegalStateException("Scoped provider was invoked recursively returning "
          + "different results: " + currentInstance + " & " + newInstance + ". This is likely "
          + "due to a circular dependency.");
    }
    return newInstance;
  }

  public static <P extends Provider<T>, T> Provider<T> provider(P delegate) {
    checkNotNull(delegate);
    if (delegate instanceof DoubleCheck) {
      return delegate;
    }
    return new DoubleCheck<T>(delegate);
  }

  public static <P extends Provider<T>, T> Lazy<T> lazy(P provider) {
    if (provider instanceof Lazy) {
      final Lazy<T> lazy = (Lazy<T>) provider;
      return lazy;
    }
    return new DoubleCheck<T>(checkNotNull(provider));
  }
}
```

