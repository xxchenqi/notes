# Frida

## 说明

Frida是一个强大的动态插桩工具，可以在多种平台上使用，包括Android、iOS、Windows、macOS等。它允许安全研究人员和逆向工程师在运行时修改、监视和分析应用程序的行为。Frida提供了一个JavaScript API，使用户可以编写脚本来与目标应用程序进行交互，从而实现各种功能，如函数钩子、数据修改、网络拦截等。Frida的灵活性和强大的功能使其成为移动应用程序逆向工程和安全分析的重要工具。



Objection是建立在Frida之上的一个开源工具，专注于移动应用程序的运行时安全性评估。它提供了一个命令行界面和一组功能强大的插件，使用户可以通过Frida的能力来自动化执行常见的安全测试任务。Objection可以帮助用户快速分析应用程序的漏洞、绕过安全措施、修改敏感数据等。它支持Android和iOS平台，并提供了丰富的功能，如反调试、SSL Pinning绕过、数据篡改、文件系统访问等。



Wallbreaker 是一个实时分析 Java 堆的有用工具。提供一些命令从内存中搜索对象或类，并精美地可视化目标的真实结构。



## 环境搭建

kali虚拟机

**pyenv 安装:**

参考:https://github.com/pyenv/pyenv

```
git clone https://github.com/pyenv/pyenv.git ~/.pyenv

此处可能不一样(zsh):
echo 'export PYENV_ROOT="$HOME/.pyenv"' >> ~/.zshrc
echo 'command -v pyenv >/dev/null || export PATH="$PYENV_ROOT/bin:$PATH"' >> ~/.zshrc
echo 'eval "$(pyenv init -)"' >> ~/.zshrc

exec "$SHELL"
```

**python 安装:**

```
pyenv install 3.8.0
pyenv global 3.8.0
pyenv versions
python --version
```

缺少库:

```
apt install zlib1g zlib1g-dev libssl-dev libbz2-dev libsqlite3-dev
apt-get install python3-tk
apt-get install liblzma-dev
```

安装完成后需要重新安装python

```
pyenv install 3.8.0
```





**GIT访问问题:**

```
cat /etc/hosts
199.232.68.133 raw.githubusercontent.com
199.232.68.133 user-images.githubusercontent.com
199.232.68.133 avatars2.githubusercontent.com
199.232.68.133 avatars1.githubusercontent.com
```

**node安装:**

```
curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash
apt update
apt install -y nodejs
```

**firda安装(非常稳定版):**
参考地址: https://www.daixiaohou.com/693.html

```
pip install frida==12.8.0
pip install frida-tools==5.3.0
pip install objection==1.8.4
```

**frida安装问题解决:**

```
frida 下载太慢
pip install frida==12.8.0 -i https://pypi.tuna.tsinghua.edu.cn/simple/

frida安装卡在Running setup.py install for frida … 不动？
下载对应的egg文件(frida-12.8.0-py3.7-win-amd64.egg) https://pypi.org/simple/frida/
执行命令安装:
easy_install frida-12.8.0-py3.7-win-amd64.egg
检测是否安装成功
python -c "import frida; print(frida.__version__)"
```

**frida-server服务**

```
1.下载frida-server(fs1280arm64)
TODO

2.将server push到手机上 
adb push fs1280arm64 /data/local/tmp

3.启动server
adb shell
su
cd /data/local/tmp
chmod 777 server

```

**启动服务**

```
./fs1280arm64 -l 0.0.0.0:8888
或者
./fs1280arm64
```

**验证:**

```
frida-ps -H 192.168.71.96:8888
注入
frida -H 192.168.71.96:8888 -f com.android.settings -l x.js --no-pause
```



## Frida基础使用

#### 参考链接

https://frida.re/docs/javascript-api/



#### Frida启动的两种模式以及区别

```
frida启动有两种方式，分别为Spawn模式和 Attach 模式；
区别：
在 Spawn模式下，Frida 直接启动目标进程，然后在该进程中注入 Frida 的 Agent，也就是说，启动既注入；
在 Attach 模式下，Frida 会依附到已经运行的目标进程上，并在该进程中注入 Agent；

Spawn 模式启动方式：frida -U {package} -l xxx.js
Attach 模式启动方式：frida -U -l xxx.js -f {package}
```



#### 启动

```
通过USB启动
frida -U pagkageName -l xx.js

// -f 主动找到包名启动
frida -U -f pagkageName -l xx.js --no-pause
%resume

通过host启动
frida -H 192.168.71.96:8888 -f packageName -l x.js 

说明：
-f 重新启动程序，如果已经启动了，可以不需要加-f
"--no-pause" 选项告诉 Frida 在注入代码后不要暂停目标应用程序的执行。
执行 "%resume" 命令，以恢复目标应用程序的执行。

```



