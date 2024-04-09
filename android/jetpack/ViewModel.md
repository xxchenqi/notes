# ViewModel



ViewModel 和 onSaveInstanceState方法有什么区别？

onSaveInstanceState只能存储轻量级的k-v数据，非配置变更导致的页面被回收时才会触发，此时数据存储在ActivityRecord中。

viewmodel可以存放任意Object数据。因配置变更导致的页面被回收才有效，此时存在ActivityThread#ActivityClientRecord中。



SaveState本质是利用onSaveInstanceState的时机，每个ViewModel的数据单独存储在bundle中，再合并成一个整体。再存放在Activity的outBundle中。







ViewModel创建的写法：

1.无参数:反射创建

2.有SaveStateHandle参数：从saveStateRegistry中取出缓存的数据，并且构建出SaveStateHandle对象传递进来

3.ViewModel实现AndroidViewModel，此时就有2个参数 application和SaveStateHandle