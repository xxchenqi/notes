# APK优化 



## APK的结构

**包含以下目录：**
assets/: 包含了应用的资源，这些资源能够通过AssetManager对象获得。
lib/: 包含了针对处理器层面的被编译的代码。这个目录针对每个平台类型都有一个子目录，比如armeabi, armeabi-v7a, arm64-v8a, x86, x86_64和mips。
res/: 包含了没被编译到resources.arsc的资源。
META-INF/: 包含CERT.SF和CERT.RSA签名文件，也包含了MANIFEST.MF文件。（译注：校验这个APK是否被人改动过）

**包含以下文件：**
classes.dex: 包含了能被Dalvik/Art虚拟机理解的 dex 文件格式的类。
resources.arsc: 包含了被编译的资源。该文件包含了res/values目录的所有配置的 xml 内容。打包工具将 xml 内容编译成二进制形式并压缩。这些内容包含了语言字符串和styles，还包含了那些内容虽然不直接存储在resources.arsc文件中，但是给定了该内容的路径，比如布局文件和图片。所以又叫 资源映射表
AndroidManifest.xml: 包含了主要的Android配置文件。这个文件列出了应用名称、版本、访问权限、引用的库文件。该文件使用二进制 xml 格式存储。（译注：该文件还能看到应用的minSdkVersion, targetSdkVersion等信息）



## 图片优化

SVG(Scalable Vector Graphics)，可缩放矢量图。SVG不会像位图一样因为缩放而让图片质量下降。优点在于可以减小APK的尺寸。常用于简单小图标。
svg是由xml定义的，标准svg根节点为<svg>。
Android中只支持 <vector>，我们可以通过 vector 将svg的根节点 <svg> 转换为 <vector>。

在Android Studio中打开工程，在res目录中点击右键，选中Vector Asset，加载本地的svg图片



### SVG批量转换

通过第三方工具 svg2vector 进行批量转换

执行转换命令：

java -jar svg2vector-cli-1.0.0.jar -d . -o a -h 20 -w 20 

-d 指定svg文件所在目录
-o 输出android vector图像目录
-h 设置转换后svg的高
-w 设置转换后svg的宽



###  SVG 文件不支持的功能举例

滤镜效果：不支持投影，模糊和颜色矩阵等效果。
文本：建议使用其他工具将文本转换为形状。



### 矢量图向后兼容--生成PNG

Android 5.0（API 21）之前的版本不支持矢量图，使用 Vector Asset Studio 有两种方式适配。

方式一：生成 png 格式的图片

Vector Asset Studio 可在构建时 针对每种屏幕密度将矢量图转换为不同大小的位图，在 build.gradle 中配置如下： 

SVG 适用于 Gradle 插件1.5 及以上版本；

```groovy
android{
    defaultConfig{
        // 5.0（API 21）版本以下,将svg图片生成指定维度的png图片
        generatedDensities = ['xhdpi','xxhdpi']
    }
}
```



方式二：支持库

在 build.gradle 中配置如下，适用于 Gradle 插件2.0及以上版本：

```groovy
android{
    // Gradle Plugin 2.0+
    defaultConfig{
        // 利用支持库中的 VectorDrawableCompat 类,可实现 2.1 版本及更高版本中支持 VectorDrawable
        vectorDrawables.useSupportLibrary = true
    }
}
dependencies {
  // 支持库版本需要是 23.2 或更高版本
  compile 'com.android.support:appcompat-v7:23.2.0'
}
```

使用矢量图 必须使用 app:srcCompat 属性，而不是 android:src，如下：



### Tint着色器

虽然可以直接在 xml 文件中修改矢量图的颜色，但是并不建议直接修改，我们一般让矢量图为黑色，然后用 Tint 着色器去修改矢量图的颜色。





## 移除无用资源

### AS 提供了一键移除所有无用的资源

Refactor->Remove Unused Resorces...

但是这种方式不建议使用，因为如果某资源仅存在动态获取资源id 的方式，那么这个资源会被认为没有使用过，从而会直接被删除。

