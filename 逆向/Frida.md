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



**代码提示:**

```
npm install --save @types/frida-gum
```



## Frida基础使用

### 参考链接

https://frida.re/docs/javascript-api/



### Frida启动的两种模式以及区别

```
span 模式：frida 重新打开一个进程
frida -U -f 包名 -l js路径 --no-pause

attch 模式: 附加在当前打开的进程
frida -U -l js路径 --no-pause

区别就是一个带 -f 一个不带


通过host启动
frida -H 192.168.71.96:8888 -f packageName -l x.js 
通过USB启动
frida -U pagkageName -l xx.js


每次frida（以Attach后Spawn去）启动调试后，程序自动暂停运行
解决办法：手动输入%resume

希望：frida调试开始后，自动继续运行，不要每次都输入%resume才继续运行
解决办法：加--no-pause参数

```



### 常用选项

```
--version             显示程序版本号
-h, --help            显示帮助信息
-D ID, --device=ID    连接到具有给定ID的设备
-U, --usb             连接到USB设备
-R, --remote          连接到远程frida-server
-H HOST, --host=HOST  连接到HOST上的远程frida-server
-f FILE, --file=FILE  生成FILE
-F, --attach-frontmost
                      附加到前台应用程序
-n NAME, --attach-name=NAME
                      附加到NAME
-p PID, --attach-pid=PID
                      附加到PID
--stdio=inherit|pipe  生成时的stdio行为（默认为“inherit”）
--runtime=duk|v8      使用的脚本运行时（默认为“duk”）
--debug               启用与Node.js兼容的脚本调试器
-l SCRIPT, --load=SCRIPT
                      加载SCRIPT
-P PARAMETERS_JSON, --parameters=PARAMETERS_JSON
                      作为JSON的参数，与Gadget相同
-C CMODULE, --cmodule=CMODULE
                      加载CMODULE
-c CODESHARE_URI, --codeshare=CODESHARE_URI
                      加载CODESHARE_URI
-e CODE, --eval=CODE  评估CODE
-q                    安静模式（没有提示）并在-l和-e之后退出
--no-pause            在启动后自动启动主线程
-o LOGFILE, --output=LOGFILE
                      输出到日志文件
--exit-on-error       在SCRIPT中遇到任何异常后以代码1退出
```



### 在Java层面执行代码

```js
Java.perform(function(){...})
```

### 方法hook

```js
Java.use("packageName.className").methodName.implementation = function(arg1) {
    // 主动调用方法
    var result = this.methodName(arg1);
    // 方法返回
    return result;
}
```

### 方法重载

```js
Java.use("packageName.className").methodName.overload('java.lang.String', 'int').implementation = function(arg1, arg2) {}
```

### 创建String实例

```js
var strInstance = Java.use('java.lang.String').$new("test");
```

### 寻找实例

```js
Java.choose("packageName.className", {
    onMatch:function(instance) {
        // 通过实例调用方法
		instance.methodName();
    },
    onComplete:function(){}
})
```

### 调用静态方法

```js
var result = Java.use("packageName.className").staticMethodName();
```

### 执行

```js
// 立即执行 main函数
setImmediate(main)

// 延迟执行 invoke函数
setTimeout(invoke, 3000)
```

### 堆栈信息打印

```js
console.log(Java.use("android.util.Log").getStackTraceString(Java.use("java.lang.Throwable".$new())));
```

### 数组打印 或 [object]打印

```js
console.log(JSON.stringify(arg));
```

### 构造数组

```js
var charArray = Java.array('char', ['a', 'b', 'c']);
```

### 类型转换

```js
var testInstance = Java.cast(instance, Java.use("packageName.className"));
```

### 创建java类

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

### 相同函数和字段名称通过下划线区分

```
instance._same_name_bool_var.value = true ;
```

### 反射修改函数

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

### 枚举 Java VM 中存在的类加载器

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

### 枚举加载的类

