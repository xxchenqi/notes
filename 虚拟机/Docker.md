# Docker

## 推荐参考文档

Docker快速入门

https://docker.easydoc.net/

Docker命令参考

https://docs.docker.com/engine/reference/commandline/run/

Docker 官方镜像仓库

https://hub.docker.com/

Docker菜鸟教程

https://www.runoob.com/docker/docker-tutorial.html



## 镜像加速源

| 镜像加速器          | 镜像加速器地址                       |
| ------------------- | ------------------------------------ |
| Docker 中国官方镜像 | https://registry.docker-cn.com       |
| DaoCloud 镜像站     | http://f1361db2.m.daocloud.io        |
| Azure 中国镜像      | https://dockerhub.azk8s.cn           |
| 科大镜像站          | https://docker.mirrors.ustc.edu.cn   |
| 阿里云              | https://ud6340vz.mirror.aliyuncs.com |
| 七牛云              | https://reg-mirror.qiniu.com         |
| 网易云              | https://hub-mirror.c.163.com         |
| 腾讯云              | https://mirror.ccs.tencentyun.com    |

Settings-Docker Engine

```
"registry-mirrors": [
    "https://registry.docker-cn.com",
    "https://docker.mirrors.ustc.edu.cn"
]
```



## 基础架构

**镜像(lmage):** Docker镜像 (lmage)，就相当于是一个root文件系统。比如官方镜像ubuntu16.04就包含了完整的一套Ubuntu16.04最小系统的root文件系统。

**容器 (Container):**镜像 (Image)和容器(Container)的关系，就像是面向对象程序设计中的类和对象一样，镜像是静态的定义，容器是镜像运行时的实体。容器可以被创建、启动、停止、删除、暂停等。

**仓库 (Repository):**仓库可看成一个代码控制中心用来保存镜像。



## 常用命令

```
docker volume ls 查看 volume 列表
docker network ls 查看网络列表
```

### 查看镜像列表

```
docker images 
```

```
REPOSITORY               TAG               IMAGE ID       CREATED         SIZE
redis                    6.0.9             6060df96cef3   2 years ago     104MB
```

### 搜索镜像

```
docker search redis
```

```
NAME                                DESCRIPTION                                      STARS     OFFICIAL   AUTOMATED
redis                               Redis is an open source key-value store that…   12338     [OK]
```

### 下载镜像

```
docker pull redis

指定版本:
docker pull redis:6.0.9
```

### 删除镜像

```
通过镜像id删除
docker rmi [image-id]

通过版本删除
docker rmi redis:latest

删除所有镜像
docker rmi 'docker images -q'
注：docker images -q 是查看所有镜像id 
```

### 创建容器

```
docker run
	-i 保持容器一直运行
	-t 给容器分配伪终端,容器创建后自动进入容器中，退出容器后，容器自动关闭。
	-d 后台运行创建,创建容器在后台运行，退出后，容器不会关闭。
	--name=[name] 容器名称(=也可用空格代替)
	--user=root 以root权限启动容器
	images:version 镜像和镜像的版本
	/bin/bash 进入容器的初始化指令(相当于打开shell)
```

### 退出容器

```
exit
```

### 查看容器

```
docker ps

查看所有容器(包含已关闭的)
docker ps -a

查看所有容器id
docker ps -aq

查看容器大小
docker ps -s
```

```
CONTAINER ID   IMAGE     COMMAND       CREATED          STATUS          PORTS     NAMES
42e445c41b45   centos    "/bin/bash"   16 seconds ago   Up 15 seconds             c2
```

### 进入容器

```
docker exec -it [name] /bin/bash

-u root 指定root权限
```

### 停止容器

```
docker stop [name]
```

### 启动容器

```
docker start [name]
```

### 删除容器

```
docker rm [name/id]

删除所有容器
docker rm 'docker ps -aq'
```

### 查看容器信息

```
docker inspect [container id/name] 
```



## 数据卷

- 数据卷是宿主机中的一个目录或文件
- 当容器目录和数据卷目录绑定后，对方的修改会立即同步
- 一个数据卷可以被多个容器同时挂载
- 一个容器也可以被挂载多个数据卷