#### 在Java层面执行代码

```js
Java.perform(function(){...})
```

#### 方法hook

```js
Java.use("packageName.className").methodName.implementation = function(arg1) {
    // 主动调用方法
    var result = this.methodName(arg1);
    // 方法返回
    return result;
}
```

#### 方法重载

```js
Java.use("packageName.className").methodName.overload('java.lang.String', 'int').implementation = function(arg1, arg2) {}
```

#### 创建String实例

```js
var strInstance = Java.use('java.lang.String').$new("test");
```

#### 寻找实例

```js
Java.choose("packageName.className", {
    onMatch:function(instance) {
        // 通过实例调用方法
		instance.methodName();
    },
    onComplete:function(){}
})
```

#### 调用静态方法

```js
var result = Java.use("packageName.className").staticMethodName();
```

#### 执行

```js
// 立即执行 main函数
setImmediate(main)

// 延迟执行 invoke函数
setTimeout(invoke, 3000)
```

#### 堆栈信息打印

```js
console.log(Java.use("android.util.Log").getStackTraceString(Java.use("java.lang.Throwable".$new())));
```

#### 数组打印 或 [object]打印

```js
console.log(JSON.stringify(arg));
```

#### 构造数组

```js
var charArray = Java.array('char', ['a', 'b', 'c']);
```

#### 类型转换

```js
var testInstance = Java.cast(instance, Java.use("packageName.className"));
```

#### 创建java类

```js
var testClass = Java.registerClass({
  name: 'packageName.className',
  implements: [Java.use('packageName.className')],
  methods: {
    methodName: function() {}
  }
});

// 创建实例并调用方法
testClass.$new().methodName();
```

#### 相同函数和字段名称通过下划线区分

```
instance._same_name_bool_var.value = true ;
```

#### 反射修改函数

```js
Java.perform(function(){
    var class_name = "com.example.androiddemo.Activity.FridaActivity4$InnerClasses" ;
    var InnerClass = Java.use(class_name);
    var all_methods = InnerClass.class.getDeclaredMethods();
    for(var i = 0;i<all_methods.length;i++){
        var method = all_methods[i];
        var substring = method.toString().substr(method.toString().indexOf(class_name)+class_name.length+1);
        var finalMethodString = substring.substr(0,substring.indexOf("("));
        console.log(finalMethodString);
        InnerClass[finalMethodString].implementation = function(){return true};
    }
})
```

#### 枚举 Java VM 中存在的类加载器

```js
Java.enumerateClassLoaders({
    onMatch:function(loader){
        try {
            if(loader.findClass("com.example.androiddemo.Dynamic.DynamicCheck")){
                // 修改类加载器
                Java.classFactory.loader = loader;
            }
        } catch (error) {
            console.log("found error "+error)
        }
    },onComplete:function(){"enum completed!"}
})
```

#### 枚举加载的类

```js
Java.enumerateLoadedClasses({
    onMatch:function(name,handle){
        if(name.toString().indexOf("com.example.androiddemo.Activity.Frida6.Frida6") >= 0){
            Java.use(name).check.implementation = function(){return true}
        }
    },onComplete(){}
})
```







#### 例子

```js
function main(){
    Java.perform(function(){
        console.log("Inside Frida Java Perform !")
        // hook
        // overload 重载
        // Java.use("com.cq.test.MainActivity").test.overload('java.lang.String', 'int').implementation
        Java.use("com.cq.test.MainActivity").test.implementation = function(arg1, arg2) {
            // 创建String实例
            //Java.use('java.lang.String').$new("test");
			// 主动调用test方法
            var result = this.test(arg1, arg2);
            // 打印堆栈信息
            console.log(Java.use("android.util.Log").getStackTraceString(Java.use("java.lang.Throwable".$new())));
            console.log("arg1,arg2,result", arg1, arg2, result);
            return result;
        }

        // 寻找MainActivity实例
        Java.choose("com.cq.test.MainActivity", {
            onMatch:function(instance) {
                console.log(instance);
                // 通过实例调用方法
                instance.test(10, 10);
            }, onComplete:function(){}
        })

        // 调用静态方法
        var result = Java.use("com.cq.test.MainActivity").staticTest();

    })
}

function invoke(){
    Java.perform(function(){
        //...
    })
}

// 立即执行 main函数
setImmediate(main)

// 延迟执行 invoke函数
setTimeout(invoke, 3000)
```







## objection使用

