# IDA

## 关闭操作

```
Don't pack database 不打包数据库文件，会将数据库文件分开存放
Pack database(Store) 将数据库文件打包成1个文件，将原来的数据库文件删除，后续打开可以直接使用打包的文件
Pack database(Deflate) 将数据库文件打包成1个文件，将原来的数据库文件压缩打包进去

Collect garbage 收集垃圾，将无用的内存清理掉
DON'T SAVE the database 最后一次操作不保留(主要是解决在IDA中错误操作)
```



## 窗口

### IDA-View-A 反汇编窗口

```
1.空格切换图形和文本界面

2.显示硬编码：option-general-Number of opcode bytes 16

3.ACDU指令
A:用字符串显示
C:用代码显示
D:用数据显示
U:未定义

4.G:跳转地址

5.ALT+T:搜索指令

6.N:修改名字

7.T:局部变量选择结构体

8.注释
;:注释(引用地方有副本)
::注释(引用地方没有副本)
选中;:函数注释 

9.交叉引用: view-Open subviews-Cross references
(所有引用这些函数)


```



### Hex View-A 十六进制窗口



### Imports 导入函数



### Exports 导出函数



### Structures 结构体



创建结构体

```
添加结构体:Edit-Add struct type

给结构体添加成员：选中按D 

修改类型为结构体：alt+q

修改为数组：右键-Struct field size
```







## 快捷键

```
N 修改名称
Y 修改类型
G 跳转到指定地址
X 跳转到引用
ctrl+s 跳转到段
```



## 调试技巧

### 查看内存数据

```
1.将IDA中的android_server/android_server64拷贝到手机里
目录: IDA_Pro_7.7/dbgsrv/.
2.修改权限
chmod 777 android_server
3.指定端口执行
./android_server -p1235
4.adb转发到指定端口
adb forward tcp:1235 tcp:1235
5.打开IDA运行
调试器-附加-Remote ARM Linux/Android debugger
6.填写主机和端口
127.0.0.1 1235
7.选择调试进程，等待加载完成(需要等一会)
8.在路径中搜索so库，并找到基地址
9.通过基地址+你需要查的数据偏移地址(16进制)
10.按G，输入地址，查看数据

```





