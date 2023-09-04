# Rust

##  `fn` 关键字定义函数。

```rust
fn main(){
    println!("Hello Rust!")
}
```

**宏** 都会以 **感叹号 ( `!` )** 结尾。以后看到以 `!` 结尾的类似函数调用，都是 **宏调用**。



## `let` 关键字来声明和定义一个变量

```
let 变量名=值
```

| 大小    | 有符号 | 无符号 |
| ------- | ------ | ------ |
| 8 bit   | i8     | u8     |
| 16 bit  | i16    | u16    |
| 32 bit  | i32    | u32    |
| 64 bit  | i64    | u64    |
| 128 bit | i128   | u128   |
| Arch    | isize  | usize  |

整型可以进一步划分为 `1字节`、`2字节`、`4字节`、`8字节`、`16字节`。1 字节 = 8 位

整型的长度还可以是 `arch`。`arch` 是由 CPU 构架决定的大小的整型类型。大小为 `arch` 的整数在 `x86` 机器上为 `32` 位，在 `x64` 机器上为 `64` 位。

```rust
let price = 100;    // i32 默认
let price2:u32 = 200;
let price3:i32 = -300;
let price4:isize = 400;
let price5:usize = 500;

println!("price is {}", price);
//输出 price is 100

println!("price2 is {} and price3 is {}", price3, price2);
//输出 price2 is -300 and price3 is 200

println!("price4 is {} and price5 is {}", price4, price5);
//输出 price4 is 400 and price5 is 500
```



溢出：

```rust
let price7:i8=192;
println!("price7 is {}", price7);

报错如下：
16 |     let price7:i8=192;
 |                   ^^^
 |
 = note: `#[deny(overflowing_literals)]` on by default
 = note: the literal `192` does not fit into the type `i8` whose range is `-128..=127`
 = help: consider using the type `u8` instead
```



浮点型划分为 `f32` 和 `f64`。其中 `f64` 是默认的浮点类型。

- `f32` 又称为 **单精度浮点型**。
- `f64` 又称为 **双精度浮点型**，它是 Rust 默认的浮点类型.

```rust
let price9 = 18.00;        // 默认是 f64
let price10:f32 = 8.88;
let price11:f64 = 168.125;  // 双精度浮点型

println!("price9 {}", price9); //输出 price9 18
println!("price10 {}", price10);//输出 price10 8.88
println!("price11 {}", price11);//输出 price11 168.125
```



使用 bool 关键字来声明一个 布尔类型 的变量。

```rust
let checked:bool = true;
println!("checked {}", checked);//输出 checked true
```



## 定义变量

```
let 变量名 = 值;           // 不指定变量类型
let 变量名:数据类型 = 值;   // 指定变量类型
```



### let 关键字-不可变变量

```
let price = 188;
price=288;
print!("{}",price);

编译器报错提示：Cannot assign twice to immutable variable [E0384]
```

### mut 关键字-可变变量

```
let mut 变量名 = 值;
let mut 变量名:数据类型 = 值;
```

```
let mut price = 188;
price=288;
print!("{}",price); //输出 288
```



## 常量

- const：不可改变的值（通常使用这种）。
- static：具有 ‘static 生命周期的，可以是可变的变量（须使用 static mut 关键字）。

有个特例就是 “string” 字面量。它可以不经改动就被赋给一个 static 变量，因为它 的类型标记：&’static str 就包含了所要求的生命周期 ‘static。其他的引用类型都 必须特地声明，使之拥有’static 生命周期。



定义

```
const 常量名称:数据类型=值;
```



变量的隐藏

```
let name ="《Go语言极简一本通》";
let name="《从0到Go语言微服务架构师》";
println!("{}",name); //输出 《从0到Go语言微服务架构师》
```

```
let price=199;
let price="299";
println!("{}",price);//输出 299
```



同名常量

Rust 中，**常量不能被隐藏，也不能被重复定义**。

```
const DISCOUNT:f64=0.8;
const DISCOUNT:f64=0.6;

编辑器报错
A value named `DISCOUNT` has already been defined in this block [E0428]
```



static

```
static BOOK: &'static str = "《Go语言极简一本通》";
```



## 字符串

- Rust 核心内置的数据类型`&str`，字符串字面量 。
- Rust 标准库中的一个 **公开 `pub`** 结构体。字符串对象 `String`。



### 字面量&str

字符串字面量的核心代码可以在模块 `std::str` 中找到。Rust 中的字符串字面量被称之为 **字符串切片**。因为它的底层实现是 **切片**。



### 字符串对象

字符串对象并不是 Rust 核心内置的数据类型，它只是标准库中的一个 **公开 `pub`** 的结构体。

```
String::new() //创建一个新的空字符串,它是静态方法。
String::from() //从具体的字符串字面量创建字符串对象。
```





## 条件判断

| 条件判断语句                | 说明                                                        |
| --------------------------- | ----------------------------------------------------------- |
| `if` 语句                   | `if` 语句用于模拟现实生活中的 **如果…就…**                  |
| `if...else` 语句            | `if...else` 语句用于模拟 **如果…就…否则…**                  |
| `else...if` 和嵌套`if` 语句 | 嵌套`if` 语句用于模拟 **如果…就…如果…就…**                  |
| `match` 语句                | `match` 语句用于模拟现实生活中的 **老师点名** 或 **银行叫** |



```rust
match variable_expression {
   constant_expr1 => {
      // 语句;
   },
   constant_expr2 => {
      // 语句;
   },
   _ => {
      // 默认
      // 其它语句
   }
};

let code = "10010";
let choose = match code {
   "10010" => "联通",
   "10086" => "移动",
   _ => "Unknown"
};
println!("选择 {}", choose);
//输出 选择 联通


let code = "80010";
let choose = match code {
   "10010" => "联通",
   "10086" => "移动",
   _ => "Unknown"
};
println!("选择 {}", choose);
//输出 选择 Unknown
```



## 循环

- `loop` 语句。一种重复执行且永远不会结束的循环。
- `while` 语句。一种在某些条件为真的情况下就会永远执行下去的循环。
- `for` 语句。一种有确定次数的循环。



```rust
for 临时变量 in 左区间..右区间 {
   // 执行业务逻辑
}

for num in 1..5{
   println!("num is {}", num);
}
//输出
num is 1
num is 2
num is 3
num is 4

// 可以使用 a..=b 表示两端都包含在内的范围。
for num in 1..=5 {
   println!("num is {}", num);
}
输出
num is 1
num is 2
num is 3
num is 4
num is 5
```

iter - 在每次迭代中借用集合中的一个元素。这样集合本身不会被改变，循环之后仍可以使用。

```rust
let studyList = vec![
	"《Go语言极简一本通》",
	"Go语言微服务架构核心22讲",
	"从0到Go语言微服务架构师",
];
for name in studyList.iter() {
    match name {
        &"从0到Go语言微服务架构师" => println!("恭喜你进阶到第三阶段-{}!", name),
        _ => println!("学习: {}", name),
    }
}
//输出
学习: 《Go语言极简一本通》
学习: Go语言微服务架构核心22讲
恭喜你进阶到第三阶段-从0到Go语言微服务架构师!
```

into_iter - 会消耗集合。在每次迭代中，集合中的数据本身会被提供。一旦集合被消耗了，之后就无法再使用了，因为它已经在循环中被 “移除”（move）了。

```rust
let studyList2 = vec![
	"《Go语言极简一本通》",
	"Go语言微服务架构核心22讲",
	"从0到Go语言微服务架构师",
];
for name in studyList2.into_iter() {
    match name {
        "从0到Go语言微服务架构师" => println!("恭喜你进阶到第三阶段-{}!", name),
        _ => println!("学习: {}", name),
    }
}

