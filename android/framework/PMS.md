# PMS



## 作用

1.解析AndroidManifest.xml清单文件，解析清单文件中的所有节点信息
2.扫描.apk文件，安装系统应用，安装本地应用等
总结.管理本地应用，主要有， 安装，卸载，应用信息查询 等



## PKMS 启动过程分析

SystemServer启动PKMS： 先是在SystemServer.startBootstrapServices()函数中启动PKMS服务， 

再调用startOtherServices()函数中对dex优化，磁盘管理功能，让PKMS进入systemReady状态。

**第一步 到 第四步：** 

**startBootstrapServices**()首先启动Installer服务，也就是安装器，随后判断当前的设备是否处于加密 

状态，如果是则只是解析核心应用，接着调用PackageManagerService的静态方法main来创建pms对 

象 

**第一步**： 启动Installer服务 

**第二步**：获取设备是否加密(手机设置密码)，如果设备加密了，则只解析"core"应用 

**第三步**： 调用PKMS main方法初始化PackageManagerService，其中调用PackageManagerService() 

构造函数创建了PKMS对象 

**第四步**： 如果设备没有加密，操作它。管理A/B OTA dexopting

**第五步，第六步，第七步：** 

**startOtherServices****第五步**： 执行 updatePackagesIfNeeded ，完成dex优化； 

**第六步**： 执行 performFstrimIfNeeded ，完成磁盘维护； 

**第七步**： 调用systemReady，准备就绪。 



## PKMS构造方法

PKMS的构造函数中由 

```
两个重要的锁(mInstallLock、mPackages): 

mInstallLock ：用来保护所有安装apk的访问权限，此操作通常涉及繁重的磁盘数据读写等操作，并 

且是单线程操作，故有时候会处理很慢, 

此锁不会在已经持有mPackages锁的情况下火的，反之，在已经持有mInstallLock锁的情况下，立即 

获取mPackages是安全的 

mPackages：用来解析内存中所有apk的package信息及相关状态。 
```

和

```
5个阶段构成，下面会详细的来分析这些内容。 

阶段1：BOOT_PROGRESS_PMS_START 

阶段2：BOOT_PROGRESS_PMS_SYSTEM_SCAN_START 

阶段3：BOOT_PROGRESS_PMS_DATA_SCAN_START 

阶段4：BOOT_PROGRESS_PMS_SCAN_END 

阶段5：BOOT_PROGRESS_PMS_READY 
```



**阶段1细节:**

(1) 构造 DisplayMetrics ，保存分辨率等相关信息； 

(2) 创建Installer对象，与installd交互； 

(3) 创建mPermissionManager对象，进行权限管理； 

(4) 构造Settings类，保存安装包信息，清除路径不存在的孤立应用，主要涉及/data/system/目录的 

packages.xml，packages-backup.xml，packages.list，packages-stopped.xml，packages-stopped

backup.xml等文件。 

(5) 构造PackageDexOptimizer及DexManager类，处理dex优化； 

(6) 创建SystemConfig实例，获取系统配置信息，配置共享lib库； 

(7) 创建PackageManager的handler线程，循环处理外部安装相关消息。 



**阶段2细节:**

(1) 从init.rc中获取环境变量BOOTCLASSPATH和SYSTEMSERVERCLASSPATH； 

(2) 对于旧版本升级的情况，将安装时获取权限变更为运行时申请权限； 

(3) 扫描system/vendor/product/odm/oem等目录的priv-app、app、overlay包； 

(4) 清除安装时临时文件以及其他不必要的信息。



**阶段3细节:**

对于不仅仅解析核心应用的情况下，还处理data目录的应用信息，及时更新，祛除不必要的数据



**阶段4细节:**

(1) sdk版本变更，更新权限； 

(2) OTA升级后首次启动，清除不必要的缓存数据； 

(3) 权限等默认项更新完后，清理相关数据； 

(4) 更新package.xml



**阶段5细节:**

GC回收内存 和一些细节而已







## APK的扫描

第一步：扫描APK，解析AndroidManifest.xml文件，得到清单文件各个标签内容 

第二步：解析清单文件到的信息由 Package 保存。从该类的成员变量可看出，和 Android 四大组件相关 

的信息分别由 activites、receivers、providers、services 保存，由于一个 APK 可声明多个组件，因此 

activites 和 receivers等均声明为 ArrayList





## APK的安装

安装其实就是将apk的文件拷贝到对应的目录 

1.安装时把apk文件复制到此目录(可以将文件取出安装，和我们本身的apk是一样的)

data/app/包名->data/data/packagename/test.apk

2.开辟存放应用程序的文件数据的文件夹包括我们应用的so库，缓存文件等等

data/data/包名->data/data/packagename/(db,cache)

3.将apk中的dex文件安装到data/dalvik-cache目录下

data/dalvik-cache/(profiles,x86)





## PMS之权限扫描

权限扫描，扫描/system/etc/permissions中的xml，存入相应的结构体中，供之后权限管理使







**权限申请源码流程总结**:

第一步：MainActivity 调用 requestPermissions 进行动态权限申请； 

第二步：requestPermissions函数通过隐士意图，激活PackageInstaller的GrantPermissionsActivity界 

面，让用户选择是否授权； 

第三步：经过PKMS把相关信息传递给PermissionManagerService处理； 

第四步：PermissionManagerService处理结束后回调给---->PKMS中的onPermissionGranted方法把处 

理结果返回； 

第五步：PKMS通知过程中权限变化，并调用writeRuntimePermissionsForUserLPr函数让 

PackageManager的settings记录下相关授 权信息； 





**checkPermission**:

第一步：MainActivity会调用checkSelfPermission方法检测是否具有权限（红色区域） 

第二步：通过实现类ContextImpl的checkPermission方法经由ActivityManager和 

ActivityManagerService处理（紫色区域） 

第三步：经过ActivityManager处理后会调用PKMS的checkUidPermission方法把数据传递给 

PermissionManagerService处理（蓝色） 

第四步：在PermissionManagerService.checkUidPermission中经过一系列查询返回权限授权的状态 

（绿色区域）