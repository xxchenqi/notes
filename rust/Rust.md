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
     // 将 `a` *复制*到 `b`——不存在资源移动
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



```
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