**数据卷作用**

- 容器数据持久化
- 外部机器和容器间接通信
- 容器之间数据交换



### 配置数据卷

```
docker run ... -v 宿主机目录(文件):容器内目录(文件) ...

挂载单个目录示例
docker run -it --name=c1 -v D:\cq:/root/data_container centos /bin/bash

挂载多个目录示例
docker run -it --name=c1 -v D:\cq:/root/data_container -v D:\cq2:/root/data_container2 centos /bin/bash
```

注：

1.目录必须是绝对路径

2.如果目录不存在，会自动创建

3.可以挂载多个数据卷



### 数据卷容器

多容器进行数据交换

1.多个容器挂载同一个数据卷

2.数据卷容器



#### 配置数据卷容器

创建数据卷

```
docker run -it --name=[data_name] -v /volume centos
```

设置数据卷

```
docker run -it --name=[name] --volumes-from [data_name] centos /bin/bash
```



## Docker应用部署

### 部署MySQL

1.搜索mysql镜像

```
docker search mysql
```

2.拉取mysql镜像

```
docker pull mysql
```

3.创建容器，设置端口映射、目录映射

```
# 在/root目录下创建mysql目录用于存储mysql数据信息
mkdir ~/mysql
cd ~/mysql
```

```
docker run -id \
-p 3307:3306 \
--name=c_mysql \
-v $PWD/conf:/etc/mysql/conf.d \
-v $PWD/logs:/logs \
-v $PWD/data:/var/1ib/mysql \
-e MYSQL_ROOT_PASSWORD=123456 \
mysql
```

参数说明:

- **-p 3307:3306:** 将容器的 3306 端口映射到宿主机的 3307 端口。
- **-v $PWD/conf:/etc/mysql/conf.d:** 将主机当前目录下的 conf/my.cnf 挂载到容器的/etc/mysql/my.cnf。配置目录
- **-v $PWD/logs:/logs:** 将主机当前目录下的 logs 目录挂载到容器的 /logs。日志目录
- **-v $PWD/data:/var/lib/mysql:** 将主机当前目录下的data目录挂载到容器的 ar/lib/mysql。数据目录
- **-e MYSQL ROOT PASSWORD=123456:** 初始化 root 用户的密码



### 部署Tomcat

1.搜索tomcat镜像

```
docker search tomcat
```

2.拉取tomcat镜像

```
docker pu11 tomcat
```

3.创建容器，设置端口映射、目录映射

```
# 在/root目录下创建tomcat目录用于存储tomcat数据信息
mkdir ~/tomcat
cd ~/tomcat
```

```
docker run -id --name=c_tomcat \
-p 8080:8080 \
-v $PWD:/usr/local/tomcat/webapps \
tomcat
```

参数说明:
-p 8080:8080: 将容器的8080端口映射到主机的8080端口
-v $PWD:/usr/local/tomcat/webapps: 将主机中当前目录挂载到容器的webapps



### 部署Nginx

1.搜索nginx镜像

```
docker search nginx
```

2.拉取nginx镜像

```
docker pull nginx
```

3.创建容器，设置端口映射、目录映射

```
# 在/root目录下创建nginx目录用于存储nginx数据信息
mkdir ~/nginx
cd ~/nginx
mkdir conf
cd conf
# 在~/nginx/conf/下创建nginx.conf文件,粘贴下面内容
vim nginx.conf
```

配置文件(略)

```
docker run -id --name=c_nginx \
-p 80:80 \
-v $PWD/conf/nginx.conf:/etc/nginx/nginx.conf \
-v $PWD/logs:/var/log/nginx \
-v $PWD/html:/usr/share/nginx/html \
nginx
```

参数说明:

-p 80:80: 将容器的 80端口映射到宿主机的 80 端口

-v $PWD/conf/nginx.conf:/etc/nginx/nginx.conf: 将机当前目录下的/conf/nginx.conf挂载到容器的 :/etc/nginx/nginx.conf。配置目录

