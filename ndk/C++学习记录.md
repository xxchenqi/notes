# C++ 学习记录

### 1.C/C++部分区别:

1. C++可以运行C的代码，但是C不能运行C++
2. C的常量是个假常量，C++的常量，是真常量(无法修改)
3. C互换两个数用地址，C++互换两个数用引用
4. C++常量引用是只读的
5. C不支持重载，C++是支持的
6. C++参数名可以省略，只需要写参数类型，增强后期扩展功能
7. C的开辟堆空间是malloc和free，C++是new和delete



### 2.命名空间

相当于内部类，我有时候理解为java的静态打包

```c
using namespace std;

namespace cq {
    void action() {
        cout << "cq action" << endl;
    }
}
namespace cq2 {
    void action() {
        cout << "cq2 action" << endl;
    }
}

int main() {
    cout << "命名空间" << endl;
	// 声明各个写的 命名空间
    using namespace cq;
    action();
    using namespace cq2;
    
    cq::action();
    cq2::action();
    return 0;
}
```





### 3.构造函数、拷贝构造函数、析构函数

构造函数执行顺序分析

```c
#include<iostream>;
using namespace std;

class Student{
public:
	char *name;
	int age;

	Student() {
		cout << "空构造" << endl;
	}
	Student(char *name):Student(name, 18){
		cout << "一参构造" << endl;
	}
	Student(char *name, int age) {
		cout << "二参构造" << endl;
	}
	Student(const Student &stu){
		cout << "拷贝构造" <<"stu->"<<&stu<<",this->"<<this<< endl;
	}
	~Student(){
		cout << "析构" << "this->" << this << endl;
	}
};

Student getStu(){
	Student stu("cq");
	cout << "getStu->"<<&stu<< endl;
	return stu;
}

int main(){
	Student stu = getStu();
	cout << "main stu->" << &stu << endl;
	getchar();
	return 0;
}

/*
输出：
二参构造
一参构造
getStu->00CFFC20
拷贝构造stu->00CFFC20,this->00CFFD28
析构this->00CFFC20
main stu->00CFFD28
*/


/*
情况分析 
Student s1;
Student s2 = s1;
以上情况只会调用1次构造函数，
s2 = s1 会调用拷贝构造函数,不会调用构造函数
*/



```

### 4.浅拷贝和深拷贝

默认的拷贝构造函数是浅拷贝



浅拷贝

```c
// 默认有一个拷贝构造函数 隐士的 我们看不见
Student2(const Student2 & stu) {
	// 【浅拷贝】：新地址name  旧地址name 指向同一个空间，会造成，重复free的问题，引发奔溃
	// 新地址name = 旧地址 （浅拷贝）
	this->name = stu.name;
} 
```

深拷贝

```C
// 自定义拷贝构造函数 如果有堆成员，必须采用深拷贝
Student(const Student &stu) {
	// 【深拷贝】
	this->name = (char *)malloc(sizeof(char *)* 10);
	strcpy(this->name, name);
}

~Student() {
	free(this->name);
	this->name = NULL;
}
```



### 5.可变参数

```c
#include <iostream>
#include <stdarg.h> // 可变参数的支持
using namespace std;

// Java的可变参数: int ...
// C++的可变参数写法：...
// count的第一个用处：内部需要一个 存储地址用的参考值，如果没有第二个参数，内部他无法处理存放参数信息
void sum(int count, ...) {
    va_list vp; // 可变参数的动作

    // 参数一：可变参数开始的动作vp
    // 参数二：存储地址用的参考值，如果没有第二个参数，内部他无法处理存放参数信息
    va_start(vp, count);

    // 取出可变参数的值
    for (int i = 0; i < count; ++i) {
        int r = va_arg(vp, int);
        cout << r << endl;
    }
    
    // 关闭阶段
    va_end(vp);
}
```



### 6.static

```C
/**
 * 静态的总结：
 * 1.可以直接通过类名::静态成员（字段/函数）
 * 2.静态的属性必须要初始化，然后再实现
 * 3.静态的函数只能操作静态的属性和方法
 */

#include <iostream>
using namespace std;

class Dog {
public:
    char * info;
    int age;

    // 先声明
    static int id;
	// 不允许这样初始化
	// static int id = 9;

    static void update() {
        id += 100;
        // 报错:静态函数不能调用非静态函数
        // update2();
    }

    void update2() {
        id = 13;
    }
};

// 再实现
int Dog::id = 9;

int main() {
    Dog dog;
    dog.update2(); // 普通函数
    Dog::update(); // 静态函数
    dog.update(); // 对象名.静态函数（一般都是使用::调用静态成员）
    cout << Dog::id << endl;
    return 0;
}

```



### 7.默认值、常量指针、指针常量、常量指针常量

1.C++中不像Java，Java有默认值， 如果你不给默认值，那么就是系统值 -64664

2.类型* const  指针常量【地址对应的值能改，地址不可以修改】

3.const 类型*  常量指针【地址可以修改，地址对应的值不能改】

4.const 类型* const 常量指针常量【地址不能改，地址对应的值不能改】-> void changeAction() const {}



```C
#include <iostream>
using namespace std;

class Worker {
public:
    char * name;
    int age = NULL; // C++中不像Java，Java有默认值， 如果你不给默认值，那么就是系统值 -64664

    // int * const  指针常量 【地址对应的值能改，地址不可以修改】
    // const int *  常量指针 【地址可以修改，地址对应的值不能改】

    // 纠结：原理：为什么可以修改age
    // 默认持有隐士的this，类型 * const 指针常量
    void change1() {
        // 地址不可以修改
        // this = 0x43563;
        // 地址对应的值能改
        this->age = 100;
        this->name = "JJJ";
    }

    // 默认现在：this 等价于 const Student * const  常量指针常量（地址不能改，地址对应的值不能改）
    void changeAction() const {
        // 地址不能改
        // this = 0x43563;
        // 地址对应的值不能改
        // this->age = 100;
    }
};
```



### 8.友元

可以访问所有私有成员

友元的实现不需要关键字，也不需要 对象:: ,只需要保证 函数名（参数）

```c
#include <iostream>
using namespace std;

class Person {
private: // 私有的age，外界不能访问
    int age = 0;

public:
    Person(int age) {
        this->age = age;
    }

    int getAge() {
        return this->age;
    }

    // 定义友元函数 (声明，没有实现)
    friend void updateAge(Person * person, int age);
};

// 友元函数的实现，可以访问所有私有成员
void updateAge(Person* person, int age) {
    // 默认情况下：不能修改 私有的age
    // 谁有这个权限：友元（拿到所有私有成员）
    person->age = age;
}

int main() {
    Person person = Person(9);
    updateAge(&person, 88);
    cout << person.getAge() << endl;
    return 0;
}
```



### 9.各个函数区别

```c
#include <iostream>
using namespace std;

#ifndef PIG_H // 你有没有这个宏（Java 宏==常量）
#define PIG_H // 定义这个宏

class Pig {
private:
    int age;
    char * name;

public:
    // 静态成员声明
    static int id;

    // 构造函数的声明系列
    Pig();
    Pig(char *);
    Pig(char *,int);

    // 析构函数
    ~Pig();

    // 拷贝构造函数
    Pig(const Pig & pig);

    // 普通函数 set get
    int getAge();
    char * getName();
    void setAge(int);
    void setName(char *);

    void showPigInfo() const; // 常量指针常量 只读

    // 静态函数的声明
    static void changeTag(int age);

    // 友元函数的声明
    friend void changeAge(Pig * pig, int age);
};

#endif // 关闭/结尾

```

