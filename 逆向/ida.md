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





### 关闭操作

```
Don't pack database 不打包数据库文件，会将数据库文件分开存放
Pack database(Store) 将数据库文件打包成1个文件，将原来的数据库文件删除，后续打开可以直接使用打包的文件
Pack database(Deflate) 将数据库文件打包成1个文件，将原来的数据库文件压缩打包进去

Collect garbage 收集垃圾，将无用的内存清理掉
DON'T SAVE the database 最后一次操作不保留(主要是解决在IDA中错误操作)
```





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

```
ACDU指令
A:用字符串显示
C:用代码显示
D:用数据显示
U:未定义

T:局部变量选择结构体

;:注释(引用地方有副本)
::注释(引用地方没有副本)
选中;:函数注释 


N 修改名称
Y 修改类型
ctrl+s 跳转到段
```





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



### 创建结构体

```
添加结构体:Edit-Add struct type

给结构体添加成员：选中按D 

修改类型为结构体：alt+q

修改为数组：右键-Struct field size
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

步过(F8):执行一条指令(反汇编窗口) / 执行一行语句(伪代码)，不进入函数调用
步入(F7):执行一条指令(反汇编窗口) / 执行一行语句(伪代码)，进入函数调用
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





### 路由器固件模拟与调试

TODO



### 硬件断点

```
由硬件提供的断点机制，同时需要内核支持(Android不支持)
硬件断点不会修改内存数据（隐蔽性高、不会破坏壳数据、自解密代码、代码校验等）
普通断点实现原理:将断点位置的指令数据改成一条断点指令，在x86上的断点指令是int 3中断,0xcc
自解密代码：如果用普通断点，会修改掉指令，导致解密失败，硬件断点就能够规避此风险

支持对数据 / 代码设置断点
一般有数量限制

应用场景句
加壳程序设置断点
自解密程序设置断点
自校验程序设置断点
监控内存地址读写
...

设置方法：
先设置普通断点，编辑断点，选中 [hardware]
Read：断点地址被读取时命中
Write：断点地址被写入时命中
Execute：断点地址被执行时命中
Size：地址的范围大小（指令写1即可）

```



### 内存断点

```
对内存地址设置读写断点，命中修改该内存的指令

设置方法：
在 IDA-View 窗口中按下 G 键，定位到目标内存，再按下 F2 键设置

默认情况下，内存断点，IDA 使用硬件断点实现，若要修改则在 [edit Break Pointer] 中手动修改
内存断点也可以设置 IDAPython 脚本

```



### API断点

```
所谓 API 断点就是指的直接对系统库提供的API接口设置断点，以此来达到定位关键代码的目的。
设置方法与普通断点一致。

跳转到API的方法如下
方法1：IDA-View 窗口 -> G 键 -> 输入导出符号名
方法2：[Debugger] -> [Debugger Window] -> [Module List] -> 双击API所在的模块 -> 双击对应的函数
```

### 调试内存提取/修改

```
小段格式数据 -> LazyIDA -> 对应语言类型的数组
Dump 提取到文件 ，已知起始地址以及大小【dex脱壳等场景】
右键 -> [Lazy Dumper] -> 填入地址和大小，即可 dump 到文件

在修改内存起始地址(IDAView) -> 右键 -> [Paste Data]
支持3种格式数据导入：HEX/ASCII/Base64
```

### Trace 分析

```
指令级 Trace，开启后 IDA 记录每一条指令的执行情况。

基本操作：
打开菜单项 [Debugger] -> [Tracing] -> [Tracing Options] 配置 Trace 选项
在准备开始 Trace 的指令位置设置断点，并使该断点命中
开启 Trace：[Debugger] -> [Tracing] -> [Instruction Tracing]
开启后，单步执行一条指令，发现已经执行的指令变色表示开启 Trace 成功
按下 F9 继续运行，直到下一个断点手动停止 Trace 或直到程序结束


查看 Trace 记录
打开菜单项 [Debugger] -> [Tracing] -> [Tracing Window] 查看 Trace 记录
Result：当前指令执行后，被修改的寄存器列表及对应的值
导出 Trace 记录:[Tracing Windows] 表格窗口 -> 右键 -> [export trace to text file]

分析:
依据栈平衡原理，用 result rsp 的变换判断栈平衡，删除两个平衡点之间的代码
删除无关指令后，只剩下真实的指令，反混淆成功
```

### Fork 双进程调试法

```
将子进程的第一条指令 patch 为死循环
重新打开另外一个 IDA，附加子进程，恢复第一条指令，调试

