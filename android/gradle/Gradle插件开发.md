# Gradle插件开发

## 独立项目插件开发

一个独立的 Java 项目/模块，可以将文件包发布到仓库(Jcenter)，使其他项目方便引入。

### 插件创建

创建java模块

修改gradle

```groovy
apply plugin: 'groovy'

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
    implementation 'com.android.tools.build:gradle:3.5.2' //gradle 具体api 没有文档
    implementation gradleApi() //配置gradle API
}
```

将java文件名修改为groovy

新建文件后缀名修改成groovy



### 插件实现

```groovy
package com.cq.myplugin

import com.android.build.gradle.AppExtension
import com.android.build.gradle.api.ApplicationVariant
import com.android.build.gradle.api.BaseVariantOutput
import com.android.builder.model.SigningConfig
import org.gradle.api.Plugin
import org.gradle.api.Project

class MyPlugin implements Plugin<Project> {

    @Override
    void apply(Project project) {
        Jiagu jiagu = project.extensions.create("jiagu", Jiagu)
        //回调, 在gradle配置完成之后回调，在解析完build.gradle之后回调
        project.afterEvaluate {
            AppExtension android = project.extensions.android
            android.applicationVariants.all {
                ApplicationVariant variant ->
                    //对应变体(debug/release)的签名配置
                    SigningConfig signingConfig = variant.signingConfig
                    variant.outputs.all {
                        BaseVariantOutput output->
                            //输出的apk文件
                            File apk = output.outputFile
                            //创建加固任务  jiag  Debug
                            MyTask jiaguTask = project.tasks.create("jiagu${variant.baseName.capitalize()}", MyTask)
                            jiaguTask.jiagu = jiagu
                            jiaguTask.signingConfig = signingConfig
                            jiaguTask.apk = apk
                    }
            }
        }
    }
}
```



```groovy
package com.cq.myplugin

class Jiagu {
    String userName;
    String password;
    String jiaguTools

    String getUserName() {
        return userName
    }

    void setUserName(String userName) {
        this.userName = userName
    }

    String getPassword() {
        return password
    }

    void setPassword(String password) {
        this.password = password
    }

    String getJiaguTools() {
        return jiaguTools
    }

    void setJiaguTools(String jiaguTools) {
        this.jiaguTools = jiaguTools
    }
}
```



```groovy
package com.cq.myplugin

import com.android.builder.model.SigningConfig
import org.gradle.api.DefaultTask
import org.gradle.api.tasks.TaskAction;

class MyTask extends DefaultTask {

    Jiagu jiagu
    SigningConfig signingConfig
    File apk

    MyTask() {
        //任务分组
        group = "myTask"
    }

    @TaskAction
    def run() {
        //调用命令行工具
        project.exec {
            // java -jar jiagu.jar -login user password
            it.commandLine("java", "-jar", jiagu.jiaguTools, "-login", jiagu.userName, jiagu.password)
        }

        if (signingConfig) {
            project.exec {
                // java -jar jiagu.jar -importsign  xxxx
                it.commandLine("java", "-jar", jiagu.jiaguTools, "-importsign", signingConfig.storeFile.absolutePath,
                        signingConfig.storePassword, signingConfig.keyAlias, signingConfig.keyPassword)
            }
        }
        project.exec {
            // java -jar jiagu.jar -jiagu  xxxx
            it.commandLine("java", "-jar", jiagu.jiaguTools, "-jiagu", apk.absolutePath,
                    apk.parent, "-autosign")
        }
    }
}

```





### 上传插件

在main目录下创建resources目录

resources下创建META-INF

META-INF 下在创建gradle-plugins目录 (注意:这里不要用META-INF.gradle-plugins去创建)

新建com.cq.jiagu.properties 文件

其中com.cq.jiagu 相当于引入插件时要写的字符串

例：apply plugin: 'com.cq.jiagu'

文件里指定插件具体的实现:

实现中的MyPlugin为插件的全类名

```
implementation-class=com.cq.myplugin.MyPlugin
```



配置gradle

```groovy
apply plugin:'maven-publish'

publishing{
    publications{
        Jiagu(MavenPublication){//Jiagu名字随意
            from components.java  //要把源码生成的jar包上传
            groupId 'com.cq' 
            artifactId "jiagu"
            version "1.0"
            //相当于:classpath 'com.cq:jiagu:1.0'
        }
    }
}

```

配置同步完成后，任务列表会多了publishing分组

点击执行publishToMavenLocal

成功上传后的目录：

C:\Users\Administrator\.m2\repository\com\cq\jiagu



### 使用插件

配置本地仓库

```groovy
buildscript {
    repositories {
        google()
        jcenter()
        //配置本地仓库
        mavenLocal()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:3.5.2'
		//引入上传的插件
        classpath 'com.cq:jiagu:1.0'

    }
}
```



引入插件

```groovy
apply plugin:'com.cq.jiagu'

jiagu{
    userName '360U3190525523'
    password 'q8851551'
    jiaguTools 'D:\\jiagu\\jiagu.jar'
}
```



配置成功同步后，app任务列表里的Tasks会多出jiagu分组



## buildSrc插件开发

描述：将插件源代码放在 buildSrc/src/main/groovy/ 中，只对该项目中可见。 



创建buildSrc目录

创建src/main/groovy

创建build.gradle

```groovy
apply plugin: 'groovy'
```

创建plugin

```groovy
package com.cq.plugin

import org.gradle.api.Plugin
import org.gradle.api.Project

class MyPlugin implements Plugin<Project>{

    @Override
    void apply(Project project) {
        println(222)
    }
}
```

在main目录下创建resources目录

resources下创建META-INF/gradle-plugins目录

新建com.cq.plugin.properties 文件

其中com.cq.plugin相当于引入插件时要写的字符串

实现中的MyPlugin为插件的全类名

```
implementation-class=com.cq.plugin.MyPlugin
```



通过包名使用

```groovy
apply plugin: com.cq.plugin.MyPlugin
```

通过资源名使用

```groovy
apply plugin: 'com.cq.plugin'
```



## Build script脚本开发

把插件写在 build.gradle 文件中，一般用于简单的逻辑，只在该 build.gradle 文件可见。

```groovy
apply plugin: MyPlguin

class MyPlguin implements Plugin<Project> {

    @Override
    void apply(Project project) {
        println 111
    }
}
```