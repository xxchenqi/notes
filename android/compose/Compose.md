# Compose

## 基础控件

```kotlin
//TextView
Text(text = "cq", fontSize = 16.sp, color = Color.Blue)
//Button
Button(onClick = { }) {}
//ImageView
Image(painterResource(id = R.drawable.ic_launcher_background), contentDescription = "无障碍描述")
//FrameLayout
Box {}
//LinearLayout-vertical
Column {}
//LinearLayout-horizontal
Row {}

//RecyclerView
val list = listOf("1", "2", "3")
LazyColumn {
    items(list) { item ->
        Text(item)
    }
}
LazyRow{
    items(list) { item ->
        Text(item)
    }
}
```



## ViewPager

```groovy
implementation "com.google.accompanist:accompanist-pager:0.24.8-beta"
```

```kotlin
HorizontalPager(count = 4, Modifier.weight(1f)) { page ->
    when(page){
        0->chatList()
        1->chatList()
        2->chatList()
        3->chatList()
    }
}
```





## Modifier

### padding

Compose中没有margin，通过调整设置padding的顺序来解决.

```kotlin
Modifier.padding(8.dp).background(Color.Blue)//相当于外边距
Modifier.background(Color.Blue).padding(8.dp)//相当于内边距
Modifier.padding(8.dp).background(Color.Blue).padding(8.dp)//同时设置
```



### background

```kotlin
Column(
    Modifier
        .background(Color.Red, RoundedCornerShape(8.dp)) //RoundedCornerShape 设置圆角
        .padding(8.dp)) {
}
```



### clip

```kotlin
Image(
    painterResource(id = R.drawable.ic_launcher_background),
    contentDescription = "无障碍描述",
    Modifier.clip(CircleShape)//图片圆形切割
)
```



### 尺寸

不设置默认为wrap_content

```kotlin
Modifier.width(66.dp)
Modifier.height(66.dp)
Modifier.size(66.dp)

//类似于match_parent
Modifier.fillMaxWidth()
Modifier.fillMaxHeight()
Modifier.fillMaxSize()
```



### click

注意clickable的位置，会导致点击区域不一样。

```kotlin
Column(
    Modifier
        .padding(8.dp)
        .clickable { }//注意位置
        .background(Color.Red, RoundedCornerShape(8.dp))
        .padding(8.dp)
) {
    Text(text = "cq", fontSize = 16.sp, color = Color.Blue)
    Image(
        painterResource(id = R.drawable.ic_launcher_background),
        contentDescription = "无障碍描述",
        Modifier.clip(CircleShape)
    )
}
```



### 设置顺序

从右往左设置

```kotlin
Modifier.size(10.dp).size(20.dp)//结果为10.dp
```

LayoutNode

```kotlin
override var modifier: Modifier = Modifier
    set(value) {
        //不断装箱,所以解析从右往左
    	val outerWrapper = modifier.foldOut(innerLayoutNodeWrapper)
    }
```





### other

CombinedModifier 2个合成一个的Modifier

Element 除了compain object:Modifier 和 CombinedModifier ，Modifier里的所有接口和实现类都继承Element



```kotlin
interface Modifier {
    fun <R> foldIn(initial: R, operation: (R, Element) -> R): R

    fun <R> foldOut(initial: R, operation: (Element, R) -> R): R

    fun any(predicate: (Element) -> Boolean): Boolean

    fun all(predicate: (Element) -> Boolean): Boolean

    infix fun then(other: Modifier): Modifier =
        if (other === Modifier) this else CombinedModifier(this, other)
    
    
    Element ...
    companion object : Modifier ...
}
```

```kotlin
interface Element : Modifier {
    override fun <R> foldIn(initial: R, operation: (R, Element) -> R): R =
        operation(initial, this)
    override fun <R> foldOut(initial: R, operation: (Element, R) -> R): R =
        operation(this, initial)
    override fun any(predicate: (Element) -> Boolean): Boolean = predicate(this)
    override fun all(predicate: (Element) -> Boolean): Boolean = predicate(this)
}
```

