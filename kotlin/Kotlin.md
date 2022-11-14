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







## tailrec

使用递归来替代循环都是有调用栈开销的，所以我们应该尽量使用尾递归



尾递归会经过栈复用优化以后，它的开销就可以忽略不计了，我们可以认为它的空间复杂度是 O(1)

```kotlin
fun recursionLoop(): Int {
// 变化在这里
//     ↓
    tailrec fun go(i: Int, sum: Int): Int =
        if (i > 10) sum else go(i + 1, sum + i)

    return go(1, 0)
}
```

使用 Kotlin 集合操作符一行代码就能搞定

```kotlin
fun reduce() = (1..10).reduce { acc, i -> acc + i } 
```



## 委托

### 委托类

Kotlin 的委托类提供了语法层面的委托模式。通过这个 by 关键字，就可以自动将接口里的方法委托给一个对象，从而可以帮我们省略很多接口方法适配的模板代码。

```kotlin
interface DB {
    fun save()
}

class SqlDB() : DB {
    override fun save() { println("save to sql") }
}

class GreenDaoDB() : DB {
    override fun save() { println("save to GreenDao") }
}
//               参数  通过 by 将接口实现委托给 db 
//                ↓            ↓
class UniversalDB(db: DB) : DB by db

fun main() {
    UniversalDB(SqlDB()).save()
    UniversalDB(GreenDaoDB()).save()
}

/*
输出：
save to sql
save to GreenDao
*/
```

等价于以下 Java 代码

```java
class UniversalDB implements DB {
    DB db;
    public UniversalDB(DB db) { this.db = db; }
             //  手动重写接口，将 save 委托给 db.save()
    @Override//            ↓
    public void save() { db.save(); }
}
```

### 委托属性

```kotlin
class Item {
    var count: Int = 0
    var total: Int by ::count
}
```

```java
// 近似逻辑，实际上，底层会生成一个Item$total$2类型的delegate来实现

class Item {
    var count: Int = 0

    var total: Int
        get() = count

        set(value: Int) {
            count = value
        }
}
```

### 懒加载委托

```kotlin
//            定义懒加载委托
//               ↓   ↓
val data: String by lazy {
    request()
}

fun request(): String {
    println("执行网络请求")
    return "网络数据"
}

fun main() {
    println("开始")
    println(data)
    println(data)
}

结果：
开始
执行网络请求
网络数据
网络数据
```



### 自定义委托

```kotlin
class StringDelegate(private var s: String = "Hello") {
    operator fun getValue(thisRef: Owner, property: KProperty<*>): String {
        return s
    }
    operator fun setValue(thisRef: Owner, property: KProperty<*>, value: String) {
            s = value
    }
}
class Owner { 
    var text: String by StringDelegate()
}
```

也可以借助 Kotlin 提供的 ReadWriteProperty、ReadOnlyProperty 这两个接口，来自定义委托。

```kotlin
public fun interface ReadOnlyProperty<in T, out V> {
    public operator fun getValue(thisRef: T, property: KProperty<*>): V
}

public interface ReadWriteProperty<in T, V> : ReadOnlyProperty<T, V> {
    public override operator fun getValue(thisRef: T, property: KProperty<*>): V

    public operator fun setValue(thisRef: T, property: KProperty<*>, value: V)
}
```

```kotlin
class StringDelegate(private var s: String = "Hello"): ReadWriteProperty<Owner, String> {
    override operator fun getValue(thisRef: Owner, property: KProperty<*>): String {
        return s
    }
    override operator fun setValue(thisRef: Owner, property: KProperty<*>, value: String) {
        s = value
    }
}
```



### 提供委托（provideDelegate）

通过 provideDelegate 这样的方式，我们不仅可以嵌套 Delegator，还可以根据不同的逻辑派发不同的 Delegator。

```kotlin
class SmartDelegator {

    operator fun provideDelegate(
        thisRef: Owner,
        prop: KProperty<*>
    ): ReadWriteProperty<Owner, String> {

        return if (prop.name.contains("log")) {
            StringDelegate("log")
        } else {
            StringDelegate("normal")
        }
    }
}

class Owner {
    var normalText: String by SmartDelegator()
    var logText: String by SmartDelegator()
}

fun main() {
    val owner = Owner()
    println(owner.normalText)
    println(owner.logText)
}

结果：
normal
log
```



## 泛型

### 型变

型变就是为了解决泛型的不变性问题。

已知 Cat 是 Animal 的子类的情况下，MutableList\<List>与MutableList\<Animal>之间是没有任何关系的。
在默认情况下，编译器会认为MutableList\<List>与MutableList\<Animal>之间不存在任何继承关系，它们也无法互相替代，这就是泛型的不变性。

但是在某些特定场景下，编译器这种行为还是会给我们带来麻烦的。而这个时候，就需要泛型的逆变与协变了。

### 逆变

```kotlin
open class TV {
    open fun turnOn() {}
}

class XiaoMiTV1: TV() {
    override fun turnOn() {}
}

class Controller<T> {
    fun turnOn(tv: T) {}
}
//                      需要一个小米电视1的遥控器
//                                ↓
fun buy(controller: Controller<XiaoMiTV1>) {
    val xiaoMiTV1 = XiaoMiTV1()
    // 打开小米电视1
    controller.turnOn(xiaoMiTV1)
}

fun main() {
//                             实参
//                              ↓
    val controller = Controller<TV>()
    // 传入万能遥控器，报错
    buy(controller)
}

```

在这段代码中，由于我们传入的泛型实参是 TV

不过 Kotlin 编译器会报错，报错的内容是说“类型不匹配”，需要的是小米遥控器的Controller\<XiaoMiTV1>，你却买了个万能遥控器Controller。在默认情况下，Kotlin 编译器就是这么认死理。

如何解决?

1.使用处型变(使用处逆变)

```kotlin
//                         变化在这里
//                             ↓
fun buy(controller: Controller<in XiaoMiTV1>) {
    val xiaoMiTV1 = XiaoMiTV1()
    // 打开小米电视1
    controller.turnOn(xiaoMiTV1)
}
```

2.声明处型变(声明处逆变)

```kotlin
//            变化在这里
//               ↓
class Controller<in T> {
    fun turnOn(tv: T)
}
```



这样修改之后，我们就可以使用Controller\<TV>来替代Controller\<XiaoMiTV1>，也就是说，Controller\<TV>是Controller\<XiaoMiTV1>的子类。

所以父子关系颠倒的现象，我们就叫做“泛型的逆变”

另一种父子关系一致的现象，也就是泛型的协变。



### 协变

```kotlin
open class Food {}

class KFC: Food() {}


class Restaurant<T> {
    fun orderFood(): T { /*..*/ }
}


//                      这里需要一家普通的饭店，随便什么饭店都行
//                                     ↓
fun orderFood(restaurant: Restaurant<Food>) {
   restaurant.orderFood()
}

fun main() {
//                  找到一家肯德基
//                        ↓
    val kfc = Restaurant<KFC>()
// 需要普通饭店，传入了肯德基，编译器报错
    orderFood(kfc)
}
```

在这段代码中，会发现编译器提示最后一行代码报错，报错的原因同样是：“类型不匹配”，我们需要的是一家随便类型的饭店Restaurant，而传入的是肯德基Restaurant，不匹配。

如何解决?

1.使用处协变

```kotlin
//                                变化在这里
//                                    ↓
fun orderFood(restaurant: Restaurant<out Food>) {
    // 从这家饭店，点一份外卖
    val food = restaurant.orderFood()
}
```

2.声明处协变

```kotlin
//            变化在这里
//                ↓
class Restaurant<out T> {
    fun orderFood(): T { /*..*/ }
}
```

在做完以上任意一种修改以后，我们可以使用Restaurant\<KFC>替代Restaurant\<Food>，也就意味着Restaurant\<KFC>可以看作是Restaurant\<Food>的子类。

食物与饭店它们之间的父子关系一致了。这种现象，我们称之为“泛型的协变”。



### 星投影

星投影，其实就是用“星号”作为泛型的实参。

```kotlin
//                   区别在这里
//                       ↓
class Restaurant<out T: Food> {
    fun orderFood(): T {}
}

//如果我们并不关心找到的饭店到底是什么类型，那么，我们就完全可以把“星号”作为泛型的实参
fun findRestaurant(): Restaurant<*> {}

fun main() {
    val restaurant = findRestaurant()
    //       注意这里
    //          ↓
    val food: Food = restaurant.orderFood() // 返回值是：Food或其子类
    //当我们调用 restaurant.orderFood() 的时候，就无法确定它返回的值到底是什么类型。这时候，变量 food 的实际类型可能是任意的,为Restaurant 的泛型类型加上边界的话，food 的类型就可以更精确一些。
}
```

