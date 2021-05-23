# ReactNative

### 组件生命周期

常用方法：

Mounting:

constructor

render

componentDidMount



Updating:

render

componentDidUpdate



Unmounting:

componentWillUnmount



### debug

启动服务：

npm start



打开调试菜单：

win：ctrl+m

mac：command+m

点击debug按钮：会打开浏览器网页

localhost:8081/debugger-ui/



### 自定义组件

将组件导出

```jsx
export default class NavigationBar {}
```



所有的组件需要继承自Component

```jsx
import React,{Component} from 'react'
export default class NavigationBar extends Component{}
```



Component必须要有render方法，render方法返回jsx语法结构，这个结构代表在页面展示的样式和具体UI

```jsx
import React,{Component} from 'react'
// 导入View , 创建样式StyleSheet
import {View,StyleSheet} from 'react-native'
export default class NavigationBar extends Component{
    render(){
        //返回View
        return <View>
            <View style={styles.line}/>
        </View>
    }
}
const styles=StyleSheet.create({
    line:{
        height:1,
        backgroundColor:"#d2d2d2"
    },
});
```



导入使用

```jsx
// ./js/component/NavigationBar 制定自定义的目录
import NavigationBar from './js/component/NavigationBar'

//...
    <NavigationBar/>
//...
```









### 已有的项目集成RN

#### 1.创建rn项目

```
npx react-native init rn_module
```

测试下能否运行

npm run android  或者 yarn android



打开项目，删除android 和 ios文件夹，因为要集成到我们自己项目这2个文件夹用不到



#### 2.添加RN所需依赖

##### 配置maven

module中的build.gradle

```groovy
//rn
project.ext.react = [
        entryFile: "index.android.js",
        enableHermes: false
]

def enableHermes = project.ext.react.get("enableHermes", false);
def jscFlavor = 'org.webkit:android-jsc:+'
def safeExtGet(prop, fallback) {
    rootProject.ext.has(prop) ? rootProject.ext.get(prop) : fallback
}
//rn end


dependencies {
    //rn
    if (enableHermes) {
        //如果项目不再当前rn项目里的话目录要再次确认
        def hermesPath = "../../rn_module/node_modules/hermesvm/android/";
        debugImplementation files(hermesPath + "hermes-debug.aar")
        releaseImplementation files(hermesPath + "hermes-release.aar")
    } else {
        implementation jscFlavor
    }
    implementation "com.facebook.react:react-native:+" // From node_modules
    //rn end
}
```



project的build.gradle

```groovy
allprojects {
 		//rn
        maven {
            // All of React Native (JS, Android binaries) is installed from npm
            //如果项目不再当前rn项目里的话目录要再次确认
            url "$rootDir/../rn_module/node_modules/react-native/android"
        }
        maven {
            // Android JSC is installed from npm
            //如果项目不再当前rn项目里的话目录要再次确认
            url("$rootDir/../rn_module/node_modules/jsc-android/dist")
        }
        //rn end
}
```



##### 配置权限

```xml
<uses-permission android:name="android.permission.INTERNET" />
```

##### 开启调试菜单

```xml
<activityandroid:name="com.facebook.react.devsupport.DevSettingsActivity"/>
```

##### 指定ndk需要兼容的架构

rn支持：armeabi-v7a , x86 , arm64-v8a , x86_64

```groovy
defaultConfig{
	ndk {
            abiFilters 'armeabi-v7a', 'x86'
        }
}     
```

##### 允许http铭文调试

在开发模式中需要用http加载电脑的js bundle

```xml
<application android:networkSecurityConfig="@xml/network_sercurity_config">
```

```xml
<?xml version="1.0" encoding="utf-8"?>
<network-security-config xmlns:android="http://schemas.android.com/apk/res/android">
    <base-config cleartextTrafficPermitted="true">
        <trust-anchors>
            <certificates
                src="system" />
            <certificates
                src="user" />
        </trust-anchors>
    </base-config>
</network-security-config>
```

#### 3.HiRNActivity

