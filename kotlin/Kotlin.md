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

注：上面测试有点小问题，实际在方法里写的局部变量var d: Long? = 4L，编译后为int，其他地方(方法外，类外)编译后都是Integer



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





## object：

### 匿名内部类

可以在继承一个抽象类的同时，来实现多个接口。

```kotlin
interface A {
    fun funA()
}

interface B {
    fun funB()
}

abstract class Man {
    abstract fun findMan()
}

fun main() {
    // 这个匿名内部类，在继承了Man类的同时，还实现了A、B两个接口
    val item = object : Man(), A, B{
        override fun funA() {
            // do something
        }
        override fun funB() {
            // do something
        }
        override fun findMan() {
            // do something
        }
    }
}
```

### 单例模式

Kotlin 编译器会将其转换成静态代码块的单例模式。

因为static{}代码块当中的代码，由虚拟机保证它只会被执行一次，因此，它在保证了线程安全的前提下，同时也保证我们的 INSTANCE 只会被初始化一次。

缺点:

1.不支持懒加载

2.不支持传参构造单例

```kotlin
object UserManager {
    fun login() {}
}
```

```java
public final class UserManager {

   public static final UserManager INSTANCE; 

   static {
      UserManager var0 = new UserManager();
      INSTANCE = var0;
   }

   private UserManager() {}

   public final void login() {}
}
```

### 伴生对象

Kotlin 当中没有 static 关键字，所以我们没有办法直接定义静态方法和静态变量。不过，Kotlin 还是为我们提供了伴生对象，来帮助实现静态方法和变量。



在伴生对象的内部，如果存在“@JvmStatic”修饰的方法或属性，它会被挪到伴生对象外部的类当中，变成静态成员。

```kotlin
class Person {
    companion object InnerSingleton {
        @JvmStatic
        fun foo() {}
    }
}
```

```java
public final class Person {

   public static final Person.InnerSingleton InnerSingleton = new Person.InnerSingleton((DefaultConstructorMarker)null);

   // 注意这里
   public static final void foo() {
      InnerSingleton.foo();
   }

   public static final class InnerSingleton {
      public final void foo() {}

      private InnerSingleton() {}

      public InnerSingleton(DefaultConstructorMarker $constructor_marker) {
         this();
      }
   }
}
```

### 工厂模式

```kotlin
//  私有的构造函数，外部无法调用
//            ↓
class User private constructor(name: String) {
    companion object {
        @JvmStatic
        fun create(name: String): User? {
            // 统一检查，比如敏感词过滤
            return User(name)
        }
    }
}
```

### 另外 4 种单例模式的写法

#### 懒加载委托

```kotlin
object UserManager {
    // 对外暴露的 user
    val user by lazy { loadUser() }

    private fun loadUser(): User {
        // 从网络或者数据库加载数据
        return User.create("tom")
    }

    fun login() {}
}
```

#### 伴生对象 Double Check

```kotlin

class UserManager private constructor(name: String) {
    companion object {
        @Volatile private var INSTANCE: UserManager? = null

        fun getInstance(name: String): UserManager =
            // 第一次判空
            INSTANCE?: synchronized(this) {
            // 第二次判空
                INSTANCE?:UserManager(name).also { INSTANCE = it }
            }
    }
}

// 使用
UserManager.getInstance("Tom")
```

#### 抽象类模板

```kotlin
abstract class BaseSingleton<in P, out T> {
    @Volatile
    private var instance: T? = null

    protected abstract fun creator(param: P): T

    fun getInstance(param: P): T =
        instance ?: synchronized(this) {
            instance ?: creator(param).also { instance = it }
    }
}
```

```kotlin
class PersonManager private constructor(name: String) {
    companion object : BaseSingleton<String, PersonManager>() {
        override fun creator(param: String): PersonManager = PersonManager(param)
    }
}

class UserManager private constructor(name: String) {
    companion object : BaseSingleton<String, UserManager>() {
        override fun creator(param: String): UserManager = UserManager(param)
    }
}
```

#### 接口模板(不推荐)

