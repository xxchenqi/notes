# CMake

include_directories(目录名)	导入头文件

add_library(主库名称，动态/静态库，文件名.../变量名)	生成库

set()	设置环境变量，可以将jniLibs添加到CMake环境变量里

find_library(变量名，库名称)	寻找库

target_link_libraries(主库名称，第三方库名称...)	链接到我们的主库

```cmake
cmake_minimum_required(VERSION 3.4.1) # 最低支持Cmake版本

# TODO 第一步：导入头文件
include_directories("inc")

# 批量导入所有源文件 , allCPP是变量名
file(GLOB allCPP *.c *.h *.cpp)

#生成动态/静态库
add_library(
        native-lib # 库名称 libnative-lib.so
        SHARED # 动态库
        ${allCPP} #上述定义的变量，如果不定义变量，就要写好多文件
)

# CMAKE_CXX_FLAGS  可以理解为CMAKE环境变量目录
# CMAKE_SOURCE_DIR == CMakeLists.txt所在的路径
# CMAKE_ANDROID_ARCH_ABI == 当前的CPU架构 == armeabi-v7a
# ${ANDROID_ABI} 之前用的等价于 CMAKE_ANDROID_ARCH_ABI

# TODO 第二步：导入库文件，将jniLibs目录添加到CMAKE_CXX_FLAGS环境变量里
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}")

# 寻找ndk的库
# 自动寻找此路径的库 D:\Android\Sdk\ndk\21.0.6113669\platforms\android-23\arch-arm64\usr\lib\liblog.so
find_library(
        log-lib # 变量名称
        log # 寻找哪个库，log对应的库是liblog.so
        )
        
# TODO 第三步，将上述定义的库链接 到我们的 libnative-lib.so总库
target_link_libraries(
        native-lib
        #${log-lib} 这里完全可以简写成log
        log # 自动寻找log库，链接到 libnative-lib.so
        fmod # fmod 链接到 libnative-lib.so
        fmodL # fmodL 链接到 libnative-lib.so
)
```



输出

```cmake

#[[
(无) = 重要消息；
STATUS = 非重要消息；
WARNING = CMake 警告, 会继续执行；
AUTHOR_WARNING = CMake 警告 (dev), 会继续执行；
SEND_ERROR = CMake 错误, 继续执行，但是会跳过生成的步骤；
FATAL_ERROR = CMake 错误, 终止所有处理过程；
]]

message(STATUS "666>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
```



引入子目录下的CMakeLists.txt 

```cmake
cmake_minimum_required(VERSION 3.10.2)

file(GLOB SOURCE *.cpp)

add_library(
        count
        SHARED # 最终生成 libcount.so
        ${SOURCE}
)
```



```cmake
cmake_minimum_required(VERSION 3.10.2)

file(GLOB SOURCE *.c)

add_library(
        get
        STATIC # 最终生成 libget.a
        ${SOURCE}
)
```



静态库需要在build.gradle defaultConfig文件中增加这个

```
externalNativeBuild {
            cmake {
                cppFlags ""
            }

            cmake {
                arguments '-DANDROID_PLATFORM=android-14','-DANDROID_TOOLCHIAN=clang'
                targets 'static-lib'
            }
        }
```





```cmake
#引入get子目录下的CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/cpp/libget)

#引入count子目录下的CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/cpp/libcount)

target_link_libraries( # native-lib是我们的总库
        native-lib # 被链接的总库
        log # 自动寻找  # 具体的库 链接到 libnative-lib.so里面去
        get # 具体的库 链接到 libnative-lib.so里面去
        count # 具体的库 链接到 libnative-lib.so里面去
        )

```