```js
Java.enumerateLoadedClasses({
    onMatch:function(name,handle){
        if(name.toString().indexOf("com.example.androiddemo.Activity.Frida6.Frida6") >= 0){
            Java.use(name).check.implementation = function(){return true}
        }
    },onComplete(){}
})
```



### 动态加载dex,并调用方法

```
打包jar
jar -cvf test.jar com/example/androiddemo/DecodeUtils.class

打包dex
dx --dex --output=test.dex test.jar
```

```js
function hook_java() {
	var testdex = Java.openClassFile("/data/local/tmp/test.dex");
    Java.perform(function () {
        testdex.load();
        var DecodeUtils = Java.use("com.example.androiddemo.DecodeUtils");
        console.log("DecodeUtils.decode_p:", DecodeUtils.decode_p());
    });
}
```







## native hook

http://demangler.com/

### 查找指定模块

```js
var module = Process.findModuleByName("libart.so")
```

```js
var module = Process.findModuleByAddress(addr)
```



### 枚举符号

```js
// 返回对象数组
var symbols = Module.enumerateSymbols()
```



### 查找指定模块基地址

```js
var native_lib_addr = Module.findBaseAddress("libnative-lib.so");
```



### 获取模块中特定导出的地址

```js
// PS:函数的地址可能和你实际看到的地址不一样，因为要加上基地址
var add_addr = Module.findExportByName("libnative-lib.so", "_Z5r0addii");
```



### 函数hook

```js
Interceptor.attach(add_addr, {
    // args 是一个数组，包含了目标函数的参数。可以通过修改这个数组来改变目标函数的参数值。
    onEnter: function (args) {
        // 调用前执行的逻辑
        
        // 修改第一个参数的值
        args[0] = newValue;
    },
    // retval 是目标函数的返回值。可以通过修改这个值来改变目标函数的返回结果。这对于在函数调用后修改返回值很有用。
    onLeave: function (retval) {
        // 调用后执行的逻辑
        
        // 修改返回值
        retval.replace(newValue);
    }
});
```



### 函数构建

```js
var NewStringUTF = new NativeFunction(NewStringUTF_addr,"pointer",["pointer","pointer"])
```



### 函数替换

```js
Interceptor.replace(NewStringUTF_addr, new NativeCallback(function(parg1,parg2){
    var newPARG2 = Memory.allocUtf8String("newPARG2")
    var result = NewStringUTF(parg1,newPARG2);
    return result;
},"pointer", ["pointer","pointer"]))
```



### 主动调用

```js
// NativeFunction(address, returnType, argumentTypes, abi);
var add = new NativeFunction(add_addr,"int",["int","int"]);
var add_result = add(1,2);
```



### 加载自定义so

```js
// 可脱离app，在其他app加载
var module = Module.load("/data/local/tmp/xx.so")
```



### JNI调用案例

```js
function hook_nativelib(){
    var myfirstjniJNI = Module.findExportByName("libnative-lib.so", "Java_com_example_demoso1_MainActivity_myfirstjniJNI");
    var myfirstjniJNI_invoke = new NativeFunction(myfirstjniJNI,"pointer",["pointer","pointer","pointer"])

    Interceptor.attach(myfirstjniJNI, {
        onEnter:function(args){
            var invoke_result_addr = myfirstjniJNI_invoke(args[0],args[1],args[2])
            // jstring读取
            Java.vm.getEnv().getStringUtfChars(invoke_result_addr,null).readCString()
        },onLeave:function(retval){
        }
    })
}
```



### char*、jstring

```js
getEnv()：获取当前线程的包装器JNIEnv。如果当前线程未附加到 VM，则引发异常。
tryGetEnv()：尝试获取当前线程的包装器JNIEnv。null如果当前线程未附加到 VM，则返回。



// char * -> js string
char_ptr.readCString();
Memory.readCString(char_ptr);
ptr(args[0]).readCString();

// jstring_addr -> js string 
Java.vm.getEnv().getStringUtfChars(jStringAddr,null).readCString()

// 构造jstring
Java.vm.getEnv().newStringUtf("123456")
```



