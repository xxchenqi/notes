### 常量

宏常量

```c++
#define Day 7
```

const修饰的变量

```c++
const int month = 7;
```



### sizeof

查看数据类型所占内存大小

```c++
sizeof(short);//2
sizeof(int);//4
sizeof(long);//4
sizeof(long long);//8

// short < int <= long <= longlong 
```



### goto

语法:goto 标记;

```c++
goto FLAG;

FLAG:
```



### 数组

```c++
int arr[10] = { 1,2,3,4,5,6,7,8,9,10 };
cout << "整个数组占用的空间:" << sizeof(arr) << endl;
cout << "每个元素占用的空间:" << sizeof(arr[0]) << endl;
cout << "数组中的元素个数:" << sizeof(arr) / sizeof(arr[0]) << endl;

cout << "数组首地址:" << arr << endl;
cout << "数组首地址:" << &arr[0] << endl;
```



### 二维数组

```c++
int arr[2][3] = 
{
	{1,2,3},
	{4,5,6}
};
cout << "二维数组占用的空间:" << sizeof(arr) << endl;
cout << "二维数组第一行占用的空间:" << sizeof(arr[0]) << endl;
cout << "二维数组第一个元素占用的空间:" << sizeof(arr[0][0]) << endl;

cout << "二维数组数组行数:" << sizeof(arr) / sizeof(arr[0]) << endl;
cout << "二维数组数组列数:" << sizeof(arr[0]) / sizeof(arr[0][0]) << endl;

cout << "二维数组首地址:" << arr << endl;
cout << "二维数组第一行首地址:" << arr[0] << endl;
cout << "二维数组第二行首地址:" << arr[1] << endl;
cout << "二维数组第一行第一个元素首地址:" << &arr[0][0] << endl;
```



### 函数值传递

函数值传递时，函数的形参发生改变，并不会影响实参的值

```c++
void swap(int num1,int num2) {
	int temp = num1;
	num1 = num2;
	num2 = temp;
}
```





### 指针

作用：通过指针间接访问内存

```c++
int a = 10;
int* p = &a;

cout << "a的地址:" << &a << endl;
cout << "指针p为:" << p << endl;

//指针前加 * 代表解引用，找到指针指向的内存中的数据
*p = 100;
cout << "a:" << a << endl;
cout << "*p:" << *p << endl;
```



#### 指针占用内存空间

32位系统占用4字节

64位系统占用8字节

```c++
sizeof(int *)
```



#### 空指针

指针变量指向内存中编号为0的空间

用途：初始化指针变量

注意：空指针指向的内存是不可访问的

```c++
int * p = NULL;
// 0 ~ 255 之间的内存编号是系统占用的，因此不能访问
cout << *p << endl;
```



#### 野指针

指针变量指向非法的内存空间

```c++
int * p = (int *)0x1100;
```



#### const修饰指针

```c++
int a = 10;
int b = 20;

// 常量指针
// 特点:指针指向能修改，指针指向的值不能修改
const int * p1 = &a;
*p1 = 20;//错误,指针指向的值不能修改
p1 = &b;//正确,指针指向能修改

// 指针常量
// 特点:指针指向不能修改，指针指向的值能修改
int * const p2 = &a;
*p2 = 20;//正确,指针指向的值能修改
p2 = &b;//错误,指针指向不能修改

// 常量指针常量
// 特点：指针指向 和 指针指向的值 都不可以改
const int * const p3 = &a;
*p3 = 20;//错误,指针指向的值不能修改
p3 = &b;//错误,指针指向不能修改

```

记忆法:

const在 int * p前，代表 *p不能改

const在 p前，代表 p不能改



#### 指针访问数组

```c++
int arr[10] = { 1,3,3,4,5,6,7,8,9,10 };
int* p = arr;

cout << "第一个元素:" << arr[0] << endl;
cout << "指针访问第一个元素:" << *p << endl;
p++; // 指针向后偏移4个字节
cout << "指针访问第二个元素:" << *p<< endl;
```



#### 地址传递

利用指针作函数参数，可以修改实参的值

```c++
void swap(int * p1, int * p2) {
	int temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

int main() {
	int a = 10;
	int b = 20;
	swap(&a, &b);
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	return 0;
}
```