```c
#include "Pig.h"

// 实现构造函数
Pig::Pig() {
    cout << "默认构造函数" << endl;
}

Pig::Pig(char * name) {
    cout << "1个参数构造函数" << endl;
}

Pig::Pig(char * name, int age) {
    cout << "2个参数构造函数" << endl;
}

// 实现析构函数
Pig::~Pig() {
    cout << "析构函数" << endl;
}

// 实现 拷贝构造函数
Pig::Pig(const Pig &pig) {
    cout << "拷贝构造函数" << endl;
}

int Pig::getAge() {
    return this->age;
}
char * Pig::getName() {
    return this->name;
}
void Pig::setAge(int age) {
    this->age = age;
}
void Pig::setName(char * name) {
    this->name = name;
}

void Pig::showPigInfo() const {
} // 常量指针常量 只读

// 实现 静态属性【不需要增加 static关键字】
int Pig::id = 878;

// 实现静态函数，【不需要增加 static关键字】
void Pig::changeTag(int age) {
}

// 友元的实现
// 友元特殊：不需要关键字，也不需要 对象:: ,只需要保证 函数名（参数）
void changeAge(Pig * pig, int age) {

}


```

### 10.运算符重载

```c
#include <iostream>
using namespace std;

class Cq{
private:
    int x,y;

public:
    Cq() {}

    Cq(int x, int y) :x(x), y(y) {}

    void setX(int x) {
        this->x = x;
    }
    void setY(int y) {
        this->y = y;
    }
    int getX() {
        return this->x;
    }
    int getY() {
        return this->y;
    }
	
    //// 运算符+ 重载
    // 常量引用：不允许修改，只读模式
    // const 关键字的解释
    // & 性能的提高，如果没有&  运行+ 构建新的副本，会浪费性能
    // 如果增加了& 引用是给这块内存空间取一个别名而已
    Cq operator + (const Cq& cq1) {
        int x = this->x + cq1.x; 
        int y = this->y + cq1.y;
        return Cq(x, y);
    }

    // 运算符- 重载
    Cq operator - (const Cq & cq1) {
        int x = this->x - cq1.x;
        int y = this->y - cq1.y;
        return Derry(x, y);
    }

    // 对象++ 运算符 重载
    void operator ++() { //  ++对象
        this->x = this->x + 1;
        this->y = this->y + 1;
    }
    void operator ++ (int) { // 对象++
        this->x = this->x + 1;
        this->y = this->y + 1;
    }

    // istream 输入 系统的
    // ostream 输出 系统的
    // 输出 运算符重载
    friend void operator << (ostream & _START, Cq cq) {
        // 输出换行：<< endl;
        _START << " 单 输出 " << cq.x << " ! " << cq.y << " 结束 " << endl;
    }

    // 多个的  ostream 输出 系统的
    // 输出 运算符重载 复杂 涉及到规则  重载 >>
    /*friend ostream & operator >> (ostream & _START, const Cq & cq) {
        _START << " 多 输出 " << cq.x << " ! " << cq.y << " 结束 " << endl;
        return _START;
    }*/

    // istream 输入 系统的
    friend istream & operator >> (istream & _START, Cq & cq) {
        // 接收用户的输入，把输入的信息，给x
        // _START >> cq.x;
        // _START >> cq.y;

        // 也可以这样写，可读性不好，简化了
        _START >> cq.x >> cq.y;
        return _START;
    }
};
```

括号运算符

```c
//数组 系统源码把此括号[i]给重载，  系统重载后的样子 *(arr+i)

#include <iostream>
using namespace std;

class ArrayClass {

private:
    // C++ 默认都是系统值  size 系统值 -13275
    int size = 0; // 大小  开发过程中，给size赋默认值，不然会出现，后患无穷的问题
    int * arrayValue; // 数组存放 int 类型的很多值

public:
    void set(int index, int value) {
        arrayValue[index] = value;
        size+=1;
    }
    int getSize() {
        return this->size;
    }
    // 运算符重载 [index]
    int operator[](int index) {
        return this->arrayValue[index];
    }
};

// 输出容器的内容
void printfArryClass(ArrayClass arrayClass) {
    cout << arrayClass.getSize() << endl;
    for (int i = 0; i < arrayClass.getSize(); ++i) {
        cout << arrayClass[i] << endl; // []是我们自己的 重载符号
    }
}
```



### 11.继承

1.默认是私有继承  : private Person
2.私有继承：在子类里面是可以访问父类的成员，但是在类的外面不行
3.必须公开继承，才可以访问父类的成员



c++多继承二义性问题：

```c
// 多继承 二义性2：
// 在真实开发过程中，严格避免出现 二义性

#include <iostream>
using namespace std;

// 祖父类
class Object {
public:
    int number;
};

// 父类1
class BaseActivity1 : public Object {
};

// 父类2
class BaseActivity2 : public Object {
};

// 子类
class Son : public BaseActivity1, public BaseActivity2 {
// 第二种解决方案： 在类中定义同名成员，覆盖掉父类的相关成员
public:
    int number;
};


int main() {
    Son son;

    // error: request for member 'show' is ambiguous  二义性 歧义
    // son.number = 2000;

    // 第一种解决方案： :: 明确指定
    son.BaseActivity1::number  = 1000;
    son.BaseActivity2::number  = 1000;

    // 第二种解决方案： 在类中定义同名成员，覆盖掉父类的相关成员
    son.number = 3000;

    // 第三种解决方案： 【虚基类】 属于 虚继承的范畴

    return 0;
}
```

#### 虚基类

```c
// 第三种解决方案： 【虚基类】 属于 虚继承的范畴
// 真实C++开始，是很少出现，二义性（歧义） 如果出现， 系统源码（系统用 第三种解决方案）
#include <iostream>
using namespace std;

// 祖父类
class Object{
public:
    int number;
    void show() {
        cout << "Object show run..." << endl;
    }
};

// 父类1
class BaseActivity1 : virtual public Object {
// public:int number; // 人为制作二义性  error: request for member 'number' is ambiguous
};

// 父类2
class BaseActivity2 : virtual public Object {
// public:int number;
};

// 子类
class Son : public BaseActivity1, public BaseActivity2 {

};
```



### 12.多态

#### 动态多态

C++默认关闭多态，怎么开启多态？ 虚函数在父类上给函数增加 virtual关键字

```c
// 3.多态（虚函数）。   动态多态（程序的角度上：程序在运行期间才能确定调用哪个类的函数 == 动态多态的范畴）
// Java语言默认支持多态
// C++默认关闭多态，怎么开启多态？ 虚函数在父类上给函数增加 virtual关键字
#include <iostream>
using namespace std;

class BaseActivity {
public:
     virtual void onStart() {
        cout << "BaseActivity onStart" << endl;
    }
};

class HomeActivity : public BaseActivity {
public:
    void onStart() { // 重写父类的函数
        cout << "HomeActivity onStart" << endl;
    }
};

class LoginActivity : public BaseActivity {
public:
    void onStart() { // 重写父类的函数
        cout << "LoginActivity onStart" << endl;
    }
};

// 在此函数 体系多态，例如：你传入HomeActivity，我就帮你运行HomeActivity
void startToActivity(BaseActivity * baseActivity) {
    baseActivity->onStart();
}

int main() {
    // TODO 第一版本
    HomeActivity *homeActivity = new HomeActivity();
    LoginActivity *loginActivity = new LoginActivity();

    startToActivity(homeActivity);
    startToActivity(loginActivity);

    if (homeActivity && loginActivity) delete homeActivity; delete loginActivity;


    // TODO 第二个版本
    BaseActivity * activity1 = new HomeActivity();
    BaseActivity * activity2 = new LoginActivity();
    startToActivity(activity1);
    startToActivity(activity2);


    // TODO 抛开 C++ 抛开Java 等等，请问什么是多态？ 父类的引用指向之类的对象，同一个方法有不同的实现，重写（动态多态）和   重载(静态多态)

    return 0;
}

```

