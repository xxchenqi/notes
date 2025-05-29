# Runnable组件

## bind

bind() **函数用于修改 Runnable 底层的默认调用参数**，并在调用时会自动传递该参数，无需手动传递，像原始链一样正常调用即可。所以如果在构建 Runnable 链应用时就知道对应的参数，可以使用 bind 函数来绑定参数（事先指定）。

```python
prompt = ChatPromptTemplate.from_messages([
    ("human", "{query}")
])
llm = ChatOpenAI(model="gpt-3.5-turbo")

chain = prompt | llm.bind(model="gpt-4o") | StrOutputParser()

```

**解决多参 RunnableLambda 函数传参**

在 LangChain 中，如果要将一个函数变成 Runnable 组件，可以通过 RunnableLambda 函数进行包装。但是封装后，所有的 Runnable 组件的 invoke 函数，调用时，只能传递一个参数（类型不限制），如果原本的函数支持多个参数，并且是必填参数，就会出现报错。

```python
get_weather_runnable = RunnableLambda(get_weather).bind(unit="摄氏度", name="慕小课")
```



**原理**

bind() 函数的底层，本质上是往 Runnable 的 kwargs 属性添加对应的字段，并生成一个新的 Runnable，当 Runnable 组件执行调用时，会自动将 kwargs 字段里的所有参数合并并覆盖默认调用参数。从而完成动态添加默认调用参数的效果。





## configurable_fields

和 bind() 方法接近，但是并不是在构建时传递对应的参数，而是在链运行时为链中的给定步骤指定参数，比 bind() 更灵活。

```python
# 动态调整 temperature
llm = ChatOpenAI(model="gpt-3.5-turbo-16k").configurable_fields(
    temperature=ConfigurableField(
        id="llm_temperature"
    )
)

chain.with_config(configurable={"llm_temperature": 0})
```



## configurable_alternatives

在链运行时，动态替换掉特定的模型、提示词等整个组件本身，而不是替换组件里的参数信息。

```python
prompt = ChatPromptTemplate.from_template("{query}")
llm = ChatOpenAI(model="gpt-3.5-turbo-16k").configurable_alternatives(
    ConfigurableField(id="llm"),
    default_key="gpt-3.5",
    gpt4=ChatOpenAI(model="gpt-4o"),
    wenxin=QianfanChatEndpoint(),
)

chain = prompt | llm | StrOutputParser()

content = chain.invoke(
    {"query": "你好，你是什么模型呢?"},
    config={"configurable": {"llm": "gpt4"}}
)
```

**原理**

底层会通过一个字典 alternatives 存储所有替换组件，并且从传递的 configurable 字典中获取当前需要选择的组件 key，根据对应的 key 在 alternatives 中找到对应的组件返回并执行后续的操作。



## 重试机制

在 LangChain 中，针对 Runnable 抛出的异常提供了重试机制——with_retry()，当 Runnable 组件出现异常时，支持针对特定的异常或所有异常，重试特定的次数，并且配置每次重试时间的时间进行指数增加。

```python
chain = RunnableLambda(func).with_retry(stop_after_attempt=2)
```



## 回退机制

在某些场合中，对于 Runnable 组件的出错，并不想执行重试方案，而是执行特定的备份/回退方案，例如 OpenAI 的 LLM 大模型出现异常时，自动切换到 文心一言 的模型上，在 LangChain 中也提供了对应的回退机制—— with_fallbacks

```python
llm = ChatOpenAI(model="gpt-3.5-turbo-18k").with_fallbacks([QianfanChatEndpoint()])
```



## 生命周期监听器

在 LangChain 中，除了在链执行时传递 config+callbacks 来对链进行监听，Runnable 还提供了一个简约的方法 with_listeners 来监听

```python
def on_start(run_obj: Run, config: RunnableConfig) -> None:
    print("on_start")
def on_end(run_obj: Run, config: RunnableConfig) -> None:
    print("on_end")
def on_error(run_obj: Run, config: RunnableConfig) -> None:
    print("on_error")

runnable = RunnableLambda(lambda x: time.sleep(x)).with_listeners(
    on_start=on_start,
    on_end=on_end,
    on_error=on_error
)
```