-v $PWD/logs:/var/log/nginx: 将主机当前目录下的 logs 目录挂载到容器的/ar/log/nginx。日志目录



## Dockerfile

### Docker镜像原理

- Docker镜像是由特殊的文件系统叠加而成
- 最底端是bootfs，并使用宿主机的bootfs
- 第二层是root文件系统rootfs,称为base image
- 然后再往上可以叠加其他的镜像文件
- 统一文件系统(Union File System)技术能够将不同的层整合成一个文件系统，为这些层提供了一个统一的视角，这样就隐藏了多层的存在，在用户的角度看来，只存在一个文件系统
- 一个镜像可以放在另一个镜像的上面。位于下面的镜像称为父镜像，最底部的镜像成为基础镜像
- 当从一个镜像启动容器时，Docker会在最顶层加载一个读写文件系统作为容器



1.Docker镜像本质是什么?

是一个分层文件系统

2.Docker中一个centos镜像为什么只有200MB，而一个centos操作系统的iso文件要几个G?

Centos的iso镜像文件包含bootfs和rootfs，而docker的centos镜像复用操作系统的bootfs，只有rootfs和其他镜像层

3.Docker中一个tomcat镜像为什么有500MB，而一个tomcat安装包只有70多MB?

由于docker中镜像是分层的，tomcat虽然只有70多MB，但他需要依赖于父镜像和基础镜像，所有整个对外暴露的tomcat镜像大小500多MB



### 镜像制作

```
容器转镜像
docker commit 容器id 镜像名称:版本号

镜像转压缩文件
docker save -o 压缩文件名称 像名称:版本号

加载本地镜像压缩文件
docker load -i 压缩文件名称
```

示例

```
docker commit b849a46e288f cq
docker save -o cq.zip cq
docker load -i cq.zip
```





### Dockerfile概念

- Dockerfile是一个文本文件

- 包含了一条条的指令
- 每一条指令构建一层，基于基础镜像，最终构建出一个新的镜像
- 对于开发人员:可以为开发团队提供一个完全一致的开发环境
- 对于测试人员:可以直接拿开发时所构建的镜像或者通过Dockerfile文件构建一个新的镜像开始工作了
- 对于运维人员:在部署时，可以实现应用的无缝移植





| 关键字      | 作用                     | 备注                                                         |
| ----------- | ------------------------ | ------------------------------------------------------------ |
| FROM        | 指定父镜像               | 指定dockerfile基于那个image构建                              |
| MAINTAINER  | 作者信息                 | 用来标明这个dockerfile谁写的                                 |
| LABEL       | 标签                     | 用来标明dockerfile的标签 可以使用Label代替Maintainer 最终都是在docker image基本信息中可以查看 |
| RUN         | 执行命令                 | 执行一段命令 默认是/bin/sh 格式: RUN command 或者 RUN ["command" , "param1","param2"] |
| CMD         | 容器启动命令             | 提供启动容器时候的默认命令 和ENTRYPOINT配合使用.格式 CMD command param1 param2 或者 CMD ["command" , "param1","param2"] |
| ENTRYPOINT  | 入口                     | 一般在制作一些执行就关闭的容器中会使用                       |
| COPY        | 复制文件                 | build的时候复制文件到image中                                 |
| ADD         | 添加文件                 | build的时候添加文件到image中 不仅仅局限于当前build上下文 可以来源于远程服务 |
| ENV         | 环境变量                 | 指定build时候的环境变量 可以在启动的容器的时候 通过-e覆盖 格式ENV name=value |
| ARG         | 构建参数                 | 构建参数 只在构建的时候使用的参数 如果有ENV 那么ENV的相同名字的值始终覆盖arg的参数 |
| VOLUME      | 定义外部可以挂载的数据卷 | 指定build的image那些目录可以启动的时候挂载到文件系统中 启动容器的时候使用 -v 绑定 格式 VOLUME ["目录"] |
| EXPOSE      | 暴露端口                 | 定义容器运行的时候监听的端口 启动容器的使用-p来绑定暴露端口 格式: EXPOSE 8080 或者 EXPOSE 8080/udp |
| WORKDIR     | 工作目录                 | 指定容器内部的工作目录 如果没有创建则自动创建 如果指定/ 使用的是绝对地址 如果不是/开头那么是在上一条workdir的路径的相对路径 |
| USER        | 指定执行用户             | 指定build或者启动的时候 用户 在RUN CMD ENTRYPONT执行的时候的用户 |
| HEALTHCHECK | 健康检查                 | 指定监测当前容器的健康监测的命令 基本上没用 因为很多时候 应用本身有健康监测机制 |
| ONBUILD     | 触发器                   | 当存在ONBUILD关键字的镜像作为基础镜像的时候 当执行FROM完成之后 会执行 ONBUILD的命令 但是不影响当前镜像 用处也不怎么大 |
| STOPSIGNAL  | 发送信号量到宿主机       | 该STOPSIGNAL指令设置将发送到容器的系统调用信号以退出。       |
| SHELL       | 指定执行脚本的shell      | 指定RUN CMD ENTRYPOINT 执行命令的时候 使用的shell            |