#### 静态多态

```C
// 静态多态 （编译期已经决定，调用哪个函数了，这个就属于静态多态的范畴）  重载（静态多态）
#include <iostream>
using namespace std;

void add(int number1, int number2) {
    cout << number1 + number2 << endl;
}

void add(float number1, float number2) {
    cout << number1 + number2 << endl;
}

void add(double number1, double number2) {
    cout << number1 + number2 << endl;
}

int main() {
    add(10000, 10000);
    add(1.9f, 2.8f);
    add(545.4, 654.54);
    return 0;
}
```



### 13.纯虚函数

相当于java的抽象类。

纯虚函数是必须继承的（如果子类没有重写纯虚函数，子类就是抽象类）， 虚函数是不是不必须的

抽象类型绝对不能实例化

```C
// 纯虚函数（Java版抽象类）
// C++纯虚函数(C++没有抽象类)  相当于Java的抽象类
#include <iostream>
using namespace std;

// 抽象类/纯虚函数： 分为：1.普通函数， 2.抽象函数/纯虚函数
class BaseActivity {
private:
    void setContentView(string layoutResID) {
        cout << "XmlResourceParser解析布局文件信息... 反射" << endl;
    }

public:
    // 1.普通函数
    void onCreate() {
        setContentView(getLayoutID());
        initView();
        initData();
        initListener();
    }

    // 纯虚函数是必须继承的（如果子类没有重写纯虚函数，子类就是抽象类）， 虚函数是不是不必须的

    // 2.抽象函数/纯虚函数
    // virtual string getLayoutID(); // 虚函数
    virtual string getLayoutID() = 0; // 纯虚函数
    virtual void initView() = 0;
    virtual void initData() = 0;
    virtual void initListener() = 0;
};

// 子类 MainActivity
class MainActivity : public BaseActivity { // MainActivity如果没有重新父类的纯虚函数，自己就相当于 抽象类了
    string getLayoutID() {
        return "R.layout.activity_main";
    }
    void initView()  {}
    void initData() {}
    void initListener() {}
};

int main() {
    // 错误：抽象类型 MainActivity 绝对不能实例化
    // MainActivity mainActivity;

    // 重写了父类所有的纯虚函数
    MainActivity mainActivity;
    return 0;
}

```

全纯虚函数相当于接口(省略)

#### 接口回调例子

```C
// 7.回调。  Java的登录 简单的 接口

#include <iostream>
using namespace std;

// 登录成功的Bean
class SuccessBean {
public:
    string username;
    string userpwd;

    SuccessBean(string username, string userpwd)
    :username(username), userpwd(userpwd) {}
};

// 登录响应的接口  成功，错误
class ILoginResponse {
public:
    // 登录成功
    virtual void loginSuccess(int code, string message, SuccessBean successBean) = 0;

    // 登录失败
    virtual void loginError(int code, string message) = 0;
};

// 登录的API动作
void loginAction(string name, string pwd, ILoginResponse & loginResponse) {
    if (name.empty() || pwd.empty()) {
        cout << "用户名或密码为空!" << endl;
        return;
    }

    if ("cq" == name && "123" == pwd) {
        loginResponse.loginSuccess(200, "登录成功", SuccessBean(name, "恭喜你进入"));
    } else {
        loginResponse.loginError(404, "登录错误，用户名或密码错误...");
    }
}

// 写一个实现类，继承接口
// 接口实现类
class ILoginResponseImpl : public ILoginResponse {
public:
    // 登录成功
    void loginSuccess(int code, string message, SuccessBean successBean) {
        cout << "恭喜登录成功 " << "code:" << code << " message:" << message
        << "successBean:" << successBean.username << "," << successBean.userpwd << endl;
    }

    // 登录失败
    void loginError(int code, string message) {
        cout << " 登录失败 " << "code:" << code << " message:" << message << endl;
    }
};

int main() {
    // Allocating an object of abstract class type 'ILoginResponse'
    // 正在分配抽象类型为ILoginResponse的对象  不能被实例化
    // 为什么不可以:
    // 1.他不是Java的接口，C++也没有接口，他只是像接口而已。
    // 2.他也不是抽象类，C++也没有抽象类，他只是像抽象类而已。
    // 3.他是纯虚函数的类，此类决定不准你实例化  无论堆区 还是栈区
    /*new ILoginResponse() {
        // 登录成功
        void loginSuccess(int code, string message, SuccessBean successBean) {

        }

        // 登录失败
        void loginError(int code, string message) {

        }
    }*/

    string username;
    cout << "请输入用户名.." << endl;
    cin >> username;

    string userpwd;
    cout << "请输入密码.." << endl;
    cin >> userpwd;

    ILoginResponseImpl iLoginResponse;
    loginAction(username, userpwd, iLoginResponse);

    return 0;
}
```



### 14.模板函数

```C
// 8.模版函数（Java版泛型）。  C++没有泛型 C++的模板函数 非常类似于 Java的泛型
#include <iostream>
using namespace std;

// 模板函数  == Java的泛型解决此问题
template <typename TT>
void addAction(TT n1, TT n2) {
    cout << "模板函数：" << n1 + n2 << endl;
}

int main() {
    addAction(1, 2);
    addAction(10.2f, 20.3f);
    addAction(545.34, 324.3);
    addAction<string>("AAA", "BBB");

    return 0;
}

```





### 15.STL - Standard Template Library

#### (1).vector

封装动态大小数组作为容器，能够存放任意的动态数组

```c
#include <iostream>
#include <vector> // 引入 vector 容器的支持
using namespace std;

int main() {
    vector<int> vector1;
    vector<int> vector2(10); // 指定10的空间大小
    vector<int> vector3(10, 0); // 有了10个值了 每个值都是0
    vector<int> vector4;

    // vector4.begin() 迭代器 插入到前面
    // vector4.end() 迭代器 插入到后面

    // 插入数据
    vector4.insert(vector4.begin(), 40);
    // 第一个
    vector4.front() = 99; // 默认修改第一个
    // 最后一个
    vector4.back() = 777; // 默认修改最后
	// 移除第一个元素（内部：通过迭代器的位置 进行移除）
    vector4.erase(vector4.begin()); 

    // 循环打印，默认 从大到小输出
    for (int i = 0; i < vector4.size(); ++i) {
        cout << "item:" << vector4[i] << endl;
    }

    // 迭代器 循环遍历
    // for (vector<int>::iterator iteratorVar = vector4.begin(); iteratorVar != vector4.end(); iteratorVar++) {
    for (auto iteratorVar = vector4.begin(); iteratorVar != vector4.end(); iteratorVar++) {
        // 迭代器 当中指针操作  iteratorVar++
        cout << "迭代器：" << *iteratorVar << endl;
    }

    return 0;
}
```



#### (2).stack

先进后出，后进先出

