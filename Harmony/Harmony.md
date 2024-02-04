# 鸿蒙开发文档

## 概念

### 什么是声明式UI

声明式描述

状态驱动视图更新



### 应用模型

- **FA（Feature Ability）模型：** HarmonyOS API 7开始支持的模型，已经不再主推。
- **Stage模型：** HarmonyOS API 9开始新增的模型，是目前主推且会长期演进的模型。在该模型中，由于提供了AbilityStage、WindowStage等类作为应用组件和Window窗口的“舞台”，因此称这种应用模型为Stage模型。



## 装饰器

```
@Builder装饰器：自定义构建函数
@Styles装饰器：定义组件重用样式
@Extend装饰器：定义扩展组件样式

@State：组件内状态，标记的变量必须初始化，支持类型:Object、class、string、number、boolean、enum类型,以及这些类型的数组。
不支持any，不支持简单类型和复杂类型的联合类型，不允许使用undefined和null。嵌套类型以及数组中的对象属性无法触发视图更新	
@Prop装饰器：父子单向同步（从父组件单向同步状态）,只支持string,number,boolean,enum,不支持数组，any,不允许子组件初始化（父组件更新数据，子组件也会更新,子组件更新数据，父组件不更新）
@Link装饰器：父子双向同步,父子类型一致（支持的类型和State类似）,禁止子组件初始化
@Provide装饰器和@Consume装饰器：与后代组件双向同步(跨组件层级双向同步状态)
@Observed装饰器和@ObjectLink装饰器：嵌套类对象属性变化
@Watch装饰器：状态变量更改通知
```



# 页面和自定义组件生命周期

```
页面生命周期，即被@Entry装饰的组件生命周期，提供以下生命周期接口：
onPageShow：页面每次显示时触发。
onPageHide：页面每次隐藏时触发一次。
onBackPress：当用户点击返回按钮时触发。

组件生命周期，即一般用@Component装饰的自定义组件的生命周期，提供以下生命周期接口：
aboutToAppear：组件即将出现时回调该接口，具体时机为在创建自定义组件的新实例后，在执行其build()函数之前执行。
aboutToDisappear：在自定义组件即将析构销毁时执行。
```



![](.\res\页面组件生命周期.png)







## 路由

### router.pushUrl()

router.RouterMode.Single单实例模式和router.RouterMode.Standard多实例模式。

在单实例模式下：如果目标页面的url在页面栈中已经存在同url页面，离栈顶最近同url页面会被移动到栈顶，移动后的页面为新建页，原来的页面仍然存在栈中，页面栈的元素数量不变；如果目标页面的url在页面栈中不存在同url页面，按多实例模式跳转，页面栈的元素数量会加1。



### router.replaceUrl()

router.RouterMode.Single单实例模式和router.RouterMode.Standard多实例模式。

在单实例模式下：如果目标页面的url在页面栈中已经存在同url页面，离栈顶最近同url页面会被移动到栈顶，替换当前页面，并销毁被替换的当前页面，移动后的页面为新建页，页面栈的元素数量会减1；如果目标页面的url在页面栈中不存在同url页面，按多实例模式跳转，页面栈的元素数量不变。





## UIAbility

UIAbility是一种包含用户界面的应用组件，主要用于和用户进行交互。UIAbility也是系统调度的单元，为应用提供窗口在其中绘制界面。

每一个UIAbility实例，都对应于一个最近任务列表中的任务。

一个应用可以有一个UIAbility，也可以有多个UIAbility。



### 生命周期

UIAbility的生命周期包括Create、Foreground、Background、Destroy四个状态，WindowStageCreate和WindowStageDestroy为窗口管理器（WindowStage）在UIAbility中管理UI界面功能的两个生命周期回调，从而实现UIAbility与窗口之间的弱耦合。



<img src=".\res\UIAbility生命周期.png" style="zoom: 33%;" />





Create状态，在UIAbility实例创建时触发

Foreground和Background状态，分别在UIAbility切换至前台或者切换至后台时触发。

Destroy状态，在UIAbility销毁时触发



UIAbility实例创建完成之后，在进入Foreground之前，系统会创建一个WindowStage。每一个UIAbility实例都对应持有一个WindowStage实例。

WindowStage为本地窗口管理器，用于管理窗口相关的内容，例如与界面相关的获焦/失焦、可见/不可见。

可以在onWindowStageCreate回调中，设置UI页面加载、设置WindowStage的事件订阅。

在onWindowStageCreate(windowStage)中通过loadContent接口设置应用要加载的页面。



### 启动模式

UIAbility当前支持singleton（单实例模式）、multiton（多实例模式）和specified（指定实例模式）3种启动模式。

singleton：在最近任务列表中只存在一个该类型的UIAbility实例。

multiton：在最近任务列表中可以看到有多个该类型的UIAbility实例。

specified：运行时由UIAbility内部业务决定是否创建多实例。





## 注意事项

1.

```
判断相等建议用===，而不是==
=== 不会去判断数据类型
```

2.

```
跳转
// 正确
router.pushUrl({ url: 'pages/second' })
// 错误
// router.pushUrl({ url: './second' })
```

3.

```
Button(){
  
}
.width('100')
.height('50')
这样写是错误的应该把{}删除


Button('aaa'){
  Text('666')
}
.width('100')
.height('50')
错误写法2个Text重复了


```







## 常见问题

### 1.Failure[INSTALL_PARSE_FAILED_USESDK_ERROR]

```
工程中配置的releaseType字段与设备（模拟器和真机）镜像版本不匹配

修改build-profile.json5指定版本
"compileSdkVersion": ?,
"compatibleSdkVersion": ?,
```

查看手机releaseType

- 在手机/平板HarmonyOS 3.1 Release设备上：

  ```
  hdc -c shell 
  getprop persist.sys.ohc.apiversion
  getprop persist.sys.ohc.releasetype
  ```

- 在手机/平板HarmonyOS 3.1 Release之前版本的设备上：

  ```
  复制代码hdc -c shell
  getprop hw_sc.build.os.apiversion
  getprop hw_sc.build.os.releasetype
  ```



### 2.ERROR: API version 8 does not support the Stage model.

```
API8不支持Stage模型
API 9开始支持Stage模型，API 8只能选FA模型
```

### 3.The type of the target device does not match the deviceType configured in the config.json file of the selected module.

```
你的设备类型不对,tablet是平板

修改config.json中的deviceType
"deviceType": [
  "phone","tablet"
],
```

### 4.Failure[INSTALL_FAILED_NO_BUNDLE_SIGNATURE]

```
签名文件没有

打开Project Structure
Project-Signing Configs-登录-登录后自动生成签名
```







## 参考文档

https://developer.harmonyos.com/cn/documentation/overview/?catalogVersion=V3



