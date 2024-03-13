# Android Root

参考地址:

[玩机必看！带你入坑安卓刷机，小白也能看懂的ROOT基础指南来啦！](https://www.bilibili.com/video/BV1BY4y1H7Mc/?spm_id_from=333.337.search-card.all.click&vd_source=332f625deec165e69a48ae42c0edf5e1)

[Google Pixel解锁BL、刷入Twrp及Root折腾小记](https://www.itfanr.cc/2018/10/16/google-pixel-unlock-bl-and-root/)



## 分区概念

Boot分区：存放启动和引导文件，包含操作系统kernel(内核)和Ramdisk(虚拟内存)

System分区：系统分区，包含整个操作系统和预装软件，系统升级或刷机通常就是在操作这个分区

Vendor分区：包含开发厂商定制的应用和库文件

Data分区：数据分区

Cache分区：缓存区

Recovery分区：恢复分区，刷机刷系统很多时候都是在这个分区里进行的



注：

部分手机不存在Recovery分区的手机，因为Android7.0开始引入了新的OTA升级方式A/B System Updates。

这种升级方式将boot、system等分区变为两套，叫做slot A(boot_a，system_a...)和slot B，也就是AB分区，平时用的是主分区,没在用的是备用分区

这样做有好处呢？

1.系统升级可以做到无缝升级，用户可以边用手机边在后台升级系统，这个升级的是备份分区里的系统，升级完成重启手机，备份分区会自动切换到主分区，这样升级系统方便省时

2.防止升级失败导致卡机，但系统无法成功启动时就会切回之前的分区，缺点就是占用双倍的system分区空间，因为总有一个分区时拿来做备份的（于是就有了VAB）



VAB虚拟AB分区：

目前出厂打在安卓11的机型普通采用了这种新的虚拟AB分区(VAB)，解决了空间占用的问题



无论是AB还是VAB刷机会麻烦一些，因为Recovery被并到boot分区里了,想刷第三方Recovery就会非常折腾，所以一般不推荐Recovery刷机，而是通过底层的fastboot来操作



## fastboot

bootloader 引导加载器(类似BIOS)

长按电源开机的一刻起，bootloader就开始工作，这段时间会初始化硬件设备，引导操作系统内核(也就是boot分区)，引导后期我们可以选择进入fastboot阶段(刷机模式)



### 卡刷和线刷区别

线刷：fastboot来刷机，因为fastboot需要连数据线到电脑

卡刷：要刷的包拷进手机里，直接手机上刷







## ROOT步骤

### 1.解锁Bootloader(简称BL)

开发者选项打开OEM解锁,后进行解锁

解锁步骤省略。。。根据不同厂商有不同解锁方式





为什么要解锁Bootloader？

因为厂商都会对bootloader上锁，如果不解锁无法对分区进行更改，而ROOT过程就是在修改boot分区，不解锁自然就无法root



### 2.刷第三方Recovery(TWRP)

#### 查看是否是AB分区

```shell
getprop | grep treble
或者
getprop ro.build.ab_update

没有输出就是非AB分区
```



#### 非A/B分区

进入fastboot,执行刷写命令

```
fastboot flash recovery twrp.img
```

#### A/B分区

1.刷入临时twrp

```
fastboot boot twrp.img
```

注意：这里没有flash



2.刷入永久Twrp

选择 `Install` – 找到目录 – 选择 `twrp*.zip` 文件 – 弹出安装界面 – 直接滑动底部的滑条 `Swipe to confirm Flash` 安装：

安装完成后，点击下面的 `Reboot System` 按钮，重启系统。直接点击中间的 **`Do Not Install`** 即可。



### 2.ROOT(magisk)

方法1：Recovery卡刷

将magisk的包复制进手机，进到TWRP里刷入这个包，重启

将magisk.zip改成magisk.apk，就可以安装magisk管理端





方法2：Fastboot线刷

直接在fastboot中刷Magisk修复后的boot镜像

```
fastboot flash boot boot.img
```



boot镜像需要去各平台下载

安装Magisk，点击安装选择boot.img进行修复后导出，执行刷入



## 刷镜像(补充)

### 【TWRP】使用adb sideload刷OTA镜像

```
进入TWRP
adb reboot recovery 

点击高级-点击ADB Sideload-滑动进入Sideload模式

adb devices 查看是否进入
显示 XXXXXX    sideload

adb sideload xxx.zip

注：windows 可能不支持sideload。会一直卡在verify，mac上sideload正常使用
目前未找到问题，更换过usb2.0和3.0都无效，下面这个链接试了也不行，暂时未找到解决办法。
https://xdaforums.com/t/fix-on-windows-and-getting-errors-using-fastboot-sideload-over-usb-3-0-solution-inside.4547931/
```





## 常用命令

```
重启到恢复模式
adb reboot recovery

重启到fastboot模式
adb reboot bootloader
```



```
显示fastboot设备
fastboot devices

获取手机相关信息
fastboot getvar al

擦除分区
fastboot erase (分区名)
例：清除system分区：fastboot erase system

刷入分区
fastboot flash (分区名) (分区镜像)
例：将boot镜像 "boot.img" 刷入boot分区：fastboot flash boot boot.img

引导启动镜像
fastboot boot (分区镜像)
例：启动到recovery分区：fastboot boot recovery.img

刷入ROM
fastboot update (刷机包)
例：将 update.zip 刷入：fastboot update update.zip

解锁Bootloader
fastboot oem unlock

多设备使用
fastboot -s (命令)
通过fastboot devices获取序列号，控制多设备中的一个
例：清除序列号为'abc'设备的system分区：fastboot -s abc erase system

```



```
备份:

进入fastboot模式，刷机recovery
fastboot flash recovery D:\t.img


pixel2刷机命令:
fastboot flash boot boot.img
fastboot flash vendor vendor.img
fastboot flash system_a system.img
fastboot flash system_b system_other.img
fastboot flash userdata userdata.img
fastboot flash vbmeta vbmeta.img
fastboot reboot



fastboot -w update root.zip
fastboot reboot
```







## 资源

驱动程序二进制文件下载
https://developers.google.com/android/drivers?hl=zh-cn

获取 Google USB 驱动程序(win fastboot无法识别设备)
https://developer.android.com/studio/run/win-usb?hl=zh-cn

ADB/Fastboot驱动：(备份下载地址)

https://cz-jam.lanzouj.com/iZICY02v2k8j



出厂映像

https://developers.google.com/android/images?hl=zh-cn

完整OTA映像

https://developers.google.com/android/ota?hl=zh-cn



```
下载镜像后可以直接执行
flash-all.bat
```





TWRP(刷第三方Recovery)

https://twrp.me/

Magisk：

https://github.com/topjohnwu/Magisk



pixle刷机文章


https://www.itfanr.cc/2018/10/16/google-pixel-unlock-bl-and-root/



其他系统镜像:

https://lineageos.org/



## 其他注意事项

刷机成功后一直卡在logo界面loading

进入recovery页面，长按power off键，然后按一下声音+键，重新进入recovery页面，选中清除数据(Wipe data)，然后重启手机



“已连接，但无法访问互联网“开机后wifi有感叹号,时间无法同步解决办法
在手机的shell里以root用户执行:

```shell
settings put global captive_portal_http_url https://www.google.cn/generate
settings put global captive_portal_https_url https://www.google.cn/generate_204
settings put global ntp_server 1.hk.pool.ntp.org
reboot
```





## 各资源版本

```
Nexus 5 (GSM/LTE) 专用“hammerhead”
4.4~6.0.1

Nexus 5X 专用“bullhead”
6.0.0~8.1.0

Pixel 专用“sailfish”
7.1.0~10.0.0

Pixel 2 专用“walleye”
8.0.0~11.0.0
```