```
docker build  -f [path] -t [name]:[version] .
-f : 指定Dockerfile的路径和文件名
-t : 指定构建镜像的名称和标签
```

### 案例

#### 部署Springboot

配置文件

```
FROM java:8
ADD springboot-he11o-0.0.1-SNAPSHOT.jar app.jar
CMD java -jar app.jar
```

执行脚本

```
docker build  -f ./springboot_dockerfile -t app .
```



#### 自定义CentOs

```
FROM centos:7
RUN yum instal1 -y vim
WORKDIR /usr
cmd /bin/bash
```

```
docker build  -f ./centos_dockerfile -t cq_centos:1 .
```



## DockerCompose

Docker Compose是一个编排多容器分布式部署的工具，提供命令集管理容器化应用的完整开发周期，包括服务构建,启动和停止。

使用步骤:

1.利用 Dockerfile定义运行环境镜像

2.使用 docker-compose.yml定义组成应用的各服务

3.运行 docker-compose up 启动应用



### 使用Docker Compose编排nginx+springboot

1.创建docker-compose目录

```
mkdir ~/docker-compose
cd ~/docker-compose
```

2.编写 docker-compose.yml 文件

```yaml
version:'3'
services :
  nginx:
   image: nginx
   ports:
    - 80:80
   links:
    - app
   volumes :
    - ./nginx/conf.d:/etc/nginx/conf.d
  app:
    image: app
    expose:
      - "8080"
```

3.创建./nginx/conf.d目录

```
mkdir -p ./nginx/conf.d
```

4.在/nginx/conf.d目录下 编写test.conf文件

```
server {
	listen 80;
	access_log off;
	location / {
		proxy_pass http://app:8080;
	}
}
```

5.在~/docker-compose 目录下 使用docker-compose 启动容器

```
docker-compose up
```

6.测试访问





## Docker私有仓库

### 私有仓库搭建

```shell
# 1、拉取私有仓库镜像
docker pull registry
# 2、启动私有仓库容器
docker run -id --name=registry -p 5000:5000 registry
# 3、打开浏览器 输入地址http://私有仓库服务器ip:5000/v2/_catalog,看到{"repositories":[]} 表示私有仓库 搭建成功
# 4、修改daemon.json
vim /etc/docker/daemon.json
#在上述文件中添加一个key，保存退出。此步用于让 docker 信任私有仓库地址;注意将私有仓库服务器ip修改为自己私有仓库服务器真实ip
{"insecure-registries":["私有仓库服务器ip:5000"]}
# 5、重启docker 服务
systemctl restart docker
docker start registry
```

### 将镜像上传至私有仓库

```shell
# 1、标记镜像为私有仓库的镜像
docker tag centos:7 私有仓库服务器IP:5000/centos:7
# 2、上传标记的镜像
docker push 私有仓库服务器IP:5000/centos:7
```

### 私有仓库拉取镜像

```
docker pull 私有仓库服务器IP:5000/centos:7
```





## Docker私有仓库Harbor(推荐)

```
TODO
```