动态获取方式:
getResources().getIdentifier("name","defType",getPackageName());



### Lint移除无用资源

...



### 国际化资源配置

```groovy
android{
    defaultConfig{
        // 只适配英语
        resConfigs 'en'
    }
}
```



### 动态库打包配置

首先我们需要知道 so文件是由ndk编译出来的动态库，是 c/c++ 写的，所以不是跨平台的。即每一个平台需要使用对应的so库。

ABI 是应用程序二进制接口简称（Application Binary Interface），定义了二进制文件（尤其是.so文件）如何运行在相应的系统平台上，从使用的指令集，内存对齐到可用的系统函数库。

在Android 系统上，每一个CPU架构对应一个ABI：armeabi，armeabi-v7a，arm64- v8a，x86，x86_64，mips，mips64。

现在我们一般只需要配置armeabi-v7a即可。

```groovy
android{
    defaultConfig{
        ndk{
            abiFilters "armeabi-v7a"
        }
    }
}
```



## 压缩代码

将 minifyEnabled 设置为 true 即可 

```groovy
proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
```



## 压缩资源

shrinkResources true

默认情况未启用严格模式，如需启动则需设置 shrinkMode，创建keep.xml，

```xml
<?xml version="1.0" encoding="utf-8"?>
<resources xmlns:tools="http://schemas.android.com/tools"
    tools:shrinkMode="strict" />
```

将该文件保存在项目资源中，例如，保存在 res/raw/keep.xml。构建不会将该文件打包到 APK 之中。 



## 资源混淆



1. 解压待处理的apk

   记录APK内文件存储方式（结合强制压缩文件列表，/config/compressData.txt）解析 arsc 文件（ZIP中存储文件两种方式:DEFLATED(压缩)/STORED(仅存储)，对于APK文件来说某些资源不允许压缩(如：SoudPool加载raw下的mp3)，而有些资源可以压缩但是AS打包APK时却没有压缩(如png/jpg等)。）

2. 解析ARSC文件

   - 将文件全读出来，保存到 bytebuffer中（ByteOrder.LITTLE_ENDIAN 小端模式）
   - 解析 RES_TABLE_TYPE，解析头信息 type+头大小+块大小
   - 解析 RES_STRING_POOL_TYPE
     全局字符串资源池 包含了在string.xml中定义的值 与 res/xx/xx的路径地址
   - 解析 RES_TABLE_PACKAGE_TYPE
     - 包中的类型字符串池 (color、drawable等)
     - 包中的关键字字符串池 (app_name、colorPrimary等)
   - 后面的不需要解析了

3. 混淆字符,产生混淆名集合 (混淆全局字符串池中的res与资源名称字符串池，不能混淆资源类型字符串池)

   - 混淆全局字符串池
     res/layout/activity_main.xml => r/a/a.xml
     res/mipmap-anydpi-v26/ic_launcher.xml => r/b/b.xml
   - 混淆资源名称字符串池
      activity_main =》 a
      ic_launcher => b
   - 制作混淆后的字符串池

4. 制作新的arsc文件

   - 写入 RES_TABLE_TYPE
   - 写入 RES_STRING_POOL_TYPE 全局字符串池
   - 写入 RES_TABLE_PACKAGE_TYPE
   - 写入剩余数据

5. 将apk中其他文件拷贝到 app目录,并根据混淆修改 res/目录下文件名

   - 把不需要处理的文件拷贝到新的文件夹中
   - 修改res下文件名（通过之前保存的map找到混淆名）

6. 打包、对齐、签名

   - 7z打包 (需要先安装7z工具，压缩效果最好的压缩工具)
   - zipalign对齐 (在Android SDK/build-tools/xx/目录下,需要配置环境变量)
     让Android系统能更高效的操作APK（如读取apk中的资源）
   - apksigner签名 (在Android SDK/build-tools/xx/目录下,需要配置环境变量)
      --v1-signing-enabled true/false 是否开启v1签名
      --v2-signing-enabled true/false 是否开启v2签名

7. 写出mapping

















