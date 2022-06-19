# Hilt

## 基本使用

### 配置

```groovy
classpath 'com.google.dagger:hilt-android-gradle-plugin:2.28-alpha' // 导入gradle-plugin 字节码插庄

apply plugin: 'dagger.hilt.android.plugin' // gradle-plugin   // 导入gradle-plugin 字节码插庄

// hilt 依赖导入
implementation "com.google.dagger:hilt-android:2.28-alpha"
annotationProcessor "com.google.dagger:hilt-android-compiler:2.28-alpha"
//kapt "com.google.dagger:hilt-android-compiler:2.28-alpha" 

compileOptions {
    sourceCompatibility JavaVersion.VERSION_1_8
    targetCompatibility JavaVersion.VERSION_1_8
}
```



### application注册

```java
@HiltAndroidApp
public class MyApplication extends Application {
}
```



### 提供对象

```java
public class HttpObject {}
```



### 注入Activity

```java
@AndroidEntryPoint
public class MainActivity extends AppCompatActivity {
    @Inject
    HttpObject httpObject;
}
```



### module

#### 一般module

```java
@InstallIn(ActivityComponent.class)
@Module
public class HttpModule {
    @Provides
    public HttpObject getHttpObject(){
        return new HttpObject();
    }
}

```

#### 局部单例module

```java
@InstallIn(ActivityComponent.class)
@Module
public class HttpModule {
    @Provides
    @ActivityScoped // 上面的InstallIn 必须是 (ActivityComponent.class) 才能局部单例
    public HttpObject getHttpObject(){
        return new HttpObject();
    }
}
```

#### 全局单例module

```java
@InstallIn(ApplicationComponent.class)
@Module
public class HttpModule {
    @Provides
    @Singleton // 上面的InstallIn 必须是 (ApplicationComponent.class) 才能全局单例
    public HttpObject getHttpObject(){
        return new HttpObject();
    }
}
```



## @Binds 接口注入

接口

```java
public interface TestInterface {
    void method();
}
```

实现类

```java
public class TestClassImpl implements TestInterface {

    @Inject
    TestClassImpl() {}

    @Override
    public void method() {
        Log.i("cq", "666");
    }
}

```

module

```java
@Module
@InstallIn(ActivityComponent.class)
public abstract class TestInterfaceModule {
    @Binds //接口与实现类的注入
    public abstract TestInterface bindTestClass(TestClassImpl testClass);
}
```

使用

```java
@AndroidEntryPoint
public class MainActivity extends AppCompatActivity {

    @Inject
    TestInterface testInterface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void click(View view) {
        testInterface.method();
    }
}
```



## @Qualifier 自定义限定符

自定义注解

```java
@Qualifie
@Retention(RetentionPolicy.RUNTIME)
public @interface BindOkhttp {}
```

```java
@Qualifier
@Retention(RetentionPolicy.RUNTIME)
public @interface BindVolley {}
```

```java
@Qualifier
@Retention(RetentionPolicy.RUNTIME)
public @interface BindXUtils {}
```

module

```java
@Module
@InstallIn(ApplicationComponent.class)
public abstract class HttpRequestModule {
    @BindOkhttp
    @Binds
    @Singleton // 全局单例 必须用 @InstallIn(ApplicationComponent.class)
    abstract IHttpRequest bindOkHttp(OkHttpRequest okHttpRequest);

    @BindVolley
    @Binds
    @Singleton // 全局单例 必须用 @InstallIn(ApplicationComponent.class)
    abstract IHttpRequest bindVolley(VolleyRequest volleyRequest);

    @BindXUtils
    @Binds
    @Singleton // 全局单例 必须用 @InstallIn(ApplicationComponent.class)
    abstract IHttpRequest bindXUtils(XUtilsRequest xUtilsRequest);
}
```

application

```java
@HiltAndroidApp
public class MyApplication extends Application {
    // @BindVolley
    // @BindXUtils
    @BindOkhttp
    @Inject
    IHttpRequest iHttpRequest;

    public IHttpRequest getHttpRequest() {
        return iHttpRequest;
    }
}
```





## 原理

apt代码生成目录

```
/build/intermediates/transforms
```



### @AndroidEntryPoint

```JAVA
@AndroidEntryPoint
public class MainActivity extends AppCompatActivity {
    @Inject
	HttpObject httpObject;
}
```

通过ASM修改为,继承自Hilt_MainActivity

```java
@AndroidEntryPoint
public class MainActivity extends Hilt_MainActivity {
}
```

apt生成Hilt_MainActivity

```java
@Generated("dagger.hilt.android.processor.internal.androidentrypoint.ActivityGenerator")
abstract class Hilt_MainActivity extends AppCompatActivity implements GeneratedComponentManager<Object> {

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    inject();
    super.onCreate(savedInstanceState);
  }

  protected void inject() {
      // 给 httpObject 赋值
    ((MainActivity_GeneratedInjector) generatedComponent()).injectMainActivity(UnsafeCasts.<MainActivity>unsafeCast(this));
  }
}
```

赋值具体流程

```java
@Override
public void injectMainActivity(MainActivity mainActivity) {
  injectMainActivity3(mainActivity);
}
```

```java
private MainActivity injectMainActivity3(MainActivity instance) {
  MainActivity_MembersInjector.injectHttpObject(instance, HttpModule_GetHttpObjectFactory.getHttpObject(httpModule));
  return instance;
}
```

```java
@InjectedFieldSignature("com...MainActivity.httpObject")
public static void injectHttpObject(MainActivity instance, HttpObject httpObject) {
  instance.httpObject = httpObject;
}
```



### @HiltAndroidApp

```java
@HiltAndroidApp
public class MyApplication extends Application {
}
```

通过ASM修改为,继承自Hilt_MyApplication

```java
@HiltAndroidApp
public class MyApplication extends Hilt_MyApplication {
}
```

apt生成Hilt_MyApplication

```java
@Generated("dagger.hilt.android.processor.internal.androidentrypoint.ApplicationGenerator")
abstract class Hilt_MyApplication extends Application implements GeneratedComponentManager<Object> {

  @Override
  public void onCreate() {
      //赋值操作
    ((MyApplication_GeneratedInjector) generatedComponent()).injectMyApplication(UnsafeCasts.<MyApplication>unsafeCast(this));
    super.onCreate();
  }
}
```