当我们为 Restaurant 泛型类型增加了上界 Food 以后，即使我们使用了“星投影”，也仍然可以通过调用 restaurant.orderFood()，来拿到 Food 类型的变量。在这里，food 的实际类型肯定是 Food 或者是 Food 的子类，因此我们可以将其看作是 Food 类型。



### 到底什么时候用逆变，什么时候用协变？

```kotlin
//              逆变
//               ↓
class Controller<in T> {
//                 ①
//                 ↓
    fun turnOn(tv: T)
}

//               协变
//                ↓
class Restaurant<out T> {
//                   ②
//                   ↓
    fun orderFood(): T { /*..*/ }
}
```

被传入函数的里面，这往往是一种写入行为，这时候，我们使用关键字 in。

被传出函数的外面，这往往是一种读取行为，这时候，我们使用关键字 out。

传入 in，传出 out。或者也可以说：泛型作为参数的时候，用 in，泛型作为返回值的时候，用 out。



### 案例

1.

```kotlin
//                          逆变
//                           ↓
public interface Comparable<in T> {
//                                   泛型作为参数
//                                       ↓
    public operator fun compareTo(other: T): Int
}

//                        协变
//                         ↓
public interface Iterator<out T> {
//                         泛型作为返回值
//                              ↓    
    public operator fun next(): T
    
    public operator fun hasNext(): Boolean
}
```



2.这里入参为什么可以使用协变呢？

```kotlin
sealed class Result<out R> {
//                     协变     ①
//                      ↓      ↓
    data class Success<out T>(val data: T, val message: String = "") : Result<T>()

    data class Error(val exception: Exception) : Result<Nothing>()

    data class Loading(val time: Long = System.currentTimeMillis()) : Result<Nothing>()
}
```

val 在 Kotlin 当中，代表不可变的变量，当它修饰类成员属性的时候，代表它只有 getter，没有 setter。

所以，我们可以用 out 修饰 Success 泛型的原因，是因为 data 的 getter 方法，它本质上是一个返回 T 类型的方法。如果改为 var，那么代码就会立马报错。



3.泛型E为什么既作为了返回值类型，又作为了参数类型。

```kotlin
//                   协变    
//                    ↓      
public interface List<out E> : Collection<E> {
//                                泛型作为返回值
//                                       ↓    
    public operator fun get(index: Int): E
//                                           泛型作为参数
//                                                 ↓    
    override fun contains(element: @UnsafeVariance E): Boolean
//                                        泛型作为参数
//                                              ↓   
    public fun indexOf(element: @UnsafeVariance E): Int
}
```

Kotlin 官方源码当中的 List，也就是这里的泛型 E，它既作为了返回值类型，又作为了参数类型。

对于 contains、indexOf 这样的方法，它们虽然以 E 作为参数类型，但本质上并没有产生写入的行为。所以，我们用 out 修饰 E 并不会带来实际的问题。

所以这个时候，我们就可以通过 @UnsafeVariance 这样的注解，来让编译器忽略这个型变冲突的问题。



4.instance为什么可以用协变的泛型 T 呢

```kotlin
//                           逆变   协变
//                            ↓     ↓
abstract class BaseSingleton<in P, out T> {
//                        ①
    @Volatile//           ↓
    private var instance: T? = null
    //                              参数  返回值
    //                               ↓    ↓
    protected abstract val creator: (P)-> T

    //                    参数 返回值
    //                     ↓   ↓
    fun getInstance(param: P): T =
        instance ?: synchronized(this) {
            instance ?: creator(param).also { instance = it }
    }
}
```

Instance 是用泛型 T 修饰的，而它是 var 定义的成员变量，这就意味着，它既有 getter，又有 setter。

这是因为它是 private 的，如果把 private 关键字删掉的话，上面的代码就会报错了。





### 其他

Java 当中也有型变的概念，但是呢，Java 当中是没有声明处型变的。

|        | 使用处协变                  | 使用处逆变                     |
| ------ | --------------------------- | ------------------------------ |
| kotlin | Restaurant\<out Food>       | Controller\<in XiaoMiTV1>      |
| java   | Restaurant\<? extends Food> | Controller\<? super XiaoMiTV1> |

 Java中的“星投影”：<?>



“使用处型变”和“声明处型变”，它们有什么区别呢？

```
声明处型变无法支持又有in又有out，只能在使用处根据情况型变。
```



## 注解

元注解：本身是注解的同时，还可以用来修饰其他注解。

Kotlin 常见的元注解有四个：

@Target，这个注解是指定了被修饰的注解都可以用在什么地方，也就是目标；

@Retention，这个注解是指定了被修饰的注解是不是编译后可见、是不是运行时可见，也就是保留位置；

@Repeatable，这个注解是允许我们在同一个地方，多次使用相同的被修饰的注解，使用场景比较少；

@MustBeDocumented，指定被修饰的注解应该包含在生成的 API 文档中显示，这个注解一般用于 SDK 当中。

```kotlin
public enum class AnnotationTarget {
    // 类、接口、object、注解类
    CLASS,
    // 注解类
    ANNOTATION_CLASS,
    // 泛型参数
    TYPE_PARAMETER,
    // 属性
    PROPERTY,
    // 字段、幕后字段
    FIELD,
    // 局部变量
    LOCAL_VARIABLE,
    // 函数参数
    VALUE_PARAMETER,
    // 构造器
    CONSTRUCTOR,
    // 函数
    FUNCTION,
    // 属性的getter
    PROPERTY_GETTER,
    // 属性的setter
    PROPERTY_SETTER,
    // 类型
    TYPE,
    // 表达式
    EXPRESSION,
    // 文件
    FILE,
    // 类型别名
    TYPEALIAS
}

public enum class AnnotationRetention {
    // 注解只存在于源代码，编译后不可见
    SOURCE,
    // 注解编译后可见，运行时不可见
    BINARY,
    // 编译后可见，运行时可见
    RUNTIME
}
```

## 反射

KClass 代表了一个 Kotlin 的类，下面是它的重要成员：

```
simpleName，类的名称，对于匿名内部类，则为 null；
qualifiedName，完整的类名；
members，所有成员属性和方法，类型是Collection<KCallable<*>>；
constructors，类的所有构造函数，类型是Collection<KFunction<T>>>；
nestedClasses，类的所有嵌套类，类型是Collection<KClass<*>>；
visibility，类的可见性，类型是KVisibility?，分别是这几种情况，PUBLIC、PROTECTED、INTERNAL、PRIVATE；
isFinal，是不是 final；
isOpen，是不是 open；
isAbstract，是不是抽象的；
isSealed，是不是密封的；
isData，是不是数据类；
isInner，是不是内部类；
isCompanion，是不是伴生对象；
isFun，是不是函数式接口；
isValue，是不是 Value Class
```

KCallable 代表了 Kotlin 当中的所有可调用的元素，比如函数、属性、甚至是构造函数。下面是 KCallable 的重要成员：

```
name，名称，这个很好理解，属性和函数都有名称；
parameters，所有的参数，类型是List，指的是调用这个元素所需的所有参数；
returnType，返回值类型，类型是 KType；
typeParameters，所有的类型参数 (比如泛型)，类型是List；
call()，KCallable 对应的调用方法，在前面的例子中，我们就调用过 setter、getter 的 call() 方法。
visibility，可见性；
isSuspend，是不是挂起函数。
```

KParameter，代表了KCallable当中的参数，它的重要成员如下：

```
index，参数的位置，下标从 0 开始；
name，参数的名称，源码当中参数的名称；
type，参数的类型，类型是 KType；
kind，参数的种类，对应三种情况：INSTANCE 是对象实例、EXTENSION_RECEIVER 是扩展接受者、VALUE 是实际的参数值。
```

KType，代表了 Kotlin 当中的类型，它重要的成员如下：

```
classifier，类型对应的 Kotlin 类，即 KClass，我们前面的例子中，就是用的 classifier == String::class 来判断它是不是 String 类型的；
arguments，类型的类型参数，看起来好像有点绕，其实它就是这个类型的泛型参数；
isMarkedNullable，是否在源代码中标记为可空类型，即这个类型的后面有没有“?”修饰。
```





## 类型系统

Any 是所有非空类型的根类型；而 Any? 是所有可空类型的根类型。

我们可以将 Any 类型赋值给“Any？”类型，反之则不行。我们可以认为“Any？”是所有 Kotlin 类型的根类型。



