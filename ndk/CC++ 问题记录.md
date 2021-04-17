# C/C++ 问题记录



### cmake配置:

target_link_libraries必须在add_executable之后，否则找不到



### system("pause");

在C++中一般在main函数中的return之前添加system("pause");这样就可以看清楚输出的结果，pause会输出"`press any key to continue`. . ."。



### size_t

size_t在32位架构上是4字节，在64位架构上是8字节，在不同架构上进行编译时需要注意这个问题。而int在不同架构下都是4字节，与size_t不同；且int为带符号数，size_t为无符号数。



### 获取参数的类型

decltype(lock)



### 防止头文件重复包含

```
#ifndef MMKV_SCOPEDLOCK_H
#define MMKV_SCOPEDLOCK_H
#endif
```





### 0,NULL,nullptr

在C语言中，我们使用NULL表示空指针

C++中更习惯使用0来表示空指针而不是NULL

但是重载时候会存在问题

test(int a);

test(A *a);



可以用static_cast<A *>(0)解决



C++ 11的nullptr就很好的解决了这个问题





### "+1",偏移量的问题

一个类型为T的指针的移动，是以sizeof(T)为移动单位
即array+1：在数组首元素的首地址的基础上，偏移一个sizeof(array[0])单位,此处的类型T就是数组中的一个int型的首元素
即&array+1：在数组的首地址的基础上，偏移一个sizeof(array)单位。此处的类型T就是数组中的一个含有5个int型元素的数组







### 指针数组和数组指针

指针数组：是指一个数组里面装着指针，也即指针数组是一个数组；

定义形式:int *a[10]；



数组指针:是指一个指向数组的指针，它其实还是一个指针，只不过是指向数组而已；

定义形式:int (\*p)[10]; 其中，由于[]的优先级高于*,所以必须添加(*p).



区分方法:

主要看后面的两个字是什么（前面是修饰作用），因此指针数组是数组，而数组指针是指针。





### 常量指针和指针常量的区别

常量指针：表示const修饰的为所申明的类型。

const char *p

因为const修饰的是char，所以就是说：p所指向的内存地址所对应的值，是 

const，因此不可修改。但指针所指向的内存地址是可以修改的，因为其并不是 

const类型。 

指向的地址可以变，但内容不可以重新赋值，内容的改变只能通过修改地址指向后变换。



指针常量：表示const修改的指针。

char *const p

因为const修饰的是指针p，也就是说：指针所指向的内存地址是const，不可修 

改。但p所指向内存地址所对应的值是可以修改的，因为其并不是const类型。 

指向的地址不可以重新赋值，但内容可以改变，必须初始化，地 

址跟随一生。





互斥锁

```
pthread_mutex_lock()
pthread_mutex_unlock(&mutex);

```

发送一个信号给另外一个正在处于阻塞等待状态的线程,使其脱离阻塞状态,继续执行

```
pthread_cond_wait
pthread_cond_signal
```





类和结构体区别：

类是私有的，结构体是公有的