```c
#include <iostream>
#include <stack>
using namespace std;

int main() {
    stack<int> stackVar;

    // 压栈（注意：stack无法指定那个位置去压栈）
    stackVar.push(30);

    while (!stackVar.empty()) {
        int top = stackVar.top(); // top == 获取栈顶的元素
        cout << "获取栈顶的元素：" << top << endl; // 永远拿 90

        stackVar.pop(); // 把栈顶的元素 弹出去  【删除】
    }


    return 0;
}
```



#### (3).queue

```C
#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> queueVar;
    queueVar.push(20);
    queueVar.push(40);
    while (!queueVar.empty()) {
        cout << "while:" << queueVar.front() << endl;
        queueVar.pop(); // 把前面的元素 给消费掉  【删除】
    }
    return 0;
}
```



#### (4).priority_queue

```C
#include <iostream>
#include <queue>
using namespace std;

int main() {
    
    // less return __x < __y;  // 从大到小
    // greater return __x > __y; // 从小到大
    priority_queue<int ,vector<int>, less<int>> priorityQueue;
    // priority_queue<int ,vector<int>, greater<int>> priorityQueue;

    priorityQueue.push(10);
    priorityQueue.push(20);
    priorityQueue.push(30);

    while (!priorityQueue.empty()) {
        cout << "while:" << priorityQueue.top() << endl;
        priorityQueue.pop(); // 最前面的元素消费掉
    }
    return 0;
}
```



#### (5).list

```C
// Java：ArrayList采用Object[]数组，   C++的list 内部：采用链表
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> listVar;
    // 插入操作
    listVar.push_front(50); // 插入到前面
    listVar.push_back(60); // 插入到后面
    listVar.insert(listVar.begin(), 70); // 插入到前面
    listVar.insert(listVar.end(), 80); // 插入到后面

    // 修改操作
    listVar.back() = 88;
    listVar.front() = 55;

    // 删除
    listVar.erase(listVar.begin()); // 删除最前面的 55
    listVar.erase(listVar.end()); // 删除最后面的 88

    // list 迭代器
    for (list<int>::iterator it = listVar.begin(); it != listVar.end() ; it ++) {
        cout << *it << endl;
    }

    return 0;
}
```



#### (6).set

```C
// set（内部：红黑树结构），会对你存入的数据进行排序

#include <iostream>
#include <set>
using namespace std;

int main() {
    set<int, less<int>> setVar; //  __x < __y 从小到大，默认情况下 就是 less

    // 添加参数，不需要用迭代器，也不需要指定位置
    setVar.insert(1);
    setVar.insert(3);
    setVar.insert(2);
    setVar.insert(4);

    // 重复插入，并不会报错  std::pair<iterator, bool>
    pair<set<int, less<int>>::iterator, bool> res = setVar.insert(8);

    // res.first 获取第一个元素 迭代器   当前迭代器   最后一个位置
    // res.second 获取第二个元素 bool
    bool insert_success = res.second;
    if (insert_success) {
        cout << "插入成功" << endl;
        // 插入成功后，用第一个元素遍历
        for (; res.first != setVar.end(); res.first ++) {
            cout << *res.first << endl;
        }
    } else {
        cout << "插入失败.." << endl;
    }

    // 全部遍历  auto 自动推到
    for (auto it = setVar.begin(); it != setVar.end() ; it ++) {
        cout << *it << endl;
    }

    return 0;
}
```



#### (7).map

```C
#include <iostream>
#include <map>
using namespace std;

int main() {
    // 注意：map会对key进行排序，默认 key不能重复
    map<int, string> mapVar;

    // TODO 添加数据
    // 第一种方式
    mapVar.insert(pair<int, string>(1, "一"));

    // 第二种方式
    mapVar.insert(make_pair(2, "二"));

    // 第三种方式
    mapVar.insert(map<int, string>::value_type (3, "三"));

    // 上面三种方式 key不能重复
    // 思考：既然会对key进行排序，那么key是不能重复的（会插入失败）
    mapVar.insert(pair<int, string>(3, "三3"));

    // 第四种方式    mapVar[key]=Value
    mapVar[4] = "四";
    mapVar[4] = "肆"; // 第四种方式覆盖/替换（常用）

    /**
     *  typedef typename _Rep_type::iterator		 iterator;  之前常规的迭代器
        typedef typename _Rep_type::const_iterator	 const_iterator;  只读的，只能读，不能修改 的迭代器
        typedef typename _Rep_type::reverse_iterator	 reverse_iterator;  倒序打印的迭代器
     */

    // 循环打印，迭代器
    for (map<int, string>::iterator it = mapVar.begin() ; it != mapVar.end() ; it ++) {
        cout << it->first << "," << it->second.c_str() << "\t";
    }

    // 怎么判断插入成功、失败
    std::pair<map<int, string>::iterator, bool> result = mapVar.insert(map<int, string>::value_type (6, "66三san"));
    if (result.second) {
        cout << "插入成功" << endl;
    } else {
        cout << "插入失败" << endl;
    }
    
    // 插入后的数据
    for (; result.first != mapVar.end() ; result.first++) {
        cout << result.first->first << " , " << result.first->second <<  endl;
    }

    // 查找，操作
    map<int, string> ::iterator findResult = mapVar.find(3); // 查找
    if (findResult != mapVar.end()) {
        cout << "找到了" << findResult->first << "," << findResult->second.c_str() << endl;
    } else {
        cout << "未找到" << endl;
    }

    return 0;
}
```



#### (8).multimap

```C
#include <iostream>
#include <map>
using namespace std;

int main() {
    // 1.key可以重复， 2.key重复的数据可以分组,  3.key会排序，  4.value不会排序
    multimap<int, string> multimapVar;

    multimapVar.insert(make_pair(10, "十个1"));
    multimapVar.insert(make_pair(10, "十个2"));
    multimapVar.insert(make_pair(10, "十个3"));

    multimapVar.insert(make_pair(30, "三十1"));
    multimapVar.insert(make_pair(30, "三十3"));
    multimapVar.insert(make_pair(30, "三十2"));

    multimapVar.insert(make_pair(20, "二十1"));
    multimapVar.insert(make_pair(20, "二十2"));
    multimapVar.insert(make_pair(20, "二十3"));

    for (auto iteratorVar = multimapVar.begin(); iteratorVar != multimapVar.end() ; iteratorVar ++) {
        cout << iteratorVar->first << "," << iteratorVar->second << endl;
    }
   
    // TODO 核心功能是分组
    int result;
    cout << "请输入你要查询的key，为int类型:" << endl;
    cin >> result;

    multimap<int, string>::iterator iteratorVar = multimapVar.find(result);
    while (iteratorVar != multimapVar.end()) {
        cout << iteratorVar->first << "," << iteratorVar->second << endl;
        iteratorVar++;

        if (iteratorVar->first != result) {
            break; // 循环结束
        }

        // 严谨性
        if (iteratorVar == multimapVar.end()) {
            break; // 循环结束
        }
    }

    return 0;
}
```







### 16.谓词/仿函数



#### 自定义对象比较功能

```C
#include <iostream>
#include <set>
using namespace std;

class Person {
public:
    string name;
    int id;
    Person(string name, int id) : name(name), id(id) {}
};


// C++源码没有对象比较的功能
// bool operator()(const _Tp& __x, const _Tp& __y) const { return __x < __y; }

// 我们就自定义这个功能  【自定义谓词 没有达到谓词的标准】
bool doCompareAction(const Person& person1, const Person& person2) {
    return person1.id < person2.id;
};

// 真正的谓词
struct doCompareAction2 {
public:
    bool operator() (const Person& __x, const Person& __y) {
        return __x.id < __y.id;
    }
};

struct doCompareAction3 {
public:
    bool operator() (const Person& __x, const Person& __y) {
        return __x.id > __y.id;
    }
};

int main() {
    
    set<Person, doCompareAction2> setVar;
    // set<Person, doCompareAction3> setVar;

    // 构建对象
    Person p1 ("Snake", 1);
    Person p2 ("kevin", 2);
    Person p3 ("Derry", 3);

    // 把构建的对象 插入到 set 容器里面去
    setVar.insert(p1);
    setVar.insert(p2);
    setVar.insert(p3);

    for (set<Person>::iterator it = setVar.begin(); it != setVar.end() ; it ++) {
        cout << it->name.c_str() << " , " << it->id << endl;
    }

    return 0;
}
```



