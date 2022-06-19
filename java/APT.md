# APT

## 1.创建注解javaLib模块

annotation模块

```java
@Retention(RetentionPolicy.SOURCE)
@Target(ElementType.TYPE)
public @interface MyClass {

}
```



## 2.创建注解处理器javaLib模块

compiler模块

### gradle配置

```groovy
plugins {
    id 'java-library'
}

java {
    sourceCompatibility = JavaVersion.VERSION_1_7
    targetCompatibility = JavaVersion.VERSION_1_7
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
    implementation project(':annotation')
}


tasks.withType(JavaCompile) {
    options.encoding = "UTF-8"
}
```



### 创建注解处理器

```java
package com.cq.compiler;

import java.util.Set;

import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.Messager;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.TypeElement;
import javax.tools.Diagnostic;


@SupportedSourceVersion(SourceVersion.RELEASE_8)
@SupportedAnnotationTypes({"com.cq.annotation.MyClass"})
public class TestProcessor extends AbstractProcessor {
    /**
     * javac调用此方法
     *
     * @param set
     * @param roundEnvironment
     * @return
     */
    @Override
    public boolean process(Set<? extends TypeElement> set, RoundEnvironment roundEnvironment) {
//        判断是否结束
//        roundEnvironment.processingOver()
//        set.isEmpty()
        
        Messager messager = processingEnv.getMessager();
        messager.printMessage(Diagnostic.Kind.NOTE, "==========>roundEnvironment=" + roundEnvironment.processingOver());
        messager.printMessage(Diagnostic.Kind.NOTE, "==========>set=" + set.isEmpty());
        return false;
    }


    /**
     * 允许此注解处理器处理的注解
     * @return
     */
//    @Override
//    public Set<String> getSupportedAnnotationTypes() {
//        return super.getSupportedAnnotationTypes();
//    }
}

```

### 创建资源文件

src/main/resources/META-INF/services/javax.annotation.processing.Processor

```
com.cq.compiler.TestProcessor
```

填写注解处理器的路径





## 3.app模块

```groovy
dependencies {
    annotationProcessor project(':compiler')
    implementation project(':annotation')
}
```