```kotlin
companion object : Modifier {
    override fun <R> foldIn(initial: R, operation: (R, Element) -> R): R = initial
    override fun <R> foldOut(initial: R, operation: (Element, R) -> R): R = initial
    override fun any(predicate: (Element) -> Boolean): Boolean = false
    override fun all(predicate: (Element) -> Boolean): Boolean = true
    override infix fun then(other: Modifier): Modifier = other
    override fun toString() = "Modifier"
}
```

```kotlin
class CombinedModifier(
    private val outer: Modifier,
    private val inner: Modifier
) : Modifier {
    override fun <R> foldIn(initial: R, operation: (R, Modifier.Element) -> R): R =
        inner.foldIn(outer.foldIn(initial, operation), operation)

    override fun <R> foldOut(initial: R, operation: (Modifier.Element, R) -> R): R =
        outer.foldOut(inner.foldOut(initial, operation), operation)

    override fun any(predicate: (Modifier.Element) -> Boolean): Boolean =
        outer.any(predicate) || inner.any(predicate)

    override fun all(predicate: (Modifier.Element) -> Boolean): Boolean =
        outer.all(predicate) && inner.all(predicate)

    override fun equals(other: Any?): Boolean =
        other is CombinedModifier && outer == other.outer && inner == other.inner

    override fun hashCode(): Int = outer.hashCode() + 31 * inner.hashCode()

    override fun toString() = "[" + foldIn("") { acc, element ->
        if (acc.isEmpty()) element.toString() else "$acc, $element"
    } + "]"
}
```





## 层级关系

```
decoverView(FramleLayout)
	LinearLayout
		android.R.id.content
			ComposeView
				AndroidComposeView
					root:LayoutNode
						LayoutNode--自己写的控件
						...


```



```
调用处:Text("Hello")

SlotTable //有任何变化，从老的结构变为新的结构，以最低成本更新

显示：LayoutNode
```



## setContent流程

```kotlin
ComponentActivity.setContent
ComposeView.setContent
createComposition
ensureCompositionCreated
ViewGroup.setContent
doSetContent
WrappedComposition.setContent
CompositionImpl.setContent
Recomposer.composeInitial
CompositionImpl.composeContent
ComposerImpl.composeContent
doCompose
invokeComposable(this, content)  //content: (@Composable () -> Unit)?

internal fun invokeComposable(composer: Composer, composable: @Composable () -> Unit) {
    val realFn = composable as Function2<Composer, Int, Unit> //@Composable 由编译器识别后，处理添加参数,
    realFn(composer, 1)//调用的就是setContent{}里的内容
}

```



## Layout流程

```kotlin
ReusableComposeNode<ComposeUiNode, Applier<Any>>(
    factory = ComposeUiNode.Constructor,
    update = {
        set(measurePolicy, ComposeUiNode.SetMeasurePolicy)
        set(density, ComposeUiNode.SetDensity)
        set(layoutDirection, ComposeUiNode.SetLayoutDirection)
        set(viewConfiguration, ComposeUiNode.SetViewConfiguration)
        set(materialized, ComposeUiNode.SetModifier)
    },
)

currentComposer.createNode { factory() } 
    recordFixup { applier, slots, _ ->
        val node = factory() //创建LayoutNode
        	//factory = ComposeUiNode.Constructor,
            //val Constructor: () -> ComposeUiNode = LayoutNode.Constructor
            //internal val Constructor: () -> LayoutNode = { LayoutNode() }
        slots.updateNode(groupAnchor, node)
        val nodeApplier = applier as Applier<T>
        nodeApplier.insertTopDown(insertIndex, node)
        	//UiApplier.insertTopDown
        applier.down(node)
    }
    recordInsertUpFixup { applier, slots, _ ->
        val nodeToInsert = slots.node(groupAnchor)
        applier.up()
        val nodeApplier = applier as Applier<Any?>
        nodeApplier.insertBottomUp(insertIndex, nodeToInsert)
        	//UiApplier.insertBottomUp
			//current.insertAt(index, instance)
				//current 是 AndroidComposeView 的 root
            //LayoutNode.insertAt
			//最终放进根的LayoutNode里
    }



set(measurePolicy, ComposeUiNode.SetMeasurePolicy)
...

将Modifier赋值给新创建的LayoutNode

```