Java 当中的 Object 类型，对应 Kotlin 的“Any？”类型。但两者并不完全等价，因为 Kotlin 的 Any 可以没有 wait()、notify() 之类的方法。因此，我们只能说 Kotlin 的“Any？”与 Java 的 Object 是大致对应的。



Unit 与 Void 与 void

Kotlin 的 Unit 与 Java 的 Void 或者 void 并不存在等价的关系，但它们之间确实存在一些概念上的相似性。



Nothing

Nothing 就是 Kotlin 所有类型的子类型。

在函数式编程当中，也被叫做底类型（Bottom Type），因为它位于整个类型体系的最底部。



Nothing 才是底类型，而“Nothing?”则不是底类型。





## 协程

### 什么是协程？

协程，可以理解为更加轻量的线程，成千上万个协程可以同时运行在一个线程当中；

协程，其实是运行在线程当中的轻量的 Task；

协程，不会与特定的线程绑定，它可以在不同的线程之间灵活切换。



广义的协程，可以理解为“互相协作的程序”，也就是“Cooperative-routine”。

协程框架，是独立于 Kotlin 标准库的一套框架，它封装了 Java 的线程，对开发者暴露了协程的 API。

程序当中运行的“协程”，可以理解为轻量的线程；

一个线程当中，可以运行成千上万个协程；

协程，也可以理解为运行在线程当中的非阻塞的 Task；

协程，通过挂起和恢复的能力，实现了“非阻塞”；

协程不会与特定的线程绑定，它可以在不同的线程之间灵活切换，而这其实也是通过“挂起和恢复”来实现的。



协程的轻量，到底意味着什么呢？

尝试启动 10 亿个线程，这样的代码运行在大部分的机器上都是会因为内存不足等原因而异常退出的。

我们启动了 10 亿个协程。由于协程是非常轻量的，所以代码不会因为内存不足而异常退出。



Kotlin 协程的魅力：以同步的方式完成异步任务。



### 协程的“非阻塞”

挂起和恢复。这两个能力也是协程才拥有的特殊能力，普通的程序是不具备的。



那么，协程是如何通过挂起和恢复来实现非阻塞的呢？

大部分的语言当中都会存在一个类似“调度中心”的东西，它会来实现 Task 任务的执行和调度。

而协程除了拥有“调度中心”以外，对于每个协程的 Task，还会多出一个类似“抓手”“挂钩”的东西，可以方便我们对它进行“挂起和恢复”。



### 协程 VM 参数

```
-Dkotlinx.coroutines.debug
```

完成这个设置后，当我们在 log 当中打印“Thread.currentThread().name”的时候，如果当前代码是运行在协程当中的，那么它就会带上协程的相关信息。



### 如何启动协程

#### launch 启动协程

非阻塞,无返回结果



launch 启动一个协程以后，并没有让协程为我们返回一个执行结果。

这个函数的返回值是一个 Job，它其实代表的是协程的句柄（Handle），它并不能为我们返回协程的执行结果。

```kotlin

fun main() {
    GlobalScope.launch {                
        println("Coroutine started!")   
        delay(1000L)                    
        println("Hello World!")        
    }

    println("After launch!")           
    Thread.sleep(2000L)               
    println("Process end!")
}

/*
输出结果：
After launch!
Coroutine started!
Hello World!
Process end!
*/
```

launch 并不会阻塞线程的执行，甚至，我们可以认为 launch() 当中 Lambda 一定就是在函数调用之后才执行的。当然，在特殊情况下，这种行为模式也是可以打破的。



函数签名:

```kotlin
public fun CoroutineScope.launch( // 扩展函数
    context: CoroutineContext = EmptyCoroutineContext, // 协程的上下文
    start: CoroutineStart = CoroutineStart.DEFAULT, // 协程的启动模式,最常使用的就是 DEFAULT、LAZY
    block: suspend CoroutineScope.() -> Unit // 挂起函数,是 CoroutineScope 类的成员方法或是扩展方法
): Job { ... }
```





#### runBlocking 启动协程

阻塞式启动协程,有返回结果

```kotlin
fun main() {
    val result = runBlocking {
        delay(1000L)
        // return@runBlocking 可写可不写
        return@runBlocking "Coroutine done!" 
    }

    println("Result is: $result")
}
/*
输出结果：
Result is: Coroutine done!
*/
```

函数签名:

```kotlin
public actual fun <T> runBlocking( // 普通的顶层函数,调用它的时候，不需要 CoroutineScope 的对象
    context: CoroutineContext,
    block: suspend CoroutineScope.() -> T): T { // 函数类型是有返回值类型 T 的，说明runBlocking 其实是可以从协程当中返回执行结果的
...
}
```



#### async 启动协程

非阻塞式启动协程，有返回结果

使用 async{} 创建协程，能通过它返回的句柄拿到协程的执行结果。

```kotlin

fun main() = runBlocking {
    println("In runBlocking:${Thread.currentThread().name}")

    val deferred: Deferred<String> = async {
        println("In async:${Thread.currentThread().name}")
        delay(1000L) // 模拟耗时操作
        return@async "Task completed!"
    }

    println("After async:${Thread.currentThread().name}")

    val result = deferred.await()
    println("Result is: $result")
}
/*
输出结果：
In runBlocking:main @coroutine#1
After async:main @coroutine#1 // 注意，它比“In async”先输出
In async:main @coroutine#2
Result is: Task completed!
*/
```

函数签名:

```kotlin
public fun <T> CoroutineScope.async(
    context: CoroutineContext = EmptyCoroutineContext,
    start: CoroutineStart = CoroutineStart.DEFAULT,
    block: suspend CoroutineScope.() -> T 
): Deferred<T> {} 
```



### 深入理解 suspend

suspend，是 Kotlin 当中的一个关键字，它主要的作用是用于定义“挂起函数”。



挂起函数示例:

```kotlin
//挂起函数
// ↓
suspend fun getUserInfo(): String {
    withContext(Dispatchers.IO) {
        delay(1000L)
    }
    return "BoyCoder"
}
```

挂起函数反编译成 Java，结果会是这样：

```kotlin
//                              Continuation 等价于 CallBack
//                                         ↓         
public static final Object getUserInfo(Continuation $completion) {
  ...
  return "BoyCoder";
}
```

从反编译的结果来看，挂起函数确实变成了一个带有 CallBack 的函数，只是这个 CallBack 换了个名字，叫做 Continuation。



Continuation 在 Kotlin 中的定义：

```kotlin
public interface Continuation<in T> {
// ...

//      相当于 CallBack的onSuccess   结果   
//                 ↓                 ↓
    public fun resumeWith(result: Result<T>)
}

interface CallBack {
    void onSuccess(String response);
}
```

Continuation 本质上也就是一个带有泛型参数的 CallBack





Continuation 到底是什么？

Continuation 的词源是 Continue。Continue 是“继续”的意思，Continuation 则是“接下来要做的事情”。放到程序中，Continuation 就代表了，“程序继续运行下去需要执行的代码”，“接下来要执行的代码”，或者是“剩下的代码”。



CPS 是什么?

这个“从挂起函数转换成 CallBack 函数”的过程，被叫做是 CPS 转换（Continuation-Passing-Style Transformation）。

CPS 其实就是将程序接下来要执行的代码进行传递的一种模式。

而 CPS 转换，就是将原本的同步挂起函数转换成 CallBack 异步代码的过程。这个转换是编译器在背后做的，我们程序员对此并无感知。



协程之所以是非阻塞，是因为它支持“挂起和恢复”；而挂起和恢复的能力，主要是源自于“挂起函数”；而挂起函数是由 CPS 实现的，其中的 Continuation，本质上就是 Callback。





### 协程生命周期

New、Active、Completing、Cancelling、Completed、Cancelled 这些状态

对于协程的 Job 来说，它有两种初始状态:

如果 Job 是以懒加载的方式创建的，那么它的初始状态将会是 New；

而如果一个协程是以非懒加载的方式创建的，那么它的初始状态就会是 Active。





#### 等待和监听协程的结束事件

invokeOnCompletion {} 的作用是 监听协程结束的事件。如果 job 被取消了，invokeOnCompletion {} 这个回调仍然会被调用。

job.join() 其实是一个“挂起函数”，它的作用就是：挂起当前的程序执行流程，等待 job 当中的协程任务执行完毕，然后再恢复当前的程序执行流程。

