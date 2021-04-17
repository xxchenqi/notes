# Flutter框架

![img](https://upload-images.jianshu.io/upload_images/4531604-72ebc8e2d5fb5391.png?imageMogr2/auto-orient/strip|imageView2/2/w/815/format/webp)

框架层:

Foundation：工具库

Animation:动画库

Painting:绘制库

Gestures:手势库

Rendering:绘制层

Widgets:各种Widgert

引擎层：

Skia：2D图形库

Dart：DartVm，Dart执行环境，让Dart可以再Android ，IOS 运行

Plattform Channel :平台渠道





# Flutter绘制原理

Gpu发送VSYNC->Dart构建抽象视图结构（UI Thread）->绘制好视图结构在GPU线程进行合成

->在提供给SKIA->在给GPU进行渲染



RN是扩展调用OEM组件，而Flutter实现了自己的渲染。这就是Flutter能达到更高的性能因素。



# Flutter渲染流程

GPU->Animate->Build->Layout->Paint->Layer Tree

Animate：动画阶段，会随着VSYNC信号到来，更改Widget

Build：构建阶段，需要被重新构建的Widget会进行重新构建

Layout：布局阶段，确认元素的大小和位置

Paint：绘制阶段，RenderObject的paint方法就会被调用





# Flutter生命周期

![img](https://pic4.zhimg.com/v2-670a74cdf2368d6c0d9e33e7e483073b_1440w.jpg?source=172ae18b)







# Flutter三棵树

Widget树

Element树

RenderObject树



StatelessWidget内创建了StatelessElement