### 调用栈打印

```js
console.log('CCCryptorCreate called from:\n' + Thread.backtrace(this.context, Backtracer.ACCURATE)
        .map(DebugSymbol.fromAddress).join('\n') + '\n')
```



### 枚举所有so和函数

```js
function EnumerateAllExports(){
    var modules = Process.enumerateModules();
    //console.log("Process.enumerateModules => ",JSON.stringify(modules))
    for(var i=0;i<modules.length;i++){
        var module = modules[i];
        var module_name = modules[i].name;
        var exports = module.enumerateExports();
        console.log("module_name=>",module_name,"  module.enumerateExports = > ",JSON.stringify(exports))
    }
}
```



### 动态注册hook案例

函数签名

```c
jint RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods);

typedef struct {
    const char *name; // 本地方法的名称
    const char *signature; // 本地方法的签名
    void *fnPtr; // 指向本地方法实现的函数指针
} JNINativeMethod;
```



```js
function hook_RegisterNatives(){
    var RegisterNatives_addr = null;
    var symbols = Process.findModuleByName("libart.so").enumerateSymbols()
    for(var i = 0;i<symbols.length;i++){
        var symbol = symbols[i].name;
        if((symbol.indexOf("CheckJNI")==-1)&&(symbol.indexOf("JNI")>=0)){
            if(symbol.indexOf("RegisterNatives")>=0){
                console.log("finally found RegisterNatives_name :",symbol);
                RegisterNatives_addr =symbols[i].address ;
                console.log("finally found RegisterNatives_addr :",RegisterNatives_addr);
            }
        }
    }

    if(RegisterNatives_addr!=null){
        Interceptor.attach(RegisterNatives_addr,{
            onEnter:function(args){
                console.log("[RegisterNatives]method counts :",args[3]);
                var env = args[0];
                var jclass = args[1];
                var class_name = Java.vm.tryGetEnv().getClassName(jclass);
                var methods_ptr = ptr(args[2]);
                var method_count = parseInt(args[3]);
                for (var i = 0; i < method_count; i++) {
                    var name_ptr = Memory.readPointer(methods_ptr.add(i * Process.pointerSize * 3));
                    var sig_ptr = Memory.readPointer(methods_ptr.add(i * Process.pointerSize * 3 + Process.pointerSize));
                    var fnPtr_ptr = Memory.readPointer(methods_ptr.add(i * Process.pointerSize * 3 + Process.pointerSize * 2));
                    var name = Memory.readCString(name_ptr);
                    var sig = Memory.readCString(sig_ptr);
                    var find_module = Process.findModuleByAddress(fnPtr_ptr);
                    console.log("[RegisterNatives] java_class:", class_name, "name:", name, "sig:", sig, "fnPtr:", fnPtr_ptr, "module_name:", find_module.name, "module_base:", find_module.base, "offset:", ptr(fnPtr_ptr).sub(find_module.base));
                }
            },onLeave:function(retval){
            }
        })

    }else{
        console.log("didn`t found RegisterNatives address")
    }
}
```



### pthread hook

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
```

```js
function hook_pthread(){
    var pthread_create_addr = Module.findExportByName("libc.so", "pthread_create");

    Interceptor.attach(pthread_create_addr,{
        onEnter:function(args) {
            console.log("args=>",args[0],args[1],args[2],args[4])
           
            // 函数替换
            var libnativebaseaddress = Module.findBaseAddress("libnative-lib.so")
            if (libnativebaseaddress != null) {
                // 确定参数在so中的地址(如果不判断会替换掉其他的pthread)
                if(args[2]-libnativebaseaddress == 64900){
            		var time_addr = Module.findExportByName("libc.so", "time");
                    args[2]=time_addr;
                }
            }
        },onLeave:function(retval){
            console.log("retval is =>",retval)
        }
    })
}
```