输出
学习: 《Go语言极简一本通》
学习: Go语言微服务架构核心22讲
恭喜你进阶到第三阶段-从0到Go语言微服务架构师!
```

iter_mut - 可变地（mutably）借用集合中的每个元素，从而允许集合被就地修改。
就是停止本次执行剩下的语句，直接进入下一个循环。

```rust
let mut studyList3 = vec![
    "《Go语言极简一本通》",
    "Go语言微服务架构核心22讲",
    "从0到Go语言微服务架构师",
];
for name in studyList3.iter_mut() {
    *name = match name {
        &mut "从0到Go语言微服务架构师" => {
            "恭喜你进阶到第三阶段---从0到Go语言微服务架构师"
        }
        _ => *name,
    }
}
println!("studyList3: {:?}", studyList3);
输出
studyList3: ["《Go语言极简一本通》", "Go语言微服务架构核心22讲", "恭喜你进阶到第三阶段---从0到Go语言微服务架构师"]
```



```rust
while ( 条件表达式 ) {
    // 执行业务逻辑
}

let mut num = 1;
while num < 20{
   println!("num is {}",num);
   num= num*2;
}
//输出
num is 1
num is 2
num is 4
num is 8
num is 16
```



```rust
loop {
    // 执行业务逻辑
}
break; 中断的意思，就是跳出loop循环

let mut num = 1;
loop {
   if num > 20{
      break;
   }
   println!("num is {}",num);
   num= num*3;
}
//输出
num is 1
num is 3
num is 9
```



## 函数

```rust
fn 函数名称([参数:数据类型]) -> 返回值 {
   // 函数代码
}
```



### 函数返回值

返回值定义语法，在 **小括号后面使用 箭头 ( `->` ) 加上数据类型** 来定义。

```rust
fn 函数名称() -> 返回类型 {
   return 返回值;
}
```

如果函数代码中没有使用 `return` 关键字，那么函数会默认使用最后一条语句的执行结果作为返回值。

```rust
fn 函数名称() -> 返回类型 {
   // 业务逻辑
   返回值 // 没有分号则表示返回值
}
```



### 参数-值传递

```rust
fn double_price(mut price:i32){
    price=price*2;
    println!("内部的price是{}",price)
}

fn main() {
    let mut price=99;
    double_price(price); //输出 内部的price是198
    println!("外部的price是{}",price); //输出 外部的price是99
}
```



### 参数-引用传递

引用传递在参数类型的前面加上 `&` 符号

**星号（`*`）** 用于访问变量 `price` 指向的内存位置上存储的变量的值。这种操作也称为 **解引用**。 因此 **星号（`*`）** 也称为 **解引用操作符**。

```rust
fn 函数名称(参数: &数据类型) {
   // 执行逻辑代码
}

fn double_price2(price:&mut i32){
    *price=*price*2;
    println!("内部的price是{}",price)
}

fn main() {
    let mut price=88;
    double_price2(&mut price); //输出 内部的price是176
    println!("外部的price是{}",price);//输出 外部的price是176
}
```



### 复合类型传参

对于复合类型，比如字符串，如果按照普通的方法传递给函数后，那么该变量将不可再访问。

```rust
fn show_name(name:String){
    println!("充电科目 :{}",name);
}

fn main() {
    let name:String = String::from("从0到Go语言微服务架构师");
    show_name(name);
    println!("调用show_name函数后: {}",name);
}

报错如下
error[E0382]: borrow of moved value: `name`
let name:String = String::from("从0到Go语言微服务架构师");
  |---- move occurs because `name` has type `String`, which does not implement the `Copy` trait
  |show_name(name);
  |  ---- value moved here
  |println!("调用show_name函数后: {}",name);
  |  ^^^^ value borrowed here after move
```



## 元组(tuple)

Tuple 元组是一个 **复合类型** ，可以存储多个不同类型的数据。 

```rust
let tuple变量名称:(数据类型1,数据类型2,...) = (数据1，数据2，...);
let tuple变量名称 = (数据1，数据2，...);
```

注：tuple 使用一对小括号 `()` 把所有元素放在一起，元素之间使用逗号 `,` 分隔。如果显式指定了元组的数据类型，那么数据类型的个数必须和元组的个数相同，否则会报错。

```rust
fn main() {
    let t:(&str,&str) = ("Go语言极简一本通","掌握Go语言语法，并且可以完成单体服务应用");
    println!("{:?}",t);
}
```

访问元素

```rust
元组变量.索引数字

println!("{}",t.0);//输出 Go语言极简一本通
println!("{}",t.1);//输出 掌握Go语言语法，并且可以完成单体服务应用
```

元组作参数

```rust
fn 函数名称(tuple参数名称:(&str,i32)){}

fn show_tuple(tuple:(&str,&str)){
    println!("{:?}",tuple);
}

show_tuple(t);