```kotlin
class HiRNActivity : AppCompatActivity() {
    private var mReactRootView: ReactRootView? = null
    private var mReactInstanceManager: ReactInstanceManager? = null
    override fun onCreate(savedInstanceState: Bundle?) {
 		super.onCreate(savedInstanceState)
        initRN()
        setContentView(mReactRootView)
    }
    fun initRN() {
         mReactRootView = ReactRootView(this)
         mReactInstanceManager = ReactInstanceManager.builder()
            .setApplication(application)
            .setCurrentActivity(this)
        	// 打包时放在assets目录下的JS bundle包的名字,App release之后会从该目录下加载JS bundle
            .setBundleAssetName("index.android.bundle")
        	// JS bundle中主入口的文件名，也就是index.js
            .setJSMainModulePath("index")
        	// 向RN添加Native Module，MainReactPackage这个是必须的
        	// 另外,如果我们创建一些其他的Native Module也需要通过addPackage的方式注册到RN中
            .addPackage(MainReactPackage())
        	// 设置RN是否开启开发者模式(debugging,reload,dev menu)
            .setUseDeveloperSupport(BuildConfig.DEBUG)
        	// 设置初始化RN时所处的生命周期
            .setInitialLifecycleState(LifecycleState.RESUMED)
            .build()
       
        //参数1：ReactInstanceManager 
        //参数2:和index.js中AppRegistry.registerComponent()注册的module保持对应
        //appName -> ./app.json -> name : rn_module
        //参数3:Bundle来作为RN初始化时传递JS的初始化数据
         
        mReactRootView?.startReactApplication(mReactInstanceManager, "rn_module")
    }
}

```



#### 4.运行项目

在rn_module中，启动服务

npm_start

启动成功后，运行项目



在终端中输入d可以开启调试菜单





### 实战技巧

#### RN生命周期绑定、RN返回键处理、RN开发者菜单实现

```kotlin
class HiRNActivity : AppCompatActivity(), DefaultHardwareBackBtnHandler {
    private var mReactRootView: ReactRootView? = null
    private var mReactInstanceManager: ReactInstanceManager? = null
    override fun onCreate(savedInstanceState: Bundle?) {
 		super.onCreate(savedInstanceState)
        initRN()
        setContentView(mReactRootView)
    }
    fun initRN() {
         ...
    }
    
    override fun onPause() {
        super.onPause()
        // 生命周期绑定
        mReactInstanceManager?.onHostPause(this)
    }

    override fun onResume() {
        super.onResume()
        // 第二个this就是返回按钮的监听接口
        mReactInstanceManager?.onHostResume(this, this)
    }

    override fun onDestroy() {
        super.onDestroy()
        mReactInstanceManager?.onHostDestroy(this)
    }

    //返回按钮的监听
    override fun onBackPressed() {
        if (mReactInstanceManager != null) {
            mReactInstanceManager?.onBackPressed()
        } else {
            super.onBackPressed()
        }
    }

    //RN消费返回不会回调，没有消费会回调此方法
    override fun invokeDefaultOnBackPressed() {
        super.onBackPressed()
    }

    //实现开发者菜单,默认不支持，所以需要重写
    override fun onKeyUp(keyCode: Int, event: KeyEvent?): Boolean {
        if (keyCode == KeyEvent.KEYCODE_MENU && mReactInstanceManager != null) {
            mReactInstanceManager!!.showDevOptionsDialog()
            return true
        }
        return super.onKeyUp(keyCode, event)
    }

}
```



#### 多组件实现

DemoPage.js

```js
import React from 'react'
import {View,Text} from 'react-native'

export class DemoPage extends React.Component {
    render() {
        return <View>
            <Text>DemoPage</Text>
        </View>
    }
}
```

index.js

```js
import {AppRegistry} from 'react-native';
import {DemoPage} from "./DemoPage"

AppRegistry.registerComponent('demo_page',() => DemoPage);
```

修改RN初始化

```kotlin
reactRootView?.startReactApplication(reactInstanceManager, "demo_page")
```

重启服务运行。。。





#### 打包