### 文件操作案例

```js
function writeSomething(path,contents){
    var fopen_addr = Module.findExportByName("libc.so", "fopen");
    var fputs_addr = Module.findExportByName("libc.so", "fputs");
    var fclose_addr = Module.findExportByName("libc.so", "fclose");

    var fopen = new NativeFunction(fopen_addr,"pointer",["pointer","pointer"])
    var fputs = new NativeFunction(fputs_addr,"int",["pointer","pointer"])
    var fclose = new NativeFunction(fclose_addr,"int",["pointer"])

    console.log(path,contents)

    var fileName = Memory.allocUtf8String(path);
    var mode = Memory.allocUtf8String("a+");
	
    var fp = fopen(fileName,mode);

    var contentHello = Memory.allocUtf8String(contents);
    // 注意权限
    var ret = fputs(contentHello,fp)
    
    fclose(fp);
}
```



### 函数导出到文件

```js
function EnumerateAllExports(){
    var modules = Process.enumerateModules();
    for(var i=0;i<modules.length;i++){
        var module = modules[i];
        var module_name = modules[i].name;
        var exports = module.enumerateSymbols();
        console.log("module_name=>",module_name,"  module.enumerateExports = > ",JSON.stringify(exports))
        for(var m =0; m<exports.length;m++){
            writeSomething("/sdcard/settings/"+module_name+".txt", "type:"+exports[m].type+ " name:"+ exports[m].name+" address:"+exports[m].address+"\n")
        }        
    }
}
```



## 示例

### 综合示例(java)

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

### 打印堆栈

```js
console.log(Java.use("android.util.Log").getStackTraceString(Java.use("java.lang.Throwable".$new())));
```

```js
function LogPrint(log) {
    var theDate = new Date();
    var hour = theDate.getHours();
    var minute = theDate.getMinutes();
    var second = theDate.getSeconds();
    var mSecond = theDate.getMilliseconds();

    hour < 10 ? hour = "0" + hour : hour;
    minute < 10 ? minute = "0" + minute : minute;
    second < 10 ? second = "0" + second : second;
    mSecond < 10 ? mSecond = "00" + mSecond : mSecond < 100 ? mSecond = "0" + mSecond : mSecond;
    var time = hour + ":" + minute + ":" + second + ":" + mSecond;
    var threadid = Process.getCurrentThreadId();
    console.log("[" + time + "]" + "->threadid:" + threadid + "--" + log);
}

function printJavaStack(name) {
    Java.perform(function () {
        var Exception = Java.use("java.lang.Exception");
        var ins = Exception.$new("Exception");
        var straces = ins.getStackTrace();
        if (straces != undefined && straces != null) {
            var strace = straces.toString();
            var replaceStr = strace.replace(/,/g, " \n ");
            LogPrint("=============================" + name + " Stack strat=======================");
            LogPrint(replaceStr);
            LogPrint("=============================" + name + " Stack end======================= \n ");
            Exception.$dispose();
        }
    });
}
```



### 打印native堆栈

```js
function printNativeStack(context, name) {
    var array = Thread.backtrace(context, Backtracer.ACCURATE);
    // 只回溯一层
    var first = DebugSymbol.fromAddress(array[0]);
    // 通过此判断,避免java和native多次打印
    if (first.toString().indexOf('libopenjdk.so!NET_Send') < 0) {
        var trace = Thread.backtrace(context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join("\n");
        LogPrint("-----------start:" + name + "--------------");
        LogPrint(trace);
        LogPrint("-----------end:" + name + "--------------");
    }
}
```



### 获取线程id

```js
var threadid = Process.getCurrentThreadId();
```



### dumpDex/脱壳