## @Composeable规则

```
1.所有加了@Composeable注解的函数,都需要在别的加了@Composeable的函数里面才能调用
2.所有内部没调用任何其他@Composeable函数的函数，都没必要加上这个注解，因为没意义
```





## 状态更新

### remember

相当于全局缓存，第一次调用会执行{...}，把结果返回，并且保存在全局slotTable，之后执行会直接返回保存的结果

```kotlin
setContent {
    //数据更新后,此处代码会重新执行,这样就导致了name字段没更新
    //var name by mutableStateOf("cq")
    
    //使用remember后,相当于全局缓存
    //第一次调用会执行{...}，把结果返回，并且保存在全局slotTable，之后执行会直接返回保存的结果
    //这里的by是代理，可以让你少写.value
    var name by remember { mutableStateOf("cq") }
    Text(name)
    lifecycleScope.launch {
        delay(3000)
        name = "cq2"
    }
}

//带key的缓存策略
var name by remember(key1,key2) { mutableStateOf("cq") }
```



State Hoisting : 状态上提

原则：能不往上提,就不往上提，越往外,能访问到的范围就越广，出错概率就越高





```kotlin
 //list 状态监听,不使用 by
 var list = mutableStateListOf(0)
```



### 可靠性

```kotlin
//val可靠,compose使用结构性来判断是否相等(equals)
data class User(var name: String)

//var不可靠,compose使用引用性来判断是否相等(==)
data class User(var name: String)
```



```kotlin
    //list 状态监听,不使用 by
    var list = mutableStateListOf(0)

    var user = User("cq")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            Column {
                Button(onClick = {
                    list.add(list.last() + 1)

                }) {
                    Text("+1")
                    user = User("cq")
                }
                test(user)

                list.forEach {
                    Text(it.toString())
                }
            }
        }
    }

    @Composable
    fun test(user: User) {
        //如果是user是可靠的,方法会进入,但下面的代码块是不会执行的,有编译器会生成部分代码判断
        //反之则执行
        println("修改")
        Text(user.name)
    }

    //val可靠,compose使用结构性来判断是否相等(equals)
    data class User(val name: String)
    //var不可靠,compose使用引用性来判断是否相等(==)
//    data class User(var name: String)
```



### @Stable

强制可靠





## 动画

基本设置

```kotlin
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        var big by mutableStateOf(false)
        setContent {
            // 不会直接修改值,而是开启协程,多次频繁逐渐修改值
            val size by animateDpAsState(if (big) 96.dp else 48.dp)
            println("修改$size") //多次打印(size是渐变的)
//            val size by remember {
//                mutableStateOf(if (big) 96.dp else 48.dp)
//            }
            Box(Modifier.size(size).background(Color.Green).clickable { big = !big }) {
            }
        }
    }
```

初始值设置

```kotlin
override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        var big by mutableStateOf(false)
        setContent {
            val anim = remember {
                Animatable(48.dp, Dp.VectorConverter)
            }
            //Compose专用协程,在ReCompose中不会重复执行,如果需要执行可以通过参数的改变
            LaunchedEffect(big) {
                //设置起始值
                anim.snapTo(if (big) 144.dp else 0.dp)
                //spring：设置回弹效果
//                anim.animateTo(if (big) 96.dp else 48.dp, spring(Spring.DampingRatioMediumBouncy))
                //tween设置动画时长
                anim.animateTo(if (big) 96.dp else 48.dp, tween(2000))
            }
            println("修改${anim.value}") //多次打印(size是渐变的)
            Box(
                Modifier
                    .size(anim.value)
                    .background(Color.Green)
                    .clickable { big = !big }) {
            }
        }
    }
```

并行的多属性动画用

```kotlin
override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    var big by mutableStateOf(false)
    setContent {
        //创建,每次执行内部会更新状态,并且是渐变过程,内部会记录百分比状态值
        //并行的多属性动画用Transition(性能更好)
        val bigTransition = updateTransition(big)
        val size by bigTransition.animateDp { if (it) 96.dp else 48.dp }
        val cornerSize by bigTransition.animateDp { if (it) 16.dp else 0.dp }
        println("修改${size}") //多次打印(size是渐变的)
        Box(
            Modifier
                .size(size)
                .clip(RoundedCornerShape(cornerSize))
                .background(Color.Green)
                .clickable { big = !big }) {
        }
    }
}
```

