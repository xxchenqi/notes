# GIT操作命令

### 创建版本库

#### 克隆远程库

```
git clone <url>
```

#### 初始化本地仓库

```
git init
```



### 修改和提交

#### 查看状态

```
git status
```

#### 跟踪所有改动过的文件

```
git add .
```

#### 提交所有更新过的文件

```
git commit -m "init"
```

#### 修改最后一次提交的记录

注：如果你想修改本次提交记录的日志，可以用此命令，前提是还没被推送，如果推送了就没办法用此命令修改了

```
git commit --amend
```





### 查看

#### 查看文件状态

```
git status
```

#### 查看日志

```
git log
```

#### 查看head指向

```
cat .git/HEAD
```

```
git symbolic-ref HEAD
```





### 分支

#### 查看分支

```
git branch
```

#### 创建分支

```
git branch <new-branch>
```

#### 删除本地分支

```
git branch -d <branch>
```

#### 切换指定分支

```
git checkout <branch>
```

#### 创建并切换到新分支

```
git checkout -b <branch>
```

#### 设置远程追踪分支

```
git branch -u <remote> <branch>
```



### 特性

#### 相对引用^

移动到main的父节点

```
git checkout main^
```

移动到main的第二个父节点

```
git checkout main^^
```

你也可以将 `HEAD` 作为相对引用的参照。

```
git checkout HEAD^
```

#### 相对引用2~

一次后退四步

```
git checkout HEAD~4
```

#### 移动分支

`-f` 选项让分支指向另一个提交。

```
git branch -f main HEAD~3
```

#### 撤销

```
git reset HEAD~1
```

在reset后， 所做的变更还在，但是处于未加入暂存区状态

```
git revert HEAD
```

在我们要撤销的提交记录后面会多一个新提交，新提交记录引入了**更改**，这些更改刚好是用来撤销这个提交的



git reset “改写历史”的方法对大家一起使用的远程分支是无效的

revert 之后就可以把你的更改推送到远程仓库与别人分享啦



#### 复制提交

将一些提交复制到当前所在的位置（`HEAD`）下面

```
git cherry-pick <提交号>...
```



#### 交互式rebase

--interactive

```
git rebase -i HEAD~4
```

```
参数:
pick：正常选中
reword：选中，并且修改提交信息；
edit：选中，rebase时会暂停，允许你修改这个commit（参考这里）
squash：选中，会将当前commit与上一个commit合并
fixup：与squash相同，但不会保存当前commit的提交信息
exec：执行其他shell命令
```







### 远程

#### 查看clone地址

```
git remote -v
```

#### 移除指定地址

```
 git remote rm origin <url>
```

#### 添加远程地址

```
git remote add origin <url>
```



#### 从远程库下载代码

```
git fetch <remote> <branch>
```

```
git fetch <remote> <source>:<destination>
```

`source` 指的是远程仓库中的位置，而 `<destination>` 才是要放置提交的本地仓库的位置。它与 git push 刚好相反

如果目标分支不存在在会创建分支

```
git fetch 
```

如果参数都不指定，它会下载所有的提交记录到各个远程分支……



#### 合并

注：会多生成1个合并操作的提交记录

```
git merge <branch>
```

注：不会额外生成合并的操作记录，使得时间线会干净。

```
git rebase <branch>
```

优点:

- Rebase 使你的提交树变得很干净, 所有的提交都在一条线上

缺点:

- Rebase 修改了提交树的历史

怎么选？

一些开发人员喜欢保留提交历史，因此更偏爱 merge。而其他人（比如我自己）可能更喜欢干净的提交树，于是偏爱 rebase。仁者见仁，智者见智。



#### 下载代码并合并

注：此命令相当于 fetch + merge

```
git pull <remote> <branch>
```

注：此命令相当于 fetch + rebase

```
git pull --rebase <remote> <branch>
```



#### 推送

```
git push <remote> <place>
```

git push origin main

切到本地仓库中的“main”分支，获取所有的提交，再到远程仓库“origin”中找到“main”分支，将远程仓库中没有的提交记录都添加上去，搞定之后告诉我。

place参数来告诉 Git 提交记录来自于 main, 要推送到远程仓库中的 main。它实际就是要同步的两个仓库的位置。



**如果来源和去向分支的名称不同使用此命令**

```
git push origin <source>:<destination>
```

`source` 指的是本地仓库中的位置，而 `destination` 是远程仓库的位置。它与 git fetch相反



#### 删除远程分支

```
git push origin :<place>
```



#### 回滚提交

--hard 硬重置，本地的一起恢复，暂存区也没有了

--mixed 默认，暂存区还是会有未提交的更改

```
git reset --hard HEAD^
```



-force 强制推送，如果不加，服务器会拒绝此次推送

```
git push -f origin <branch>
```





#### Tag

查看所有tag

```
git tag
```

查看tag文字说明

```
git show <tag_name>
```

创建tag

```
git tag <tag> <提交记录>
```

```
git tag -a <tagname> -m "commit message" <提交记录>
```

推送tag

```
git push <remote> <tag>
```

推送所有tag

```
git push --tags
```







## 常见问题：

### 1.每次push需要输入密码

**原因**：在clone 项目的时候，使用了 https方式，而不是ssh方式。

修改：

查看clone地址

```
git remote -v
```

移除指定地址

```
git remote rm origin <url>
```

添加远程地址

```
git remote add origin <url>
```



推送(可能会出现key不匹配问题)

```
git push origin master
```



### 2.Permission denied (publickey).

Permission denied (publickey) 没有权限的publickey ，出现这错误一般是以下两种原因

- 客户端与服务端未生成 ssh key
- 客户端与服务端的ssh key不匹配

找到问题的原因了，解决办法也就有了，重新生成一次ssh key ，服务端也重新配置一次即可。



**客户端生成ssh key:**

```
ssh-keygen -t rsa -C "812046652@qq.com"
```

途中会让你输入密码,直接回车就行



```
ssh-agent -s 
ssh-add ~/.ssh/id_rsa
```

出现Could not open a connection to your authentication agent. 这个问题执行以下2句命令

```
ssh-agent bash 
ssh-add ~/.ssh/id_rsa
```



配置服务端：

打开你刚刚生成的id_rsa.pub，将里面的内容复制，进入你的github账号，在settings下，SSH and GPG keys下new SSH key，然后将id_rsa.pub里的内容复制到Key中，完成后Add SSH Key。



验证Key

```
ssh -T git@github.com 
```



### 3.解决冲突

当提交到本地，在拉去远程代码后，如有冲突。

此时你的分支树为：no branch, rebasing <branch>

你需要解决冲突后，再次执行

```
git add .
git commit -m "此处填写的信息和之前一样，或者你可以随意修改"
git rebase --continue
```

 rebase结束，分支树又回到原来的分支。

此时就可以push了。

```
git push origin <branch>
```