```js
//只适用于存在getDex和getBytes api的android版本
function dumpdex() {
    Java.perform(function () {
        var File = Java.use('java.io.File');
        var FileOutputStream = Java.use('java.io.FileOutputStream');
        Java.enumerateClassLoadersSync().forEach(function (loader) {
            console.log(loader + "\n");
            try {
                var aclass = loader.loadClass("a.a.a.a$a")
                var dexobj = aclass.getDex();
                var dexbytes = dexobj.getBytes();
                var dexsavepath = "/data/data/com.test.test/dump.dex";
                var dexfile = File.$new(dexsavepath);
                if (!dexfile.exists()) {
                    dexfile.createNewFile();
                }
                var fileoutputstream = FileOutputStream.$new(dexfile);
                fileoutputstream.write(dexbytes);
                fileoutputstream.flush();
                fileoutputstream.close();
                console.log("save dex success!");
            } catch (e) {
            }
        })
    })
}
```

### 通过内部类对象得到外部类对象

```js
this.this$0.value;
```

### 系统调用hook

```js
function printNativeStack(context, name) {
    var trace = Thread.backtrace(context, Backtracer.FUZZY).map(DebugSymbol.fromAddress).join("\n");
    LogPrint("=============================" + name + " Native Stack start=======================");
    LogPrint(trace);
    LogPrint("=============================" + name + " Native Stack end=======================\r\n");
}

function LogPrint(log) {
    var theDate = new Date();
    var hour = theDate.getHours();
    var minute = theDate.getMinutes();
    var second = theDate.getSeconds();
    var mSecond = theDate.getMilliseconds();

    hour < 10 ? hour = "0" + hour : hour;
    minute < 10 ? minute = "0" + minute : minute;
    second < 10 ? second = "0" + second : second;
    mSecond < 10 ? mSecond = "00" + mSecond : mSecond < 100 ? mSecond = "0" + mSecond : mSecond;
    var time = hour + ":" + minute + ":" + second + ":" + mSecond;
    var threadid = Process.getCurrentThreadId();
    console.log("[" + time + "]" + "->threadid:" + threadid + "--" + log);

}
function hookaddr(addr) {
    Interceptor.attach(addr, {
        onEnter: function (args) {
            this.arg0 = this.context.r0;
            this.arg1 = this.context.r1;
            this.arg2 = this.context.r2;
            this.syscallnum = this.context.r7.toInt32();
            if (this.syscallnum == 290) {
                //sendto
                LogPrint("this is a sendto!")

            }
            if (this.syscallnum == 292) {
                //sendto
                LogPrint("this is a __NR_recvfrom !")
            }
            console.log(Process.getCurrentThreadId() + "--go into " + addr + ",syscallnumm:" + this.syscallnum);
            printNativeStack(this.context, "go into " + addr + ",syscallnum:" + this.syscallnum);
        }, onLeave(retval) {
            var arg0 = this.context.r0;
            console.log(Process.getCurrentThreadId() + "--leave " + addr + ",retval:" + arg0);

        }
    })

}

function disassemble(addr, count) {
    var start = addr;
    for (var i = 0; i < count; i++) {
        try {
            // 解析内存中的二进制指令的方法
            var ins = Instruction.parse(start);
            LogPrint("addr:" + start + ",dis:" + ins.toString());
            if (ins.toString() == 'svc #0') {
                hookaddr(start);
            }
            start = ins.next;
        } catch (e) {

        }

    }
}

function search(range) {
    // 扫描内存
    Memory.scanSync(range.base, range.size, '00 00 00 ef').forEach(function (match) {
        LogPrint(JSON.stringify(match));
        disassemble(match.address, 10);
    })
}

// cat /proc
/*
	找pid
	ps -ef|grep [name]
	cat /proc/pid/maps
*/
function findsyscall() {
    // 检索内存区间
    Process.enumerateRanges('--x').forEach(function (range) {
        if (JSON.stringify(range).indexOf('libnative-lib.so') >= 0) {
            LogPrint(JSON.stringify(range));
            search(range);
        }
    })
}

function hookjava() {
    Java.perform(function () {
        var Runtime = Java.use('java.lang.Runtime');
        Runtime.nativeLoad.implementation = function (arg0, arg1, arg2) {
            console.log(Process.getCurrentThreadId() + "--load so:" + arg0);
            var result = this.nativeLoad(arg0, arg1, arg2);
            if (arg0.indexOf('libnative-lib.so') >= 0) {
                findsyscall();
            }
            return result;
        }
    })
}

function main() {
    //hookjava();
    findsyscall();
}

setImmediate(main);
```



