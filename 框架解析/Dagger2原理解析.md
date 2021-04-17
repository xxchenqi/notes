# Dagger2原理解析

![](C:\Users\Administrator\Desktop\md笔记\框架解析\Dagger2原理.png)





Dagger2自动生成3个辅助类

DaggerActivityComponent:为程序入口和桥梁，负责初始化 WatchModule_ProvideWatchFactory和Dagger2Activity_MemberInject，并将它们串联起来

WatchModule_ProvideWatchFactory:来生成 Watch 实例

MainActivity_MembersInjector: 将 Watch 实例赋值 给 MainActivity 的成员变量





定义:

module ->提供初始化对象

component->初始化入口桥梁，提供inject注入方法

使用:

DaggerActivityComponent.create().inject(this);



编译后会生成3个辅助类

