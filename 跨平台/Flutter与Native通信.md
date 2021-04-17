# Flutter与Native通信

通信机制:

通过MethodChannel通信,通信是异步的

3种Channel:

1.BasicMessageChannel

用于传递字符串和半结构化的信息，持续通信。

例：Native将遍历的文件传送到Flutter

2.MethodChannel

用于传递方法调用，一次性通信

例：调用Native拍照

3.EventChannel

用于数据流通信，持续通信

例：手机电量变化，网络链接变化



原理：

通过BinaryMessengers消息信使，将数据转换成字节数据传送。







Android端定义通信接口

```kotlin
interface IBridge<P,Callback>{
    fun onBack(p:P?)
    fun goToNative(p:P)
    fun getHeaderParams(callback:Callback)
}
```

定义通信接口实现类，并且实现MethodChannel.MethodCallHandler

**MethodCallHandler的onMethodCall方法主要是用来Flutter对Native的通信回调**

```kotlin

import com.alibaba.android.arouter.launcher.ARouter
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel

class FlutterBridge : IBridge<Any?, MethodChannel.Result>, MethodChannel.MethodCallHandler {

    private var methodChannels = mutableListOf<MethodChannel>()

    companion object {
        @JvmStatic
        var instance: FlutterBridge? = null
            private set

        @JvmStatic
        fun init(flutterEngine: FlutterEngine): FlutterBridge {
            //FlutterBridge名称需要和Fluuter的一致
            val methodChannel = MethodChannel(flutterEngine.dartExecutor, "FlutterBridge")
            if (instance == null) {
                FlutterBridge().also { instance = it }
            }
            methodChannel.setMethodCallHandler(instance)
            //由于多个引擎就有多个channel，所以用数组存入MethodChannel
            instance!!.apply {
                methodChannels.add(methodChannel)
            }
            return instance!!
        }
    }

    fun fire(method: String, arguments: Any?) {
        methodChannels.forEach {
            it.invokeMethod(method, arguments)
        }
    }

    fun fire(method: String, arguments: Any?, callback: MethodChannel.Result) {
        methodChannels.forEach {
            it.invokeMethod(method, arguments, callback)
        }
    }

    override fun onBack(p: Any?) {
        TODO("Not yet implemented")
    }

    override fun goToNative(p: Any?) {
        if(p is Map<*,*>){
            val action = p["action"]
            if(action=="goToDetail"){
                ARouter.getInstance().build("/order/main").navigation()
            }
        }
    }

    override fun getHeaderParams(callback: MethodChannel.Result) {
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        when (call.method) {
            "onBack" -> onBack(call.arguments)
            "getHeaderParams" -> getHeaderParams(result)
            "goToNative" -> goToNative(call.arguments)
            else -> result.notImplemented()
        }
    }
}
```



**初始化FlutterBridge插件，必须放在引擎初始化之后**

```kotlin

import android.content.Context
import android.os.Looper
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.embedding.engine.FlutterEngineCache
import io.flutter.embedding.engine.dart.DartExecutor
import io.flutter.view.FlutterMain

class HiFlutterCacheManager private constructor() {

    /**
     * 预加载Flutter
     */
    fun preLoad(context: Context) {
        //在线程空闲时执行预加载任务
        Looper.myQueue().addIdleHandler {
            initFlutterEngine(context, MODULE_NAME_FAVORITE)
            initFlutterEngine(context, MODULE_NAME_RECOMMEND)
            false
        }
    }

    fun getCachedFlutterEngine(moduleName: String, context: Context?): FlutterEngine {
        var engine = FlutterEngineCache.getInstance()[moduleName]
        if (engine == null && context != null) {
            initFlutterEngine(context, moduleName)
        }
        return engine!!
    }

    /**
     * 初始化flutter
     */
    private fun initFlutterEngine(context: Context, moduleName: String): FlutterEngine {
        val flutterEngine = FlutterEngine(context)
        //插件注册要紧跟引擎初始化之后，否则会有在dart中调用插件因为还未初始化完成而导致的时序问题
        FlutterBridge.init(flutterEngine)
        flutterEngine.dartExecutor.executeDartEntrypoint(
            DartExecutor.DartEntrypoint(FlutterMain.findAppBundlePath(), moduleName)
        )
        FlutterEngineCache.getInstance().put(moduleName, flutterEngine)
        return flutterEngine
    }

    companion object {
        const val MODULE_NAME_FAVORITE = "main"
        const val MODULE_NAME_RECOMMEND = "recommend"

        @JvmStatic
        @get:Synchronized
        var instance: HiFlutterCacheManager? = null
            get() {
                if (field == null) {
                    field = HiFlutterCacheManager()
                }
                return field
            }
            private set
    }
}
```



Flutter定义通信类

```dart
import 'package:flutter/services.dart';

class FlutterBridge {
  static FlutterBridge _instance = FlutterBridge._();
  //FlutterBridge需要和Android端定义的一致
  MethodChannel _bridge = const MethodChannel("FlutterBridge");
  var _listeners = {};

  FlutterBridge._() {
    _bridge.setMethodCallHandler((MethodCall call) {
      String method = call.method;
      if (_listeners[method] != null) {
        return _listeners[method](call);
      }
      return null;
    });
  }

  static FlutterBridge getInstance() {
    return _instance;
  }

  register(String method, Function(MethodCall) callBack) {
    _listeners[method] = callBack;
  }

  unRegister(String method) {
    _listeners.remove(method);
  }

  goToNative(Map params) {
    _bridge.invokeMethod("goToNative", params);
  }

  MethodChannel bridge() {
    return _bridge;
  }

}

```

方法注册和使用

在生命周期initState中通过FlutterBridge.getInstance().register 注册

回信通过Future.value方法



在dispose中注销FlutterBridge.getInstance().unRegister



通过FlutterBridge.getInstance().goToNative()，进行Native通信



```dart
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_like/core/flutter_bridge.dart';

class RecommendPage extends StatefulWidget {
  @override
  _RecommendPageState createState() => _RecommendPageState();
}

class _RecommendPageState extends State<RecommendPage> {
  @override
  void initState() {
    super.initState();
    //test方法注册
    FlutterBridge.getInstance().register("test", (MethodCall call) {
      print("test");
      //回信
      return Future.value("Flutter received.");
    });
  }

  @override
  void dispose() {
    super.dispose();
    FlutterBridge.getInstance().unRegister("test");
  }

  @override
  Widget build(BuildContext context) {
    return Container(
        child: Column(
      children: [
        Text("推荐模块"),
        FlatButton(
          child: Text("goToDetail"),
          onPressed: () {
              //调用到Native
            FlutterBridge.getInstance().goToNative(
              {"action": "goToDetail"}
            );
          },
        )
      ],
    ));
  }
}
```





Android调用native

```kotlin
title.setOnClickListener {
    FlutterBridge.instance!!.fire("test", "cq", object : MethodChannel.Result {
        override fun notImplemented() {
        }
        override fun error(errorCode: String?, errorMessage: String?, errorDetails: Any?) {
            Toast.makeText(context, errorMessage, Toast.LENGTH_SHORT).show()
        }
        override fun success(result: Any?) {
            Toast.makeText(context, result as String, Toast.LENGTH_SHORT).show()
        }
    })
}
```









简单概述:

```
Native
创建MethodChannel，设置回调setMethodCallHandler，用来监听Flutter到Native的通信
通过MethodChannel的invokeMethod来调用Flutter的方法

Flutter
创建MethodChannel，设置回调setMethodCallHandler，用来监听Native到Flutter的通信
通过MethodChannel的invokeMethod来调用Native的方法


```