```
记载所有的输出信息
cat .objection/objection.log

附加需要调试的app, 进入交互界面
# objection -g com.app.name explore
objection -N -h 192.168.71.96 -p 8888 -g com.android.settings explore
-N：这个选项表示不要加载 Frida 脚本

列出当前进程所有的activity
android hooking list activities 
列出当前进程所有的services
android hooking list services
列出当前进程所有的so
memory list modules 
查看so的导出函数
memory list exports libssl.so 
同上，保存函数地址
memory list exports libssl.so  --json /root/libart.json
在内存中搜索实例
android heap search instances com.android.settings.DisplaySettings 
直接调用实例方法(0x2092是实例地址,通过搜索实例获取到)
android heap execute 0x2092 getPreferenceScreenResId 

进入实例模拟环境
android heap evaluate 0x2092
输入
console.log("xxchenqi="+clazz.getPreferenceScreenResId())
按esc+enter直接打印出来

开启Activity
android intent launch_activity com.android.settings.wifi.WifiSettings
开启服务
android intent launch_service com.xxx
列出所有的类
android hooking list classes 
列出类的所有方法
android hooking list class_methods sun.util.locale.LocaleUtils
搜索与display相关的类
android hooking search classes display
搜索与display相关的方法
android hooking search methods display

hook类(类中的任何方法被触发都会有日志打印)
android hooking watch class android.bluetooth.BluetoothDevice

hook方法(参数不需要,打印出返回值、参数、堆栈信息)
android hooking watch class_method android.bluetooth.BluetoothDevice.equals --dump-args --dump-backtrace --dump-return

查看job列表(watch之后就会出现在job列表里)
jobs list
取消任务列表
jobs kill <id>


启动时hook方法
objection -d -g <packageName> explore --startup-command 'android hooking watch class_method <methodName> --dump-args --dump-backtrace --dump-return'
```



## Wallbreaker安装和使用

安装
```
mkdir -p ~/.objection/plugins/
git clone https://github.com/hluwa/Wallbreaker ~/.objection/plugins/Wallbreaker
```

使用

```
进入交互界面
# objection -g com.app.name explore
objection -N -h 192.168.71.96 -p 8888 -g com.android.settings explore

加载插件
plugin load /root/.objection/plugins/Wallbreaker
打印类的信息(变量、方法)
plugin wallbreaker classdump --fullname android.bluetooth.BluetoothDevice
```





## frida-dexdump

安装(下载2.0之前的版本)

```
git clone https://github.com/hluwa/frida-dexdump ~/.objection/plugins/frida-dexdump
```

使用

```
进入交互界面
# objection -g com.app.name explore
objection -N -h 192.168.71.96 -p 8888 -g com.android.settings explore

加载插件
plugin load /root/.objection/plugins/frida-dexdump/frida_dexdump
搜索dex
plugin dexdump search

dump dex
plugin dexdump dump 
```



## RPC

### 方式一

```js
function invoke(){
    Java.perform(function(){
        Java.choose("com.example.lesson4one.MainActivity",{
            onMatch:function(instance){
                console.log("found instance :",instance)
                console.log("found instance :",instance.secret())
            },onComplete:function(){}
        })
    })
}

// 将invoke函数导出为Frida的RPC（远程过程调用）接口，以便在外部脚本或工具中调用该函数。
rpc.exports = {
    invokefunc:invoke
}
```



```python
import time
import frida

def my_message_handler(message,payload):
    print(message)
    print(payload)

device = frida.get_device_manager().add_remote_device("192.168.1.102:9999")
print(device.enumerate_processes())

session = device.attach("com.example.lesson4one")
with open("lesson7lesson4.js") as f :
    script = session.create_script(f.read())
script.on("message",my_message_handler)
script.load()

command = ""
while True :
    command = input("Enter Command:")
    if command == "1":
        break
    elif command == "2":
        script.exports.invokefunc()
```



### 方式二

```js
Java.perform(function () {
    Java.use("android.widget.TextView").setText.overload('java.lang.CharSequence').implementation = function (x) {
        var string_to_send_x = x.toString();
        var string_to_recv;
        console.log("js-string_to_send_x:" + string_to_send_x)
        // 使用Frida的send()函数将字符串string_to_send_x发送给外部。(回调my_message_handler函数)
        send(string_to_send_x);
        /*
        	使用Frida的recv()函数接收外部发送的消息，并在回调函数中处理接收到的消息。
        	这里使用了wait()函数来等待接收到消息后再继续执行。
        	调用了script.post({"my_data":data})后,回调到这里
        */
        recv(function (received_json_objection) {
            string_to_recv = received_json_objection.my_data
            console.log("js-string_to_recv2:" + string_to_recv)
        }).wait();
        var javaStringToSend = Java.use('java.lang.String').$new(string_to_recv);

        var result = this.setText(javaStringToSend);
        return result;
    }
})
```