#### 空谓词

```C
#include <iostream>
using namespace std;

// TODO 第一版: 为什么叫仿函数 （空谓词 一元谓词 二元谓词 三元谓词）
class ComPareObject {
public:
    void operator()() { // 重装了括号运算符
        cout << "仿函数" << endl;
    }
};

// 普通函数
void fun2() {
    cout << "普通函数" << endl;
}

int main() {
    ComPareObject fun1;

    // 非常像函数的调用，很像函数 = （仿函数）
    fun1();
    
    fun2();

    return 0;
}
```



```C
// 谓词中戏

#include <iostream>
#include <set> // STL包
#include <algorithm> // 算法包

using namespace std;

// 我如何阅读C++源码，来写我们的仿函数
// 明明白白的仿函数（一元谓词==一元函数对象）
class showActionObj {
public:
    void operator()(int content) {
        cout << "自定义仿函数" << content << endl;
    }
};

// 回调函数 如果叫 仿函数 有点合理
// 简洁方式（回调函数、一元谓词      但是不能称为 仿函数）
void showAction(int content) {
    cout << "自定义 一元谓词" << content << endl;
}

using namespace std;

int main() {
    set<int> setVar;

    setVar.insert(10);
    setVar.insert(20);
    setVar.insert(30);
    setVar.insert(40);
    setVar.insert(50);
    setVar.insert(60);

    // for_each(setVar.begin(), setVar.end(), showActionObj());

    for_each(setVar.begin(), setVar.end(), showAction);

    return 0;
}
```



#### 一元谓词

```C
#include <iostream>
#include <set> // STL包
#include <algorithm> // 算法包

using namespace std;

//仿函数（一元谓词==一元函数对象）
class showActionObj {
public:
    void operator()(int content) {
        cout << "自定义仿函数" << content << endl;
    }
};

// 回调函数 如果叫 仿函数 有点合理
// 简洁方式（回调函数、一元谓词,但是不能称为 仿函数）
void showAction(int content) {
    cout << "自定义 一元谓词" << content << endl;
}


int main() {
    set<int> setVar;

    setVar.insert(10);
    setVar.insert(20);
    setVar.insert(30);
    setVar.insert(40);
    setVar.insert(50);
    setVar.insert(60);

    // for_each(setVar.begin(), setVar.end(), showActionObj());

    for_each(setVar.begin(), setVar.end(), showAction);

    return 0;
}

```



#### 仿函数实现计数功能

```C
#include <iostream>
#include <set> // STL包
#include <algorithm> // 算法包

using namespace std;

// 回调函数 (功能够简单)
void showAction(int __first) {
    cout << "一元谓词" << __first << endl;
}

// 仿函数(扩展性强) C++内置源码使用仿函数频率高，扩展性强
class showActionObj {
public:
    int count = 0;
    void _count() { cout << "本次输出次数是:" << this->count << endl; }

    void operator() (int __first) {
        cout << "仿函数" << __first << endl;
        count++;
    }
};

int main() {
    // 理解：类型传递
    // set<int, showActionObj> setVar; 这样写的语法是OK的，不能加括号
    set<int> setVar;

    setVar.insert(10);
    setVar.insert(20);
    setVar.insert(30);
    setVar.insert(40);
    setVar.insert(50);
    setVar.insert(60);

    // TODO 第一种方式
    for_each(setVar.begin(), setVar.end(), showAction);


    // TODO 第二种方式
    showActionObj s; 
    s = for_each(setVar.begin(), setVar.end(), s);
    s._count();

    return 0;
}

```

#### 二元谓词

```C
#include <iostream>
#include <set>
using namespace std;

// C++源码：typename _Compare = std::less   less内置的仿函数，根据内置仿函数去写 自定义
//  bool operator()(const _Tp& __x, const _Tp& __y) const 二元谓词
class CompareObjectClass {
public:
    bool operator() (const string & __x, const string & __y) const { // const 指针 const  常量指针常量 = 只读
        return __x > __y;
    }
};

int main() {
    set<string, CompareObjectClass> setVar; // 第一版

    setVar.insert(setVar.begin(), "AAAAAAA");
    setVar.insert(setVar.begin(), "BBBBBBB");
    setVar.insert(setVar.begin(), "CCCCCCC");
    setVar.insert(setVar.begin(), "DDDDDDD");
    setVar.insert(setVar.begin(), "EEEEEEE");
    setVar.insert(setVar.begin(), "FFFFFFF");

    // 迭代器 循环
    for (set<string>::iterator iteratorVar = setVar.begin(); iteratorVar != setVar.end(); iteratorVar++) {
        cout << "循环item:" << *iteratorVar  << "\t";
    }

    return 0;
}
```



### 17.算法包

#### bind2nd 函数适配器

```C
#include <iostream>
#include <set> // stl包
#include <algorithm> // 算法包
using namespace std;

int main() {
    set<string, less<string>> setVar;
    setVar.insert("AAAA");
    setVar.insert("BBBB");
    setVar.insert("CCCC");

    set<string, less<string>>::iterator iteratorResult =find_if(setVar.begin(), setVar.end(), bind2nd(equal_to<string>(), "CCCC"));

    if (iteratorResult != setVar.end()) {
        cout << "查找到了" << endl;
    } else {
        cout << "没有查找到" << endl;
    }

    return 0;
}
```

#### for_each 遍历

```
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包
using namespace std;

class __F {
public:
    void operator() (int __first) {
        cout << "自定义一元谓词:" << __first << endl;
    }
};

int main() {
    vector<int> vectorVar;
    vectorVar.insert(vectorVar.begin(), 10000);
    vectorVar.insert(vectorVar.begin(), 20000);
    vectorVar.insert(vectorVar.begin(), 30000);

    for_each(vectorVar.begin(), vectorVar.end(), __F());

    return 0;
}
```



#### transform 变化

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包
using namespace std;

class __unary_op {
public:
    int operator() (const int __first) {
        return __first + 9; // 修改每个元素 +9
    }
};

int main() {
    vector<int> vectorVar;
    vectorVar.insert(vectorVar.begin(), 10000);
    vectorVar.insert(vectorVar.begin(), 20000);
    vectorVar.insert(vectorVar.begin(), 30000);
    vectorVar.insert(vectorVar.begin(), 40000);

    // 迭代器 result == 参数三
    transform(vectorVar.begin(), vectorVar.end(), vectorVar.begin(), __unary_op());

    for (auto it = vectorVar.begin(); it != vectorVar.end() ; it++) {
        cout << "第一种方式:" << *it << endl;
    }

    // TODO 第二种方式
    vector<int> vectorVarResult; // vectorVarResult 大小空间
    vectorVarResult.resize(vectorVar.size());
    transform(vectorVar.begin(), vectorVar.end(), vectorVarResult.begin(), __unary_op());

    for (auto it = vectorVarResult.begin(); it != vectorVarResult.end() ; it++) {
        cout << "第二种方式:" << *it << endl;
    }

    return 0;
}
```



#### find_if 查找

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包
using namespace std;

// find_if 有自定义仿函数 / find 没有自定义仿函数
class __pred {
public:
    int number;
    __pred(int number) : number(number) {}
    bool operator() (const int value) {
        return number == value;
    }
};

int main() {
    vector<int> vectorVar;
    vectorVar.insert(vectorVar.begin(), 10000);
    vectorVar.insert(vectorVar.begin(), 20000);
    vectorVar.insert(vectorVar.begin(), 30000);
    vectorVar.insert(vectorVar.begin(), 40000);

    auto it = find_if(vectorVar.begin(), vectorVar.end(), __pred(30000));

    if (it != vectorVar.end()) {
        cout << "查找到了" << endl;
    } else {
        cout << "没有找到" << endl;
    }
    return 0;
}
```