```kotlin
fun main() = runBlocking {
    suspend fun download() {
        // 模拟下载任务
        val time = (Random.nextDouble() * 1000).toLong()
        logX("Delay time: = $time")
        delay(time)
    }
    val job = launch(start = CoroutineStart.LAZY) {
        logX("Coroutine start!")
        download()
        logX("Coroutine end!")
    }
    delay(500L)
    job.log()
    job.start()
    job.log()
    job.invokeOnCompletion {
        job.log() // 协程结束以后就会调用这里的代码
    }
    job.join()      // 等待协程执行完毕
    logX("Process end!")
}

/**
 * 打印Job的状态信息
 */
fun Job.log() {
    logX("""
        isActive = $isActive
        isCancelled = $isCancelled
        isCompleted = $isCompleted
    """.trimIndent())
}

/**
 * 控制台输出带协程信息的log
 */
fun logX(any: Any?) {
    println("""
================================
$any
Thread:${Thread.currentThread().name}
================================""".trimIndent())
}

/*
运行结果：
================================
isActive = false
isCancelled = false
isCompleted = false
Thread:main @coroutine#1
================================
================================
isActive = true
isCancelled = false
isCompleted = false
Thread:main @coroutine#1
================================
================================
Coroutine start!
Thread:main @coroutine#2
================================
================================
Delay time: = 252
Thread:main @coroutine#2
================================
================================
Coroutine end!
Thread:main @coroutine#2
================================
================================
isActive = false
isCancelled = false
isCompleted = true
Thread:main @coroutine#2
================================
================================
Process end!
Thread:main @coroutine#1
================================
*/
```



#### Job源码

```kotlin
public interface Job : CoroutineContext.Element {

    // 省略部分代码

    // ------------ 状态查询API ------------

    public val isActive: Boolean

    public val isCompleted: Boolean

    public val isCancelled: Boolean

    public fun getCancellationException(): CancellationException

    // ------------ 操控状态API ------------

    public fun start(): Boolean

    public fun cancel(cause: CancellationException? = null)

    public fun cancel(): Unit = cancel(null)

    public fun cancel(cause: Throwable? = null): Boolean

    // ------------ 等待状态API ------------

    public suspend fun join()

    public val onJoin: SelectClause0

    // ------------ 完成状态回调API ------------

    public fun invokeOnCompletion(handler: CompletionHandler): DisposableHandle

    public fun invokeOnCompletion(
        onCancelling: Boolean = false,
        invokeImmediately: Boolean = true,
        handler: CompletionHandler): DisposableHandle

}
```



#### Deferred

函数签名

```kotlin
public interface Deferred<out T> : Job {
//          注意这里
//            ↓
    public suspend fun await(): T
}
```

deferred.await() 这个方法，不仅可以帮助我们获取协程的执行结果，它还会阻塞当前协程的执行流程，直到协程任务执行完毕。在这一点的行为上，await() 和 join() 是类似的。



#### Job 与结构化并发

“结构化并发”就是：带有结构和层级的并发。

```kotlin
fun main() = runBlocking {
    val parentJob: Job
    var job1: Job? = null
    var job2: Job? = null
    var job3: Job? = null

    parentJob = launch {
        job1 = launch {
            delay(1000L)
        }

        job2 = launch {
            delay(3000L)
        }

        job3 = launch {
            delay(5000L)
        }
    }

    delay(500L)

    parentJob.children.forEachIndexed { index, job ->
        when (index) {
            0 -> println("job1 === job is ${job1 === job}")
            1 -> println("job2 === job is ${job2 === job}")
            2 -> println("job3 === job is ${job3 === job}")
        }
    }

    parentJob.join() // 这里会挂起大约5秒钟
    logX("Process end!")
}

/*
输出结果：
job1 === job is true
job2 === job is true
job3 === job is true
// 等待大约5秒钟
================================
Process end!
Thread:main @coroutine#1
================================
*/
```



```kotlin
public interface Job : CoroutineContext.Element {
    // 省略部分代码

    // ------------ parent-child ------------

    public val children: Sequence<Job>

    @InternalCoroutinesApi
    public fun attachChild(child: ChildJob): ChildHandle
}
```

每个 Job 对象，都会有一个 children 属性，它的类型是 Sequence，它是一个惰性的集合，我们可以对它进行遍历。

而 attachChild() 则是一个协程内部的 API，用于绑定 ChildJob 的







### Context

```kotlin
public fun CoroutineScope.launch(
//                这里
//                 ↓
    context: CoroutineContext = EmptyCoroutineContext,
    start: CoroutineStart = CoroutineStart.DEFAULT,
    block: suspend CoroutineScope.() -> Unit
): Job {}
```

默认值 EmptyCoroutineContext

官方另外提供:

- Dispatchers.Main：UI线程。
- Dispatchers.Unconfined：代表无所谓，当前协程可能运行在任意线程之上。
- Dispatchers.Default：用于 CPU 密集型任务的线程池。
- Dispatchers.IO：用于 IO 密集型任务的线程池。

注意：Dispatchers.IO 底层是可能复用 Dispatchers.Default 当中的线程





#### Dispatchers.Unconfined不应该随意使用

```kotlin
fun main() = runBlocking {
    logX("Before launch.") // 1
    launch {
        logX("In launch.") // 2
        delay(1000L)
        logX("End launch.") // 3
    }
    logX("After launch")   // 4
}
```

运行顺序应该是：1、4、2、3

```kotlin

// 代码段8

fun main() = runBlocking {
    logX("Before launch.")  // 1
//               变化在这里
//                  ↓
    launch(Dispatchers.Unconfined) {
        logX("In launch.")  // 2
        delay(1000L)
        logX("End launch.") // 3
    }
    logX("After launch")    // 4
}

/*
输出结果：
================================
Before launch.
Thread:main @coroutine#1
================================
================================
In launch.
Thread:main @coroutine#2
================================
================================
After launch
Thread:main @coroutine#1
================================
================================
End launch.
Thread:kotlinx.coroutines.DefaultExecutor @coroutine#2
================================
*/
```

运行顺序就变成了：1、2、4、3

Unconfined 代表的意思就是，当前协程可能运行在任何线程之上，不作强制要求。

由此可见，Dispatchers.Unconfined 其实是很危险的。所以，我们不应该随意使用 Dispatchers.Unconfined。



#### CoroutineScope

CoroutineScope 只是对 CoroutineContext 做了一层封装而已，它的核心能力其实都来自于 CoroutineContext。

而 CoroutineScope 最大的作用，就是可以方便我们批量控制协程。

```kotlin
// CoroutineScope 源码
public interface CoroutineScope {
    public val coroutineContext: CoroutineContext
}
```

#### Job

Job 继承自 CoroutineContext.Element，而 CoroutineContext.Element 仍然继承自 CoroutineContext，这就意味着 Job 是间接继承自 CoroutineContext 的。所以说，Job 确实是一个真正的 CoroutineContext。

```kotlin
public interface Job : CoroutineContext.Element {}

public interface CoroutineContext {
    public interface Element : CoroutineContext {}
}
```

#### CoroutineContext

```kotlin
// 类似map的接口设计
public interface CoroutineContext {

    public operator fun <E : Element> get(key: Key<E>): E?

    public operator fun plus(context: CoroutineContext): CoroutineContext {}

    public fun minusKey(key: Key<*>): CoroutineContext

    public fun <R> fold(initial: R, operation: (R, Element) -> R): R

    public interface Key<E : Element>
}
```



```kotlin
val mySingleDispatcher = Executors.newSingleThreadExecutor { Thread(it, "MySingleThread").apply { isDaemon = true }}.asCoroutineDispatcher()

@OptIn(ExperimentalStdlibApi::class)
fun main() = runBlocking {
    // 使用了“Job() + mySingleDispatcher”这样的方式创建 CoroutineScope，代码之所以这么写，是因为 CoroutineContext 的 plus() 进行了操作符重载。
    val scope = CoroutineScope(Job() + mySingleDispatcher)

    scope.launch {
        // 使用了“coroutineContext[CoroutineDispatcher]”这样的方式，访问当前协程所对应的 Dispatcher。这也是因为 CoroutineContext 的 get()，支持了操作符重载。
        logX(coroutineContext[CoroutineDispatcher] == mySingleDispatcher)
        delay(1000L)
        logX("First end!")  // 不会执行
    }

    delay(500L)
    scope.cancel()
    delay(1000L)
}
```



Dispatcher 本身也是 CoroutineContext，不然它怎么可以实现“Job() + mySingleDispatcher”这样的写法呢？

最重要的是，当我们以这样的方式创建出 scope 以后，后续创建的协程就全部都运行在 mySingleDispatcher 这个线程之上了。



#### Dispatcher 到底是如何跟 CoroutineContext 建立关系的呢？

