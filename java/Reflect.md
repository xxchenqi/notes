# Reflect

## 反射能否修改final

可以修改引用类型，但是基本类型和String要注意，

在编译期间会自动被优化，直接返回具体的值，而不是字段。

所以要通过反射动态获取就不会被优化了。

```java
package com.enjoy.reflect;

import java.lang.reflect.Field;

public class ReflectTest {
    //编译期间final类型的数据自动被优化
    public final int i = 1 ;

    public ReflectTest() {
//            i = 1;
    }

    public int getI() {
        return i;
    }

    public static void main(String[] args) throws Exception {
        ReflectTest reflectTest = new ReflectTest();
        Field i = ReflectTest.class.getDeclaredField("i");
        i.setAccessible(true);
        i.set(reflectTest, 2);

    	// 动态拿
        System.out.println(i.get(reflectTest));
        // 编译时写死
        System.out.println(reflectTest.i);
    }
}

```



## 反射为什么慢

1. Method#invoke 需要进行自动拆装箱

  invoke 方法的参数是 Object[] 类型，如果是基本数据类型会转化为Integer装箱，同时再包装成Object数组。在执行时候又会把数组拆解开，并拆箱为基本数据类型。

2. 反射需要按名检索类和方法

  http://androidxref.com/9.0.0_r3/xref/art/runtime/mirror/class.cc#1265

  遍历所有方法进行匹配

3. 需要检查方法

  反射时需要检查方法可见性以及每个实际参数与形式参数的类型匹配性

4. 编译器无法对动态调用的代码做优化，比如内联

  反射涉及到动态解析的类型，影响内联判断并且无法进行JIT	

  