这个技巧适用于所有无法直接启动调试，而只能附加的场景，比如 Android Native 某些极难调试的库。
```



### Frida 辅助 IDA 调试 Linker / JNI_OnLoad

```
这类函数只在 APP 启动初始化时执行，调试附加的方式
Frida -> spawn -> 附加 -> Frida %resume 
```





## IDA疑难杂症（TODO）

### IDA Decompile as call

```
将一条特殊指令标记为 call 指令。
示例：逆向重建后的代码， 其 syscall 与标准的 Linux syscall 有区别，导致 IDA 对其进行优化产生错误的代码。

ida 直接将某一条指令强行解释为函数调用  操作方法：
1.选中 syscall 指令
2.菜单: Edit -> Other -> Decompile as call输入定义
```

### IDA 修复枚举值

TODO

### IDA positive sp value

```
函数栈不平衡，这个时候就应该去检查函数中对栈操作的指令，并判断是否平衡  首先看函数头部与函数尾部是否平衡

解决方法是将 nop 掉即可
```

### IDA too big stack frame

```
跟栈不平衡原理差不多，IDA 里面的选项可以显示 Stack Pointer

最左侧的是stack pointer偏移量，发现异常干扰指令，nop这个干扰指令就可以反编译成功。
```

### IDA 跳转表修复

```
Edit -> Other -> Specify Switch.. 打开配置窗口
```

### 函数大小限制处理

```
IDA 反编译函数有大小限制，超过最大长度将报错
cfg/hexrays.cfg 配置文件可以修改最大反编译函数大小配置项
配置项：MAX_FUNCSIZE，默认 64
```



##  IDA脚本编程

### vscode 智能提示

```
在环境变量里添加 PYTHONPATH 值为 ida7.7\python\3 完整路径
```

?

```
vscode中setting.json

{
	"python.pythonPath":"xxx/python2.7",
	"python.autoComplete.extraPaths":["/XXX/IDA/python"]

}
```



### 常用命令

```
寄存器操作
idc.get_reg_value('rax’)
idaapi.set_reg_val("rax", 1234)

读取 xmm 寄存器
def read_xmm_reg(name):
	rv = idaapi.regval_t()
	idaapi.get_reg_val(name,rv)
	return (struct.unpack('Q', rv.bytes())[0])

调试内存操作
idc.read_dbg_byte(addr)
idc.read_dbg_memory(addr, size)
idc.read_dbg_dword(addr)
idc.read_dbg_qword(addr)
idc.patch_dbg_byte(addr, val)

调试内存读写封装
def patch_dbg_mem(addr, data):
	for i in range(len(data)):
		idc.patch_dbg_byte(addr +i, data[i])

def read_dbg_mem(addr, size):
	dd = []
	for i in range(size):
		dd.append(idc.read_dbg_byte(addr + i))
	return bytes(dd)

本地内存操作（会修改idb数据库）
idc.get_qword(addr)
idc.patch_qword(addr, val)
idc.patch_dword(addr, val)
idc.patch_word(addr, val)
idc.patch_byte(addr, val)
idc.get_db_byte(addr)
idc.get_bytes(addr, size)

反汇编操作
GetDisasm(addr)  # 获取反汇编文本
idc.next_head(ea) # 获取下一条指令地址

交叉引用分析
def ref in idautils.XrefTo(ea):
	print(hex(ref.frm))


o-llvm批量断点设置
fn = 0x401F60
ollvm_tail = 0x405D4B
f_blocks = idaapi.FlowChart(idaapi.get_func(fn), flags=idaapi.FC_PREDS)
for block in f_blocks:
    for succ in block.succs():
        if succ.start_ea == ollvm_tail:
            print(hex(block.start_ea))
            idc.add_bpt(block.start_ea)


杂项常用接口
idc.add_bpt(0x409437) 添加断点
idaapi.get_imagebase() 获取基地址
idc.create_insn(addr) # c, Make Code
ida_funcs.add_func(addr) # p , create function
ida_bytes.create_strlit(addr) # 创建字符串，A 键效果

函数遍历
for func in idautils.Functions():
    print("0x%x, %s" % (func, idc.get_func_name(func)))

基本块遍历
fn = 目标哈数地址
f_blocks = idaapi.FlowChart(idaapi.get_func(fn), flags=idaapi.FC_PREDS)
for block in f_blocks:
    print hex(block.start_ea)

基本块的前驱
for pred in block.preds():
    print hex(pred.start_ea)

基本块的后继
for succ in block.succs():
    print hex(pred.start_ea)

指令遍历
for ins in idautils.FuncTims(0x401000):
    print(hex(ins))

```

### 条件断点脚本编写

```
编写断点函数脚本，并在 IDA 底部输出窗口导入该函数
设置普通断点，在 call 之后设置
[右键] -> [Edit Break …]
点击 [Condition] -> […]
输入 方法() ，并选择语言为 Python
```







### Microcode

```
TODO
```





## 参考资料

https://github.com/P4nda0s



