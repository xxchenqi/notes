# Bottom



1.定义上层对外接口

- ITab：设置数据源，动态修改高度，选择器实现
- ILayout：找到对应tab，添加选择器，默认选择，填充数据



2.数据源定义

- 图片：BITMAP,ICON
- Class类型的fragment
- 名称，默认图片，选择图片，iconFont，默认名称，选择名称，默认颜色，选择颜色
- 颜色<泛型>：int 和 Stirng 



3.ITab实现

- 数据源
- 图片：ImageView ，TextView(iconFont)
- 文字
- 初始化：view
- 设置数据源：初始化具体显示哪个View，选中的图片和未选中的图片
- 选择器：选中-未选中
- 动态高度修改：获取布局参数-修改高度-重设布局参数-隐藏名称



4.ILayout实现

继承自FrameLayout

- 定义：选择器集合，选中的数据源，透明度，bottom的高度，头部线条的高度和颜色，数据源集合，TAG

- 通过数据源找对应tab：通过TAG方式找到ViewGroup进行遍历，如果数据源相等返回

- 默认选中：遍历选择器集合，进行对应的改变

- 添加选择器：添加到集合中

- 解决底部数据被bottom覆盖问题：通过添加bottom高度的padding，通过**clipToPadding**为false，让view内部的padding区也可以显示view。

- 填充数据：

  - 添加高度为50左右，背景白色的底部到布局的底层
  - 创建FrameLayout设置tag，以便后续查找到
  - 定义bootom的宽高，宽度=屏幕宽度/数据源长度
  - 遍历数据源添加到FrameLayout
  - 添加分割线
  - 将FrameLayout添加到布局底部

  

5.用户在开发者选项里开启保留活动

到了后台在回到前台会导致重新创建activity，

需要在onSaveInstanceState中处理保存当前选中的fragment索引，在初始化默认选择时，获取索引的info信息进行选择 





# TOP

1.定义上层接口

。。。

2.数据源定义

- 枚举：图片文字
- Class类型的fragment
- 名称，默认图片，选择图片，默认颜色，选择颜色
- 颜色<泛型>：int 和 Stirng 



3.ITab实现

。。。



4.ILayout实现

继承自HorizontalScrollView



点击自适应滚动实现：

- 获取点击的tab在屏幕上的坐标
- 获取tab的宽度
- 判断点击了屏幕左侧还是右侧：
  - 左侧(tab的x轴坐标+tab宽度的一半>屏幕宽度的一半)：
    - 滚动后需要显示几个tab
    - 计算滚动距离
    - 获取前面需要滚动显示tab的rect（getLocalVisibleRect来获取控件是否显示）
    - 如果left坐标小于等于控件的负宽度时，说明控件完全没有显示，返回控件宽度
    - 如果left坐标大于0，说明控件宽度只显示了部分，返回rect.left
  - 右侧(tab的x轴坐标+tab宽度的一半<=屏幕宽度的一半)：
    - 滚动后需要显示几个tab
    - 计算滚动距离
    - 获取后面需要滚动显示tab的rect（getLocalVisibleRect来获取控件是否显示）
    - 如果right坐标大于控件的宽度时，说明控件完全没有显示，返回控件宽度
    - 如果right坐标小于等于控件的宽度时，说明控件宽度只显示了部分，返回控件宽度-rect.right
- 滚动当前X轴+滚动的距离

