# IDA

## 环境配置

IDA 7.7

Python 环境 3.8.x

必备插件

LazyIDA: https://github.com/P4nda0s/LazyIDA

Keypatch: https://github.com/keystone-engine/keypatch （pip install keystone-engine）

将插件的 xxx.py 放到 IDA 目录的 plugins 目录

系统中有多个 python，使用 IDA 目录中的 idapyswitch.exe 切换



## 基础知识

### IDA 的反汇编窗口（IDA-View）

空格键切换文本/图形模式



### IDA 的伪代码窗口

反汇编 -> 伪代码： Tab 键 / F5

伪代码 -> 反汇编： Tab 键

提示：光标位置处 Tab 键切换有一定对应关系，可以快速定位伪代码某条语句的汇编。



### IDA 的字符串表（strings）

Shift + F12 键

菜单打开 [View] -> [Open Subviews] -> [Strings]

字符串表收集了 IDA 识别到的所有字符串（不一定齐全）

字符串信息是逆向工作中非常关键的信息字符串表按下 Ctrl + F 弹出底部筛选器，筛选包含关键字的字符串（所有表格窗格均可）

双击字符串会进去该字符串对应的数据内存区域



### IDA 的数据窗口（Hex View）

菜单 [View] -> [Open subviews] -> [Hex dump]

以 hexdump 形式展示程序的数据

F2 键进入编辑模式，再次按下 F2 键退出编辑模式 （不推荐在数据窗口修改数据）

G 键弹出地址跳转窗口 （亦可在反汇编/伪代码窗口使用）



### IDA 的交叉引用

程序中的地址用符号来表示，例如 sub__xxx，var_xxx, …

符号可能被代码、数据引用，IDA 交叉引用窗口能展示引用关系

void main() { printf(“%s”, abc); }， main 函数引用了 printf 、“%s”、abc 

IDA 可以通过 abc、printf、“%s” 交叉引用找到 main 函数 

在符号名上按下 X 键，查找当前引用 X 的代码或数据





### IDA 的代码定位

- 字符串定位法

- 字符串定位法指的是通过已知字符串来定位代码。

  - 在字符串表中找到该字符串

  - 查找字符串的交叉引用

  - 跳转到交叉引用指向的代码



### IDA 的重命名

对已经分析的符号命名

单击选中符号，按下 N 键，输入有意义的名称



### IDA 的注释

;



### 快捷键

| 空格            | 反汇编窗口切换图形/文本模式    |
| --------------- | ------------------------------ |
| Ctrl + 鼠标滚轮 | 调整反汇编图形模式大小         |
| Tab / F5        | 伪代码 / 反汇编切换            |
| Shift + F12     | 打开字符串表                   |
| Ctrl + F        | 表格窗口搜索功能               |
| F2              | Hex窗口进入/退出编辑模式，断点 |
| G               | 地址跳转窗口                   |
| X               | 查看交叉引用                   |
| N               | 重命名符号                     |





### 不同类型的整数 

| 类型                          | 字节                    |
| ----------------------------- | ----------------------- |
| byte / char / uint8 / int8    | db 1个字节              |
| word / short / uint16 / int16 | dw 2个字节              |
| dword / int / uint            | dd 4个字节 （32位指针） |
| qword / int64                 | dq 8个字节 （64位指针） |

大部分情况下，整数以小端序存储在内存0xA1B2C3D4 存储到内存的顺序为 D4 C3 B2 A1

按d键可以切换



### LazyIDA 提取数据 

提取成 python / C 的数组格式

选中数据，右键，[Convert]  （注意多选一项）



### 数据与代码的转换

在某地址处按下 U 键(转换未定义)，取消IDA对当前地址的解释

在某地址处按下 C 键(转为代码)，将地址标记为代码并反汇编

在创建的代码函数头位置，按下 P 键，创建函数

处理花指令时非常有用





实际操作:

```
IDA某些情况下可能反编译出错，有些是数据，可能反编译出代码。
示例：
jz和jnz组合是跳转
在jz和jnz下方按U键，将代码取消定义
然后将跳转之间的代码选中，右键转换成 fill with NOPs 指令
在函数开头处按C键，转换为代码,让IDA重新分析，部分残留(---)都按C键
在函数开头处按按P键，创建函数
```



### IDA 调用图

查看函数调用关系图反汇编窗口，函数符号单选中，右键, “Xref Graph to”



### 常数搜索

菜单 [Search] -> [Immediate Value] 

“Find all”选上后搜索所有



用途:

1.搜索对结构体偏移的引用代码

2.搜索 ollvm 真实块代码



示例:

某加固使用 0x81 sys_exit 系统调用退出进程，搜索所有暗桩

mov     [rbp+var_28], 83C57F1h ,只需搜索0x83C57F1



### 指令文本搜索 

菜单 [Search] -> [Text] 

“Find all”选上后搜索所有

用途:搜索特殊指令





### 字节序列搜索 

搜索满足匹配 HEX 串的序列

菜单 [Search] -> [Sequence of bytes]



用途：

特征码查找

花指令查找

代码定位



### 修改符号/字节

通过keypatch修改符号

通过pasteData修改字节



### IDA 的 Patch

直接修改数据：数据窗口 F2

插件修改数据：LazyIDA，修改位置，右键，[Paste Data],修改大段数据