### 不同版本中查找OutputStream.write函数

```js
function hookoutstreamwrite(classname) {
    Java.perform(function () {
        var Class = Java.use(classname);
        // 判断类中是否有write函数
        if (Class.write != null) {
            // overloads 获取 write 方法的所有重载函数
            Class.write.overloads.forEach(function (func) {
                LogPrint("start hook->" + func.methodName + "---" + JSON.stringify(func.argumentTypes));
                func.implementation = function () {
                    LogPrint("go into " + func.methodName + "---" + JSON.stringify(func.argumentTypes));
                    printJavaStack(func.methodName + "---" + JSON.stringify(func.argumentTypes));
                    var result = func.apply(this, arguments);
                    return result;
                }
            })
        }
    })
}

function enumerateclasses() {
    Java.perform(function () {
        // enumerateLoadedClassesSync 列举目标进程中所有已加载的类，并返回一个包含类名的数组。
        Java.enumerateLoadedClassesSync().forEach(function (classname) {
            if (classname.indexOf('OutputStream') >= 0) {
                LogPrint(classname);
                hookoutstreamwrite(classname);
            }

        })
    })
}

function main() {
    enumerateclasses();
}

setImmediate(main)
```



### Frida的文件操作

```js
function test() {
    var file = new File("/sdcard/test.dat", "w");
    file.write("cq");
    file.flush();
    file.close();
}
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


关闭sslpinning(也可以在启动objection时候加 -s ...)
android sslpinning disable


设置返回值
android hooking set return_value ...(类名.方法名) ...(value)
```



### 查看当前进程32/64位

```
frida

另一种方式:
查看zygote/zygote64的进程号
ps -e | grep -i zygote

查看父进程中fork的所有子进程
ps -e | grep -i pid(zygote/zygote64的进程号)
显示的所有进程就是64/32位

手机cpu
uname -a
```



### 常用选项

1. **`-g`（`--gadget`）：**
   - **含义：** 指定要使用的 Gadget，用于执行 Frida 操作。
   - **用法：** `objection -g <Gadget名称>`

2. **`-h`（`--help`）：**
   - **含义：** 显示帮助信息，列出可用的命令和选项。

3. **`-a`（`--attach`）：**
   - **含义：** 指定要附加到的目标应用程序的进程标识符（PID）。
   - **用法：** `objection -a <PID>`

4. **`-b`（`--binary`）：**
   - **含义：** 指定要启动并附加到的二进制文件。
   - **用法：** `objection -b <二进制文件路径>`

5. **`-l`（`--load`）：**
   - **含义：** 在启动应用程序后加载指定的 Frida 脚本。
   - **用法：** `objection -l <脚本文件路径>`

6. **`-c`（`--config`）：**
   - **含义：** 指定配置文件的路径。
   - **用法：** `objection -c <配置文件路径>`

7. **`-p`（`--password`）：**
   - **含义：** 指定设备的密码（iOS）或解锁密码（Android）。
   - **用法：** `objection -p <密码>`

8. **`-s`（`--script`）：**
   - **含义：** 指定要执行的 Objection 脚本。
   - **用法：** `objection -s <脚本名称>`

9. **`-i`（`--insecure`）：**
   - **含义：** 在启动 Frida 时禁用 SSL 校验。
   - **用法：** `objection -i`

10. **`-q`（`--quiet`）：**
    - **含义：** 禁用详细输出，以静默模式运行。





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

### Z3

微软研究院的定理证明器

https://github.com/Z3Prover/z3



案例1