```
react-native bundle --platform android --dev false --entry-file index.js --bundle-output ../ASProj/app/src/main/assets/index.android.bundle --assets-dest ../ASProj/app/src/main/res/
```

bundle包会在assets

图片资源在drawable

app.json在raw

这样就不需要在开启本地服务运行。



```
--platform android : 代表打包导出android平台
--dev false : 关闭JS开发者模式
-entry-file index.js :代表js的入口文件为index.js
--bundle-output ： 后面跟的是打包后将JS bundle包导出到的位置
--assets-dest ： 后面跟的是打包后的一些资源文件导出到的位置
```

**注意：JS bundle一定要正确放到你的Android assets目录下，这个和上文中配置的.setBundleAssetName("index.android.bundle")进行对应**



### RN与Native通信

Native

1.自定义NativeModule

```kotlin
class HiRNBridge(reactContext: ReactApplicationContext) : ReactContextBaseJavaModule(reactContext) {
    // 定义JS调用的方法名
    override fun getName(): String {
        return "HiRNBridge"
    }

    @ReactMethod
    fun onBack(p: ReadableMap?) {
        currentActivity?.run {
            runOnUiThread {
                onBackPressed()
            }
        }
    }
    
    // ReadableMap 为 JS传递过来的数据
    @ReactMethod
    fun goToNative(p: ReadableMap) {
        ARouter.getInstance().build("/detail/main").withString(
            "goodsId",
            p.getString("goodsId")
        ).navigation()

    }
    
    // callback: Promise 回调
    @ReactMethod
    fun getHeaderParams(callback: Promise) {
        val params: WritableMap = Arguments.createMap();
        params.putString("boarding-pass", HiLocalConfig.instance!!.boardingPass())
        params.putString("auth-token", HiLocalConfig.instance!!.authToken())
        callback.resolve(params)
    }
}
```



2.自定义ReactPackage

```kotlin
class HiReactPackage : ReactPackage {
    override fun createNativeModules(reactContext: ReactApplicationContext): MutableList<NativeModule> {
        val modules: MutableList<NativeModule> = ArrayList()
        modules.add(HiRNBridge(reactContext))
        return modules
    }

    override fun createViewManagers(reactContext: ReactApplicationContext): MutableList<ViewManager<View, ReactShadowNode<*>>> {
        return mutableListOf()
    }
}
```

3.注册package

```kotlin
fun initRN() {
     mReactRootView = ReactRootView(this)
     mReactInstanceManager = ReactInstanceManager.builder()
		//...
		.addPackage(HiRNImageViewPackage())
		//...
		.build() 
}
```



JS

1.点的自定义HiRNBridge.js

```
import {NativeModules} from 'react-native'

module.exports=NativeModules.HiRNBridge;

```

2.使用

```js
// 导入
import HiRNBridge from '../lib/HiRNBridge'

HiRNBridge.onBack(null)

HiRNBridge.goToNative({'goodsId': '1580374239317'})

async getHeader() {
    let header = await HiRNBridge.getHeaderParams();
    this.setState({
        header
    })
}
```





### Native UI组件开发实战

#### RN嵌入Native UI步骤：

1. 准备Native UI的三件套
2. 注册Native UI
3. JS侧对应组件封装





#### 准备Native UI的三件套

PlatformView ： 要嵌入到RN的ios view 或 android view

viewManager ： PlatformView的控制器，用来创建和管理PlatformView

ReactPackage : 用于向RN提供PlatformView

```kotlin
class HiRNImageView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : AppCompatImageView(context, attrs, defStyleAttr) {
    fun setUrl(url: String) {
        Glide.with(this) .load(url).into(this)
    }
}
```



```kotlin
class HiRNImageViewManager() : SimpleViewManager<HiRNImageView>() {
    // 返回具体的native view
    override fun createViewInstance(reactContext: ThemedReactContext): HiRNImageView {
        return HiRNImageView(context = reactContext)
    }
	
    // 模块的名字，在RN里通过什么名字来获取view
    override fun getName(): String {
        return "HiRNImageView"
    }
	
    //暴露给RN
    @ReactProp(name = "src")
    fun setUrl(view: HiRNImageView, @Nullable sources: String?) {
        sources?.let {
            view.setUrl(it)
        }
    }
}
```