Dispatchers 其实是一个 object 单例，它的内部成员的类型是 CoroutineDispatcher，而它又是继承自 ContinuationInterceptor，这个类则是实现了 CoroutineContext.Element 接口。由此可见，Dispatcher 确实就是 CoroutineContext。

```kotlin
public actual object Dispatchers {

    public actual val Default: CoroutineDispatcher = DefaultScheduler

    public actual val Main: MainCoroutineDispatcher get() = MainDispatcherLoader.dispatcher

    public actual val Unconfined: CoroutineDispatcher = kotlinx.coroutines.Unconfined

    public val IO: CoroutineDispatcher = DefaultIoScheduler

    public fun shutdown() {    }
}

public abstract class CoroutineDispatcher :
    AbstractCoroutineContextElement(ContinuationInterceptor), ContinuationInterceptor {}

public interface ContinuationInterceptor : CoroutineContext.Element {}
```



#### CoroutineName

指定协程名称

```kotlin
@OptIn(ExperimentalStdlibApi::class)
fun main() = runBlocking {
    val scope = CoroutineScope(Job() + mySingleDispatcher)
    // 协程的名字
    scope.launch(CoroutineName("MyFirstCoroutine!")) {
        logX(coroutineContext[CoroutineDispatcher] == mySingleDispatcher)
        delay(1000L)
        logX("First end!")
    }

    delay(500L)
    scope.cancel()
    delay(1000L)
}

/*
输出结果：

================================
true
Thread:MySingleThread @MyFirstCoroutine!#2  // 其中的数字“2”，其实是一个自增的唯一 ID。
================================
*/
```



#### CoroutineExceptionHandler

负责处理协程当中的异常。

```kotlin
public interface CoroutineExceptionHandler : CoroutineContext.Element {

    public companion object Key : CoroutineContext.Key<CoroutineExceptionHandler>

    public fun handleException(context: CoroutineContext, exception: Throwable)
}
```



```kotlin
//  这里使用了挂起函数版本的main()
suspend fun main() {
    val myExceptionHandler = CoroutineExceptionHandler { _, throwable ->
        println("Catch exception: $throwable")
    }
    val scope = CoroutineScope(Job() + mySingleDispatcher)

    val job = scope.launch(myExceptionHandler) {
        val s: String? = null
        s!!.length // 空指针异常
    }

    job.join()
}
/*
输出结果：
Catch exception: java.lang.NullPointerException
*/
```



### suspendCoroutine

Kotlin 官方提供的一个顶层函数：suspendCoroutine{}，它的函数签名是这样的：

```
public suspend inline fun <T> suspendCoroutine(crossinline block: (Continuation<T>) -> Unit): T {
    // 省略细节
}
```



```kotlin
suspend fun <T : Any> KtCall<T>.await(): T =
    suspendCoroutine { continuation ->
        call(object : Callback<T> {
            override fun onSuccess(data: T) {
                continuation.resume(data) // 成功回调
            }

            override fun onFail(throwable: Throwable) {
                continuation.resumeWithException(throwable)// 失败回到
            }
        })
    }
```

它是一个挂起函数，也是一个高阶函数

缺点：不支持取消



### suspendCancellableCoroutine

可以往 continuation 对象上面设置一个监听：invokeOnCancellation{}，它代表当前的协程被取消了

```kotlin

suspend fun <T : Any> KtCall<T>.await(): T =
//            变化1
//              ↓
    suspendCancellableCoroutine { continuation ->
        val call = call(object : Callback<T> {
            override fun onSuccess(data: T) {
                println("Request success!")
                continuation.resume(data)
            }

            override fun onFail(throwable: Throwable) {
                println("Request fail!：$throwable")
                continuation.resumeWithException(throwable)
            }
        })

//            变化2
//              ↓
        continuation.invokeOnCancellation {
            println("Call cancelled!")
            call.cancel()
        }
    }
```

两大优势：

第一，它可以避免不必要的挂起，提升运行效率；

第二，它可以避免不必要的资源浪费，改善软件的综合指标。







### Channel

```kotlin
fun main() = runBlocking {
    val channel = Channel<Int>()

    launch {
        (1..3).forEach {
            channel.send(it)
            logX("Send: $it")
        }
		// channel 是一种协程资源，在用完 channel 以后，如果我们不去主动关闭它的话，是会造成不必要的资源浪费的。
        // 如果我们忘记调用“channel.close()”，程序将永远不会停下来。
        channel.close()
    }

    launch {
        for (i in channel) {
            logX("Receive: $i")
        }
    }

    logX("end")
}
```



源码

```kotlin
public fun <E> Channel(
    capacity: Int = RENDEZVOUS,
    onBufferOverflow: BufferOverflow = BufferOverflow.SUSPEND,
    onUndeliveredElement: ((E) -> Unit)? = null
): Channel<E> {}
```

参数1：capacity，代表了管道的容量。

- RENDEZVOUS（默认），代表了 Channel 的容量为 0；
- UNLIMITED，代表了无限容量；
- CONFLATED，代表了容量为 1，新的数据会替代旧的数据；
- BUFFERED，代表了具备一定的缓存容量，默认情况下是 64，具体容量由这个 VM 参数决定 "kotlinx.coroutines.channels.defaultBuffer"。

参数2：onBufferOverflow，指当我们指定了 capacity 的容量，等管道的容量满了时，Channel 的应对策略是怎么样的。

- SUSPEND（默认），当管道的容量满了以后，如果发送方还要继续发送，我们就会挂起当前的 send() 方法。由于它是一个挂起函数，所以我们可以以非阻塞的方式，将发送方的执行流程挂起，等管道中有了空闲位置以后再恢复。
- DROP_OLDEST，就是丢弃最旧的那条数据，然后发送新的数据；
- DROP_LATEST，丢弃最新的那条数据。这里要注意，这个动作的含义是丢弃当前正准备发送的那条数据，而管道中的内容将维持不变。

参数3：onUndeliveredElement，异常处理回调

发送出去的 Channel 数据无法被接收方处理的时候，就可以通过 onUndeliveredElement 这个回调，来进行监听。



#### produce

使用 produce{} 以后，就不用再去调用 close() 方法了，因为 produce{} 会自动帮我们去调用 close() 方法。

```kotlin
fun main() = runBlocking {
    // produce
    val channel: ReceiveChannel<Int> = produce {
        (1..3).forEach {
            send(it)
            logX("Send: $it")
        }
    }

    launch {
        // 3，接收数据
        for (i in channel) {
            logX("Receive: $i")
        }
    }

    logX("end")
}
```



#### 为什么不推荐使用receive

```kotlin
fun main() = runBlocking {
    // 1，创建管道
    val channel: ReceiveChannel<Int> = produce {
        // 发送3条数据
        (1..3).forEach {
            send(it)
        }
    }

    // 调用4次receive()
    channel.receive() // 1
    channel.receive() // 2
    channel.receive() // 3
    channel.receive() // 异常

    logX("end")
}

/*
输出结果：
ClosedReceiveChannelException: Channel was closed
*/
```

```kotlin
fun main() = runBlocking {
    val channel: Channel<Int> = Channel()

    launch {
        (1..3).forEach {
            channel.send(it)
        }
    }

    // 调用4次receive()
    channel.receive()       // 1
    println("Receive: 1")
    channel.receive()       // 2
    println("Receive: 2")
    channel.receive()       // 3
    println("Receive: 3")
    channel.receive()       // 永远挂起

    logX("end")
}
```

从上面2段代码可以看出，直接使用 receive() 是很容易出问题的。





##### isClosedForReceive、isClosedForSend

对于发送方，我们可以使用“isClosedForSend”来判断当前的 Channel 是否关闭；

对于接收方来说，我们可以用“isClosedForReceive”来判断当前的 Channel 是否关闭。

```kotlin
fun main() = runBlocking {
    // 因为，当你为管道指定了 capacity 以后，以上的判断方式将会变得不可靠！
    val channel: ReceiveChannel<Int> = produce(capacity = 3) {
        (1..300).forEach {
            send(it)
            println("Send $it")
        }
    }

    while (!channel.isClosedForReceive) {
        val i = channel.receive()
        println("Receive $i")
    }

    logX("end")
}

/*
输出结果
// 省略部分
Receive 300
Send 300
ClosedReceiveChannelException: Channel was closed
*/
```

所以，最好不要用 channel.receive()。即使配合 isClosedForReceive 这个判断条件，我们直接调用 channel.receive() 仍然是一件非常危险的事情！



#### 推荐使用consumeEach和for读取Channel 

当我们想要读取 Channel 当中的数据时，我们一定要使用 for 循环，或者是 channel.consumeEach {}，千万不要直接调用 channel.receive()。