```python
from z3 import *
from binascii import b2a_hex, a2b_hex

def main():
    s = Solver()

    r = "0064736c707d6f510020646b73247c4d0068202b4159516700502a214d24675100"
    r_result = bytearray(a2b_hex(r))
    print(r_result)
    for i in range(int(len(r_result) / 2)):
        c = r_result[i]
        r_result[i] = r_result[len(r_result) - i - 1]
        r_result[len(r_result) - i - 1] = c

    print(b2a_hex(r_result))

    x = [BitVec("x%s" % i, 32) for i in range(len(r_result))]
    for i in range(len(r_result)):
        c = r_result[i]
        print(i, hex(c))
        s.add(((x[i] >> (i % 8)) ^ x[i]) == r_result[i])        #z3
    if (s.check() == sat):
        model = (s.model())
        print(model)
        flag = ""
        for i in range(len(r_result)):
            if (model[x[i]] != None):
                flag += chr(model[x[i]].as_long().real)
            else:
                flag += " "
        print('"' + flag + '"')
        print(len(flag), len(r_result))


if __name__ == "__main__":
    main()


'''
    private String b(String str) {
        char[] charArray = str.toCharArray();
        for (int i = 0; i < charArray.length; i++) {
            charArray[i] = (char) ((charArray[i] >> (i % 8)) ^ charArray[i]);
        }
        for (int i2 = 0; i2 < charArray.length / 2; i2++) {
            char c = charArray[i2];
            charArray[i2] = charArray[(charArray.length - i2) - 1];
            charArray[(charArray.length - i2) - 1] = c;
        }
        return new String(charArray);
    }
'''
```



案例2:

```python
from z3 import *

def main():
    s = Solver()

    result = "EoPAoY62@ElRD"
    key    = "W3_arE_whO_we_ARE"
    index_key = 2016

    x = [BitVec("x%s" % i, 8) for i in range(len(result))]

    for i in range(len(result)):
        v11 = None
        if (i % 3 == 1):
            index_key = (index_key + 5) % 16
            v11 = key[index_key + 1]
        elif (i % 3 == 2):
            index_key = (index_key + 7) % 15
            v11 = key[index_key + 2]
        else:
            index_key = (index_key + 3) % 13
            v11 = key[index_key + 3]        
        s.add((x[i]) == ord(v11) ^ ord(result[i]))
        
    if (s.check() == sat):
        model = (s.model())
        flag = ""
        for i in range(len(result)):
            flag += chr(model[x[i]].as_long().real)
        print('"' + flag + '"')

if __name__ == "__main__":
    main()
```



### native_hook

https://github.com/lasting-yang/frida_hook_libart



### jnitrace

https://github.com/chame1eon/jnitrace





### 其他案例

#### 函数在哪些so

```
libart.so
GetStringUTFChars
FindClass
GetStaticFieldID
...

libc.so
strcmp
fopen
...


```



#### ollvm字符串混淆

加密字符串打印

```js
function hook_native() {
    var base_hello_jni = Module.findBaseAddress("libhello-jni.so");
    if (base_hello_jni) {
        //ollvm默认的字符串混淆，静态的时候没法看见字符串
        //执行起来之后，先调用.init_array里面的函数来解密字符串
        //解密完之后，内存中的字符串就是明文状态了。
        
        // 此处是加密字符串的数据地址
        var addr_37070 = base_hello_jni.add(0x37070);
        console.log("addr_37070:", ptr(addr_37070).readCString());

        var addr_37080 = base_hello_jni.add(0x37080);
        console.log("addr_37080:", ptr(addr_37080).readCString());
    }
}

function print_string(addr) {
    var base_hello_jni = Module.findBaseAddress("libhello-jni.so");
    var addr_str = base_hello_jni.add(addr);
    console.log("addr:", addr, " ", ptr(addr_str).readCString());
}

function main() {
    hook_native();
}

setImmediate(main);
```