```kotlin
interface ISingleton<P, T> {
    var instance: T?

    fun creator(param: P): T

    fun getInstance(p: P): T =
        instance ?: synchronized(this) {
            instance ?: creator(p).also { instance = it }
        }
}
```

缺点:

instance 无法使用 private 修饰。这是接口特性规定的，而这并不符合单例的规范。正常情况下的单例模式，我们内部的 instance 必须是 private 的，这是为了防止它被外部直接修改。

instance 无法使用 @Volatile 修饰。这也是受限于接口的特性，这会引发多线程同步的问题。



## 扩展

### 扩展函数

```kotlin
fun String.lastElement(): Char? {
    if (this.isEmpty()) {
        return null
    }
    return this[length - 1]
}

// 使用扩展函数
fun main() {
    val msg = "Hello Wolrd"
    val last = msg.lastElement()
}
```

```java
public final class ExtKt {
   public static final Character lastElement(String $this) {
      CharSequence var1 = (CharSequence)$this;
      if (var1.length() == 0) {
        return null
      }

      return  var1.charAt(var1.length() - 1);
   }
}

public static final void main() {
  String msg = "Hello Wolrd";
  Character last = ExtKt.lastElement(msg);
}
```

### 扩展属性

```kotlin
val String.lastElement: Char?
    get() = if (isEmpty()) {
            null
        } else {
            get(length - 1)
        }

fun main() {
    val msg = "Hello Wolrd"
    val last = msg.lastElement 
}
```

反编译后和扩展函数一样，本质上，它还是静态方法。



### 扩展不能做什么？

第一个限制，Kotlin 扩展不是真正的类成员，因此它无法被它的子类重写。

第二个限制，扩展属性无法存储状态。根本原因，还是因为它们都是静态方法。

第三个限制，扩展的访问作用域仅限于两个地方。第一，定义处的成员；第二，接收者类型的公开成员。

- 如果扩展是顶层的扩展，那么扩展的访问域仅限于该 Kotlin 文件当中的所有成员，以及被扩展类型的公开成员，这种方式定义的扩展是可以被全局使用的。
- 如果扩展是被定义在某个类当中的，那么该扩展的访问域仅限于该类当中的所有成员，以及被扩展类型的公开成员，这种方式定义的扩展仅能在该类当中使用。



## 高阶函数

### 函数类型

规律：将函数的“参数类型”和“返回值类型”抽象出来后，就得到了“函数类型”

```
//         (Int,  Int) ->Float 这就是 add 函数的类型
//           ↑     ↑      ↑
fun add(a: Int, b: Int): Float { return (a+b).toFloat() }
```

### 函数的引用

```
// 函数赋值给变量                    函数引用
//    ↑                              ↑
val function: (Int, Int) -> Float = ::add
```

### 高阶函数

高阶函数是将函数用作参数或返回值的函数。

```
//                      函数作为参数的高阶函数
//                              ↓
fun setOnClickListener(l: (View) -> Unit) { ... }
```

### Lambda

#### 函数简写

```
fun onClick(v: View): Unit { ... }
setOnClickListener(::onClick)

// 用 Lambda 表达式来替代函数引用
setOnClickListener({v: View -> ...})
```

#### SAM 转换

Single Abstract Method，意思就是只有一个抽象方法的类或者接口。但在 Kotlin 和 Java 8 里，SAM 代表着只有一个抽象方法的接口。

只要是符合 SAM 要求的接口，编译器就能进行 SAM 转换，也就是我们可以使用 Lambda 表达式，来简写接口类的参数。



注意，Java 8 中的 SAM 有明确的名称，叫做函数式接口（FunctionalInterface）。FunctionalInterface 的限制如下，缺一不可：

- 必须是接口，抽象类不行；
- 该接口有且仅有一个抽象的方法，抽象方法个数必须是 1，默认实现的方法可以有多个。

转换前：

```
public void setOnClickListener(OnClickListener l)
```

转换后：

```
fun setOnClickListener(l: (View) -> Unit)
// 实际上是这样：
fun setOnClickListener(l: ((View!) -> Unit)?)
```

#### Lambda 表达式引发的 8 种写法

1.原始代码

