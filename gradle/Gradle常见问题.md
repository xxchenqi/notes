# Gradle常见问题



### 1.执行gradlew命令为什么又重新去下载了？

Android项目创建完成后会有以下文件：

```
gradlew - linux下可执行脚本
gradlew.bat - Windows下可执行脚本
gradle/wrapper/gradle-wrapper.jar - gradle wrapper的代码
gradle/wrapper/gradle-wrapper.properties - 配置文件
```



gradle-wrapper.properties描述

```shell
#GRADLE_USER_HOME一般在 ~/.gradle , windows在 C:\Users\用户名\.gradle

#下载根目录
distributionBase=GRADLE_USER_HOME
#下载根目录+具体路径 C:\Users\用户名\.gradle\wrapper\dists
distributionPath=wrapper/dists
#解压根目录
zipStoreBase=GRADLE_USER_HOME
#解压根目录+具体路径
zipStorePath=wrapper/dists
#具体下载的url
distributionUrl=https\://services.gradle.org/distributions/gradle-6.5-bin.zip
```

执行graldew命令后，会去相应的下载目录检查是否有当前需要的版本，不存在该版本就去下，存在就执行编译操作。



### 2.插件兼容性问题

项目使用4.1.2的gradle版本

```
implementation "com.android.tools.build:gradle:4.1.2"
```

报错日志:**The value for this property cannot be changed any further.**

```
Caused by: java.lang.IllegalStateException: The value for this property cannot be changed any further.
        at org.gradle.api.internal.provider.AbstractProperty$NonFinalizedValue.beforeMutate(AbstractProperty.java:386)
        at org.gradle.api.internal.provider.AbstractProperty.assertCanMutate(AbstractProperty.java:263)
        at org.gradle.api.internal.provider.AbstractProperty.setSupplier(AbstractProperty.java:212)
        at org.gradle.api.internal.provider.DefaultProperty.set(DefaultProperty.java:70)
        at com.android.build.gradle.internal.api.ApkVariantOutputImpl.setVersionCodeOverride(ApkVariantOutputImpl.java:96)
        at com.android.build.gradle.internal.api.ApkVariantOutputImpl_Decorated.setVersionCodeOverride(Unknown Source)
        at com.android.build.gradle.api.ApkVariantOutput$setVersionCodeOverride.call(Unknown Source)
        at com.hujiang.build.tasks.SwitchTask$_greet_closure1$_closure3$_closure5.doCall(SwitchTask.groovy:87)
        at com.hujiang.build.tasks.SwitchTask$_greet_closure1$_closure3.doCall(SwitchTask.groovy:77)
        at org.gradle.util.ClosureBackedAction.execute(ClosureBackedAction.java:71)
        at org.gradle.util.ConfigureUtil.configureTarget(ConfigureUtil.java:154)
        at org.gradle.util.ConfigureUtil.configure(ConfigureUtil.java:105)
        at org.gradle.util.ConfigureUtil$WrappedConfigureAction.execute(ConfigureUtil.java:166)
        at org.gradle.api.internal.DefaultDomainObjectCollection.all(DefaultDomainObjectCollection.java:163)
        at org.gradle.api.internal.DefaultDomainObjectCollection.all(DefaultDomainObjectCollection.java:198)
        at org.gradle.api.DomainObjectCollection$all.call(Unknown Source)
        at com.hujiang.build.tasks.SwitchTask$_greet_closure1.doCall(SwitchTask.groovy:42)
```

不难发现是在SwitchTask这个类里面的下面这个方法报错的。

`((ApkVariantOutputImpl) variantOutput).setVersionCodeOverride(Integer.valueOf(versionCode))`



但是之前使用的3.4.2版本确没有报错

```
implementation "com.android.tools.build:gradle:3.4.2"
```

那么跟踪4.1.2和3.4.2的源码区别

4.1.2分析：

```
SwitchTask：
ApkVariantOutputImpl.setVersionCodeOverride
->
if (variantType.isBaseModule()) {
    variantOutput.getVersionCode().set(versionCodeOverride);
}
->
DefaultProperty.set
->
AbstractProperty.setSupplier
->
assertCanMutate
->
FinalizedValue.beforeMutate
->
throw new IllegalStateException(String.format("The value for %s is final and cannot be changed any further.", displayName.getDisplayName()));
```

分析到这里就很清楚当variantType.isBaseModule()为true就会报错。

那么什么时候为true？

variantType字段类型为VariantType接口，VariantTypeImpl是VariantType实现枚举类。

VariantTypeImpl有很多枚举：BASE_APK，OPTIONAL_APK，LIBRARY，TEST_APK，ANDROID_TEST，UNIT_TEST

**只有在BASE_APK的时候会将isBaseModule设置为true**

那么之所以报错的结果就很明显了，这样设计的目的我个人猜测是，你的app里的版本和版本名称是想让你主动去设置，而不是让第三方插件去随意更改你的app版本号。



到这里3.4.2版本就没这个限制了。具体哪个版本增加了这个限制可以在看下具体修改情况。。。



目前解决方案：

1.删除这段代码就行了

2.反射获取到mVariantType判断isBaseModule

3.判断当前编译环境（未研究）







