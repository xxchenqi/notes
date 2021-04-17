# Refresh

1.定义刷新接口

​	刷新时是否禁止滚动

​	刷新完成

​	设置下拉刷新的监听器

​	设置下拉刷新的视图



2.定义抽象下拉刷新的Overlay视图,可以重载这个类来定义自己的Overlay

状态枚举（默认为初始态）：初始态、Header展示的状态、超出可刷新距离的状态、刷新中的状态、超出刷新位置松开手后的状态

定义字段：触发下拉刷新需要的最小高度，最小阻尼，最大阻尼

定义调用抽象方法：显示Overlay，超过Overlay，释放就会加载，开始加载，加载完成



3.HiRefreshLayout 继承FrameLayout实现刷新接口

```
dispatchTouchEvent：
松开手:如果head.getBottom大于0，说明head显示出来
显示的距离大于刷新的最小高度：还原bottom-最小刷新距离大写小，让head显示到头部位置，不要显示在台下面
改变状态：超出刷新位置松开手后的状态
显示的距离小于刷新的最小高度：还原bottom大小

其他情况交给GestureDetector处理
如果GestureDetector消费了或者 当前状态不是初始状态并且不是刷新状态 并且 head显示出来  发送取消事件不让父view处理


GestureDetector：onScroll重写
横向滑动或者刷新禁止则不处理->false
刷新时是否禁止滚动->true
没有刷新或没有达到可以刷新的距离，且头部已经划出或下拉
	还在滑动中->计算速度，判断是否要刷新，位置还原后进行刷新
	除非->false
其他情况->false

还原距离通过Scroller进行还原




```

