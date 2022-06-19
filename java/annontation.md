# 注解

**@Target**

作用目标

- ElementType.ANNOTATION_TYPE 可以应用于注解类型。
- ElementType.CONSTRUCTOR 可以应用于构造函数。
- ElementType.FIELD 可以应用于字段或属性。
- ElementType.LOCAL_VARIABLE 可以应用于局部变量。
- ElementType.METHOD 可以应用于方法级注解。
- ElementType.PACKAGE 可以应用于包声明。
- ElementType.PARAMETER 可以应用于方法的参数。
- ElementType.TYPE 可以应用于类的任何元素。

**@Retention**

保留级别

- RetentionPolicy.SOURCE - 源码级,编译后就没了
  - APT技术(注解处理器)

  - 语法检查(Lint检查)

  - @IntDef (PS:用枚举比较耗内存)

- RetentionPolicy.CLASS - 编译级，保留到class，但是dex里会抛弃
  - 字节码插桩

    - ARouter框架通过ASM解决初始化耗时操作
    - AOP
    - @Login(通过字节码插桩判断是否需要登录)
  
    ​	
  
- RetentionPolicy.RUNTIME - 运行时级别
  - 反射




例:

```java
//@Target(ElementType.TYPE) 只能在类上标记该注解 
@Target({ElementType.TYPE,ElementType.FIELD}) // 允许在类与类属性上标记该注解 
@Retention(RetentionPolicy.SOURCE) //注解保留在源码中 
public @interface Cq {

}
```

