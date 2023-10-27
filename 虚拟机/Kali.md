# Kali

## 用户密码

```
在虚拟机中启动kali，初始账号：kali，密码：kali。
切换root账号，切换命令：sudo su，输入密码：kali。

修改root密码
sudo passwd root
```

## 环境搭建

```
查看进程
apt install htop
实时查看当前网卡流量及ip
apt install jnettop
```



## 代理配置

```
proxychains

修改配置文件
vim /etc/proxychains4.conf
添加自己的IP和端口
socks5 10.128.167.171 7890


验证OSI 7层模型

网络层 ping ip
传输层 telnet ip port
会话层 通过浏览器配置代理测试
```

