# 动态代理

执行该方法后会生成字节码文件

```java
Proxy.newProxyInstance(ClassLoader loader, Class<?>[] interfaces,InvocationHandler invocationHandler)
```

Proxy.newProxyInstance()最终会调用到ProxyClassFactory.generateProxy()方法来生成字节码文件(该方法为native方法)

```java
@FastNative
private static native Class<?> generateProxy(String name, Class<?>[] interfaces, ClassLoader loader, Method[] methods,Class<?>[][] exceptions);
```



例子：

```java
    public static void main(String[] args) {
        NDK lance = new Lance();
        // JAVA动态代理 NDK
        /**
         * 1、类加载器
         * 2、要代理的接口
         * 3、回调
         */
        NDK ndk = (NDK) Proxy.newProxyInstance(lance.getClass().getClassLoader(),
                new Class[]{NDK.class}, new ProxyInvokeHandler(lance));
        //ndk = Proxy$0
        ndk.ndk();


        //现在要能否解决UI的问题
//        UI alvin = new Alvin();
//        UI ui = (UI) Proxy.newProxyInstance(alvin.getClass().getClassLoader(),
//                alvin.getClass().getInterfaces(), new ProxyInvokeHandler(alvin));
//        ui.ui();

    }
```



```java
public class ProxyInvokeHandler implements InvocationHandler {

    private Object realObject;

    public ProxyInvokeHandler(Object realObject) {
        this.realObject = realObject;
    }

    /**
     * @param o       代理对象
     * @param method  调用的方法
     * @param objects 方法的参数
     * @return
     * @throws Throwable
     */
    @Override
    public Object invoke(Object o, Method method, Object[] objects) throws Throwable {
        return method.invoke(realObject, objects);
    }
}
```



字节码文件

```java
public final class Proxy$0 extends Proxy implements NDK {
    private static Method m1;
    private static Method m2;
    private static Method m3;
    private static Method m0;

    //这个InvocationHandler就是我们创建时传入的,然后会记录在成员属性h中
    public Proxy$0(InvocationHandler var1) throws  {
        super(var1);
    }

    public final boolean equals(Object var1) throws  {
        try {
            return (Boolean)super.h.invoke(this, m1, new Object[]{var1});
        } catch (RuntimeException | Error var3) {
            throw var3;
        } catch (Throwable var4) {
            throw new UndeclaredThrowableException(var4);
        }
    }

    public final String toString() throws  {
        try {
            return (String)super.h.invoke(this, m2, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    public final void ndk() throws  {
        try {
            //此时会重定向到InvocationHandler的invoke方法
            super.h.invoke(this, m3, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    public final int hashCode() throws  {
        try {
            return (Integer)super.h.invoke(this, m0, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    static {
        try {
            m1 = Class.forName("java.lang.Object").getMethod("equals", Class.forName("java.lang.Object"));
            m2 = Class.forName("java.lang.Object").getMethod("toString");
            m3 = Class.forName("com.enjoy.lib.service.NDK").getMethod("ndk");
            m0 = Class.forName("java.lang.Object").getMethod("hashCode");
        } catch (NoSuchMethodException var2) {
            throw new NoSuchMethodError(var2.getMessage());
        } catch (ClassNotFoundException var3) {
            throw new NoClassDefFoundError(var3.getMessage());
        }
    }
}

```



