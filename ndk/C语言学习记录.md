# C语言学习记录：

### 1.include <> ""区别

\#include <> 是系统的库

\#include “” 是自己的库



### 2.悬空指针和野指针

(1)悬空指针：

在内存中创建堆空间malloc

释放堆空间free

此时改指针为悬空指针,因为指向了一个空地址。

所以需要将指针指向为NULL



(2)野指针：

定义一个变量,但是没给他赋值,这个指针也就没有地址，不知道指向于谁，所以就成了野指针。



### 3.静态开辟和动态开辟内存区别

静态开辟的内存大小是不能修改的，栈区

动态开辟空间是可以修改的，堆区



### 4.栈和堆区别

栈区的内存空间比较小，堆区的内存空间比较大，具体大小每个平台都不一样

栈区的内存自动回收，堆区需要手动创建和回收



### 5.malloc,realloc使用和区别

```c
//在部分编译器上需要手动转型(int *)，在clion上不需要
//在堆区开辟空间
int * arr = (int *)malloc(sizeof(int) * num);
//在堆区扩大空间 第一个参数:前面开辟的指针 ,第二个参数:总大小
int * new_arr = (int *)realloc(arr,sizeof(int) * (num + new_num));

// arr 和 new_arr 的内存地址是一样的(如果新增的空间不够了？)
//记得释放,不要重复free释放,记得判断,重复释放VS会崩溃,CLion不会崩溃
if (new_arr) {
  free(new_arr);
  new_arr = NULL;
  arr = NULL;
} else {
  //重新开辟失败的情况
  free(arr);
  arr = NULL;
}

```



### 6.字符串

```c
char str[] = {'a' , 'b' , 'c' , 'd'};
//此时用pirntf打印会出现乱码 abcdxxx，xxx为系统值，因为printf遇到\0才会结束,例如
char str2[] = {'a' , 'b' , 'c' , 'd' ,'\0'};

char* str3 = "abcd";//这样写会隐士的加上\0

//可以修改，abcd是存在静态区域，然后会复制到栈空间，str2是指向栈空间，在栈空间进行操作不会报错
str2[2] = 'tt';

//不能修改，会报错，因为str3指向的是静态区域，系统会拒绝访问
str3[2] = 'tt';
```

获取长度

```c
int getLen(char *arr) {
    int num = 0;
    while (*arr) { // *arr != '\0'
        arr ++;
        num ++;
    }
    return num;
}
```

获取长度的错误写法

```c
//C/C++编译器，数组作为参数传递，会把数组优化为指针(为了效率)
int getLen2(char arr[]) {
    printf("arr = %d\n", sizeof(arr));
    printf("char = %d\n", sizeof(char));
    return sizeof(arr) / sizeof(int);
}
```

还有很多写法是传地址进来

```c
void getLen(int *count, char *arr) {
    int num = 0;
    while (*arr) { // *arr != '\0'
        arr ++;
        num ++;
    }
    *count = num;
}
```

### 7.结构体

```c
struct Person {
    char *name;
    int age;
    char sex
} p1 = {"chenqi", 18, 'G'},
        p2,
        p3;

//也可以这么写,cq就是别名
typedef struct  {
    char *name;
    int age;
    char sex
} cq;

int main() {
    printf("p1 %s %d %c\n",p1.name,p1.age,p1.sex);
    return 0;
}

```

