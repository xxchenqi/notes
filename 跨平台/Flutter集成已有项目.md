# Flutter集成已有项目

Flutter 1.17开始仅支持AndroidX应用

在release模式下Fluuter仅支持x86_64,armeabi-v7a,armeabi-v8a,不支持mips和x86

场景：

原生页面 点击跳转 Flutter页面

原生页面 嵌套 Flutter View

Flutter页面 嵌套 原生模块



创建Flutter module

flutter create -t module flutter_module

//指定包名创建

flutter create -t module --org com.example flutter_module



在创建flutter项目，最好和app项目平级创建，这样互不干涉，可以并行开发，

不建议创建在app项目内



创建完成会有.android 和.ios 隐藏文件夹，这2个是宿主工程，可以摆脱主工程以外去运行





### 构建flutter aar (非必须)

```
cd .android/
./gradlew flutter:assembleRelease
```

会在.android/Flutter/build/outputs/aar/中生成一个aar文件





### 已存在的Android应用配置flutter module依赖

```
//for flutter
setBinding(new Binding([gradle: this]))                               
evaluate(new File(                                                   
        settingsDir.parentFile,                                          
        'flutter_module/.android/include_flutter.groovy'                     
))

//可选，主要作用是可以在当前AS的Project下显示flutter_module以方便查看和编写Dart代码
include ':flutter_module'
project(':flutter_module').projectDir = new File('../flutter_module')
```

setBinding和evaluate允许Flutter模块包括自己在内的任何Flutter插件



在创建FlutterView有两种：

FlutterTextureView和FlutterSufraceView

使用FlutterSufraceView会有问题，在压后台回来后界面被复用的问题。

所以一般情况都使用FlutterTextureView





封装BaseFlutterFragment

```kotlin

import android.content.Context
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import com.proj.common.R
import com.proj.common.ui.component.BaseFragment
import io.flutter.embedding.android.FlutterTextureView
import io.flutter.embedding.android.FlutterView
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.embedding.engine.dart.DartExecutor
import kotlinx.android.synthetic.main.fragment_flutter.*

/**
 * 在Flutter中，官方提供的FlutterFragment封装比较死，所以在项目中为了后期扩展方便，需要自己定制，
 */
open class BaseFlutterFragment : BaseFragment() {
    protected lateinit var flutterEngine: FlutterEngine
    protected var flutterView: FlutterView? = null

    override fun onAttach(context: Context) {
        super.onAttach(context)
        flutterEngine = FlutterEngine(context)
        flutterEngine.dartExecutor.executeDartEntrypoint(DartExecutor.DartEntrypoint.createDefault())
    }

    override fun getLayoutId(): Int {
        return R.layout.fragment_flutter
    }

    fun setTitle(titleStr: String) {
        rl_title.visibility = View.VISIBLE
        title_line.visibility = View.VISIBLE
        title.text = titleStr
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (layoutView as ViewGroup).addView(createFlutterView(activity!!))

    }

    private fun createFlutterView(context: Context): FlutterView {
        val flutterTextureView = FlutterTextureView(activity!!)
        flutterView = FlutterView(context, flutterTextureView)
        return flutterView!!
    }

    override fun onStart() {
        flutterView!!.attachToFlutterEngine(flutterEngine)
        super.onStart()
    }

    override fun onResume() {
        super.onResume()
        //for flutter >=1.17 需要做这些事情,如果不做会导致引擎生命周期混乱，会出现奇奇怪怪的问题
        flutterEngine.lifecycleChannel.appIsResumed()
    }

    override fun onPause() {
        super.onPause()
        //pause ->appIsInactive
        flutterEngine.lifecycleChannel.appIsInactive()
    }

    override fun onStop() {
        super.onStop()
        //stop ->appIsPaused
        flutterEngine.lifecycleChannel.appIsPaused()
    }

    override fun onDetach() {
        super.onDetach()
        flutterEngine.lifecycleChannel.appIsDetached()
    }

}
```



### 热修复

打开模拟器，关闭app

运行 flutter attach

如果是多个设备  flutter attach -d 设备号

如果没问题会提示连接成功的一大堆信息

r：热加载

R：热重启

h：获取帮助

d：断开连接



### 调试

1.关闭app

2.AS上点击Flutter Attach（安装Flutter和dart插件）

3.启动app





### 复杂场景下的Flutter混合架构设计

缓存设计，通过IdleHandler空闲加载。

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

```kotlin

/**
 * 在Flutter中，官方提供的FlutterFragment封装比较死，所以在项目中为了后期扩展方便，需要自己定制，
 */
abstract class BaseFlutterFragment : BaseFragment() {
    private val flutterEngine: FlutterEngine?
    protected var flutterView: FlutterView? = null

    abstract val moduleName: String?

    init {
        flutterEngine =
            HiFlutterCacheManager.instance!!.getCachedFlutterEngine(moduleName!!, AppGlobals.get())
    }

    override fun getLayoutId(): Int {
        return R.layout.fragment_flutter
    }

    fun setTitle(titleStr: String) {
        rl_title.visibility = View.VISIBLE
        title_line.visibility = View.VISIBLE
        title.text = titleStr
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (layoutView as ViewGroup).addView(createFlutterView(activity!!))

    }

    private fun createFlutterView(context: Context): FlutterView {
        val flutterTextureView = FlutterTextureView(activity!!)
        flutterView = FlutterView(context, flutterTextureView)
        return flutterView!!
    }

    override fun onStart() {
        flutterView!!.attachToFlutterEngine(flutterEngine!!)
        super.onStart()
    }

    override fun onResume() {
        super.onResume()
        //for flutter >=1.17 需要做这些事情,如果不做会导致引擎生命周期混乱，会出现奇奇怪怪的问题
        flutterEngine!!.lifecycleChannel.appIsResumed()
    }

    override fun onPause() {
        super.onPause()
        //pause ->appIsInactive
        flutterEngine!!.lifecycleChannel.appIsInactive()
    }

    override fun onStop() {
        super.onStop()
        //stop ->appIsPaused
        flutterEngine!!.lifecycleChannel.appIsPaused()
    }

    override fun onDetach() {
        super.onDetach()
        flutterEngine!!.lifecycleChannel.appIsDetached()
    }

}
```

```java
public class BaseApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        HiFlutterCacheManager.getInstance().preLoad(this);
    }
}
```



```dart

//至少要有一个主入口
void main() => runApp(MyApp(FavoritePage()));

@pragma('vm:entry-point')//注册多个入口
void recommend() => runApp(MyApp(RecommendPage()));

class MyApp extends StatelessWidget {
  final Widget page;

  const MyApp(this.page);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Android 架构师',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: Scaffold(
        body: page,
      ),
    );
  }
}
```