```kotlin
fun main() = runBlocking {
    val channel: ReceiveChannel<Int> = produce(capacity = 3) {
        (1..300).forEach {
            send(it)
            println("Send $it")
        }
    }

    // 变化在这里
    channel.consumeEach {
        println("Receive $it")
    }

    logX("end")
}

/*
输出结果：

正常
*/
```



补充：在某些特殊场景下，如果我们必须要自己来调用 channel.receive()，那么可以考虑使用 receiveCatching()，它可以防止异常发生。



#### 为什么说 Channel 是“热”的？

不管有没有接收方，发送方都会工作”的模式，就是我们将其认定为“热”的原因。

#### 源码

```kotlin
public interface Channel<E> : SendChannel<E>, ReceiveChannel<E> {}
```

```kotlin
public interface SendChannel<in E> 
    public val isClosedForSend: Boolean

    public suspend fun send(element: E)

    // 1，select相关
    public val onSend: SelectClause2<E, SendChannel<E>>

    // 2，非挂起函数的接收
    public fun trySend(element: E): ChannelResult<Unit>

    public fun close(cause: Throwable? = null): Boolean

    public fun invokeOnClose(handler: (cause: Throwable?) -> Unit)

}

public interface ReceiveChannel<out E> {

    public val isClosedForReceive: Boolean

    public val isEmpty: Boolean

    public suspend fun receive(): E

    public suspend fun receiveCatching(): ChannelResult<E>
    // 3，select相关
    public val onReceive: SelectClause1<E>
    // 4，select相关
    public val onReceiveCatching: SelectClause1<ChannelResult<E>>

    // 5，非挂起函数的接收
    public fun tryReceive(): ChannelResult<E>

    public operator fun iterator(): ChannelIterator<E>

    public fun cancel(cause: CancellationException? = null)
}
```



### Flow

```kotlin
fun main() = runBlocking {
    // 或者可以使用 flowOf(1, 2, 3, 4, 5)
    flow {                  // 上游，发源地
        emit(1)             // 挂起函数
        emit(2)
        emit(3)
        emit(4)
        emit(5)
    }.filter { it > 2 }     // 中转站1
        .map { it * 2 }     // 中转站2
        .take(2)            // 中转站3
        .collect{           // 下游
            println(it)
        }
}

/*
输出结果：                       
6
8
*/
```

#### Flow和List互相转换

```kotlin
fun main() = runBlocking {
    // Flow转List
    flowOf(1, 2, 3, 4, 5)
        .toList()
        .filter { it > 2 }
        .map { it * 2 }
        .take(2)
        .forEach {
            println(it)
        }

    // List转Flow
    listOf(1, 2, 3, 4, 5)
        .asFlow()
        .filter { it > 2 }
        .map { it * 2 }
        .take(2)
        .collect {
            println(it)
        }
}

/*
输出结果
6
8
6
8
*/
```

#### 生命周期

onStart，它的作用是注册一个监听事件：当 flow 启动以后，它就会被回调。

onCompletion 只会在 Flow 数据流执行完毕以后，才会回调。

onStart 和 onCompletion执行顺序，跟它在 Flow 当中的位置无关。



#### catch 异常处理

catch 的作用域，仅限于 catch 的上游。

```kotlin
fun main() = runBlocking {
    val flow = flow {
        emit(1)
        emit(2)
        throw IllegalStateException()
        emit(3)
    }

    flow.map { it * 2 }
        .catch { println("catch: $it") } // 注意这里
        .collect {
            println(it)
        }
}
/*
输出结果：
2
4
catch: java.lang.IllegalStateException
*/
```



#### 切换 Context：flowOn、launchIn

flowOn：操作符也是和它的位置强相关的。它的作用域跟前面的 catch 类似：flowOn 仅限于它的上游。

```kotlin
fun main() = runBlocking {
    val flow = flow {
        logX("Start")
        emit(1)
        logX("Emit: 1")
        emit(2)
        logX("Emit: 2")
        emit(3)
        logX("Emit: 3")
    }

    flow.filter {
            logX("Filter: $it")
            it > 2
        }
        .flowOn(Dispatchers.IO)  // 注意这里
        .collect {
            logX("Collect $it")
        }
}

/*
输出结果
================================
Start
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Filter: 1
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Emit: 1
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Filter: 2
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Emit: 2
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Filter: 3
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Emit: 3
Thread:DefaultDispatcher-worker-1 @coroutine#2
================================
================================
Collect 3
Thread:main @coroutine#1
================================
```

launchIn：

```kotlin
val scope = CoroutineScope(mySingleDispatcher)
flow.flowOn(Dispatchers.IO)
    .filter {
        logX("Filter: $it")
        it > 2
    }
    .onEach { // 借助了 onEach{} 来实现类似 collect{} 的功能
        logX("onEach $it")
    }
    .launchIn(scope)

/*
输出结果：
onEach{}将运行在MySingleThread
filter{}运行在MySingleThread
flow{}运行在DefaultDispatcher
*/
```



launchIn 的源代码

```kotlin
public fun <T> Flow<T>.launchIn(scope: CoroutineScope): Job = scope.launch {
    collect() // tail-call
}
```



注：withContext 在 Flow 当中是不被推荐的，即使要用，也应该谨慎再谨慎。



#### 为什么说 Flow 是“冷”的？

Channel 之所以被认为是“热”的原因，是因为不管有没有接收方，发送方都会工作。

Flow 被认为是“冷”的原因，就是因为只有调用终止操作符之后，Flow 才会开始工作。

Flow 一次只会处理一条数据。虽然它也是 Flow“冷”的一种表现，但这个特性准确来说是“懒”。

```kotlin
fun main() = runBlocking {
    flow {
        println("emit: 3")
        emit(3)
        println("emit: 4")
        emit(4)
        println("emit: 5")
        emit(5)
    }.filter {
        println("filter: $it")
        it > 2
    }.map {
        println("map: $it")
        it * 2
    }.collect {
        println("collect: $it")
    }
}
/*
输出结果：
emit: 3
filter: 3
map: 3
collect: 6
emit: 4
filter: 4
map: 4
collect: 8
emit: 5
filter: 5
map: 5
collect: 10
*/
```









### Select

select 就是选择“更快的结果”

可以用 async 搭配 select 来使用。async 可以实现并发，select 则可以选择最快的结果。

```kotlin
fun main() = runBlocking {
    val startTime = System.currentTimeMillis()
    val productId = "xxxId"
    //          1，注意这里
    //               ↓
    val product = select<Product?> {
        // 2，注意这里
        async { getCacheInfo(productId) }
            .onAwait { // 注意了，这里我们用的 onAwait{}，而不是 await()。
                it
            }
        // 4，注意这里
        async { getNetworkInfo(productId) }
            .onAwait {  // 注意了，这里我们用的 onAwait{}，而不是 await()。
                it
            }
    }

    if (product != null) {
        updateUI(product)
        println("Time cost: ${System.currentTimeMillis() - startTime}")
    }
}

/*
输出结果
xxxId==9.9
Time cost: 127
*/
```



如果是缓存信息优先返回，需要进一步更新最新信息。

```kotlin
fun main() = runBlocking {
    suspend fun getCacheInfo(productId: String): Product? {
        delay(100L)
        return Product(productId, 9.9)
    }

    suspend fun getNetworkInfo(productId: String): Product? {
        delay(200L)
        return Product(productId, 9.8)
    }

    fun updateUI(product: Product) {
        println("${product.productId}==${product.price}")
    }

    val startTime = System.currentTimeMillis()
    val productId = "xxxId"

    // 1，缓存和网络，并发执行
    val cacheDeferred = async { getCacheInfo(productId) }
    val latestDeferred = async { getNetworkInfo(productId) }

    // 2，在缓存和网络中间，选择最快的结果
    val product = select<Product?> {
        cacheDeferred.onAwait {
                it?.copy(isCache = true)
            }

        latestDeferred.onAwait {
                it?.copy(isCache = false)
            }
    }

    // 3，更新UI
    if (product != null) {
        updateUI(product)
        println("Time cost: ${System.currentTimeMillis() - startTime}")
    }

    // 4，如果当前结果是缓存，那么再取最新的网络服务结果
    if (product != null && product.isCache) {
        val latest = latestDeferred.await()?: return@runBlocking
        updateUI(latest)
        println("Time cost: ${System.currentTimeMillis() - startTime}")
    }
}

/*
输出结果：
xxxId==9.9
Time cost: 120
xxxId==9.8
Time cost: 220
*/
```



#### select 和 Channel