#### count_if 计数

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包

using namespace std;

int main() {
    vector<int> vectorVar;
    vectorVar.push_back(1);
    vectorVar.push_back(1);
    vectorVar.push_back(2);
    vectorVar.push_back(2);
    vectorVar.push_back(3);
    vectorVar.push_back(4);
    vectorVar.push_back(6);
    vectorVar.push_back(8);

    int number = count(vectorVar.begin(), vectorVar.end(), 2);
    cout << "等于2的个数是:" << number << endl;

    number = count_if(vectorVar.begin(), vectorVar.end(), bind2nd(less<int>(), 2)); // 函数适配器 配合 less   <
    cout << "小于2的个数是:" << number << endl;

    number = count_if(vectorVar.begin(), vectorVar.end(), bind2nd(greater<int>(), 2)); // 函数适配器 配合 less >
    cout << "大于2的个数是:" << number << endl;

    number = count_if(vectorVar.begin(), vectorVar.end(), bind2nd(equal_to<int>(), 2)); // 函数适配器 配合 less =
    cout << "等于2的个数是:" << number << endl;

    return 0;
}
```



#### merge 合并

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包

using namespace std;

int main() {
    vector<int> vectorVar1;
    vectorVar1.push_back(10);
    vectorVar1.push_back(20);
    vectorVar1.push_back(30);
    vectorVar1.push_back(40);

    vector<int> vectorVar2;
    vectorVar2.push_back(50);
    vectorVar2.push_back(60);
    vectorVar2.push_back(70);
    vectorVar2.push_back(80);

    // 合并成一个容器 result
    vector<int> vectorResult;
    vectorResult.resize(vectorVar1.size() + vectorVar2.size());

    merge(vectorVar1.begin(), vectorVar1.end(), vectorVar2.begin(), vectorVar2.end(), vectorResult.begin());
    for (auto itVar = vectorResult.begin(); itVar != vectorResult.end() ; itVar++) {
        cout << *itVar <<endl;
    }

    return 0;
}
```



#### sort 排序

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包

using namespace std;

int main() {
    vector<int> vectorVar;
    vectorVar.push_back(10);
    vectorVar.push_back(30);
    vectorVar.push_back(20);

    // sort(vectorVar.begin(), vectorVar.end(), less<int>());
    sort(vectorVar.begin(), vectorVar.end(), greater<int>());

    // 直接打印 vectorVar容器  此时 是不是就已经排序了
    for (auto itVar = vectorVar.begin(); itVar != vectorVar.end() ; itVar++) {
        cout << *itVar <<endl;
    }
}
```



#### random_shuffle 随机打乱元素的顺序

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包
using namespace std;

int main() {
    vector<int> vectorVar; // vector默认是没有排序功能的，默认输出： 65 53 84
    vectorVar.push_back(65);
    vectorVar.push_back(53);
    vectorVar.push_back(84);
    vectorVar.push_back(11);
    vectorVar.push_back(22);
    vectorVar.push_back(33);
    vectorVar.push_back(44);

    sort(vectorVar.begin(), vectorVar.end(), less<int>()); // 排序后 53 65 82

    random_shuffle(vectorVar.begin(), vectorVar.end());

    // 直接打印 vectorVar容器  此时 是不是就已经排序了
    for (auto itVar = vectorVar.begin(); itVar != vectorVar.end() ; itVar++) {
        cout << *itVar << "\t";
    }
    return 0;
}
```



#### copy 复制

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包

using namespace std;

int main() {
    vector<int> vectorVar; // vector默认是没有排序功能的，默认输出： 65 53 84
    vectorVar.push_back(100);
    vectorVar.push_back(200);
    vectorVar.push_back(300);
    vectorVar.push_back(400);
    vectorVar.push_back(500);
    vectorVar.push_back(600);
    vectorVar.push_back(700);

    vector<int> vectorResult;
    vectorResult.resize(vectorVar.size());

    copy(vectorVar.begin(), vectorVar.end(), vectorResult.begin());

    for (auto itVar = vectorResult.begin(); itVar != vectorResult.end() ; itVar++) {
        cout << *itVar << "\t";
    }

    return 0;
}
```



#### replace 替换

```C
#include <iostream>
#include <vector> // stl包
#include <algorithm> // 算法包

using namespace std;

int main() {
    vector<int> vectorVar; // vector默认是没有排序功能的，默认输出： 65 53 84
    vectorVar.push_back(100);
    vectorVar.push_back(200);
    vectorVar.push_back(300);
    vectorVar.push_back(400);
    vectorVar.push_back(500);
    vectorVar.push_back(600);

    replace(vectorVar.begin(), vectorVar.end(), 300, 888);

    for (auto itVar = vectorVar.begin(); itVar != vectorVar.end() ; itVar++) {
        cout << *itVar << "\t";
    }

    return 0;
}
```



### 16.高阶引用：左值右值

```c
#include <iostream>
using namespace std;

class Student {
private:
    string  info = "AAA"; // 旧变量

    // TODO 第一种情况【getInfo函数的info 与 main函数的result 是旧与新的两个变量而已，他们是值传递，所以右值修改时，影响不了里面的旧变量】
public:
    string getInfo() {
        return this->info;
    }

    // TODO 第二种情况【getInfo函数的info 与 main函数的result 是引用关系，一块内存空间 有多个别名而已，所以右值修改时，直接影响旧变量】
public:
    string & getInfo_front() {
        return this->info;
    }
};

int main() {
    Student student;

    // TODO 第一种情况
    student.getInfo() = "666";
    string result = student.getInfo();
    cout << "第一种情况:" << result << endl;

    // TODO 第二种情况
    student.getInfo_front() = "666"; // 右值 修改内容
    result = student.getInfo_front(); // 左值 获取内容
    cout << "第二种情况:" << result << endl;
}
```



### 17.线程

#### thread 用法

```C
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

// 异步线程 子线程
void runAction(int number) { // 相当于 Java的 run函数一样
    for (int i = 0; i < 10; ++i) {
        cout << "runAction：" << number << endl;
        sleep(1);
    }
}

// main函数的线程
int main() {
    // TODO 方式一  
    thread thread1(runAction, 100);
    sleep(3); // 我只等你三秒钟

    // TODO 方式二  我等你执行完成后，我再执行
//    thread thread2(runAction, 100);
//    thread2.join(); // 我等runAction执行完成后，我再继续执行下面代码..

    return 0;
}
```



#### p_thread 用法

```C
#include <iostream>
#include <pthread.h>

using namespace std;

