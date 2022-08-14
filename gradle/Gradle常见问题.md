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





### 3.手动下载SDK

错误问题：
\> Failed to install the following Android SDK packages as some licences have not been accepted.
   platforms;android-30 Android SDK Platform 30
   build-tools;30.0.1 Android SDK Build-Tools 30.0.1


下载sdk命令：
sdkmanager "platform-tools" "platforms;android-30"

下载完成后还是会提示相同的问题，此时在执行以下命令，自动接受license

yes | sdkmanager --licenses



### 4.gradle缓存问题

主要删除以下2个文件(最好将整个包名目录都删除了):

descriptor.bin
E:\Android\.gradle\caches\modules-2\metadata-2.96\descriptors

aar和pom文件
E:\Android\.gradle\caches\modules-2\files-2.1\com.techedux.classx.debugtool\



或者使用以下命令

强制更新缓存
gradlew build --refresh-dependencies

清除当前项目缓存
gradlew cleanBuildCache



### 5.gradle命令合并和依赖

```groovy
task cleanBuildPublish {
    dependsOn 'clean'
    dependsOn 'build'
    dependsOn 'publish'
    tasks.findByName('build').mustRunAfter 'clean'
    tasks.findByName('publish').mustRunAfter 'build'
}
```





### 6.maven publish

```groovy
plugins {
    id 'maven-publish'
}

def nexusRepositoryUrl = NEXUS_RELEASES
version = DEBUG_TOOL_NO_OP_LIB_VERSION

if (!DEBUG_TOOL_NO_OP_LIB_IS_RELEASE.toBoolean()) {
    version = "${DEBUG_TOOL_NO_OP_LIB_VERSION}-SNAPSHOT"
    nexusRepositoryUrl = NEXUS_SNAPSHOTS
}

publishing {
    publications {
        release(MavenPublication) {
            artifactId = DEBUG_TOOL_NO_OP_LIB_ARTIFACT
            groupId = DEBUG_TOOL_NO_OP_LIB_GROUP
            //依赖bundleReleaseAar，如果不依赖会找不到aar包，每次发布都更新aar，不使用缓存aar
            afterEvaluate { artifact(tasks.getByName("bundleReleaseAar")) }
            //存储在pom文件里的描述信息
            pom {
                name = DEBUG_TOOL_NO_OP_LIB_ARTIFACT
                description = DEBUG_TOOL_NO_OP_LIB_DES
                url = LIB_URL
            }
            // pom文件中声明依赖，从而传递到使用方,如果没声明，第三方依赖不会写到pom文件中
            pom.withXml {
                def dependenciesNode = asNode().appendNode('dependencies')
                configurations.implementation.allDependencies.each {
                    // 避免出现空节点或 artifactId=unspecified 的节点
                    if (it.group != null && (it.name != null && "unspecified" != it.name) && it.version != null) {
                        println it.toString()
                        def dependencyNode = dependenciesNode.appendNode('dependency')
                        dependencyNode.appendNode('groupId', it.group)
                        dependencyNode.appendNode('artifactId', it.name)
                        dependencyNode.appendNode('version', it.version)
                        dependencyNode.appendNode('scope', 'implementation')
                    }
                }
            }
        }
    }
    //仓库
    repositories {
        maven {
            url = nexusRepositoryUrl
            println(url)
            credentials {
                username = NEXUS_USERNAME
                password = NEXUS_PASSWORD
            }
        }
    }
}
```



### 7.常用仓库地址配置

```groovy
repositories {
        maven { url 'https://maven.aliyun.com/repository/google' }
        maven { url 'https://maven.aliyun.com/repository/jcenter' }
        maven { url 'https://maven.aliyun.com/repository/public' }
        maven { url 'https://maven.aliyun.com/nexus/content/repositories/releases/' }
        jcenter()
        google()        
}
```



### 8.AAR 内部三方库依赖的问题

```
使用 Android Studio 打包出来的 AAR ，不会将其依赖的三方库打包进去。


举个例子，library Test 依赖了 okhttp,打包成了 Test.aar ,app 使用本地方式引用了 Test.aar，但是无法使用 okhttp，为了不报错，app还需要添加 okhttp 依赖。
Google Android Studio 的负责人在 stackoverflow 上解释了 为什么 Android Studio 不能将多个依赖打包进一个 AAR 文件的原因，是因为将不同的library打包在一起，涉及到资源和配置文件智能合并，所以是个比较复杂的问题，同时也容易造成相同的依赖冲突。
官方虽然不支持，但是开发者的能力是无限的，为了解决此问题，开发出来了一个 Gradle 插件 android-fat-aar, 这种方式是抛弃 Android Studio 自带的打包 AAR 的方法，而是自己编写一个生成 AAR 的脚本。也是很厉害了，但是很不幸，目前来看 gradle 2.2.3+ 以上，这个就不适用了。
不过，不要慌，这个问题可以通过使用 Maven 依赖解决。因为 library Module 上传 Maven 后，会生成 一个 .pom 文件，记录 library Module 的依赖。当 Gradle 依赖 Maven 上的这个库时，会通过 pom 文件下载对应依赖。如果不想要对应依赖的话，可以通过下面的方法关闭 Gradle 的依赖传递。
```

https://juejin.cn/post/6844903992191877133
