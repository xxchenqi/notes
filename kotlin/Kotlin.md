# Kotlin

## 密封类

用来表示某种受到限制的继承结构。换个说法：密封类，是更强大的枚举类。



当我们尝试去判断枚举的“结构相等”和“引用相等”时，结果始终都是 true。而这就代表了，每一个枚举的值，它在内存当中始终都是同一个对象引用。

```kotlin
enum class Human {
    MAN, WOMAN
}

println(Human.MAN == Human.MAN)
println(Human.MAN === Human.MAN)

输出
true
true
```

密封类就可以解决此问题。

```kotlin
sealed class Result<out R> {
    data class Success<out T>(val data: T, val message: String = "") : Result<T>()

    data class Error(val exception: Exception) : Result<Nothing>()

    data class Loading(val time: Long = System.currentTimeMillis()) : Result<Nothing>()
}

fun display(data: Result) = when(data) {
    is Result.Success -> displaySuccessUI(data)
    is Result.Error -> showErrorMsg(data)
    is Result.Loading -> showLoading()
}
```





## kotlin原始类

Kotlin 对基础类型的转换规则：

只要基础类型的变量可能为空，那么这个变量就会被转换成 Java 的包装类型。

反之，只要基础类型的变量不可能为空，那么这个变量就会被转换成 Java 的原始类型。

```kotlin
// kotlin 代码

// 用 val 定义可为空、不可为空的Long，并且赋值
val a: Long = 1L
val b: Long? = 2L

// 用 var 定义可为空、不可为空的Long，并且赋值
var c: Long = 3L
var d: Long? = 4L

// 用 var 定义可为空的Long，先赋值，然后改为null
var e: Long? = 5L
e = null

// 用 val 定义可为空的Long，直接赋值null
val f: Long? = null

// 用 var 定义可为空的Long，先赋值null，然后赋值数字
var g: Long? = null
g = 6L
```

```java
// 反编译后的 Java 代码

long a = 1L;
long b = 2L;

long c = 3L;
long d = 4L;

Long e = 5L;
e = (Long)null;

Long f = (Long)null;

Long g = (Long)null;
g = 6L;
```



## 接口语法的局限性

Kotlin 接口的“默认属性”，本质上并不是一个真正的属性，当它转换成 Java 以后，就变成了一个普通的接口方法

Kotlin 接口的“方法默认实现”,只是在接口当中定义了一个静态内部类“DefaultImpls”，然后将默认实现的代码放到了静态内部类当中去了。

```kotlin
// Kotlin 代码

interface Behavior {
    // 接口内可以有成员属性
    val canWalk: Boolean

    // 接口方法的默认实现
    fun walk() {
        if (canWalk) {
            println(canWalk)
        }
    }
}
```

```java
// 等价的 Java 代码

public interface Behavior {
   // 接口属性变成了方法
   boolean getCanWalk();

   // 方法默认实现消失了
   void walk();

   // 多了一个静态内部类
   public static final class DefaultImpls {
      public static void walk(Behavior $this) {
         if ($this.getCanWalk()) {
            boolean var1 = $this.getCanWalk();
            System.out.println(var1);
         }
      }
   }
}
```



接口使用的细节

```kotlin
// Kotlin 代码

class Man: Behavior {
    override val canWalk: Boolean = true
}
```

```java
// 等价的 Java 代码

public final class Man implements Behavior {
   private final boolean canWalk = true;

   public boolean getCanWalk() {
      // 返回的还是它内部私有的 canWalk 属性，这就跟 Kotlin 当中的逻辑“override val canWalk: Boolean = true”对应上了。
      return this.canWalk;
   }

   public void walk() {
      // 它将执行流程交给了“Behavior.DefaultImpls.walk()”，并将 this 作为参数传了进去。这里的逻辑，就可以跟 Kotlin 接口当中的默认方法逻辑对应上来了。
      Behavior.DefaultImpls.walk(this);
   }
}
```



Kotlin 接口当中的属性，在它被真正实现之前，本质上并不是一个真正的属性。因此，Kotlin 接口当中的属性，它既不能真正存储任何状态，也不能被赋予初始值，因为它本质上还是一个接口方法。





## 有问题的代码

1.错误的写法

```kotlin
class Person(val name: String, var age: Int) {
    val isAdult = age >= 18
}
```

```java
// 对应java
public final class Person {
   private final boolean isAdult;
   @NotNull
   private final String name;
   private int age;

   public final boolean isAdult() {
      return this.isAdult;
   }

   @NotNull
   public final String getName() {
      return this.name;
   }

   public final int getAge() {
      return this.age;
   }

   public final void setAge(int var1) {
      this.age = var1;
   }

   public Person(@NotNull String name, int age) {
      Intrinsics.checkNotNullParameter(name, "name");
      super();
      this.name = name;
      this.age = age;
      this.isAdult = this.age >= 18;
   }
}
```





2.Kotlin 接口的“成员属性”是存在一定的局限性的

接口的属性： 1.不能设置初始值 2.val可以重写get,var的get和set都不能重写

