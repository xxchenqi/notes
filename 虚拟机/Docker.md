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



## 快速安装软件

```
docker run -d -p 6379:6379 --name redis redis:latest

docker run -p 5601:5601 -p 9200:9200 -p 5044:5044 -it --name elk sebp/elk
```

安装ubuntu

```
拉取最新版的 Ubuntu 镜像
docker pull ubuntu

运行容器
docker run -itd --name ubuntu-test ubuntu

通过 exec 命令进入 ubuntu 容器
docker exec -it ubuntu-test /bin/bash
```





## 常用命令

```
docker images 查看镜像列表
docker pull [images] 拉取镜像
docker rmi [image-id] 删除指定 id 的镜像

docker run 创建容器
	-i 保持容器一直运行。通常与-t同时使用。加入it参数后，容器创建后自动进入容器中，退出容器后，容器自动关闭。
	-t 给容器分配伪终端。通常与-i同时使用。
	-d 后台运行创建。创建一个容器在后台运行，需要使用docker exec进入容器。退出后，容器不会关闭。
	--name=xx 容器名称
	images:version 镜像和镜像的版本
    /bin/bash 进入容器的初始化指令(相当于打开shell)

decoker exec 进入容器
	-it [images NAMES] /bin/bash 

docker ps 查看当前运行中的容器
	-a 查看所有(可以查看已关闭的容器)

docker stop/start [container id/name] 停止/启动指定的容器
docker rm [container id/name] 删除指定的容器
docker inspect [container id/name] 查看容器信息

docker volume ls 查看 volume 列表
docker network ls 查看网络列表

```