### 结构体

```c++
struct student {
	string name;
	int age;
	int score;
}s1;// s1为结构体变量，可省略，分号不能漏


// 使用
struct student stu;
stu.name = "cq";
stu.age = 18;
stu.score = 100;
```



#### 结构体指针

利用->操作符可以通过结构体指针访问结构体属性



#### 结构体函数参数

```c++
// 值传递，形参修改后不会影响实参
void modifyScore(struct student stu) {
	stu.score = 80;
}
// 地址传递,形参修改会影响实参
void modifyScore2(struct student * stu) {
	stu->score = 90;
}

int main() {
	struct student stu = {"cq", 18, 100};
	modifyScore(stu);
	cout << stu.score << endl;// 100
	modifyScore2(&stu);
	cout << stu.score << endl;// 90
	return 0;
}
```





### 内存四区

- 代码区：存放函数体的二进制代码，由操作系统进行管理
- 全局区：存放全局变量、静态变量、常量
- 栈区：由编译器自动分配释放，存放函数的参数值，局部变量等
- 堆区：由程序员分配和释放，若程序员不释放，程序结束时由操作系统回收



程序运行前:代码区、全局区

程序运行后:栈区、堆区



#### 代码区：

存放CPU执行的机器指令

代码区是共享的、只读的



#### 全局区:

全局变量、静态变量、字符串常量、const修饰的全局变量(全局常量)

该区域的数据在程序结束后由操作系统释放

```c++
// 全局变量
int g_a = 10;
// 全局常量
const int c_g_a = 10;

int main() {
    // 局部变量(栈区，不在全局区)
    int a = 10;
    // const 修饰的局部变量(栈区，不在全局区)
    const int c_l_a = 10;
    
    // 静态变量
    static int s_a = 10;
    
    // 字符串常量
    "hello world";
    
    return 0;
}
```



#### 栈区:

由编译器自动分配释放，存放函数的参数值，局部变量等

注意：不要返回局部变量的地址，栈区开辟的数据由编译器自动释放

```c++
int* func() { // 形参的数据也会放在栈区
    int a = 10; // 局部变量,存放在栈区,栈区的数据在函数执行后自动释放
    return &a; // 返回局部变量的地址
}

int main() {
    int *p = func();
    cout << *p << endl; // 10   第一次可以打印正确的数字是因为编译器做了保留
    cout << *p << endl; // xxxx 第二次这个数据就不在保留了
    return 0;
}
```



#### 堆区:

由程序员分配和释放，若程序员不释放，程序结束时由操作系统回收

在C++中主要利用new在堆区开辟内存

```c++
int* func() {
    int* a = new int(10);
    return a;
}

int main() {
    int *p = func();
    cout << *p << endl; // 10
    cout << *p << endl; // 10
    return 0;
}
```



### new/delete

new 返回的是该数据类型的指针

delete 释放/释放数组也是用delete

```c++
int* arr = new int[10];
delete[] arr;
```



### 引用

作用：给变量取别名

```c++
int a = 10;
// 语法: 数据类型 &别名 = 原名
int &b = a;
b = 100;
cout << a << endl; // 打印a也是= 100
```



#### 注意事项:

引用必须初始化

引用在初始化后,不能发生改变

```c++
int a = 10;
int c = 20;

//int &b; // 错误写法,引用必须初始化
int &b = a;
b = c; // 赋值操作,而不是更改引用
```



#### 引用函数参数

作用：函数传参时，可以利用引用的技术让形参修饰实参

优点：简化指针修改实参

```c++
void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}
int main() {
    int a = 10;
    int b = 20;
    swap(a, b); // a和b发生了改变
    
    return 0;
}
```



#### 引用做函数返回值

注意：不要返回局部变量引用

```c++
// 返回局部变量引用
int& test1() {
    int a = 10;
    return a;
}

// 返回静态变量引用
int& test2() {
    static int a = 10;
    return a;
}

int main() {
    // 返回局部变量引用错误示范
    int &ref = test1();
    cout << ref << endl; // 第一次结果正确,是因为编译器做了保留
    cout << ref << endl; // 第二次结果错误,是因为a的内存已释放
    
    int &ref2 = test2();
    test2() = 1000; // 如果函数的返回值是引用，这个函数调用可以作为左值
    cout << ref2 << endl; // 输出1000
    
}
```