```
image.setOnClickListener(object: View.OnClickListener {
    override fun onClick(v: View?) {
        gotoPreview(v)
    }
})
```

2.

```
image.setOnClickListener(View.OnClickListener { v: View? ->
    gotoPreview(v)
})
```

3.

```
image.setOnClickListener({ v: View? ->
    gotoPreview(v)
})
```

4.

```
image.setOnClickListener({ v ->
    gotoPreview(v)
})
```

5.

```
image.setOnClickListener({ it ->
    gotoPreview(it)
})
```

6.

```
image.setOnClickListener({
    gotoPreview(it)
})
```

7.

```
image.setOnClickListener() {
    gotoPreview(it)
}
```

8.

```
image.setOnClickListener {
    gotoPreview(it)
}
```



#### 带接收者的函数类型

apply 方法应该怎么实现？

```kotlin
fun User.apply(self: User, block: (self: User) -> Unit): User{
    block(self)
    return this
}

user?.apply(self = user) { self: User ->
            username.text = self.name
            website.text = self.blog
            image.setOnClickListener { gotoImagePreviewActivity(this) }
}
```

```kotlin
//              带接收者的函数类型
//                     ↓  
fun User.apply(block: User.() -> Unit): User{
//  不用再传this
//       ↓ 
    block()
    return this
}

user?.apply { this: User ->
//               this 可以省略
//                   ↓
    username.text = this.name
    website.text = this.blog
    image.setOnClickListener { gotoImagePreviewActivity(this) }
}
```

从外表上看，带接收者的函数类型，就等价于成员方法。但从本质上讲，它仍是通过编译器注入 this 来实现的。



带接收者的函数类型也是能代表扩展函数的。



## inline优化

Kotlin 高阶函数当中的函数类型参数，变成了 Function0，而 main() 函数当中的高阶函数调用，也变成了“匿名内部类”的调用方式。

```kotlin
// HigherOrderExample.kt

fun foo(block: () -> Unit) {
    block()
}

fun main() {
    var i = 0
    foo{
        i++
    }
}
```

```java
public final class HigherOrderExampleKt {
   public static final void foo(Function0 block) {
      block.invoke();
   }

   public static final void main() {
      int i = 0
      foo((Function0)(new Function0() {
         public final void invoke() {
            i++;
         }
      }));
   }
}
```

Function0 是 Kotlin 标准库当中定义的接口，Kotlin 一共定义了 23 个类似的接口，从 Function0 一直到 Function22，分别代表了“无参数的函数类型”到“22 个参数的函数类型”。

```kotlin
public interface Function0<out R> : Function<R> {
    public operator fun invoke(): R
}
```

**inline 的作用其实就是将 inline 函数当中的代码拷贝到调用处。**

```kotlin
// HigherOrderInlineExample.kt
/*
多了一个关键字
   ↓                                    */
inline fun fooInline(block: () -> Unit) {
    block()
}

fun main() {
    var i = 0
    fooInline{
        i++
    }
}
```

```java
public final class HigherOrderInlineExampleKt {
   // 没有变化
   public static final void fooInline(Function0 block) {
      block.invoke();
   }

   public static final void main() {
      // 差别在这里
      int i = 0;
      int i = i + 1;
   }
}
```



是否使用 inline，main() 函数会有以下两个区别：

在不使用 inline 的情况下，我们的 main() 方法当中，需要调用 foo() 这个函数，这里多了一次函数调用的开销。

在不使用 inline 的情况下，调用 foo() 函数时，还创建了“Function0”的匿名内部类对象，这也是额外的开销。



局限性:

Kotlin 官方只建议我们将 inline 用于修饰高阶函数。对于普通的 Kotlin 函数，如果我们用 inline 去修饰它，IntelliJ 会对我们发出警告。而且，也不是所有高阶函数都可以用 inline，它在使用上有一些局限性。



函数是私有的，无法 inline。

因为**inline 的作用其实就是将 inline 函数当中的代码拷贝到调用处，无法在外部被访问**。这就是导致编译器报错的原因。





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







[JMH](https://github.com/openjdk/jmh)（Java Microbenchmark Harness）
