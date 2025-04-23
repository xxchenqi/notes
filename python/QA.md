# QA

###  `__init__.py` 文件说明

| 作用                         | 描述                                                    |
| ---------------------------- | ------------------------------------------------------- |
| ✅ 标记目录为包               | 这是它的“历史使命”，虽然在 Python 3.3+ 中已不再强制要求 |
| ✅ 编写包的初始化逻辑         | 运行一些设置代码、初始化变量等                          |
| ✅ 对外统一接口、隐藏内部结构 | 配合 `from .xxx import` 和 `__all__` 使用               |
| ✅ 提高可维护性和清晰度       | 把对外接口集中在一处管理，而不是散落在多个模块里        |



### `__all__` 变量说明

| 功能                                  | 描述                                |
| ------------------------------------- | ----------------------------------- |
| ✅ 控制 `from xxx import *` 的导入行为 | 只导入 `__all__` 列表中列出的名称   |
| ✅ 明确模块/包的“对外接口”             | 告诉使用者你希望他们使用哪些内容    |
| ✅ 隐藏内部工具函数/变量               | 不写在 `__all__` 里的就不会被“公开” |



### `__name__` 变量说明

`__name__` 是 Python 的一个内置变量，用于表示当前模块的名字。

| `__name__` 的值 | 场景                  |
| --------------- | --------------------- |
| `"__main__"`    | 文件被直接运行        |
| `"模块名"`      | 文件被其他文件 import |

🤔 为什么需要它？

原因很简单：

有些文件既可以作为**脚本直接执行**，又可以被**别的模块导入使用**，你不希望导入时它立刻执行某些操作。

```python
# tool.py
def add(a, b):
    return a + b

if __name__ == "__main__":
    print("测试 add 函数:", add(2, 3))
```





### pip freeze

`pip freeze` 是一个超实用的命令，用于**列出当前 Python 环境中已安装的所有包及其精确版本号**，格式适合用于项目依赖管理。

```
# 把当前环境依赖保存下来：
pip freeze > requirements.txt
```

缺点：

`pip freeze` 会输出你环境中**所有安装的包**，包括你没直接装的“子依赖”



更推荐的方案

pipreqs

`pipreqs` 是一个超实用的 Python 工具，它可以根据你**项目中实际导入的包（import）**，自动生成一个 **精简版的 `requirements.txt` 文件**，避免像 `pip freeze` 那样把整个环境都拷进去。

https://pypi.org/project/pipreqs/

```
pipreqs --ignore venv --force
```



### RESTful 

URL 只表示“资源”，不包含动词：

❌ 不推荐：

```
/getUserInfo
/updateUser
/deletePost
```

✅ 推荐 RESTful：

```
GET     /users/1
PUT     /users/1
DELETE  /posts/10
```

> 🚩 也就是说：**动作由 HTTP 方法决定，URL 只负责“指向谁”**



🔧 一个典型的 RESTful API 示例：

比如管理用户系统：

| 请求方法 | URL         | 含义                 |
| -------- | ----------- | -------------------- |
| `GET`    | `/users`    | 获取所有用户         |
| `GET`    | `/users/42` | 获取 id 为 42 的用户 |
| `POST`   | `/users`    | 新增用户             |
| `PUT`    | `/users/42` | 更新用户 42 的信息   |
| `DELETE` | `/users/42` | 删除用户 42          |





### ORM

**ORM** 全称：**Object-Relational Mapping**（对象关系映射）

| 数据库术语      | ORM 映射          |
| --------------- | ----------------- |
| 表（Table）     | 类（Class）       |
| 行（Row）       | 对象（Object）    |
| 列（Column）    | 属性（Attribute） |
| 增删查改（SQL） | 方法调用（CRUD）  |



### yield、@contextmanager、with

🎯 `@contextmanager` + `yield` 是干嘛的？

它的作用是：

> 自定义一个“**上下文管理器**”（可以被 `with` 使用），用来管理**资源的获取与释放**，比如打开文件、连接数据库、加锁、时间控制等。

✅ 基本语法结构：

```python
from contextlib import contextmanager

@contextmanager
def my_context():
    print("资源获取（进入）")
    yield "我是中间过程值"
    print("资源释放（退出）")
```

使用方式：

```python
with my_context() as value:
    print("处理中...", value)

# 输出顺序：
# 资源获取（进入）
# 处理中... 我是中间过程值
# 资源释放（退出）
```

🧠 解释：

- `yield` 之前：相当于 `__enter__` 方法
- `yield` 之后：相当于 `__exit__` 方法
- `yield` 的值：传给 `as` 后的变量



`with` 是什么？

> `with` 是 Python 中的一个 **上下文管理语法糖**，用于简洁、优雅、安全地管理资源。

```python
with open("file.txt") as f:
    data = f.read()
# 文件自动关闭
```

相当于：

```python
f = open("file.txt")
try:
    data = f.read()
finally:
    f.close()  # 手动释放资源
```

`with` 是让你写出更安全、干净的资源管理代码的关键工具，自动释放、避免忘记关闭，是 Python 风格的最佳实践之一 