#### 引用的本质

引用的本质在c++内部实现是一个指针常量

```c++
// 转化为: int* const ref = &a;
vod func(int& ref) {
    // 转化为 *ref = 100
    ref = 100;
}

int main() {
    int a = 10;
    int &ref = a; // 自动转换为 int* const ref = &a;
    ref = 20; // 自动转换为 *ref = 20
    
    func(a);
    return 0;
}
```



#### 常量引用

```c++
// 利用常量引用防止误操作修改实参
void showValue(const int& ref) {
	// ref = 1000; 不能修改
}

// 加上const之后,编译器将代码修改为  int temp = 10;  const int &ref = temp;
const int &ref = 10;

// ref = 20; 错误, 加上const之后变为只读,不可以修改
```



### 类和对象

#### class和struct区别

默认的访问权限不同

stuct：默认权限为公共

class：默认权限为私有



#### 构造、析构和拷贝分类及调用 

注意事项1：调用默认构造函数时，不要加()

注意事项2：不要利用拷贝构造，初始化匿名对象

```c++
class Person {
public:
    // 构造函数 (无参构造、默认构造)
	Person() {
	}
    // 有参构造
    Person(int a) {
	}
    // 拷贝构造
    Person(const Person &p) {
        // 为什么要加const,防止修改本体
        // 为什么传引用，是否能去掉,去掉的话相当于多拷贝一份数据了，没必要
        
        // ... 省略赋值操作
    }
    // 析构函数
	~Person() {
        // 在对象销毁前默认会调用析构函数
	}
};

// 调用方式
void test() {
	// 括号法
    Person p; // 默认构造
    Person p2(10); // 有参构造
    Person p3(p2); // 拷贝构造
    
    // 注意事项1
    // 调用默认构造函数时，不要加()
    // 下面这行代码，编译器认为会是一个函数的声明
    // Person p1(); // 函数声明,不是创建对象     类似 void func();
    
    // 显示法
 	Person p1;
    Person p2 = Person(10);
    Person p3 = Person(p2);
    
    Person(10); // 匿名对象 特点：当前执行结束后，系统会立即回收掉匿名对象
    
    // 注意事项2
    // 不要利用拷贝构造，初始化匿名对象, 编译器会认为 Person(p3) 等价于 Person p3;  相当于对象声明
    // Person(p3);
    
    // 隐式转换法
    Person p4 = 10; // 相当于 Person p4 = Person(10);
    Person p5 = p4; // 拷贝构造
    
}
```





####  拷贝构造函数调用时机

1.使用已经创建完毕的对象初始化一个新对象

```c++
Person p1;
Person p2(p1); // 调用拷贝构造
```

2.值传递方式给函数参数传值

```c++
void test(Person p) {
}

Person p;
test(p); // 调用拷贝构造
```



3.以值方式返回局部对象

```c++
Person test() {
    Person p;
    return p;
}

Person p = test(); // 调用拷贝构造
```



#### 构造函数调用规则

默认情况下，c++编译器至少给一个类添加3个函数

1.默认无参构造函数

2.默认析构函数

3.默认拷贝构造函数

规则:

- 如果用户定义有参构造函数，c++不再提供默认无参构造，但是会提供默认拷贝构造
- 如果用户定义拷贝构造函数，c++不会再提供其他构造函数



#### 深拷贝与浅拷贝

浅拷贝：简单的赋值拷贝操作

深拷贝：在堆区重新申请空间，进行拷贝操作



浅拷贝问题：堆区内存重复释放

```c++
Person(const Person &p) {
    // mAge = p.mAge; 浅拷贝操作
    
    // 深拷贝 解决 浅拷贝在堆区内存重复释放的问题
    mAge = new int(*p.mAge);
}
~Person() {
    if (mAge != NULL) {
        delete mAge;
        mAge = NULL;
    }
}
```



总结：如果属性又在堆区开辟的，一定要自己提供拷贝构造函数，防止浅拷贝带来的问题