```kotlin
fun main() = runBlocking {
    val startTime = System.currentTimeMillis()
    val channel1 = produce {
        send("1")
        delay(200L)
        send("2")
        delay(200L)
        send("3")
        delay(150L)
    }

    val channel2 = produce {
        delay(100L)
        send("a")
        delay(200L)
        send("b")
        delay(200L)
        send("c")
    }

    suspend fun selectChannel(channel1: ReceiveChannel<String>, channel2: ReceiveChannel<String>): String = select<String> {
        // 1， 选择channel1
        channel1.onReceive{
            it.also { println(it) }
        }
        // 2， 选择channel1
        channel2.onReceive{
            it.also { println(it) }
        }
    }

    repeat(6){// 3， 选择6次结果
        selectChannel(channel1, channel2)
    }

    println("Time cost: ${System.currentTimeMillis() - startTime}")
}

/*
输出结果
1
a
2
b
3
c
Time cost: 540
*/
```

onReceiveCatching解决异常问题

```kotlin

// 代码段13

fun main() = runBlocking {
    val startTime = System.currentTimeMillis()
    val channel1 = produce<String> {
        delay(15000L)
    }

    val channel2 = produce {
        delay(100L)
        send("a")
        delay(200L)
        send("b")
        delay(200L)
        send("c")
    }

    suspend fun selectChannel(channel1: ReceiveChannel<String>, channel2: ReceiveChannel<String>): String =
        select<String> {
            channel1.onReceiveCatching {
                it.getOrNull() ?: "channel1 is closed!"
            }
            channel2.onReceiveCatching {
                it.getOrNull() ?: "channel2 is closed!"
            }
        }

    repeat(6) {
        val result = selectChannel(channel1, channel2)
        println(result)
    }

	
    channel1.cancel()
    channel2.cancel()

    println("Time cost: ${System.currentTimeMillis() - startTime}")
}
```



当 Deferred、Channel 与 select 配合的时候，它们原本的 API 会多一个 on 前缀。

```kotlin
public interface Deferred : CoroutineContext.Element {
    public suspend fun join()
    public suspend fun await(): T

    // select相关  
    public val onJoin: SelectClause0
    public val onAwait: SelectClause1<T>
}

public interface SendChannel<in E> 
    public suspend fun send(element: E)

    // select相关
    public val onSend: SelectClause2<E, SendChannel<E>>

}

public interface ReceiveChannel<out E> {
    public suspend fun receive(): E

    public suspend fun receiveCatching(): ChannelResult<E>
    // select相关
    public val onReceive: SelectClause1<E>
    public val onReceiveCatching: SelectClause1<ChannelResult<E>>
}
```



























## 其他

### 1.错误的写法

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





### 2.Kotlin 接口的“成员属性”是存在一定的局限性的

接口的属性： 1.不能设置初始值 2.val可以重写get,var的get和set都不能重写