// void *(*)(void *)
void *customPthreadTask(void *pVoid) { // 异步线程  相当于Java的Thread.run函数一样
    // C++转换static_cast  转换指针操作的
    int *number = static_cast<int *>(pVoid); // pVoid==number int的地址，所以我用int*接收
    cout << "异步线程执行了:" << *number << endl;

    return 0; // 返回值是 void *,所以不要漏写返回值
}

int main() {
    int number = 666;

    /**
      int pthread_create (pthread_t *,  // 参数一：线程ID
                        const pthread_attr_t *, // 参数二：线程属性
		                void *(*)(void *), // 参数三：函数指针的规则
		                void *); // 参数四：给函数指针传递的内容，void * 可以传递任何内容
     */
    pthread_t pthreadID; // 线程ID，每个线程都需要有的线程ID

    pthread_create(&pthreadID, 0, customPthreadTask, &number);

    return 0;
}
```

#### p_thread join

```C
#include <iostream>
#include <pthread.h> 
#include <unistd.h>

using namespace std;

// void *(*)(void *)
void * runTask(void * pVoid) { // 异步线程  子线程
    int number = *static_cast<int *>(pVoid);
    cout << "异步线程执行了:" << number << endl;

    for (int i = 0; i < 10; ++i) {
        cout << "run:" << i << endl;
        sleep(1);
    }

    return 0;
}

int main() {
    int number = 999;

    pthread_t  pthreadID;
    pthread_create(&pthreadID, 0, runTask, &number);

    // 为了演示第二种情况
//     sleep(3); // main函数只 异步线程三秒钟

    // 异步线程在执行的过程中，我们的main函数 相当于 阻塞在这里不动了，只有异步线程执行完成后，我才开始执行join后面的代码
    pthread_join(pthreadID, 0);

    cout << "main函数即将弹栈..." << endl;
    return 0;
}
```



#### 互斥锁和条件变量

生产者和消费者

```C
#pragma once
#include <iostream>
#include "safe_queue_too.h"
using namespace std;
SafeQueueClass<int> sq;

// TODO 模拟演示 消费者
void * getMethod(void *) {
    while (true) {
        int  value;
        sq.get(value);
        printf("消费者get 得到的数据:%d\n", value);

        // 你只要传入 -1 就结束当前循环
        if (-1 == value) {
            printf("消费者get 全部执行完毕\n");
            break;
        }
    }
    return 0;
}

// TODO 模拟演示 生产者
void * setMethod(void *) {
    while (true) {
        int value;
        printf("请输入你要生成的信息:\n");
        cin >> value;

        // 你只要传入 -1 就结束当前循环
        if (-1 == value) {
            sq.add(value); // 为了让消费者 可以结束循环
            printf("消费者get 全部执行完毕\n");
            break;
        }

        sq.add(value);
    }
    return 0;
}

int main() {
    pthread_t pthreadGet;
    pthread_create(&pthreadGet, 0, getMethod, 0);

    pthread_t pthreadSet;
    pthread_create(&pthreadSet, 0, setMethod, 0);

    pthread_join(pthreadGet, 0);

    pthread_join(pthreadSet, 0);

    return 0;
}
```

```C
// 生产者 消费者 工具类   播放器 有用

#ifndef CPPCLIONPROJECT_SAFE_QUEUE_TOO_H
#define CPPCLIONPROJECT_SAFE_QUEUE_TOO_H

#endif //CPPCLIONPROJECT_SAFE_QUEUE_TOO_H

#pragma once // 防止重复写 include 的控制

#include <iostream>
#include <string>
#include <pthread.h>
#include <string>
#include <queue>

using namespace std;

// 定义模版函数 int double float == Java的泛型
template<typename T>

class SafeQueueClass {
private:
    queue<T> queue; // 定义队列
    pthread_mutex_t  mutex; // 定义互斥锁（不允许有野指针）
    pthread_cond_t cond; // 条件变量，为了实现 等待 读取 等功能 （不允许有野指针）

public:
    SafeQueueClass() {
        // 初始化 互斥锁
        pthread_mutex_init(&mutex, 0);

        // 初始化 条件变量
        pthread_cond_init(&cond, 0);
    }
    ~SafeQueueClass() {
        // 回收 互斥锁
        pthread_mutex_destroy(&mutex);

        // 回收 条件变量
        pthread_cond_destroy(&cond);
    }

    // TODO 加入到队列中（进行生成）
    void add(T t) {
        // 为了安全 加锁
        pthread_mutex_lock(&mutex);

        queue.push(t); // 把数据加入到队列中

        // 告诉消费者，我已经生产好了
        // pthread_cond_signal(&cond) // Java notify 单个的
        pthread_cond_broadcast(&cond); // Java notifyAll 所有的的

        cout << "notifyAll" << endl;

        // 解锁
        pthread_mutex_unlock(&mutex);
    }

    // TODO 从队列中获取（进行消费）
    void get(T & t) {
        // 为了安全 加锁
        pthread_mutex_lock(&mutex);

        while (queue.empty()) {
            cout << "waiting" << endl;
            pthread_cond_wait(&cond, &mutex); // 相当于 Java的 wait 等待了[有可能被系统唤醒]
        }

        // 证明被唤醒了
        t = queue.front(); // 得到 队列中的元素数据 仅此而已
        queue.pop(); // 删除元素

        // 解锁
        pthread_mutex_unlock(&mutex);
    }
};
```



### 18.智能指针

#### shared_ptr 引用计数,自动释放对象

```C
#include <iostream>
#include <memory> // 智能指针的头文件引入
using namespace std;

class Person {
public:
    ~Person() {
        cout << "Person 析构函数" << endl;
    }
};

int main() {

    Person * person1 = new Person(); // 堆区开辟

    shared_ptr<Person> sharedPtr1(person1); // 栈区开辟sharedPtr1， 加1 等于1 引用计数

    return 0;
}
// main函数弹栈，会释放 所有的栈成员 sharedPtr1 调用 sharedPtr1析构函数 减1 等于0  直接释放person1
```

缺陷：会有循环依赖问题



#### weak_ptr 

用来解决引用计数循环依赖的问题

```C
#include <iostream>
#include <memory> // 智能指针的头文件引入
using namespace std;

class Person2; // 先声明 Person2 让我们的Person1 直接找到

class Person1 {
public:
    weak_ptr<Person2> person2; // Person2智能指针  没有引用计数 无法+1
    ~Person1() {
        cout << "Person1 析构函数" << endl;
    }
};

class Person2 {
public:
    weak_ptr<Person1> person1;  // Person1智能指针  没有引用计数 无法+1
    ~Person2() {
        cout << "Person2 析构函数" << endl;
    }
};

int main() {
    Person1 * person1 = new Person1(); // 堆区开辟
    Person2 * person2 = new Person2(); // 堆区开辟

    shared_ptr<Person1> sharedPtr1(person1);
    shared_ptr<Person2> sharedPtr2(person2);

    cout << "前 sharedPtr1的引用计数是:" << sharedPtr1.use_count() << endl;
    cout << "前 sharedPtr2的引用计数是:" << sharedPtr2.use_count() << endl;

    // 给Person2智能指针赋值
    person1->person2 = sharedPtr2;
    // 给Person1智能指针赋值
    person2->person1 = sharedPtr1;

    cout << "后 sharedPtr1的引用计数是:" << sharedPtr1.use_count() << endl;
    cout << "后 sharedPtr2的引用计数是:" << sharedPtr2.use_count() << endl;

    return 0;
}
```



#### unique_ptr 

独占式智能指针

```C
#include <iostream>
#include <memory> // 智能指针的头文件引入

class Person {};