```js
function hook_libart() {
    var module_libart = Process.findModuleByName("libart.so");
    var symbols = module_libart.enumerateSymbols();    
    var addr_RegisterNatives = null;        

    for (var i = 0; i < symbols.length; i++) {
        var name = symbols[i].name;
        if (name.indexOf("art") >= 0) {
            if ((name.indexOf("CheckJNI") == -1) && (name.indexOf("JNI") >= 0)) {
                if (name.indexOf("RegisterNatives") >= 0) {
                    console.log(name);
                    addr_RegisterNatives = symbols[i].address;
                }
            }
        }
    }

    if (addr_RegisterNatives) {

        /*
        
        jniEnv->RegisterNatives(mainActivityClass, methods, sizeof(methods) / sizeof(JNINativeMethod));
        
        static const JNINativeMethod methods[] = {
        	{"dynamicJavaMethod01", "()V", (void *) dynamicJavaMethod01}
		};
        */
        
        
        Interceptor.attach(addr_RegisterNatives, {
            onEnter: function (args) {
                console.log("addr_RegisterNatives:", hexdump(args[2]));
                console.log("addr_RegisterNatives name:", ptr(args[2]).readPointer().readCString())
                console.log("addr_RegisterNatives sig:", ptr(args[2]).add(Process.pointerSize).readPointer().readCString());
            }, onLeave: function (retval) {

            }
        });
    }
}


//inline 在 arm32中不太稳定,在arm64上比较稳定
function inline_hook() {
    // 直接hook可能hook不成功，因为so还没加载
    var base_hello_jni = Module.findBaseAddress("libhello-jni.so");
    console.log("base_hello_jni:", base_hello_jni);
    if (base_hello_jni) {
        console.log(base_hello_jni);
        //inline hook
        // .text:0000000000007318 AD 61 44 39                   LDRB            W13, [X13,#0x118]
		// .text:000000000000731C AD 01 0E 4A                   EOR             W13, W13, W14
        // .text:0000000000007320 6D 69 29 38                   STRB            W13, [X11,X9]
        var addr_07320 = base_hello_jni.add(0x07320);
        Interceptor.attach(addr_07320, {
            onEnter: function (args) {
                // 结果复制到010里面查看
                console.log("addr_07320 x13:", this.context.x13);
            }, onLeave: function (retval) {
            }
        });
    }
}


function hook_dlopen() {
    var dlopen = Module.findExportByName(null, "dlopen");
    Interceptor.attach(dlopen, {
        onEnter: function (args) {
            this.call_hook = false;
            var so_name = ptr(args[0]).readCString();
            if (so_name.indexOf("libhello-jni.so") >= 0) {
                console.log("dlopen:", ptr(args[0]).readCString());
                this.call_hook = true;
            }

        }, onLeave: function (retval) {
            if (this.call_hook) {
                inline_hook();
            }
        }
    });
    // 高版本Android系统使用android_dlopen_ext
    var android_dlopen_ext = Module.findExportByName(null, "android_dlopen_ext");
    Interceptor.attach(android_dlopen_ext, {
        onEnter: function (args) {
            this.call_hook = false;
            var so_name = ptr(args[0]).readCString();
            if (so_name.indexOf("libhello-jni.so") >= 0) {
                console.log("android_dlopen_ext:", ptr(args[0]).readCString());
                this.call_hook = true;
            }

        }, onLeave: function (retval) {
            if (this.call_hook) {
                inline_hook();
            }
        }
    });
}

function main() {
    hook_dlopen();
}

setImmediate(main);
```



#### 区分指令集

```
IDA中打开opcode设置为4

每个指令都是4个字节就是arm指令集
如果指令是2个字节的就是thumb指令，hook时需要+1
```



#### 其他常用命令

```
dexdump
从内存中搜索dex字符串
打开动态库hook

hook dlopen，打开库时判断文件头是否是dex字符串

excute 有漏网之鱼，搞不定动态加载dex

dexfile 脱下来了


nm libart.so

jnitrace

objdump -T xx.so

c++filt

```