```python
import time
import frida
import base64

def my_message_handler(message,payload):
    print("py-message=",message)
    print("py-payloadstring_to_recv=",payload)
    if message["type"]=="send":
        data = message["payload"].split(":")[1].strip()
        print('data1:',data)
        data = str(base64.b64decode(data))
        print('data2:',data)
        usr,pw = data.split(":")
        print('usr:',usr)
        print('pw:',pw)
        data = str(base64.b64encode(("admin"+":"+pw).encode()))
        print("encode data:",data)
        # 将修改后的数据作为字典形式的消息发送给Frida脚本。
        script.post({"my_data":data})
        print("Modified data sent !")

# =================远程连接================
device = frida.get_device_manager().add_remote_device("10.128.128.193:8888")
# =================远程连接================

# =================USB连接================
# device = frida.get_usb_device()
# =================USB连接================

session = device.attach("com.example.lesson7sec")


with open("lesson7sec.js") as f :
    # 使用session创建一个Frida脚本对象，将读取的JavaScript脚本作为参数传入。
    script = session.create_script(f.read())

# 将自定义的消息处理函数my_message_handler注册到Frida脚本的"message"事件上，用于处理接收到的消息。
script.on("message",my_message_handler)
# 加载Frida脚本
script.load()
# 等待用户输入，保持脚本的运行，直到用户手动终止。
input()
```



## 内网穿透

NPS

https://github.com/ehang-io/nps

FRP

https://github.com/fatedier/frp



NPS搭建

```
1.docker pull ffdfgdfg/nps
2.下载conf文件夹
3.修改nps.conf文件
主要修改写端口号(当然可以不改，主要防止和其他端口冲突)
http_proxy_port=20000
https_proxy_port=20001
bridge_port=20002
web_port = 20003
4.创建容器
docker run -d -v D:\docker_data\nps\conf:/conf --name=nps ffdfgdfg/nps
5.访问主页
http://localhost:20003/
admin
123

6.点击客户端-新增-填写备注就行
7.点击+,复制客户端命令
8.去手机执行(提前下载并解压相关文件)
npc命令
启动frida
9.点击tcp隧道-新增
填写：客户端id、服务端端口、目标 (IP:端口)
```



## ZenTracer

用于辅助分析

```
pyenv install 3.8.0
git clone https://github.com/hluwa/ZenTracer
cd ZenTracer
pyenv local 3.8.0
python -m pip install --upgrade pip
pip install PyQt5
pip install frida-tools
python ZenTracer.py
```

使用

```
adb 连接手机
启动frdia 默认端口
Action-Match RegEx  输入匹配
Action-start 启动hook
```



## 加载和打包dex

打包dex

```
d8 xxx.class
```

加载dex

```
Java.openClassFile("/data/local/tmp/xxx.dex").load();
```

记得加权限 chmod 777



## 其他

Z3是微软研究院的定理证明器

https://github.com/Z3Prover/z3



native_hook

https://github.com/lasting-yang/frida_hook_libart



jnitrace

https://github.com/chame1eon/jnitrace





```
查看so函数地址
memory list exports libc.so --json /root/Desktop/libc.json
```

```js
function hook_libc() {
	var strcmp_addr = Module.findExportByName("libc.so", "strcmp");
    if (strcmp_addr) {
        Java.perform(function() {
            Interceptor.attach(strcmp_addr, {
                onEnter: function(args) {
                    var str1 = ptr(args[0]).readCString();
                    var str2 = ptr(args[0]).readCString();
                },
                onLeave:function(retval) {
                    
                }
            })
        })
    }
}
```



```js
function write_reg2(){
    var fopen_addrs = Module.findExportByName("libc.so", "fopen");
    var fputs_addrs = Module.findExportByName("libc.so", "fputs");
    var fclose_addrs = Module.findExportByName("libc.so", "fclose");

	var fopen = new NativeFunction(fopen_addrs, "pointer",["pointer","pointer"]);
    var fopen = new NativeFunction(fputs_addrs, "int",["pointer","pointer"]);
    var fopen = new NativeFunction(fclose_addrs, "int",["pointer"]);
    
    var filename = Memory.allocUtf8String("/sdcard/req.dat");
	var file_mode = Memory.allocUtf8String("w+");
	var file = fopen(filename, file_mode);
    var contents = Memory.allocUtf8String("xxxxx");
    var ret = fputs(contents, file);
    fclose(file);
}
```



