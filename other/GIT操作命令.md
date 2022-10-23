# GIT操作命令

## 配置

### 添加配置

```
git config [--local | --global | --system] user.name 'Your name'
git config [--local | --global | --system] user.email 'Your email'
```

### 查看配置

```
git config --list [--local | --global | --system]
```

区别:

local：区域为本仓库 

global: 当前用户的所有仓库

system: 本系统的所有用户

local优先级大于global



## 创建版本库

### 克隆远程库

```
git clone <url>
```

### 克隆不带工作区的仓库

bare不包含工作区，作为远端，以后备份方便一点

```
git clone --bare <url> <gitname>
```

### 初始化本地仓库

```
git init
```



## 修改和提交

### add

```
git add -u：将文件的修改、文件的删除，添加到暂存区。
git add .：将文件的修改，文件的新建，添加到暂存区。
git add -A：将文件的修改，文件的删除，文件的新建，添加到暂存区。(等同于git add -all)
```

### 提交所有更新过的文件

```
git commit -m 'init'
```

### 文件重命名

```
git mv oldName newName 
```

### 修改上一次的提交

注：如果你想修改本次提交记录的日志，可以用此命令，前提是还没被推送，如果推送了就没办法用此命令修改了

不只是修改message。 比如上一次提交时有几个文件没有add以及commit，可以重新进行add之后再commit --amend提交

```
git commit --amend
```

### 修改老旧的提交

```
git rebase -i [父commit]

弹出新窗口修改，将需要修改的提交，由pick修改为r(reword),保存退出
弹出第二个窗口，修改提交信息

如果要更改的是第一次提交:
git rebase -i --root
```

### 多个commit整理成1个

```
git rebase -i [父commit]

弹出新窗口修改，将commit合并到前面的commit里去，由pick修改为s(squash),保留前面commit(pick),保存退出
pick
s
s
...

弹出第二个窗口，修改提交信息
```

### 间隔的几个commit整理成1个

操作步骤同上，只是将间隔的commit换个位置

```
注:
git rebase --abort 终止此次rebase操作
git rebase --continue 继续此次rebase操作
```







## 查看

### 查看文件状态

```
git status
```

### 查看日志

```
git log --all 查看所有分支的历史
git log --all --graph 查看图形化的 log 地址
git log --oneline 查看单行的简洁历史。
git log --oneline -n4 查看最近的四条简洁历史。
git log --oneline --all -n4 --graph 查看所有分支最近 4 条单行的图形化历史。
git log [branch] 查看指定分支
```

### 查看head指向

```
cat .git/HEAD
```

```
git symbolic-ref HEAD
```

### 查看帮助

```
git help --web log 跳转到git log 的帮助文档网页
```

### 查看图形化

```
gitk
gitk --all
```

### 查看文件

```
git cat-file -t [hash]    显示版本库对象的类型
git cat-file -s [hash]    显示版本库对象的大小
git cat-file -p [hash]    显示版本库对象的内容
```



## 比较

### 比较暂存区和HEAD

```
git diff --cached
git diff --staged
```

### 比较工作区和暂存区

```
git diff
```

指定文件名

```
git diff -- [file]
```

### 其他比较命令

```
git diff HEAD -- readme.md      # 工作区 <===> HEAD
git diff -- readme.md           # 工作区 <===> 暂存区
git diff --cached -- readme.md  # 暂存区 <===> HEAD
```

### 比较不同提交的指定文件差异

```
git diff <branch> <branch> -- <file>
git diff <commit> <commit> -- <file>
```







## 恢复和取消

### 暂存区恢复成HEAD(add后，想撤销)

```
git reset HEAD
```

此时是工作区保留暂存区的改动

```
git reset 有三个参数
--soft 这个只是把 HEAD 指向的 commit 恢复到你指定的 commit，暂存区 工作区不变
--hard 这个是 把 HEAD， 暂存区， 工作区 都修改为  你指定的 commit 的时候的文件状态
--mixed 这个是不加时候的默认参数，把 HEAD，暂存区 修改为 你指定的 commit 的时候的文件状态，工作区保持不变
```

