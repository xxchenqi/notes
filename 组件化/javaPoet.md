### 组件化APT   高级用法JavaPoet

APT  编译的时候 ---> 处理注解

APT  传统方式 ---》生成 java文件

APT JavaPoet方式--》生成Java文件



1：传统方式 那些 开源项目有用到？

​		看看EventBus源码就知道了

​	    传统方式：优点（编程的流程写下去）  缺点（没有oop思想加入进来）

​		

2：JavaPoet到底是什么？

​		oop思想方式：优点（加入oop思想） 缺点（不习惯，倒序）



3：JavaPoet真的比传统方式好吗？

​	  并不是这样的，如果复杂的代码生成，反而效率低下

​      发展趋势，oop思想，真正的掌握JavaPoet，爱不释手



4：APT的学习

​	创建JavaLib，要监控到，**编译期** 

```groovy
	// 构建 -------》 【编译时期.....】 ------》打包 ----》安装
    // AS3.4.1 + Gradle 5.1.1 + auto-service:1.0-rc4
    compileOnly'com.google.auto.service:auto-service:1.0-rc4'
    annotationProcessor'com.google.auto.service:auto-service:1.0-rc4'
    // 帮助我们通过类调用的形式来生成Java代码 [JavaPoet]
    implementation "com.squareup:javapoet:1.9.0"
    // 依赖注解
    implementation project(":annotation")

```

APT 依赖 注解

```java
@SupportedAnnotationTypes("com.ideal.annotation.ARouter")
@SupportedSourceVersion(SourceVersion.RELEASE_7)
@AutoService(Processor.class)
// 注解处理器接收的参数
@SupportedOptions({"moduleName", "packageNameForAPT"})
public class ARouterProcessor extends AbstractProcessor {
    //用来打印日志
    private Messager messager;
    //文件处理
    private Filer filer;
    //处理Element工具
    private Elements elementUtils;

    @Override
    public synchronized void init(ProcessingEnvironment processingEnvironment) {
        super.init(processingEnvironment);
        messager = processingEnvironment.getMessager();
        filer = processingEnvironment.getFiler();
        elementUtils = processingEnvironment.getElementUtils();

        String moduleName = processingEnvironment.getOptions().get("moduleName");
        String packageNameForAPT = processingEnvironment.getOptions().get("packageNameForAPT");

        messager.printMessage(Diagnostic.Kind.NOTE, "moduleName=" + moduleName);
        messager.printMessage(Diagnostic.Kind.NOTE, "packageNameForAPT=" + packageNameForAPT);

    }

    @Override
    public boolean process(Set<? extends TypeElement> set, RoundEnvironment roundEnvironment) {
        Set<? extends Element> elements = roundEnvironment.getElementsAnnotatedWith(ARouter.class);
        if (elements.isEmpty()) {
            return false;
        }

        for (Element element : elements) {
            PackageElement packageElement = elementUtils.getPackageOf(element);
            //获取包名
            String packageName = packageElement.getQualifiedName().toString();
            messager.printMessage(Diagnostic.Kind.NOTE, "packageName=" + packageName);
            //获取类名
            String className = element.getSimpleName().toString();
            messager.printMessage(Diagnostic.Kind.NOTE, "className=" + className);
            //获取注解
            ARouter aRouter = element.getAnnotation(ARouter.class);

            /**
             * public class MainActivity$$$$$$$$$ARouter {
             *   public static Class findTargetClass(String path) {
             *     return path.equals("app/MainActivity") ? MainActivity.class : null;
             *   }
             * }
             */

            //方法
            MethodSpec methodSpec = MethodSpec
                    .methodBuilder("findTargetClass")
                    .addModifiers(Modifier.STATIC, Modifier.PUBLIC)
                    .returns(Class.class)
                    .addParameter(String.class, "path")
                    .addStatement("return path.equals($S) ? $T.class : null",
                            aRouter.path(), ClassName.get((TypeElement) element))
                    .build();

            //类
            TypeSpec typeSpec = TypeSpec
                    .classBuilder(className + "$$$$$$$$$ARouter")
                    .addModifiers(Modifier.PUBLIC)
                    .addMethod(methodSpec)
                    .build();


            JavaFile javaFile = JavaFile.builder(packageName, typeSpec)
                    .build();

            try {
                javaFile.writeTo(filer);
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
        return true;
    }
}
```



注解类

```java
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.CLASS)
public @interface ARouter {
    String path();
    String group() default "";
}
```