## Docker容器虚拟化与传统虚拟机比较

相同:

- 容器和虚拟机具有相似的资源隔离和分配优势

不同:

- 容器虚拟化的是操作系统，虚拟机虚拟化的是硬件
- 传统虚拟机可以运行不同的操作系统，容器只能运行同一类型操作系统

| 特性       | 容器               | 虚拟机     |
| ---------- | ------------------ | ---------- |
| 启动       | 秒级               | 分钟级     |
| 硬盘使用   | 一般为MB           | 一般为GB   |
| 性能       | 接近原生           | 弱于       |
| 系统支持量 | 单机支持上千个容器 | 一般几十个 |







## Docker搭建Jenkins

### Jenkins安装

```
# 拉取镜像
docker pull jenkins/jenkins:lts-jdk11

# 创建容器
docker run -d -p 8081:8080 --user=root --name jenkins_root --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home jenkins/jenkins:lts-jdk11

# 创建容器关联tomcat目录
docker run -d -p 8081:8080 --user=root --name jenkins_root --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home -v D:\docker_data\tomcat:/var/tomcat jenkins_android

说明: --user=root 创建容器后ssh都是被放在/root/.ssh下的,不指定则是放在/var/jenkins_home/.ssh下的

# 重启后直接关闭解决方式(跳过，上面已经加过了)
docker container update --restart=always 容器名字

# 访问
localhost:8081

# 初始密码文件位置
/var/jenkins_home/secrets/initialAdminPassword

# 更改镜像源(设置-系统管理-插件管理)
http://mirrors.tuna.tsinghua.edu.cn/jenkins/updates/update-center.json

安装插件
1.chinese
2.Role-based Authorization Strategy
3.Credentials Binding
4.git
5.Deploy to	container
6.Maven Integration
7.Pipeline
8.gitlab hook
9.gitlab
10.Email Extension Template
11.SonarQube Scanner
```



### 权限配置

```
1.配置策略(必须配置，否则没有规则选项)
全局安全配置-授权策略-选择Role-based Strategy-保存

2.Manage and Assign Roles 分配权限
```



### 凭证管理

```
1.进到jenkins容器中
2.生成公钥和私钥
(1)ssh-keygen -t rsa
(2)生成known_host
git ls-remote -h [sshUrl] HEAD
git ls-remote -h git@gitlab.sherc.net:chenqi/DigitalTeaching.git HEAD

3.将公钥存到gitlab
4.将私钥存到jenkins
(1)系统管理-凭据管理-全局-Add Credentials
(2)描述: gitlab-auth-ssh
(3)username: 随便写
(4)将私钥放进去

解释：
Q:gitlab中配置了自己电脑的公钥，jenkins中配置了自己电脑的私钥，那为什么还需要在jenkins服务器中在创建私钥呢？
A:实际可以不需要，主要是方便在服务器上clone代码，另外known_host文件不依赖ssh生成的公私钥,可以直接通过git ls-remote -h [sshUrl] HEAD 或者 git clone 方式生成。

Q:known_hosts 文件是什么
A:known_hosts 文件存储用户访问的主机的公钥。这是一个非常重要的文件，它通过将用户的身份保存到本地系统来确保用户连接到合法的服务器。这也有助于避免中间人攻击。
当你通过SSH连接到一个新的远程服务器时，系统会提示你是否要将远程主机添加到 known_hosts 文件：
选择 yes，服务器的连接信息会保存在你的系统中。

Q:known_hosts文件存储用户访问的主机的公钥。这个公钥和 ssh-keygen -t rsa生成的公钥是一个意思吗？
A:不完全是。known_hosts 文件存储的是远程主机的公钥，而 ssh-keygen -t rsa 命令生成的是本地用户的公钥。
```



### AndroidSdk配置