### 取消暂存区部分文件修改

```
git reset HEAD -- <file> <file> ...
```

### 工作区恢复成暂存区(修改了工作区，恢复)

```
git checkout -- <file>
```

注:

Git 2.23之后用git switch和git restore来替代git checkout功能，git switch替换git checkout切换分支的功能，git restore替换对工作区文件进行回复的功能。

### 消除最近几次提交(commit后，想撤销)

```
git reset --hard <指定恢复到哪一个commit>
```



## 删除

### 删除操作

```
git rm <file>
```

```
git reset --hard HEAD 恢复到之前的状态
```



## stash

### stash

```
git stash
```

### 恢复stash

```
stash列表中不会被删除
git stash apply   

stash列表中会被删除
git stash pop

指定
git stash pop stash@{2}
git stash pop = git stash pop stash@{0}
 
```

### 查看stash

```
git stash list
```



## 忽略

```
.gitignore
```

tips:

如果提交commit后，想再忽略一些已经提交的文件，怎么处理?

把想忽略的文件添加到 .gitignore ；然后通过 git rm  -- cached name_of_file 的方式删除掉git仓库里面无需跟踪的文件。



## 分支

### 查看分支

```
git branch

git branch -av 列出所有的分支，包括remote的
```

### 创建分支

```
git branch <new-branch>
```

### 删除本地分支

```
git branch -d <branch>  (在删除前Git会判断在该分支上开发的功能是否被合并到其它分支，如果没有，不能删除。)
git branch -D <branch>  强制删除
```

### 切换指定分支

```
git checkout <branch>
```

### 创建并切换到新分支

```
git checkout -b <branch>
```

### 设置远程追踪分支

```
git branch -u <remote> <branch>
```



## 特性

### 相对引用^

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

### 相对引用2~

一次后退四步

```
git checkout HEAD~4
```

### 移动分支

`-f` 选项让分支指向另一个提交。

```
git branch -f main HEAD~3
```

### 撤销

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



### 复制提交

将一些提交复制到当前所在的位置（`HEAD`）下面

```
git cherry-pick <提交号>...
```



### 交互式rebase

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

### 分离头指针

作用：如果临时想基于某个commit做变更，试试新方案是否可行，就可以采用分离头指针的方式。测试后发现新方案不成熟，直接reset回其他分支即可。省却了建、删分支的麻烦。

注：这种情况下比较危险，因为这个时候你提交的代码没有和分支对应起来，当切换到其他分支的时候(比如master分支)，容易丢失代码；

```
git checkout [commitId]
```

如果你觉得这个方案报错，请将头指针绑定新分支

```
git branch <new-branch-name> [commitId]
```

### 



## 远程

### 查看clone地址

```
git remote -v
```

### 移除指定地址

```
git remote rm <remote> <url>
```

### 添加远程地址

```
git remote add <remote> <url>

remote 缺省是 origin
```

### 从远程库下载代码Fetch

注:fetch不会拉到工作区的，只是把本地的远端分支和服务端对应的远端分支保持一致。

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

### 从远程库下载代码Pull

```
git pull
```

相当于fetch + merge

### fast-forwards是什么意思

```
本地分支往远端分支做push，如果远端分支不是本地分支的祖先，那它俩就不是 fast forward 了。反之，它俩就是fast forward的关系。

所以合并需要添加 --allow-unrelated-histories解决
```

### 推送

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

### 删除远程分支

```
git push origin :<place>
```







## 合并

### 合并操作

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

### 合并不相干的2个分支

```
git merge --allow-unrelated-histories <branch>
```

### 下载代码并合并

注：此命令相当于 fetch + merge

```
git pull <remote> <branch>
```

注：此命令相当于 fetch + rebase

```
git pull --rebase <remote> <branch>
```

### 不同人修改不同的文件

```
git fetch <remote>
git merge <branch>
	弹出框说明已经自动merge没问题
git push <remote>
```

### 不同人修改同文件的不同区域

```
同上
```

### 不同人修改同文件的同区域