int main() {
    Person * person1 = new Person();
    Person * person2 = new Person();

    std::unique_ptr<Person> uniquePtr1(person1);

    // 严格禁止
    // std::unique_ptr<Person> uniquePtr2 = uniquePtr1;  unique不允许，因为是独占的

    // shared_ptr 是可以的，会造成隐患问题

    return 0;
}
```



#### 手写智能指针

```c
#ifndef CPPCLIONPROJECT_CUSTOMPTR_H
#define CPPCLIONPROJECT_CUSTOMPTR_H
#pragma once

#include <iostream>
#include <memory>

using namespace std;

template<typename T>
class Ptr {
private:
    T *object; // 用于智能指针指向管理的对象
    int *count; // 引用计数

public:
    Ptr() {
        count = new int(1); // new 的对象 必须 *指针   【new只是为了后面操作方便】
        object = 0;
        cout << "构造函数" << endl;
    }

    // t = Person Student
    Ptr(T *t) : object(t) {
        // 只有你存入对象，那么引用计数为1，这个是很合理的
        count = new int(1);
    }

    // 析构函数
    ~Ptr() {
        // 引用计数减1，为0标识可以释放object了
        if (--(*count) == 0) {
            if (object) {
                delete object;
            }
            delete count;
            object = 0;
            count = 0;
        }
        if (count) {
            cout << "Ptr析构count = " << *count << endl;
        } else {
            cout << "Ptr析构count = 0" << endl;
        }
    }

    // 拷贝构造函数
    Ptr(const Ptr<T> &p) {
        cout << "拷贝构造函数" << endl;
        // sharedPtr1 == p 的引用计数 +1  = 2
        ++(*p.count);

        object = p.object;
        count = p.count; // 最终是不是 p.count==2 给 count==2
    }

    // 自定义 =号运算符重载
    Ptr<T> &operator=(const Ptr<T> &p) {
        cout << "=号运算符重载" << endl;
        ++(*p.count);
        if (--(*count) == 0) {
            if (object) {
                delete object;
            }
            delete count;
        }

        object = p.object;
        count = p.count;
        return *this; // 运算符重载的返回
    }

    // 返回引用计数的数值
    int use_count() {
        return *this->count;
    }
};

#endif //CPPCLIONPROJECT_CUSTOMPTR_H
```

```c
#include "CustomPtr.h"

class Student {
public:
    ~Student() {
        cout << "析构函数 释放Student" << endl;
    }
};

// TODO  手写的智能指针
void action2() {
    Student *student1 = new Student();
    Student *student2 = new Student();

    // TODO 第一种情况
//    Ptr<Student> sharedPtr1(student1);
//    Ptr<Student> sharedPtr2(student2);

    // TODO 第二种情况
//    Ptr<Student> sharedPtr1(student1);
//    Ptr<Student> sharedPtr2 = sharedPtr1;

    // TODO 第三种情况
    // TODO 情况一
//    Ptr<Student> sharedPtr1(student1); // sharedPtr1构建对象
//    Ptr<Student> sharedPtr2; // sharedPtr2也会构建对象，此对象指向了object 与 count，在赋值前必须释放
    //所以在=运算符重载前必须要释放sharedPtr2,释放完成后在赋值
//    sharedPtr2 = sharedPtr1;

    // TODO 情况二
    Ptr<Student> sharedPtr1 (student1); // sharedPtr1构建对象
    Ptr<Student> sharedPtr2 (student2);
    //如果=运算符重载没有去释放,那么student2就会成为野对象（每有被智能指针管理的对象 称为 野对象）
    sharedPtr2 = sharedPtr1;

    // TODO 通用的打印
    cout << "手写的智能指针 sharedPtr1:" << sharedPtr1.use_count() << endl;
    cout << "手写的智能指针 sharedPtr2:" << sharedPtr2.use_count() << endl;
}

int main() {
    action2();
}
```



### 19.类型转换

#### const_cast

将常量指针转换成非常量指针

```C
#include <iostream>

using namespace std;

class Person {
public:
    string name = "default";
};

int main() {
    const Person *p1 = new Person();
    // p1->name = "Derry"; // 报错：常量指针，不写修改值

    Person *p2 = const_cast<Person *>(p1); // 转成 非常量指针
    p2->name = "Derry";

    cout << p1->name << endl;

    return 0;
}
```



#### static_cast

static_cast(编译期) 看左边：将父类强转成子类。最终调用的是子类(左边)的方法而不是父类(右边)的方法

```C
#include <iostream>
using namespace std;

class FuClass {
public:
    void show() {
        cout << "fu show" << endl;
    }
};

class ZiClass  : public FuClass {
public:
    void show() {
        cout << "zi show" << endl;
    }
};

int main() {
    int n = 88;
    void * pVoid = &n;
    int * number = static_cast<int *>(pVoid);
    cout << *number << endl;

    // ====================

    FuClass * fuClass = new FuClass;

    // static_cast(编译期) 看左边 ZiClass *
    ZiClass * ziClass = static_cast<ZiClass *>(fuClass);
    ziClass->show();

    delete fuClass; // 回收规则：谁new了，就delete谁

    return 0;
}


```



#### dynamic_cast

动态转换必须让父类成为虚函数

动态类型转换的时候，在运行期如果是new父类，就不能转换子类

```C
#include <iostream>

using namespace std;

class FuClass {
public:
    // 动态转换必须让父类成为虚函数
    virtual void show() {
        cout << "fu show" << endl;
    }
};

class ZiClass : public FuClass {
public:
    void show() {
        cout << "zi show" << endl;
    }
};

int main() {
    // 动态类型转换的时候，在运行期由于fuClass是new父类的，就不能转换子类
    // 这样转换是不成功的
//    FuClass *fuClass = new FuClass();
//    ZiClass *ziClass = dynamic_cast<ZiClass *>(fuClass);

    FuClass * fuClass = new ZiClass;
    ZiClass * ziClass = dynamic_cast<ZiClass *>(fuClass);

    // 动态转换是有返回值， null 转换失败
    if (ziClass) { // ziClass != null
        cout << "转换成功 ";
        ziClass->show();
    } else {
        cout << "转换失败" << endl;
    }
    return 0;
}
```



#### reinterpret_cast

可以将把对象转成数值，再将数值转换成对象

这里的数值我个人理解可以把他看作内存地址

```C
#include <iostream>

using namespace std;

class DerryPlayer {
public:
    void show() {
        cout << "DerryPlayer" << endl;
    }
};

int main() {
    DerryPlayer * derryPlayer = new DerryPlayer();
    long playerValue = reinterpret_cast<long>(derryPlayer); // 把对象变成数值

    // 通过数值 变成对象
    DerryPlayer * derryPlayer2 = reinterpret_cast<DerryPlayer *>(playerValue);
    derryPlayer2->show();

    printf("derryPlayer:%p\n", derryPlayer);
    printf("derryPlayer2:%p\n", derryPlayer2);

}
```





### 20.异常

```CPP
#include <iostream>
#include <string>
using namespace std;

void exceptionMethod01() {
    throw "exception"; // const char *
}

class Student {
public:
    char * getInfo() {
        return "自定义";
    }
};

void exceptionMethod02() {
    Student student;
    throw student;
}

int main() {
    try {
        exceptionMethod01();
    } catch ( const char * & msg) {
        cout << "捕获到异常1：" << msg << endl;
    }

    try {
        exceptionMethod02();
    } catch (Student & msg) {
        cout << "捕获到异常2：" << msg.getInfo() << endl;
    }
    return 0;
}

```