```kotlin
@Preview
@Composable
fun TransitionTest() {
    var big by remember {
        mutableStateOf(false)
    }
	//label：动画调试时设置的观察属性
    val bigTransition = updateTransition(big, label = "big")
    val size by bigTransition.animateDp(label = "size") { if (it) 96.dp else 48.dp }
    val cornerSize by bigTransition.animateDp(label = "cornerSize") { if (it) 16.dp else 0.dp }

    Box(
        Modifier
            .size(size)
            .clip(RoundedCornerShape(cornerSize))
            .background(Color.Green)
            .clickable { big = !big }) {
    }
}

```



## 自定义View

自定义绘制

```kotlin
setContent {
    Text("cq", Modifier.drawWithContent {
        drawRect(Color.Red)
        //原本的绘制
        drawContent()
    })
    
    //专门用来绘制背景
    Modifier.drawBehind {}
    
    Modifier.drawWithCache {
        //准备工作
        val path = Path()
        onDrawWithContent {
            //TODO 绘制背景
            drawContent()
            //TODO 绘制前景
            //...
            //drawPath(path)
        }
    }
    
    //完全自定义绘制
	Canvas(Modifier) {}
    
}
```

自定义布局

```kotlin
setContent {
    Text(text = "cqcq",
        Modifier
            .background(Color.Red)
            // measurable = LayoutNode/ModifierLayoutNode
            // constraints = 限制
            .layout { measurable, constraints ->
                val padding = 8.dp
                    .toPx()
                    .roundToInt()
                //自定义限制
                val paddedConstrains = constraints
                    .copy()
                    .apply {
                        constrainWidth(maxWidth - padding * 2)
                        constrainHeight(maxHeight - padding * 2)
                    }
                //测量组件,placeable是测量完成的可以摆放的对象
                val placeable = measurable.measure(paddedConstrains)
                //摆放内部组件
                //参数：向上汇报尺寸
                layout(placeable.width + padding * 2, placeable.height + padding * 2) {
                    //提供摆放组件的逻辑
                    placeable.placeRelative(padding, padding)
                }
            }
            .background(Color.Green)
    )
}
```



```kotlin
@Composable
fun Custom(modifier: Modifier = Modifier, content: @Composable () -> Unit) {
    Layout(content, modifier) { measurables, costraints ->
        var width = 0
        var height = 0
        val placeables = mutableListOf<Placeable>()
        for (measurable in measurables) {
            val placeable = measurable.measure(costraints)
            width = max(width, placeable.width)
            height += placeable.height
            placeables.add(placeable)
        }
        layout(width, height) {
            var currentHeight = 0
            for (placeable in placeables) {
                placeable.placeRelative(0, currentHeight)
                currentHeight += placeable.height
            }
        }
    }
}
```



无法重复测量问题

```
measurable.measure(costraints)//只能调用1次，调用多次报错
```



使用IntrinsicSize做固有特性测量

```
setContent {
    Row(Modifier.height(IntrinsicSize.Max)) {
        Text("cq1")
        Divider(Modifier.width(1.dp).fillMaxHeight(),Color.Black)
        Text("cq2")
    }
}
```



触摸

TODO





## 传统View交互

1.方式1

```xml
<androidx.compose.ui.platform.ComposeView
        android:id="@+id/composeView"
        android:layout_width="match_parent"
        android:layout_height="match_parent"/>
```

```kotlin
setContentView(R.layout.test)
val composeView = findViewById<ComposeView>(R.id.composeView)
composeView.setContent {
    
}
```

2.方式2

```kotlin
val num = MutableLiveData<Int>(1)
setContent {
   val numCompose by num.observeAsState()
    Column {
        Text("cq$numCompose")
        AndroidView({
            //只执行1次
            ImageView(this@MainActivity).apply {
                setImageResource(R.drawable.ic_launcher_background)
            }
        }, Modifier.size(48.dp)) {
            //Recompose过程执行 (用来更新)
        }
    }
}
```