```
git pull <remote>
提示自动merge失败
解决冲突
git commit / 如果不想要此次merge，执行 git merge --abort，回到merge之前的状态
git push <remote>
```

### 同一文件改成了不同的文件名

```
git pull <remote>
提示自动merge失败
git status
	both deleted: file
	add by us: newFile1
	add by them: newFile2
解决冲突,git add/rm
	git rm fle
	git add newFile1
	git rm newFile2
git commit   因为还处于merging阶段
git push <remote>
```





## Tag

### 查看所有tag

```
git tag
```

### 查看tag文字说明

```
git show <tag_name>
```

### 创建tag

```
git tag <tag> <提交记录>
```

```
git tag -a <tagname> -m "commit message" <提交记录>
```

### 推送tag

```
git push <remote> <tag>
```

### 推送所有tag

```
git push --tags
```

### 删除本地tag

```
git tag -d <tag>
```

### 删除远程tag

```
git push origin :refs/tags/标签名  
```





## 其他

### .git目录

```
HEAD：指向当前的工作路径
config：存放本地仓库（local）相关的配置信息。
refs/heads:存放分支
refs/tags:存放tag，又叫里程牌 （当这次commit是具有里程碑意义的 比如项目1.0的时候   就可以打tag）
objects：存放对象  .git/objects/ 文件夹中的子文件夹都是以哈希值的前两位字符命名  每个object由40位字符组成，前两位字符用来当文件夹，后38位做文件。
```



### commit、tree、blob

```
每次的提交，都是一个commit，一个commit又是包含了一棵tree，每个tree里面又是包含了多棵tree和blob，而文件的的最终形式是blob。
对于blob，git会认为文件内容相同，就使用同一个blob，这样就极大的避免了频繁的提交时，git的存储空间的急剧上升。
```



### 遗留1

```
作者回复: C1 <-- C2 <-- C3 <-- C4 , Cn全部是commit，且C1是C2的父亲，依此类推。

你的问题是说“如果C3有问题，你想撤销C3的修改，为此生成C5，历史树变成C1 <-- C2 <-- C3 <-- C4 <-- C5”，对吗？如果是这样，执行 git revert C3 ，就会生成C5，且C5就是清除C3的变更 。

如果你无须保留 C3和C4，只想让分支回到C2，那么执行 git reset --hard C2 。⚠️我用了--hard，它会把工作区和暂存区都回退到C2，如果你想保留工作区，请不要使用 --hard ；如果甚至想保留暂存区的变更，那么必须加上 --soft C2。

有空请理解reset带了不同参数的三个操作的区别：
git reset --hard C2
git reset C2
git reset --soft C2

C2代表某个commit
```





### 回滚提交

--hard 硬重置，本地的一起恢复，暂存区也没有了

--mixed 默认，暂存区还是会有未提交的更改

```
git reset --hard HEAD^
```

-force 强制推送，如果不加，服务器会拒绝此次推送

```
git push -f origin <branch>
```



### git使用原则

```
1：push前一定先pull
2：合并代码必须两人结对
3：合并冲突，非自己的变动保持原样，和自己冲突的代码找相应的代码提交人确认如何解决冲突
4：合并完成后，保证本地能编译能运行再push
5：合并到主干的代码必须通过测试，必须通过代码review
6：不同的功能从主干上拉新分支进行开发工作
7：分支的命名需要加上，拉取人＋拉取说明
8：上完线的分支要及时清理
```



## 禁忌

### push -f 

```
在主干分支上禁止使用git push -f
因为可能会抹掉其他的提交节点，假如有些东西提交了想回滚，那就修改后再提交
```

### 禁止向集成分支执行变更历史的操作

```
在主干分支上禁止使用变更提交历史的命令
因为会抹掉一些历史提交信息，我们貌似一直使用rebase，主要是不喜欢混乱的提交线，不过也没出现什么问题，合代码出问题主要集中在合并冲突时，有些代码调整的存在问题。

自己的分支随意，自己可以完全掌控，自己完全负责。公共分支是公地，不要做出影响他人的行为。
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
参考网页
https://blog.51cto.com/u_15057807/4535658

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