```kotlin
class HiRNImageViewPackage : ReactPackage {

    override fun createNativeModules(reactContext: ReactApplicationContext): MutableList<NativeModule> {
        return ArrayList()
    }

    override fun createViewManagers(reactContext: ReactApplicationContext): MutableList<ViewManager<*, *>> {
        val modules: MutableList<ViewManager<*, *>> = ArrayList()
        modules.add(HiRNImageViewManager())
        return modules
    }
}
```



注册pakageManager

```kotlin
 fun initRN() {
     mReactRootView = ReactRootView(this)
     mReactInstanceManager = ReactInstanceManager.builder()
		//...
		.addPackage(HiRNImageViewPackage())
		//...
		.build() 
}
```



创建js文件

HiRNImageView.js

```js
import {requireNativeComponent} from 'react-native';
//注册的组件名对应的是 SimpleViewManager -> getName
module.exports = requireNativeComponent('HiRNImageView');
```



使用

```js
import HiImageView from '../lib/HiRNImageView'

 return <View>
     	<HiImageView
			style={{width:200,height:200}}
            src={'...'}
		/>
     </View>
```





### Native UI组件的事件通信指南

1.借助RCTEventEmitter发送事件

```kotlin
class HiRNImageView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : AppCompatImageView(context, attrs, defStyleAttr) {
    fun setUrl(url: String) {
        Glide.with(this)
            .load(url)
            .into(this)
        setOnClickListener {
            fireEvent("onClick")
        }
    }

    /**
     * 向JS层传递Native UI的事件通知
     */
    fun fireEvent(message: String) {
        val event: WritableMap = Arguments.createMap()
        event.putString("message", message)
        val reactContext = context as ReactContext
        // 参数1： 将native和RN建立关联
        // 参数2： 发送事件名
        // 参数3： 发送的数据
        reactContext.getJSModule(RCTEventEmitter::class.java).receiveEvent(
            id,
            "onNativeClick",
            event
        )
    }
}
```



2.在ViewManager中进行事件映射

```kotlin
class HiRNImageViewManager() :
    SimpleViewManager<HiRNImageView>() {
    override fun createViewInstance(reactContext: ThemedReactContext): HiRNImageView {
        return HiRNImageView(context = reactContext)
    }

    override fun getName(): String {
        return "HiRNImageView"
    }

    @ReactProp(name = "src")
    fun setSrc(view: HiRNImageView, @Nullable sources: String?) {
        sources?.let {
            view.setUrl(it)
        }
    }

    /**
     * 要把事件名topChange映射到 JavaScript 端的onChange回调属性上
     * ，需要在你的ViewManager中覆盖getExportedCustomBubblingEventTypeConstants方法，并在其中进行注册：
     */
    override fun getExportedCustomBubblingEventTypeConstants(): MutableMap<String, Any> {
        //将Native 端的事件名：onNativeClick 映射给JS端的：onJSClick
        return MapBuilder.builder<String, Any>()
            .put(
                "onNativeClick",
                MapBuilder.of(
                    "phasedRegistrationNames",//固定值
                    MapBuilder.of("bubbled", "onJSClick")
                )//bubbled 固定值
            )
            .build();
    }

}
```



3.在JS侧接受事件

```js
import {requireNativeComponent} from 'react-native';
import React from 'react'

const ImageView = requireNativeComponent('HiRNImageView');


export default class HiRNImageView extends React.Component {

    _onJSClickEvent = (event) => {
        if (!this.props.onPress) {
            return;
        }

        // 获取原生事件传递的数据
        this.props.onPress(event.nativeEvent.message);
    };

    render() {
        return <ImageView {...this.props} onJSClick={this._onJSClickEvent}/>;
    }
}
```



使用

```js
<HiImageView
    style={{width: 200, height: 200}}
    src={'https://www.devio.org/img/beauty_camera/beauty_camera1.jpg'}
    onPress={(e) => {
        this.setState(
            {
                message: e
            }
        )
    }}
/>
```