```
cd /var/jenkins_home

下载老版本的sdkmanager(此版本为老版本)
curl -o sdk.zip https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip

解压文件后目录格式需要转换下
sdk\cmdline-tools\latest\...
必须加latest

配置环境变量
cd ~/.bashrc
vim ~/.bashrc
export ANDROID_HOME="/var/jenkins_home/sdk/AndroidSdk"
export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/tools/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"

source ~/.bashrc

安装
sdkmanager "platform-tools" "platforms;android-33" "build-tools;33.0.2"
输入y


说明:
A:为什么没有下最新版本的commandlinetools
Q:最新版本需要jdk17,需要额外配置。
最新版的下载地址
https://dl.google.com/android/repository/commandlinetools-linux-10406996_latest.zip

A:为什么将sdk放在jenkins_home目录里
Q:因为jenkins_home是数据卷映射目录，方便移植

```







### 其他

#### 工具

```
apt-get update

apt-get install vim
apt-get install zip
```



#### Dockerfile(SDK外置，不推荐)

```dockerfile
# 基于已有的镜像 jenkins/jenkins 来构建新镜像
FROM jenkins/jenkins:lts-jdk11

USER root

RUN echo 'export ANDROID_HOME="/var/jenkins_home/sdk/AndroidSdk"' >> ~/.bashrc
RUN echo 'export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/tools/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"' >> ~/.bashrc
RUN /bin/bash -c "source ~/.bashrc"
```



```
docker build -t jenkins_android -f ./dockerfile . 

docker run -d -p 8081:8080 --user=root --name jenkins_android --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home jenkins_android
```



#### Dockerfile(SDK内置)

```dockerfile
# 基于已有的镜像 jenkins/jenkins 来构建新镜像
FROM jenkins/jenkins:lts-jdk11

USER root

# 设置变量
ENV SDK_TOOL_URL="https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip"
ENV ANDROID_HOME="/opt/AndroidSdk"
ENV PATH="$ANDROID_HOME/tools:$ANDROID_HOME/tools/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"


# 创建android sdk目录,并下载 sdkmanager,cmdline-tools中需要创建latest文件夹，否则执行sdkmanager会报错
RUN mkdir -p ${ANDROID_HOME} \
     && cd $ANDROID_HOME \
     && curl -o sdk.zip $SDK_TOOL_URL \
     && unzip sdk.zip \
     && rm sdk.zip \
	 && cd ${ANDROID_HOME}/cmdline-tools/ \
	 && ls \
	 && mkdir -p ${ANDROID_HOME}/cmdline-tools/latest \
	 && mv NOTICE.txt  latest/ \	 
	 && mv bin/ latest/ \	 
	 && mv source.properties latest/ \	 
	 && mv lib/ latest/ \
	 && echo yes | sdkmanager "platform-tools" "platforms;android-33" "build-tools;33.0.2"

```

```
说明:
1.
多行命令不要写多个RUN，原因是Dockerfile中每一个指令都会建立一层。多少个RUN就构建了多少层镜像，会造成镜像的臃肿、多层，不仅仅增加了构件部署的时间，还容易出错。RUN书写时的换行符是\

2.设置环境变量用ENV设置，ENV指令设置环境变量时，并不是将环境变量写入到文件中。而是将环境变量直接设置在容器的运行时环境中。
设置环境变量不要去写到bashrc里，推荐使用的是ENV

```



```
docker build -t jenkins_android -f ./dockerfile . 

docker run -d -p 8082:8080 --user=root --name jenkins_android --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home jenkins_android
```

```
执行下ssh
```



#### 镜像打包

```
docker commit [容器id] [镜像名称]

docker run -d -p 8082:8080 --user=root --name jenkins_android --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home jenkins_android

```





#### 完整启动命令

```
docker run -d -p 8081:8080 --user=root --name jenkins_android --restart=always -v D:\docker_data\jenkins_home_jdk11:/var/jenkins_home -v D:\docker_data\tomcat:/var/tomcat jenkins_android
```





## 其他问题

1.

```
Error response from daemon: Ports are not available: exposing port TCP 0.0.0.0:8081 -> 0.0.0.0:0: listen tcp 0.0.0.0:8081: bind: An attempt was made to access a socket in a way forbidden by its access permissions.
Error: failed to start containers: jenkins_android
```



```
管理员模式执行以下命令
net stop winnat
net start winnat
```