//输出 ("Go语言极简一本通", "掌握Go语言语法，并且可以完成单体服务应用")
```



元组解构

**元组 ( tuple )解构** 就是在 tuple 中的每一个元素按照顺序一个一个赋值给变量。使用 **=** ，让右边的 tuple 按照顺序给等号左变的变量一个一个赋值。

```rust
let (book,target)=t;
println!("{}",book);//输出 Go语言极简一本通
println!("{}",target);//输出 掌握Go语言语法，并且可以完成单体服务应用
```



## 数组

```rust
let 变量名:[数据类型;数组长度]=[值1,值2,值3,...];
let arr1:[&str;3]=["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];

let 变量名=[值1,值2,值3,...];
let arr2=["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];

let 变量名:[数据类型;数组长度]=[默认值,数组长度];
let arr3:[&str;3]=["";3];

获取数组长度 len()
print!("{}",arr1.len());
```

遍历数组

```rust
for item in arr1{
   print!("充电科目 :{}\n",item);
}
//输出
充电科目 :Go语言极简一本通
充电科目 :Go语言微服务架构核心22讲
充电科目 :从0到Go语言微服务架构师

for item in arr1.iter(){
   print!("已参加的充电科目 :{}\n",item);
}
//输出
已参加的充电科目 :Go语言极简一本通
已参加的充电科目 :Go语言微服务架构核心22讲
已参加的充电科目 :从0到Go语言微服务架构师

如果修改一个不可变数组，报错如下：
arr2[0]="";
error[E0594]: cannot assign to `arr2[_]`, as `arr2` is not declared as mutable

如果想修改这个错误，声明数组的时候，添加 mut 关键字
```



**值传递** 传递一个数组的副本，副本的修改，不会影响原数组。

```rust
 let mut arr2=["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
 print!("{:?}\n",arr2);
 show_arr(arr2);
 print!("{:?}\n",arr2);

fn show_arr(arr:[&str;3]){
    let l = arr.len();
    for i in 0..l {
       println!("充电科目: {}",arr[i]);
    }
}

//输出
["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
充电科目: Go语言极简一本通
充电科目: Go语言微服务架构核心22讲
充电科目: 从0到Go语言微服务架构师
["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
```



**引用传递** 传递内存的地址给函数，修改数组的任何值都会修改原来的数组。

```rust
fn modify_arr(arr:&mut [&str;3]){
    let l = arr.len();
    for i in 0..l {
        arr[i]="";
    }
}

let mut arr3=["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
print!("{:?}\n",arr3);
modify_arr(&mut arr3);
print!("{:?}\n",arr3);
```



## 所有权

### 概念

因为变量要负责释放它们拥有的资源，所以**资源只能拥有一个所有者**。这也防止了资源的重复释放。注意并非所有变量都拥有资源（例如引用）。

在进行赋值（let a = b）或通过值来传递函数参数（foo(a)）的时候，资源的所有权（ownership）会发生转移。按照 Rust 的规范，这被称为资源的移动（move）。

在移动资源之后，原来的所有者不能再被使用，这可避免悬挂指针（dangling pointer）的产生。

### 转让所有权

1.把一个变量赋值给另一个变量。

```rust
fn main() {
     // 栈分配的整型
    let a = 88;
    // 将a复制到b——不存在资源移动
    let b = a;
    // 两个值各自都可以使用
    println!("a {}, and b {}", a, b);

    let v1 = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
    let v2 =v1;
    println!("{:?}",v1);
}

报错如下:
error[E0382]: borrow of moved value: `v1`
let v1 = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
| -- move occurs because `v1` has type `Vec<&str>`, which does not implement the `Copy` trait
9 | let v2 =v1;
| -- value moved here
10 | println!("{:?}",v1);
| ^^ value borrowed here after move
|
```



- v1 拥有堆上数据的所有权。（每次只能有一个变量对堆上数据有所有权）
- v2=v1 v2 拥有了堆上数据的所有权。
- v1 已经没有对数据的所有权了，所以再使用 v1 会报错。
- 如果 Rust 检查到 2 个变量同时拥有堆上内存的所有权。会报错如上。



2.把变量传递给函数参数。

```rust
fn show(v:Vec<&str>){
    println!("v {:?}",v)
}
fn main() {
    let studyList = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
    //studyList 拥有堆上数据管理权
    let studyList2 = studyList;
    // studyList 将所有权转义给了 studyList2
    show(studyList2);
    // studyList2 将所有权转让给参数 v,studyList2 不再可用。
    println!("studyList2 {:?}",studyList2);
    //studyList2 已经不可用。
}

error[E0382]: borrow of moved value: `studyList2`
| let studyList2 = studyList; // studyList 将所有权转义给了 studyList2
| ---------- move occurs because `studyList2` has type `Vec<&str>`, which does not implement the `Copy` trait
| show(studyList2); // studyList2 将所有权转让给参数 v,studyList2 不再可用。
| ---------- value moved here
| println!("studyList2 {:?}",studyList2);//studyList2 已经不可用。
| ^^^^^^^^^^ value borrowed here after move
```



3.函数中的返回值。

```rust
fn show2(v:Vec<&str>) -> Vec<&str>{
    println!("v {:?}",v);
    return v;
}

fn main() {
    let studyList3 = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
    let studyList4 = studyList3;
    let result = show2(studyList4);
    println!("result {:?}",result);
    //输出 result ["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
}
```





### 基础数据类型与所有权

所有权只会发生在堆上分配的数据，基础数据类型(整型，浮点型，布尔，字符)存储在栈上，所以没有所有权的概念。基础类型可以认为是值拷贝，在内存上另外的地方，存储和复制来的数据，然后让新的变量指向它。

```rust
let a = 88;
let b = a;
println!("a {}, and b {}", a, b);
```



## Borrowing 借用所有权

一个函数中的变量传递给另外一个函数作为参数暂时使用。也会要求函数参数离开自己作用域的时候将**所有权** 还给当初传递给它的变量

```
&变量名  //要把参数定义的时候这样定义。
```



```rust
fn show(v:&Vec<&str>){
    println!("v:{:?}",v)
}
fn main() {
    let studyList = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
    let studyList2 =studyList;
    show2(&studyList2);
    println!("studyList2:{:?}",studyList2); //我们看到，函数show使用完v2后，我们仍然可以继续使用
}
//输出
v:["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
v2:["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
```



### 可变的借用

```rust
fn show2(v:&Vec<&str>){
    v[0]="第一个充电项目已完成";
    println!("v:{:?}",v)
}
报错如下:
error[E0596]: cannot borrow `*v` as mutable, as it is behind a `&` reference
| fn show2(v:&Vec<&str>){
| ---------- help: consider changing this to be a mutable reference: `&mut Vec<&str>`
| v[0]="第一个充电项目已完成";
| ^ `v` is a `&` reference, so the data it refers to cannot be borrowed as mutable
```

报错的原因：我们的这个借用不可以是可变的。那么 Rust 中，如果想要让一个变量是可变的，只能在前面加上 **mut** 关键字。



```rust
fn show2(v:&mut Vec<&str>){
    v[0]="第一个充电项目已完成";
    println!("v:{:?}",v)
}
fn main() {
 let mut studyList3 = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
 println!("studyList3:{:?}",studyList3);
 show2(&mut studyList3);
 println!("调用后，studyList3:{:?}",studyList3);
 }
 //输出
studyList3:["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
v:["第一阶段学习已完成", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
调用后，studyList3:["第一阶段学习已完成", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
```





## Slice(切片)

切片是指向一段连续内存的指针。在 Rust 中，连续内存够区间存储的数据结构：数组(array)、字符串(string)、向量(vector)。

**切片**是运行时才能确定的，并不像数组那样编译时就已经确定了。

```
let 切片值 = &变量[起始位置..结束位置]
```

1. [起始位置..结束位置]，这是一个左闭右开的区间。
2. **起始位置**最小值是**0**。
3. **结束位置**是数组、向量、字符串的长度。

```rust
fn main() {
   let mut v = Vec::new();
   v.push("Go语言极简一本通");
   v.push("Go语言微服务架构核心22讲");
   v.push("从0到Go语言微服务架构师");
   println!("len:{:?}",v.len());
   let s1=&v[1..3];
   println!("s1:{:?}",s1);
}
//输出
len:3
s1:["Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
```



切片当参数

```rust
fn show_slice(s:&[&str]){
   println!("show_slice函数内:{:?}",s);
}
show_slice(s1);//把上面的s1传递给函数show_slice
//输出 show_slice函数内:["Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
```



可变切片

```rust
fn modify_slice(s: &mut [&str]) {
   s[0] = "这个阶段已学习完毕";
   println!("modify_slice:{:?}", s);
}
let mut v2 = Vec::new();
v2.push("Go语言极简一本通");
v2.push("Go语言微服务架构核心22讲");
v2.push("从0到Go语言微服务架构师");
println!("modify_slice之前:{:?}", v2);

modify_slice(&mut v2[1..3]);
println!("modify_slice之后:{:?}", v2);

//输出
modify_slice之前:["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"]
modify_slice:["这个阶段已学习完毕", "从0到Go语言微服务架构师"]
modify_slice之后:["Go语言极简一本通", "这个阶段已学习完毕", "从0到Go语言微服务架构师"]
```



## 结构体(Struct)

元组结构体（tuple struct）

```
struct Pair(String, i32);
```

经典的 C 语言风格结构体

```
struct 结构体名称 {
    ...
}
```

单元结构体（unit struct），不带字段，在泛型中很有用。

```
struct Unit;
```



### 创建结构体实例

```
let 实例名称 = 结构体名称{
    field1:value1,
    field2:value2
    ...
};
```

```rust
#[derive(Debug)]
struct Study {
    name: String,
    target: String,
    spend: i32,
}

fn main() {
    let s = Study {
        name: String::from("从0到Go语言微服务架构师"),
        target: String::from("全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。"),
        spend: 3,
    };
    println!("{:?}", s);
    //输出 Study { name: "从0到Go语言微服务架构师", target: "全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。", spend: 3 }
}
```

### 访问实例属性

```
实例名称.属性

println!("{}",s.name);//输出 从0到Go语言微服务架构师
```



### 修改结构体实例

结构体实例默认是**不可修改**的，如果想修改结构体实例，声明时使用**mut**关键字。

```rust
let mut s2 = Study {
        name: String::from("从0到Go语言微服务架构师"),
        target: String::from("全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。"),
        spend: 3,
    };
 s2.spend=7;
 println!("{:?}",s2);//输出 Study { name: "从0到Go语言微服务架构师", target: "全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。", spend: 7 }
```

### 结构体做参数

```rust
fn show(s: Study) {
    println!(
        "name is :{} target is {} spend is{}",
        s.name, s.target, s.spend
    );
}
```

### 结构体作为返回值

```rust
fn get_instance(name: String, target: String, spend: i32) -> Study {
    return Study {
        name,
        target,
        spend,
    };
}

let s3 = get_instance(
    String::from("Go语言极简一本通"),
    String::from("学习Go语言语法，并且完成一个单体服务"),
    5,
);
println!("{:?}", s3);
//输出 Study { name: "Go语言极简一本通", target: "学习Go语言语法，并且完成一个单体服务", spend: 5 }
```



### 方法

方法（method）是依附于对象的函数。这些方法通过关键字 self 来访问对象中的数据和其他。方法在 impl 代码块中定义。

与函数的区别

函数：可以直接调用，同一个程序不能出现 2 个相同的函数签名的函数，因为函数不归属任何 owner。

方法：归属某一个 owner，不同的 owner 可以有相同的方法签名。



```
impl 结构体{
    fn 方法名(&self,参数列表) 返回值 {
        //方法体
    }
}
```



```rust
impl Study {
    fn get_spend(&self) -> i32 {
        return self.spend;
    }
}

println!("spend {}", s3.get_spend());
//输出 spend 5
```

### 结构体静态方法

```
fn 方法名(参数: 数据类型,...) -> 返回值类型 {
      // 方法体
   }

调用方式
结构体名称::方法名(参数列表)
```



```rust
impl Study {
    ...
    fn get_instance_another(name: String, target: String, spend: i32) -> Study {
        return Study {
            name,
            target,
            spend,
        };
    }
}

let s4 = Study::get_instance_another(
    String::from("Go语言极简一本通"),
    String::from("学习Go语言语法，并且完成一个单体服务"),
    5,
);
```



### 单元结构体

实例化一个元组结构体

```rust
let pair = (String::from("从0到Go语言微服务架构师"), 1);
```

访问元组结构体的字段

```rust
println!("pair 包含 {:?} and {:?}", pair.0, pair.1);
```

解构一个元组结构体

```rust
let (study, spend) = pair;
println!("pair contains {:?} and {:?}", study, spend);
```



## Enum(枚举)

```
enum 枚举名称{
 variant1,
 variant2,
 ...
}
```



```rust
枚举名称::variant

#[derive(Debug)]
enum RoadMap {
    Go语言极简一本通,
    Go语言微服务架构核心22讲,
    从0到Go语言微服务架构师,
}

//这是Go语言学习3个阶段

fn main() {
   let level = RoadMap::从0到Go语言微服务架构师;
    println!("level---{:?}",level);
}
```

`#[derive(Debug)]` 注解的作用，就是让 `派生自`Debug`。



### Option 枚举

```
enum Option<T> {
   Some(T),      // 用于返回一个值
   None          // 用于返回 null,用None来代替。
}
```

`Option` 枚举经常用在函数中的`返回值`，它可以表示有返回值，也可以用于表示没有返回值。如果有返回值。可以使用返回 Some(data)，如果函数没有返回值，可以返回 None。

```rust
fn getDiscount(price: i32) -> Option<bool> {
    if price > 100 {
        Some(true)
    } else {
        None
    }
}

let p=666;  //输出 Some(true)
// let p=6;//输出 None
let result = getDiscount(p);
println!("{:?}",result)
```

### match 语句

判断一个枚举变量的值，唯一操作符就是 match。

```rust
fn print_road_map(r:RoadMap){
    match r {
        RoadMap::Go语言极简一本通=>{
            println!("Go语言极简一本通");
        },
        RoadMap::Go语言微服务架构核心22讲=>{
            println!("Go语言微服务架构核心22讲");
        },
        RoadMap::从0到Go语言微服务架构师=>{
            println!("从0到Go语言微服务架构师");
        }
    }
}
print_road_map(RoadMap::Go语言极简一本通);//输出 Go语言极简一本通
print_road_map(RoadMap::Go语言微服务架构核心22讲);//输出 Go语言微服务架构核心22讲
print_road_map(RoadMap::从0到Go语言微服务架构师);//输出 从0到Go语言微服务架构师
```

### 带数据类型的枚举

```
enum 枚举名称{
    variant1(数据类型1),
    variant2(数据类型2),
    ...
}
```

```rust
#[derive(Debug)]
enum StudyRoadMap{
    Name(String),
}

let level3 = StudyRoadMap::Name(String::from("从0到Go语言微服务架构师"));
match level3 {
    StudyRoadMap::Name(val)=>{
        println!("{:?}",val);
    }
}

//输出 "从0到Go语言微服务架构师"
```



## 集合 Collections

### 向量 （`Vector`）

Rust 在标准库中定义了结构体 `Vec` 用于表示一个向量。向量和数组很相似，只是数组长度是编译时就确定了，定义后就不能改变了，那要么改数组，用**向量**这个数据结构。

特点：

- 向量中的元素都是相同类型元素的集合。
- 长度可变，运行时可以增加和减少。
- 使用索引查找元素。（索引从 0 开始）
- 添加元素时，添加到向量尾部。
- 向量的内存在堆上，长度可动态变化。

创建向量

1.`new()` 静态方法用于创建一个结构体 Vec 的实例。

```
let mut 向量的变量名称 = Vec::new();
```

2.`vec!()` 宏来简化向量的创建。

```
let 向量的变量名称 = vec![val1,val2,...]
```



```rust
let mut v = Vec::new();//调用 Vec 结构的 new() 静态方法来创建向量。
v.push("Go语言极简一本通");       //通过push方法添加元素数据。并且追加到向量尾部。
v.push("Go语言微服务核心架构22讲");
v.push("从0到Go语言微服务架构师");
println!("{:?}",v);
println!("len :{}",v.len()); // 通过len方法获取向量中的元素个数。


let mut v2 = vec!["Go语言极简一本通","Go语言微服务核心架构22讲","从0到Go语言微服务架构师"];
// 通过vect!宏创建向量时，向量的数据类型由第一个元素自动推断出来。
println!("{:?}",v2);

let x=v2.remove(0);
// remove()方法移除并返回向量中指定的下标索引处的元素，将其后面的所有元素移到向左移动一位。
println!("{}",x); //输出 Go语言极简一本通
println!("{:?}",v2);//输出 ["Go语言微服务核心架构22讲", "从0到Go语言微服务架构师"]

//contains() 用于判断向量是否包含某个值。如果值在向量中存在则返回 true，否则返回 false。
if v.contains(&"从0到Go语言微服务架构师"){
   println!("找到了")
}

//访问向量中的某个元素,使用索引
let y = v[0];
println!("{}",y); //输出 Go语言极简一本通

//遍历向量
for item in v {
   println!("充电项目: {}", item);
}
//输出
充电项目: Go语言极简一本通
充电项目: Go语言微服务核心架构22讲
充电项目: 从0到Go语言微服务架构师
```



### 哈希表 HashMap

HashMap 就是**键值对**集合。键是不能重复的，值是可以重复的。

使用 `HashMap` 结构体之前需要显式导入 `std::collections` 模块。

```
let mut 变量名称 = HashMap::new();
```

```rust
use std::collections::HashMap;

let mut process = HashMap::new();
process.insert("Go语言极简一本通", 1);
process.insert("Go语言微服务核心架构22讲", 2);
process.insert("从0到Go语言微服务架构师", 3);

println!("{:?}", process);
//输出 {"Go语言极简一本通": 1, "Go语言微服务核心架构22讲": 2, "从0到Go语言微服务架构师": 3}
println!("len {}",menu.len());
//输出  3


// get() 方法用于根据键从哈希表中获取相应的值。
match process.get(&"从0到Go语言微服务架构师"){
   Some(v)=>{
      println!("HashMap v:{}", v);
   }
   None=>{
      println!("找不到");
  }
}
//输出 HashMap v:3

//迭代哈希表 iter()
for (k, v) in process.iter() {
   println!("k: {} v: {}", k, v);
}
//输出
k: Go语言微服务核心架构22讲 v: 2
k: 从0到Go语言微服务架构师 v: 3
k: Go语言极简一本通 v: 1

// contains_key() 方法用于判断哈希表中是否包含指定的 键值对。如果包含指定的键，那么会返回相应的值的引用，否则返回 None。
if process.contains_key(&"Go语言极简一本通") {
   println!("找到了");
}
//输出 找到了

// remove() 用于从哈希表中删除指定的键值对。如果键值对存在则返回删除的键值对，返回的数据格式为 (&'a K, &'a V)。如果键值对不存在则返回 None

let x=process.remove(&"Go语言极简一本通");
println!("{:?}",x);
println!("{:?}",process);
//输出
Some(1)
{"Go语言微服务核心架构22讲": 2, "从0到Go语言微服务架构师": 3}
```



### 哈希集合 HashSet

Hashset 是相同数据类型的集合，它是没有重复值的。如果集合中已经存在相同的值，则会插入失败。

```
let mut 变量名称 = HashSet::new();
```

insert() 用于插入一个值到集合中。

```
let mut studySet = HashSet::new();
studySet.insert("Go语言极简一本通");
studySet.insert("Go语言微服务核心架构22讲");
studySet.insert("从0到Go语言微服务架构师");
println!("{:?}", studySet);
//输出 {"从0到Go语言微服务架构师", "Go语言微服务核心架构22讲", "Go语言极简一本通"}

studySet.insert("从0到Go语言微服务架构师");
println!("{:?}", studySet);
//输出 {"从0到Go语言微服务架构师", "Go语言微服务核心架构22讲", "Go语言极简一本通"}
```

len() 方法集合中元素的个数。

```
println!("len:{}",studySet.len());//输出 len:3
```

`iter()` 方法用于返回集合中所有元素组成的无序迭代器。

```
for item in studySet.iter(){
    println!("hashSet-充电项目: {}", item);
}
//输出
hashSet-充电项目: Go语言极简一本通
hashSet-充电项目: Go语言微服务核心架构22讲
hashSet-充电项目: 从0到Go语言微服务架构师
```

`get()` 方法用于获取集合中指定值的一个引用。

```
match studySet.get("从0到Go语言微服务架构师") {
    None => {
        println!("没找到");
    }
    Some(data) => {
        println!("studySet中找到:{}",data);
    }
}
//输出 studySet中找到:从0到Go语言微服务架构师
```

`contains()` 方法用于判断集合是否包含指定的值。

```
if studySet.contains("Go语言微服务核心架构22讲"){
    println!("包含 Go语言微服务核心架构22讲")
}
//输出 包含 Go语言微服务核心架构22讲
```

`remove()` 方法用于从集合中删除指定的值。如果该值在集合中，则返回 `true`，如果不存在则返回 `false`。

```
studySet.remove("Go语言极简一本通");
println!("{:?}",studySet);//输出 {"Go语言微服务核心架构22讲", "从0到Go语言微服务架构师"}

studySet.remove("golang");
println!("{:?}",studySet);//输出 {"Go语言微服务核心架构22讲", "从0到Go语言微服务架构师"}
```



## 泛型(Generic)

### 泛型集合

```
let mut v:Vec<i32> =vec![1,2,3];
v.push("4");//此处会报错 ^^^ expected `i32`, found `&str`
```

### 泛型结构体

```
struct 结构体名称<T> {
   元素:T,
}


struct Data<T> {
   value:T,
}

fn main() {
    let t:Data<i32> = Data{value:100};
    println!("值:{} ",t.value);//输出 值:100
    let t:Data<f64> = Data{value:66.00};
    println!("值:{} ",t.value);//输出 值:66
}
```

### 特质 Trait

可以把这个特质（traits）对标其他语言的接口，都是对行为的抽象。使用 `trait`关键字用来定义。特质，可以包含具体的方法，也可以包含抽象的方法。

```
trait some_trait {
   // 没有任何实现的虚方法
   fn method1(&self);

   // 有具体实现的普通方法
   fn method2(&self){
      //方法的具体代码
   }
}
```

### 实现特质

Rust 使用 impl for 为每个结构体实现某个特质。`impl` 是 `implement` 的缩写。

```
struct Book {
    name: String,
    id: u32,
    author: String,
}

trait ShowBook {
    fn Show(&self);
}

impl ShowBook for Book{
    fn Show(&self) {
        println!("Id:{},Name:{},Author:{}",self.id,self.name,self.author);
    }
}

fn main() {
		let book = Book{
        id:1,
        name: String::from("Go语言极简一本通"),
        author: String::from("欢喜")
    };
    book.Show();//输出 Id:1,Name:Go语言极简一本通,Author:欢喜
}
```



## 文件操作

| 模块                 | 方法             | 说明                                                         |
| -------------------- | ---------------- | ------------------------------------------------------------ |
| std::fs::File        | open()           | 静态方法，以 **只读** 模式打开文件                           |
| std::fs::File        | create()         | 静态方法，以 **可写** 模式打开文件。 如果文件存在则清空旧内容 如果文件不存在则新建 |
| std::fs::remove_file | remove_file()    | 从文件系统中删除某个文件                                     |
| std::fs::OpenOptions | append()         | 设置文件模式为 **追加**                                      |
| std::io::Writes      | write_all()      | 将 buf 中的所有内容写入输出流                                |
| std::io::Read        | read_to_string() | 读取所有内容转换为字符串后追加到 buf 中                      |

### 打开文件

```
let file = std::fs::File::open("data.txt").unwrap();
println!("文件打开成功\n:{:?}",file);
```

### 创建文件

```
let file = std::fs::File::create("data2.txt").expect("创建失败");
println!("文件创建成功:{:?}",file);
```

### 删除文件

```
fs::remove_file("data.txt").expect("无法删除文件");
println!("文件已删除");
```

### 追加内容

```
let mut file = OpenOptions::new().append(true).open("data2.txt").expect("失败");
file.write("\nwww.go-edu.cn".as_bytes()).expect("写入失败");
println!("\n数据追加成功");
```

### 写入所有内容

```
file.write_all("Rust".as_bytes()).expect("创建失败");
file.write_all("\nRust".as_bytes()).expect("创建失败");
println!("数据已写入完毕");
```

注意： write_all() 方法并不会在写入结束后自动写入换行符 `\n`。

### 读取内容

```
let mut file = std::fs::File::open("data2.txt").unwrap();
let mut contents = String::new();
file.read_to_string(&mut contents).unwrap();
print!("{}", contents);
//输出
www.go-edu.cnRust
Rust
```





## 迭代器

- 一个是 `iter()`，用于返回一个 **迭代器** 对象，也称之为 **项 ( items )** 。
- 一个是 `next()`，用于返回迭代器中的下一个元素。如果已经迭代到集合的末尾（最后一个项后面）则返回 `None`。

```
fn main() {
    let v = vec!["Go语言极简一本通", "Go语言微服务架构核心22讲", "从0到Go语言微服务架构师"];
    let mut it = v.iter();
    println!("{:?}",it.next());
    println!("{:?}",it.next());
    println!("{:?}",it.next());
    println!("{:?}",it.next());
}
//输出
Some("Go语言极简一本通")
Some("Go语言微服务架构核心22讲")
Some("从0到Go语言微服务架构师")
None
```



用 `for ... in` 语句遍历。

```
let iter = v.iter();
for item in iter{
   print!("{}\n",item);
}
```

| 方法          | 描述                                                    |
| ------------- | ------------------------------------------------------- |
| `iter()`      | 返回一个只读可重入迭代器，迭代器元素的类型为 `&T`       |
| `into_iter()` | 返回一个只读不可重入迭代器，迭代器元素的类型为 `T`      |
| `iter_mut()`  | 返回一个可修改可重入迭代器，迭代器元素的类型为 `&mut T` |



## 闭包

Rust 中的闭包（closure），也叫做 lambda 表达式或者 lambda，是一类能够捕获周围作用域中变量的函数。

调用一个闭包和调用一个函数完全相同，不过调用闭包时，输入和返回类型两者都可以自动推导，而输入变量名必须指明。

其他的特点包括：

- 声明时使用 || 替代 () 将输入参数括起来。
- 函数体定界符（{}）对于单个表达式是可选的，其他情况必须加上。
- 有能力捕获外部环境的变量。



```
普通函数
fn 函数名(参数列表) -> 返回值 {
   // 业务逻辑
}

// 闭包
|参数列表| {
	// 业务逻辑
}
// 无参数闭包
|| {
	// 业务逻辑
}
```

闭包也可以赋值给一个变量，可以通过调用这个变量来完成闭包的调用。

```
let 闭包变量 = |参数列表| {
   // 闭包的具体逻辑
}
```



```rust
let double = |x| { x * 2 };

let add = |a, b| { a + b };
let x = add(2, 4);
println!("{}", x);

let y = double(5);
println!("{}", y);

let v = 3;
let add2 = |x| { v + x };
println!("{}", add2(4));
//输出
6
10
7
```

### 捕获

闭包本质上很灵活，闭包可以在没有类型标注的情况下运行。可移动（move），又可借用（borrow）。闭包可以通过以下方式捕获变量：

- 通过引用：&T
- 通过可变引用：&mut T
- 通过值：T

总结

- 闭包就是在一个函数内创建立即调用的另一个函数。
- 闭包是一个匿名函数。
- 闭包虽然没有函数名，但可以把整个闭包赋值一个变量，通过调用该变量来完成闭包的调用。
- 闭包不用声明返回值，但它却可以有返回值。并且使用最后一条语句的执行结果作为返回值。闭包的返回值可以赋值给变量。
- 闭包又称之为 **内联函数**。可以让闭包访问外层函数里的变量。



## 多线程

### 创建线程

```
std::thread::spawn()

//spawn() 函数的原型
pub fn spawn<F, T>(f: F) -> JoinHandle<T>

参数 f 是一个闭包（closure ） 是线程要执行的代码。
```

```rust
fn main() {
    //子线程
    thread::spawn(|| {
        for i in 1..10 {
            println!("子线程 {}", i);
            thread::sleep(Duration::from_millis(1));
        }
    });
    // 主线程
    for i in 1..5 {
        println!("主线程 {}", i);
        thread::sleep(Duration::from_millis(1));
    }
}

```

> 当主线程执行结束，子线程就自动结束。

`thread::sleep()` 会让线程睡眠一段时间，某个线程睡眠的时候会让出 CPU，可以让不同的线程交替执行，要看操作系统如何调度线程。



### join

上面的例子主线程结束后，子线程还没有运行完，但是子线程也结束了。如果想让子线程结束后，主线程再结束，我们就要使用**Join 方法**，把子线程加入主线程等待队列。

```
spawn<F, T>(f: F) -> JoinHandle<T>
```

```rust
//子线程
let handler = thread::spawn(|| {
    for i in 1..10 {
        println!("子线程 {}", i);
        thread::sleep(Duration::from_millis(1));
    }
});
// 主线程
for i in 1..5 {
    println!("主线程 {}", i);
    thread::sleep(Duration::from_millis(1));
}
handler.join().unwrap();
```

输出结果看 主线程让子线程执行完毕后，主线程才退出。



## 错误处理

错误分为两大类：**可恢复** 和 **不可恢复**，相当于其它语言的 **异常** 和 **错误**。

| Name          | 描述                                       |
| ------------- | ------------------------------------------ |
| Recoverable   | 可以被捕捉，相当于其它语言的异常 Exception |
| UnRecoverable | 不可捕捉，会导致程序崩溃退出               |

### panic!() 不可恢复错误

`panic!()` 程序立即退出，退出时调用者抛出退出原因。

一般情况下，当遇到不可恢复错误时，程序会自动调用 `panic!()`。

```rust
fn main() {
    panic!("出错啦");
    println!("Hello Rust"); // 不可能执行的语句
}
//输出
thread 'main' panicked at '出错啦', src/main.rs:2:5


let v = vec!["Go语言极简一本通","Go语言微服务架构核心22讲","从0到Go语言微服务架构师"];
v[5]; // 因为超出了数组的长度，所以会触发不可恢复错误

```

### Result 枚举和可恢复错误

枚举的定义如下：

```rust
enum Result<T,E> {
   OK(T),
   Err(E)
}
```

OK(T) **T** `OK` 时作为正常返回的值的数据类型。
Err(E) **E** `Err` 时作为错误返回的错误的类型。

```rust
let f = File::open("abc.jpg"); //文件不存在，因此值为 Result.Err
println!("{:?}",f);

//输出
Err(Os { code: 2, kind: NotFound, message: "No such file or directory" })
```

### unwrap() 和 expect()

`unwrap()` 函数的原型如下

```
unwrap(self):T
```

`unwrap`是 Result<T, E>的方法，在实例上调用此方法时，如果是 Ok 枚举值，就会返回 Ok 中的对象，如果是 Err 枚举值，在运行时会 panic，报错信息是 format!(“{}”, error)。其缺点是，如果在不同地方都使用 unwrap，运行时出现 panic 的时候。

```rust
fn is_even(no:i32)->Result<bool,String> {
    return if no % 2 == 0 {
        Ok(true)
    } else {
        Err("输入值，不是偶数".to_string())
    }
}

let result = is_even(6).unwrap();
println!("结果 {}",result);
//输出 结果 true

let result = is_even(11).unwrap();
println!("结果 {}",result);
//输出 thread 'main' panicked at 'called `Result::unwrap()` on an `Err` value: "输入值，不是偶数"'
```



函数 `expect()` 的原型如下

```
expect(self,msg:&str):T
```

`expect`方法的作用和`unwrap`类似，区别在于，`expect`方法接受`msg: &str`作为参数，它在运行时的`panic`信息为`format!("{}: {}", msg, error)`，使用`expect`时，可以自定义报错信息，因此出现`panic`时比较容易定位。

```rust
let f = File::open("abc.txt").expect("无法打开该文件"); // 文件不存在
//输出 thread 'main' panicked at '无法打开该文件: Os { code: 2, kind: NotFound, message: "No such file or directory" }'
```



## 智能指针

Rust 可以在 **堆** 上存储数据。Rust 语言中的某些类型，如 **向量 `Vector`** 和 **字符串对象 `String`** 默认就是把数据存储在 **堆** 上的。

Rust 语言把指针封装在如下两个特质`Trait`中。

| 特质名 | 包              | Description                                                  |
| ------ | --------------- | ------------------------------------------------------------ |
| Deref  | std::ops::Deref | 用于创建一个只读智能指针，例如 `*v`                          |
| Drop   | std::ops::Drop  | 智能指针超出它的作用域范围时会回调该特质的 `drop()` 方法。 类似于其它语言的 **析构函数**。 |

当一个结构体实现了以上的接口后，它们就不再是普通的结构体了。

Rust 提供了在 **堆** 上存储数据的能力并把这个能力封装到了 `Box` 中。

这种把 **栈** 上数据搬到 **堆** 上的能力，我们称之为 **装箱**。



### Box 指针

Box 指针可以把数据存储在**堆（heap）**上，而不是**栈（stack）**上。这就是**装箱（box）**，**栈（stack）**还是包含指向 **堆（heap）** 上数据的指针。

```rust
fn main() {
    let a = 6;           // 默认保存在 栈 上
    let b = Box::new(a); // 使用 Box 后数据会存储在堆上
    println!("b = {}", b);// 输出 b = 6
}
```

### 访问 Box 存储的数据

如果想访问 Box 存储的数据，可以使用 **星号** *访问，这个操作叫做 **解引用**。 **星号** *也叫 解引用符。

```rust
let price1 = 158;           // 值类型数据
let price2 = Box::new(price1); // price2 是一个智能指针，指向堆上存储的数据 158
println!("{}",158==price1);
println!("{}",158==*price2); // 为了访问 price2 存储的具体数据，需要解引用

//输出
true
true
```



### Deref 特质

实现 `Deref` 特质需要我们实现 `deref()` 方法。`deref()` 方法返回一个指向结构体内部数据的指针。

```rust
struct CustomBox<T>{
    value: T
}

impl<T> CustomBox<T> {
    fn new(v:T)-> CustomBox<T> {
        CustomBox{value:v}
    }
}

impl<T> Deref for CustomBox<T> {
    type Target = T;
    fn deref(&self) -> &T {
        &self.value
    }
}

let x = 666;
let y = CustomBox::new(x);  // 调用静态方法 new() 返回创建一个结构体实例

println!("666==x is {}",666==x);
println!("666==*y is {}",666==*y);  // 解引用 y
println!("x==*y is {}",x==*y);  // 解引用 y
//输出
666==x is true
666==*y is true
x==*y is true
```

### Drop 特质

`Drop Trait` 只有一个方法 `drop()`。当实现了 `Drop Trait` 的结构体，在超出了它的作用域范围时会触发调用 `drop()` 方法。

```rust
impl<T> Drop for CustomBox<T>{
    fn drop(&mut self){
        println!("drop CustomBox 对象!");
    }
}
```

## 包管理

| 命令           | 说明                                                     |
| -------------- | -------------------------------------------------------- |
| `cargo new`    | 在当前目录下新建一个 cargo 项目                          |
| `cargo check`  | 分析当前项目并报告项目中的错误，但不会编译任何项目文件   |
| `cargo build`  | 编译当前项目                                             |
| `cargo run`    | 编译并运行文件 `src/main.rs`                             |
| `cargo clean`  | 移除当前项目下的 `target` 目录及目录中的所有子目录和文件 |
| `cargo update` | 更新当前项目中的 `Cargo.lock` 文件列出的所有依赖         |



## 模块(Modules)

### 定义模块

```rust
mod module_name {
   fn function_name() {
      // 具体的函数逻辑
   }
}
```

1. module_name 要是一个合法的名称。
2. Rust 语言中的模块默认是私有的。
3. 如果一个模块或者模块内的函数需要导出为外部使用，则需要添加 `pub` 关键字。
4. 私有的模块不能为外部其它模块或程序所调用。
5. 私有模块的所有函数都必须是私有的，而公开的模块，则即可以有公开的函数也可以有私有的函数。

```rust
//公开的模块
pub mod public_module {
   pub fn public_function() {
      // 公开的方法
   }
   fn private_function() {
      // 私有的方法
   }
}
//私有的模块
mod private_module {

   // 私有的方法
   fn private_function() {
   }
}
```

### `use` 关键字

```
use 公开的模块名::函数名;
```

在根目录下，执行 cargo new –lib mylib,创建类库。

```rust
pub mod add_salary {
    pub fn study(name:String) {
        println!("面向加薪学习 {}",name);
    }
}

第一步 进入 mylib目录执行cargo build
第二步 打开根目录 Cargo.toml
[dependencies]
mylib={ path="../module_demo/mylib" }
第三步 在main.rs中修改
use mylib::add_salary::study;

fn main(){
    study("从0到Go语言微服务架构师".to_string());
}

use AddSalary::study;

fn main() {
    study("从0到Go语言微服务架构师".to_string());
}
//输出
面向加薪学习 从0到Go语言微服务架构师
```

Rust 允许一个模块中嵌套另一个模块，换种说法，就是允许多层级模块。

```
pub mod mod1 {
   pub mod mod2 {
      pub mod mod3 {
         pub fn 方法名(参数) {
            //代码逻辑
         }
      }
   }
}
```

调用或使用嵌套模块的方法使用两个冒号 (`::`) 从左到右拼接从外到内的模块即可

```
use mod1::mod2::mod3::方法名;

fn main() {
    方法名();
}
```





## 变量绑定

变量绑定默认是不可变的（immutable），但加上 mut 修饰语后变量就可以改变。

### 作用域和遮蔽

变量绑定有一个作用域（scope），它被限定只在一个代码块（block）中生存（live）。 代码块是一个被 {} 包围的语句集合。另外也允许变量遮蔽（variable shadowing）。

```rust
// 此绑定生存于 main 函数中
let spend = 1;
// 这是一个代码块，比 main 函数拥有更小的作用域
{
    // 此绑定只存在于本代码块
    let target = "面向加薪学习";

    println!("inner short: {}", target);

    // 此绑定*遮蔽*了外面的绑定
    let spend = 2.0;

    println!("inner long: {}", spend);
}
// 代码块结束

// 报错！`target` 在此作用域上不存在
// error[E0425]: cannot find value `target` in this scope
println!("outer short: {}", target);

println!("outer long: {}", spend);

// 此绑定同样*遮蔽*了前面的绑定
let spend = String::from("学习时间1小时");

println!("outer spend: {}", spend);
```



### 变量先声明

可以先声明（declare）变量绑定，后面才将它们初始化（initialize）。但是这种做法很少用，因为这样可能导致使用未初始化的变量。
编译器禁止使用未经初始化的变量，因为这会产生未定义行为（undefined behavior）。

```rust
// 声明一个变量绑定
let spend;

{
    let x = 2;

    // 初始化一个绑定
    spend = x * x;
}

println!("spend: {}", spend);

let spend2;

// 报错！使用了未初始化的绑定
println!("spend2: {}", spend2);
// 改正 ^ 注释掉此行

spend2 = 1;

println!("another binding: {}", spend2);
```



### 冻结

资源存在使用的引用时，在当前作用域中这一资源是不可被修改的，称之为“冻结”。

```rust
let mut spend4 = Box::new(1);
let spend5 = &spend4;
spend4= Box::new(100);
println!("{}", spend4);
println!("{}", spend5);

报错如下
let spend5 = &spend4;
------- borrow of `spend4` occurs here
spend4= Box::new(100);
^^^^^^ assignment to borrowed `spend4` occurs here
println!("{}", spend4);
println!("{}", spend5);
------ borrow later used here
```



## 类型系统

### 类型转换

Rust 不提供原生类型之间的隐式类型转换，但可以使用 **as** 关键字进行显式类型转换。

```rust
let spend = 1;
// 错误！不提供隐式转换
// error[E0308]: mismatched types
// let cost: f64 = spend;

// 可以显式转换
let cost = spend as f64;
println!("转换: {} -> {}", spend, cost);
```

### 字面量

对数值字面量，只要把类型作为后缀加上去，就完成了类型说明。比如指定字面量 42 的 类型是 i32，只需要写 42i32。

无后缀的数值字面量，其类型取决于怎样使用它们。如果没有限制，编译器会对整数使用 i32，对浮点数使用 f64。

```rust
// 带后缀的字面量，其类型在初始化时已经知道了。
 let x = 1u8;
 let y = 2u32;
 let z = 3f32;

 // 无后缀的字面量，其类型取决于如何使用它们。
 let i = 1;
 let f = 1.0;
```

### 类型推断

Rust 的类型推断引擎是很聪明的，它不只是在初始化时看看右值（r-value）的 类型而已，它还会考察变量之后会怎样使用，借此推断类型。

```rust
// 因为有类型说明，编译器知道类型String
let study = String::from("从0到Go语言微服务架构师");

// 创建一个空向量（vector，即不定长的，可以增长的数组）。
let mut vec = Vec::new();
// 现在编译器还不知道 `vec` 的具体类型，只知道它是某种东西构成的向量（`Vec<?>`）

// 在向量中插入元素。
vec.push(study);
// 现在编译器知道 `vec` 是 String 的向量了（`Vec<String>`）。
println!("{:?}", vec);
```

### 别名

type 新名字 = 原名字;

```rust
type MyU64 = u64;
type OtherU64 = u64;
type ThirdU64 = u64;
fn main(){
    let MyU64: MyU64 = 5 as ThirdU64;
    let otherU64: OtherU64 = 2 as ThirdU64;
    println!(
        "{} MyU64 + {} OtherU64es = {} unit?",
        MyU64,
        otherU64,
        MyU64 + otherU64
    );
}
```

注意类型别名*并不能*提供额外的类型安全，因为别名*并不是*新的类型。



## 类型转换

Rust 使用 trait 解决类型之间的转换问题。最一般的转换会用到 From 和 Into 两个 trait。

### From

From trait 允许一种类型定义 “怎么根据另一种类型生成自己”，因此它提供了一种类型转换的简单机制。在标准库中有无数 From 的实现，规定原生类型及其他常见类型的转换功能。

```rust
let s1 = "从0到Go语言微服务架构师";
let s2 = String::from(s1);


#[derive(Debug)]
struct MyNumber {
    num: i32,
}

impl From<i32> for MyNumber {
    fn from(item: i32) -> Self {
        MyNumber { num: item }
    }
}
fn main() {
    let my_number = MyNumber { num: 1 };
    println!("{:?}", my_number);
}
```

### Into

Into trait 就是把 From trait 倒过来而已。也就是说，如果你为你的类型实现了 From，那么同时你也就免费获得了 Into。
使用 Into trait 通常要求指明要转换到的类型，因为编译器大多数时候不能推断它。不过考虑到我们免费获得了 Into，这点代价不值一提。

```rust
let spend = 3;
let my_spend: MyNumber = spend.into();
println!("{:?}", my_spend);
```

### 解析字符串

只要对目标类型实现了 FromStr trait，就可以用 parse 把字符串转换成目标类型。 标准库中已经给无数种类型实现了 FromStr。如果要转换到用户定义类型，只要手动实现 FromStr 就行。

```rust
let cost: i32 = "5".parse().unwrap();
println!("{}", cost);
```





## match 匹配

Rust 通过 match 关键字来提供模式匹配，和 C 语言的 switch 用法类似。第一个匹配分支会被比对，并且所有可能的值都必须被覆盖。



### 解构指针和引用

- 解引用使用 *
- 解构使用 &、ref、和 ref mut

```rust
// 获得一个 `i32` 类型的引用。`&` 表示取引用。
let num = &100;

match num {
    // 用 `&val` 这个模式去匹配 `num`
    &val => println!("&val 是: {:?}", val),
}

// 如果不想用 `&`，需要在匹配前解引用。
match *num {
    val => println!("val 是: {:?}", val),
}

// Rust 对这种情况提供了 `ref`。它更改了赋值行为，从而可以对具体值创建引用。
// 下面这行将得到一个引用。
let ref num3 = 66;

// 相应地，定义两个非引用的变量，通过 `ref` 和 `ref mut` 仍可取得其引用。
let num4 = 5;
let mut mut_num4 = 7;

// 使用 `ref` 关键字来创建引用。
// 下面的 r 是 `&i32` 类型，它像 `i32` 一样可以直接打印，因此用法上
// 似乎看不出什么区别。但读者可以把 `println!` 中的 `r` 改成 `*r`，仍然能
// 正常运行。前面例子中的 `println!` 里就不能是 `*val`，因为不能对整数解
// 引用。
match num4 {
    ref r => println!("num4 r is: {:?}", r),
}

// 类似地使用 `ref mut`。
match mut_num4 {
    ref mut m => {
        // 已经获得了 `mut_value` 的引用，先要解引用，才能改变它的值。
        *m += 10;
        println!("`mut_value`: {:?}", m);
    }
}
```

### 解构结构体

```rust
struct Study {
    name: String,
    target: String,
    spend: u32,
}
fn main(){

    let s = Study {
            name: String::from("从0到Go语言微服务架构师"),
            target: String::from("全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。"),
            spend: 3,
    };

    let Study {
            name: name,
            target: target,
            spend: spend,
    } = s;

    println!("name = {}, target = {},  spend = {} ", name, target, spend);
    // name = 从0到Go语言微服务架构师, target = 全面掌握Go语言微服务落地，代码级一次性解决微服务和分布式系统。,  spend = 3
    let s2 = Study {
            name: String::from("《Go语言极简一本通》"),
            target: String::from("学习Go语言，并且完成一个单体服务。"),
            spend: 5,
    };
    // 也可以忽略某些变量
    let Study { name, .. } = s2;
    println!("name = {}", name);
    //name = 《Go语言极简一本通》
}
```



## if let 和 while let

在一些场合下，用 match 匹配枚举类型并不优雅。
if let 在这样的场合要简洁得多，并且允许指明数种失败情形下的选项：

```rust
let s = Some("从0到Go语言微服务架构师");
let s1: Option<i32> = None;
let s2: Option<i32> = None;

// 如果 `let` 将 `s` 解构成 `Some(i)`，则执行语句块（`{}`）
if let Some(i) = s {
    println!("已上车 {:?}!", i);
}

// 如果要指明失败情形，就使用 else：
if let Some(i) = s1 {
    println!("Matched {:?}!", i);
} else {
    // 解构失败。切换到失败情形。
    println!("不匹配。");
};

// 提供另一种失败情况下的条件。
let flag = false;
if let Some(i) = s2 {
    println!("Matched {:?}!", i);
// 解构失败。使用 `else if` 来判断是否满足上面提供的条件。
} else if flag {
    println!("不匹配s2");
} else {
    // 条件的值为 false。于是以下是默认的分支：
    println!("默认分支");
};
输出
已上车 "从0到Go语言微服务架构师"!
不匹配。


```

### while let

```rust
// 将 `optional` 设为 `Option<i32>` 类型
let mut num = Some(0);

// 当 `let` 将 `optional` 解构成 `Some(i)` 时，就
// 执行语句块（`{}`）。否则就 `break`。
while let Some(i) = num {
    if i > 9 {
        println!("{},quit!",i);
        num = None;
    } else {
        println!("`i` is `{:?}`. Try again.", i);
        num = Some(i + 1);
    }
}
```



## 闭包的使用

```rust
let add = |x, y| x + y;
let result = add(3, 4);
println!("{}", result);
```

```rust
fn receives_closure<F>(closure: F)
    where
        F: Fn(i32, i32) -> i32,
{
    let result = closure(3, 5);
    println!("闭包作为参数执行结果 => {}", result);
}

fn main() {
    let add = |x, y| x + y;
    receives_closure(add);
}

输出:
闭包作为参数执行结果 => 8
```

```rust
fn receives_closure2<F>(closure:F)
    where
        F:Fn(i32)->i32{
    let result = closure(1);
    println!("closure(1) => {}", result);
}

fn main() {
    let y = 2;
    receives_closure2(|x| x + y);

    let y = 3;
    receives_closure2(|x| x + y);
}
输出:
捕获变量的结果 => 3
捕获变量的结果 => 4
```

返回闭包

当函数返回一个闭包时，由于闭包的类型是匿名的、无法直接写出的，因此需要使用 `impl Trait` 来指定闭包的类型。

```rust
fn returns_closure() -> impl Fn(i32) -> i32 {
    |x| x + 6
}

fn main() {
    let closure = returns_closure();
    println!("返回闭包 => {}", closure(1));
}
输出:
返回闭包 => 7
```

参数和返回值都有闭包

```rust
fn do1<F>(f: F, x: i32) -> impl Fn(i32) -> i32
    where
        F: Fn(i32, i32) -> i32{
    move |y| f(x, y)
}

fn main() {
    let add = |x, y| x + y;
    let result = do1(add, 5);
    println!("result(1) => {}", result(1));
}
输出
result(1) => 6
```



下面我们考虑一下是否可以做一个通用的

```rust
fn do2<F, X, Y, Z>(f: F, x: X) -> impl Fn(Y) -> Z
    where
        F: Fn(X, Y) -> Z{
    move |y| f(x, y)
}
```

报错如下

```
error[E0507]: cannot move out of `x`, a captured variable in an `Fn` closure
  --> src/main.rs:12:16
   |
9  | fn do2<F, X, Y, Z>(f: F, x: X) -> impl Fn(Y) -> Z
   |                          - captured outer variable
...
12 |     move |y| f(x, y)
   |     -----------^----
   |     |          |
   |     |          move occurs because `x` has type `X`, which does not implement the `Copy` trait
   |     captured by this `Fn` closure
```

```rust
fn do2<F, X, Y, Z>(f: F, x: X) -> impl Fn(Y) -> Z
    where
        F: Fn(X, Y) -> Z,
        X: Copy{
    move |y| f(x, y)
}
fn main(){
    let add = |x, y| x + y;
    let result = do2(add, 5);
    println!("result(2) => {}", result(2));
}
输出
result(2) => 7
```



## Async/Await

```rust
use async_std::task::{sleep, spawn};
use std::time::Duration;

async fn do3() {
    for i in 1..=5 {
        println!("do3 {}", i);
        sleep(Duration::from_millis(500)).await;
    }
}

async fn do4() {
    for i in 1..=5 {
        println!("do4 {}", i);
        sleep(Duration::from_millis(1000)).await;
    }
}

#[async_std::main]
async fn main() {
    let do3_async = spawn(do3());
    do4().await;
    do3_async.await;
}
```









## 常见问题

1.vscode没有提示和找不到linker.exe

执行

```
cargo new rust_demo
```

```
rustup toolchain install stable-x86_64-pc-windows-gnu

rustup default stable-x86_64-pc-windows-gnu
```



https://www.cnblogs.com/andy-chi/p/16786718.html