[JMH](https://github.com/openjdk/jmh)（Java Microbenchmark Harness）







### 3.表达式思维和函数式编程有联系吗?

函数式编程要求函数是一等公民，如果某些函数不能用类型描述（例如Java中的返回为void 函数），后续的赋值，参数传递就很困难，成为一等公民就成为泡影！Kotlin中Nothing，Unit 和 Any?让所有函数的返回都有固定类型，为函数式编程奠定基础。函数的返回值必然来自于某个表达式，这也要求表达式都有固定类型。表达式是函数编程的组成模块，是串联各个函数的纽带，也是决定函数返回值的重要一环。



### 4.val 一定不可变吗？

```kotlin
object TestVal {
    val a: Double
        get() = Random.nextDouble()

    fun testVal() {
        println(a)
        println(a)
    }
}

// 结果
0.0071073054825220305
0.6478886064282862
```



### 5.Smart Cast失效例子

```
class JavaConvertExample {
    private var name: String? = null
    fun init() {
        name = ""
    }

    fun foo() {
        name = null;
    }

    fun test() {
        if (name != null) {
            // 几百行代码
            foo()
            //几百行代码
            val count = name!!.length
        }
    }
}
```



解决:

第一种，避免直接访问成员变量或者全局变量，将其改为传参的形式：

```kotlin
//    改为函数参数
//        ↓
fun test(name: String?) {
    if (name != null) {
//             函数参数支持Smart Cast
//                      ↓
        val count = name.length
    }
}
```



第二种，避免使用可变变量 var，改为 val 不可变变量：

```kotlin
class JavaConvertExample {
//       不可变变量
//           ↓
    private val name: String? = null

    fun test() {
        if (name != null) {
//               不可变变量支持Smart Cast
//                          ↓
            val count = name.length
        }
    }
}
```

第三种，借助临时的不可变变量：

```kotlin
class JavaConvertExample {
    private var name: String? = null

    fun test() {
//        不可变变量
//            ↓
        val _name = name
        if (_name != null) {
            // 在if当中，只使用_name这个临时变量
            val count = _name.length
        }
    }
}
```

第四种，是借助 Kotlin 提供的标准函数 let：

```kotlin
class JavaConvertExample {
    private var name: String? = null

    fun test() {
//                      标准函数
//                         ↓
        val count = name?.let { it.length }
    }
}
```

第五种，是借助 Kotlin 提供的 lateinit 关键字：

```kotlin
class JavaConvertExample {
//         稍后初始化             不可空
//            ↓                   ↓
    private lateinit var name: String

    fun init() {
        name = "Tom"
    }

    fun test() {
        if (this::name.isInitialized) {
            val count = name.length
        } else {
            println("Please call init() first!")
        }
    }
}

fun main() {
    val example = JavaConvertExample()
    example.init()
    example.test()
}
```

第六种，使用 by lazy 委托：

```kotlin
class JavaConvertExample {
//         不可变        非空   懒加载委托
//           ↓           ↓        ↓
    private val name: String by lazy { init() }
    
    fun init() = "Tom"
    
    fun test() {
        val count = name.length
    }
}
```



### 6.泛型可空性

```kotlin
fun <T> saveSomething(data: T) {}
//   ↑ 
//  等价              
//   ↓                      
fun <T: Any?> saveSomething(data: T) {}
```

解决

```kotlin
// 增加泛型的边界限制              
//       ↓                      
fun <T: Any> saveSomething(data: T) {
    val set = sortedSetOf<T>()
    set.add(data)
}

fun main() {
//              编译无法通过
//                  ↓
    saveSomething(null)
}
```



推测出这段代码的执行结果吗？

```kotlin
fun main() = runBlocking {
    val deferred: Deferred<String> = async {
        println("In async:${Thread.currentThread().name}")
        delay(1000L) // 模拟耗时操作
        println("In async after delay!")
        return@async "Task completed!"
    }

    // 不再调用 deferred.await()
    delay(2000L)
}

//In async ....
//In async after delay....
```





凭什么挂起函数可以调用挂起函数，而普通函数不能调用挂起函数？它的底层逻辑到底什么？

```kotlin
public interface Continuation<in T> {
    /**
     * The context of the coroutine that corresponds to this continuation.
     */
    public val context: CoroutineContext

    /**
     * Resumes the execution of the corresponding coroutine passing a successful or failed [result] as the
     * return value of the last suspension point.
     */
    public fun resumeWith(result: Result<T>)
}
```

suspend函数的入参Continuation，看源码可以知道需要有一个协程上下文CoroutineContext信息，只有在协程作用域里才能传递。





执行结果?

```kotlin
fun main() = runBlocking {
    val job = launch {
        logX("First coroutine start!")
        delay(1000L)
        logX("First coroutine end!")
    }

    job.join()      
    val job2 = launch(job) {
        logX("Second coroutine start!")
        delay(1000L)
        logX("Second coroutine end!")
    }
    job2.join()
    logX("Process end!")
}
```

```
代码的执行结果是：
> First coroutine start!
> First coroutine end!
> Process end!
可见 job2 的代码块并没有被执行。

分析原因：
分别打印出 job2 在 job2.join() 前后的状态：

job2 before join: isActive === false
job2 before join: isCancelled === true
job2 before join: isCompleted === false
// job2.join()
job2 after join: isActive === false
job2 after join: isCancelled === true
job2 after join: isCompleted === true

可见 job2 创建后并没有被激活。

val job2 = launch(job) {} 这一行代码指示 job2 将运行在 job 的 CoroutineContext 之下, 而之前的代码 job.join() 时 job 已经执行完毕了，根据协程结构化的特性，job2 在创建后不会被激活，并且标记为Cancelled，然后执行 job2 时，发现 job2 未被激活，并且已经被取消，则不会执行 job2 的代码块，但是会将 job2 标记为 Completed
```



“挂起函数”与 CoroutineContext 也有着紧密的联系?

```kotlin
import kotlinx.coroutines.*
import kotlin.coroutines.coroutineContext

//                        挂起函数能可以访问协程上下文吗？
//                                 ↓                              
suspend fun testContext() = coroutineContext

fun main() = runBlocking {
    println(testContext())
}
```

suspend方法需要在协程中执行，协程又一定有上下文，所以可以访问的到~ 也就是在suspend方法中可以访问当前协程上下文，并且拿到一些有用的信息



Flow 当中直接使用 withContext{}，是很容易出现问题的

```kotlin
fun main() = runBlocking {
    flow {
        withContext(Dispatchers.IO) {
            emit(1)
        }
    }.map { it * 2 }
        .collect()
}

/*
输出结果
IllegalStateException: Flow invariant is violated
*/
```

不允许在 withContext 里 调用 emit() 是因为 emit() 默认不是线程安全的，而且还给出了一种解决方案，那就是使用 channel 来处理。





## 常用函数

### takeIf

- 返回值：如果**代码块predicate里面返回为true**，则返回这个对象本身，否则返回空
- 使用注意：结果要用?判空

```kotlin
public inline fun <T> T.takeIf(predicate: (T) -> Boolean): T? {
    contract {
        callsInPlace(predicate, InvocationKind.EXACTLY_ONCE)
    }
    return if (predicate(this)) this else null
}
```

e.g

```kotlin
val takeIf = "666".takeIf { it == "666" }
println(takeIf) // 打印 666

val takeIf2 = "666".takeIf { it == "2" }
println(takeIf2) // 打印 null
```

### filter

过滤

```kotlin
public inline fun <T> Iterable<T>.filter(predicate: (T) -> Boolean): List<T> {
    return filterTo(ArrayList<T>(), predicate)
}
public inline fun <T, C : MutableCollection<in T>> Iterable<T>.filterTo(destination: C, predicate: (T) -> Boolean): C {
    for (element in this) if (predicate(element)) destination.add(element)
    return destination
}
```

e.g

```kotlin
val filter = listOf(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
    .filter {
        if (it > 5) {
            return@filter true
        }
        return@filter false
    }
println(filter)// 打印 [6, 7, 8, 9, 10]
```





### groupBy

分组。即根据条件把集合拆分为为一个Map<K,List>类型的集合

```kotlin
public inline fun <T, K> Iterable<T>.groupBy(keySelector: (T) -> K): Map<K, List<T>> {
    return groupByTo(LinkedHashMap<K, MutableList<T>>(), keySelector)
}

public inline fun <T, K, M : MutableMap<in K, MutableList<T>>> Iterable<T>.groupByTo(destination: M, keySelector: (T) -> K): M {
    for (element in this) {
        val key = keySelector(element)
        val list = destination.getOrPut(key) { ArrayList<T>() }
        list.add(element)
    }
    return destination
}
```

e.g

```kotlin
val groupBy = listOf(1, 2, 3, 4, 5)
    .groupBy { it }
println(groupBy) // 打印(LinkedHashMap<Int,ArrayList<Int>>) {1=[1], 2=[2], 3=[3], 4=[4], 5=[5]}
```



### map

变换

```kotlin
public inline fun <T, R> Iterable<T>.map(transform: (T) -> R): List<R> {
    return mapTo(ArrayList<R>(collectionSizeOrDefault(10)), transform)
}
public inline fun <T, R, C : MutableCollection<in R>> Iterable<T>.mapTo(destination: C, transform: (T) -> R): C {
    for (item in this)
        destination.add(transform(item))
    return destination
}
```

e.g

```kotlin
val map = listOf(1, 2, 3, 4, 5)
    .map { "$it" } //变换为字符串
println(map) //打印字符串集合 [1, 2, 3, 4, 5]
```

### mapValues

修改map的Value

```kotlin
public inline fun <K, V, R> Map<out K, V>.mapValues(transform: (Map.Entry<K, V>) -> R): Map<K, R> {
    return mapValuesTo(LinkedHashMap<K, R>(mapCapacity(size)), transform) // .optimizeReadOnlyMap()
}
public inline fun <K, V, R, M : MutableMap<in K, in R>> Map<out K, V>.mapValuesTo(destination: M, transform: (Map.Entry<K, V>) -> R): M {
    return entries.associateByTo(destination, { it.key }, transform)
}
```

e.g

```kotlin
val numbersMap = mapOf("key1" to 1, "key2" to 2, "key3" to 3)
val mapValues = numbersMap.mapValues {
    it.key
}
println(mapValues) //打印 {key1=key1, key2=key2, key3=key3}
```

### maxBy

返回最大值

```kotlin
public inline fun <T, R : Comparable<R>> Iterable<T>.maxBy(selector: (T) -> R): T? {
    return maxByOrNull(selector)
}
public inline fun <T, R : Comparable<R>> Iterable<T>.maxByOrNull(selector: (T) -> R): T? {
    val iterator = iterator()
    if (!iterator.hasNext()) return null
    var maxElem = iterator.next()
    if (!iterator.hasNext()) return maxElem
    var maxValue = selector(maxElem)
    do {
        val e = iterator.next()
        val v = selector(e)
        if (maxValue < v) {
            maxElem = e
            maxValue = v
        }
    } while (iterator.hasNext())
    return maxElem
}
```

e.g

```kotlin
val maxBy = listOf(1, 2, 3, 4, 5).maxBy {
    it
}
println(maxBy)
```

### reduce

聚合操作符

```kotlin
public inline fun <S, T : S> Iterable<T>.reduce(operation: (acc: S, T) -> S): S {
    val iterator = this.iterator()
    if (!iterator.hasNext()) throw UnsupportedOperationException("Empty collection can't be reduced.")
    var accumulator: S = iterator.next()
    while (iterator.hasNext()) {
        accumulator = operation(accumulator, iterator.next())
    }
    return accumulator
}
```

e.g

```kotlin
val numbers = listOf(1, 2, 3, 4)
val sum = numbers.reduce { sum, element ->
    // 1, 2
    // 3, 3
    // 6, 4
    sum + element
}
println(sum) // 打印10
```



### toMutableList

转换可变list

```kotlin
public fun <T> Collection<T>.toMutableList(): MutableList<T> {
    return ArrayList(this)
}
```

e.g

```kotlin
val numbers = listOf(1, 2, 3, 4) // 无法调用add
val toMutableList = numbers.toMutableList()  // 可以调用add
```



### let

```kotlin
public inline fun <T, R> T.let(block: (T) -> R): R {
    contract {
        callsInPlace(block, InvocationKind.EXACTLY_ONCE)
    }
    return block(this)
}
```

e.g

```kotlin
val let = "1,2,3".let { // it
    it.split(",") // 返回值
}
println(let) // 打印[1, 2, 3]
```

### run

```kotlin
public inline fun <T, R> T.run(block: T.() -> R): R {
    contract {
        callsInPlace(block, InvocationKind.EXACTLY_ONCE)
    }
    return block()
}
```

e.g

```kotlin
val run = "1,2,3".run { // this
    split(",")  // 返回值，省去it
}
println(run) // 打印[1, 2, 3]
```

### also

```kotlin
public inline fun <T> T.also(block: (T) -> Unit): T {
    contract {
        callsInPlace(block, InvocationKind.EXACTLY_ONCE)
    }
    block(this)
    return this
}
```

e.g

```kotlin
val also = "1,2,3".also { // it
    println(it.length)
    // 返回的还是原本对象
}
println(also) // 打印 1,2,3 ，返回的还是原本对象
```

### apply

```kotlin
public inline fun <T> T.apply(block: T.() -> Unit): T {
    contract {
        callsInPlace(block, InvocationKind.EXACTLY_ONCE)
    }
    block()
    return this
}
```

e.g

```kotlin
val apply = "1,2,3".apply { // this
    println(length) //省略 it
    // 返回的还是原本对象
}
println(apply) // 打印 1,2,3 ，返回的还是原本对象
```

### take

```kotlin
public fun <T> Iterable<T>.take(n: Int): List<T> {
    require(n >= 0) { "Requested element count $n is less than zero." }
    if (n == 0) return emptyList()
    if (this is Collection<T>) {
        if (n >= size) return toList()
        if (n == 1) return listOf(first())
    }
    var count = 0
    val list = ArrayList<T>(n)
    for (item in this) {
        list.add(item)
        if (++count == n)
            break
    }
    return list.optimizeReadOnlyList()
}
```

e.g

```kotlin
val listOf = listOf(1, 2, 3, 4)
val take = listOf.take(2)
println(take) // 打印[1, 2]
```