修改汇编代码：Keypatch



注:以上所有修改，仅修改了IDA的数据库，对应文件并未修改

应用修改到文件：菜单项 [Edit] -> [Patch program] -> [Apply Patches …]

注：Apply 时，IDA 不会还原文件，注意第二次 Patch 时手动还原文件





### IDA 的 符号恢复

适用于已知目标程序使用的库（例如 OpenSSL、lua 等）

需要插件配合：BinDiff

思想：比较两份代码，查找相似函数，并将匹配上的相似函数命名成对应的符号。



流程：

- 通过字符串判断开源库使用情况
- 编译相同架构的开源库
- 使用 IDA 分析得到 a.idb
- 目标程序 IDA 中用 bindiff 插件分析 a.idb
- 导入符号



```
IDA Bindiff 插件，载入另外一个 idb 后

按下 Ctrl + 6 弹出 bindiff 控制窗口

点击控制窗口中的 Import 导入符号。
```





## 调试

### Windows 本地调试

```
按下 F2 设置断点，在 main 函数设置一个断点

步骤1: 选择调试器: 菜单项 [Debugger] -> [Select …] -> 选择 [Local Windows …]
步骤2：启动调试: [Debugger] -> [Start Process] 或 [Debugger] -> [Attach]
步骤3：弹出 [Debugger warning] 点击 [Yes]
```

#### 寄存器窗口

```
蓝色小箭头点击可以跳转到对应地址
光标最后一次所在的窗口为跳转窗口
光标最后一次在【反汇编】，则反汇编窗口跳转
光标最后一次在【数据窗口】，则数据窗口跳转
寄存器窗口，右键，可选择“稀有寄存器”查看
```

#### 栈窗口

```
上低，下高
先点击【栈窗口】，【寄存器】点击【sp】箭头，跳转栈顶
若栈窗口的数据是地址，单击可进一步跳转
```

#### 数据窗口

```
数据窗口跟随寄存器自动切换地址：右键 -> [sycnc with] -> 选择跟随寄存器
G 键打开跳转地址窗口
```

#### 模块窗口

```
打开: 菜单项 [Debugger] -> [Debugger Windows] -> [Module List]
双击模块可查看对应模块的导出函数
双击导出函数可以在反编译窗口查看伪代码
TIPS 【模块窗口】【导出函数窗口】都是表格窗口，用 Ctrl + F 设置关键字筛选
```

#### 调试命令行参数

```
菜单项 [Debugger]  -> [Process option]  -> 填写 [Parameters]
```

#### 调试基本操作

```
快捷键 F2 设置软件断点

步过(F7):执行一条指令(反汇编窗口) / 执行一行语句(伪代码)，不进入函数调用
步入(F8):执行一条指令(反汇编窗口) / 执行一行语句(伪代码)，进入函数调用
执行到返回(Ctrl+F7): 执行到当前函数的返回后位置(步入了xx函数，快速步出的方法)
执行到光标(F4): 执行到光标位置停下来（适用于跳出循环等操作…）
运行直到断点(F9): 运行
SetIP(Ctrl + N): 强制代码从光标位置处开始执行
```



### Windows 远程调试

```
目标机器运行IDA的调试代理程序(调试服务端程序 dbgsrv)
IDA 通过 TCP 与调试代理程序通信
IDA 调试服务端程序目录： IDA\dbgsrv


操作步骤:
IDA 菜单项 [Debugger] -> [Select Debugger] -> 选择 [Remote Windows]
IDA 菜单项 [Debugger] -> [Process Option] -> Host name 填写 dbgsrv 的 ip
配置成功后可以正常【启动调试】与【附加调试】
注意：Password 不需要填写
```

### Linux 远程调试

```
Windows IDA 调试 Linux 程序只能采用远程调试的方式
在 Linux 系统中运行 dbgsrv/linux_server 或 dbgsrv/linux_server64
IDA [Debugger] -> [Select debugger] -> 选择 [Remote Linux]
[Process Option] 中填写 dbgsrv 的 ip
[Debugger] -> [Start Process] 启动调试，若 IDA 无法找到可执行文件则会提示你上传。
[Debugger] -> [Attach] 附加到一个正在运行的进程并调试（需要权限）

Linux 远程调试的运行目录为 dbgsrv 的运行目录，标准输入输出为 dbgsrv 的标准输入输出。

```

### 动态链接库调试

```
只需要调试动态链接库的宿主程序即可，IDA 会自动识别动态链接库
IDA 会根据动态链接库加载的基地址重定位(rebase)

启动调试模式
	菜单项 [Debugger]  [Process Option]  Application 修改为宿主程序路径

附加调试模式
	无需修改，直接附加即可
	常见于 Android 调试 Native 库

调试后，IDA 会询问 xxx.dll/xxx.so 与当前分析的模块文件名一致，是否真的一致，此时点击“yes”即可调试。
```



### Android Native 库调试

```
Android Native 调试 = Linux 调试 + 动态链接库调试
需要上传 dbgsrv/android_server 到 Android 设备
Android Native 调试一般以附加模式进行

adb 转发端口(这样可以不需要看设备的IP)
adf forward tcp:23946 tcp:23946
```



### 路由器固件模拟与调试

TODO









## 参考资料

https://github.com/P4nda0s



