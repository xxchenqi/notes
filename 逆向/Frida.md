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

**frida-server服务启动**

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
./fs1280arm64 -l 0.0.0.0:8888
```

**验证:**

```
frida-ps -H 192.168.71.96:8888
注入
frida -H 192.168.71.96:8888 -f com.android.settings -l 0530.js --no-pause
```



## 基础使用

```
./fs1280arm64
```



```
frida -U pagkageName -l xx.js

// -f 主动找到包名启动
frida -U -f pagkageName -l xx.js
%resume
```



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

查看job列表
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

