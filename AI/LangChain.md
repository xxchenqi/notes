# LangChain

## 概念

`LangChain` 是一个开源的 **大语言模型应用开发框架**，专门用来帮助开发者更方便地构建基于 **LLM（如 GPT、Claude、Gemini 等）** 的复杂应用程序。



## 核心功能模块

| 模块名称                                                  | 作用                                                         |
| --------------------------------------------------------- | ------------------------------------------------------------ |
| 📦 **Prompt 模板**                                         | 统一管理提示词（Prompt），支持参数化、拼接等操作             |
| 🔗 **链（Chain）**                                         | 将多个 LLM 调用、函数、工具串联成一个逻辑流程                |
| 📚 **文档加载与处理（Document Loaders / Text Splitters）** | 加载 PDF、网页、数据库等数据源，并切分为小块                 |
| 🧠 **向量数据库（Vector Stores）**                         | 把文档嵌入成向量用于搜索（如 FAISS、Pinecone、Milvus）       |
| 🕵️‍♂️ **Retriever（检索器）**                                | 提供智能文档查找（用于 RAG）                                 |
| 🤖 **Agent（智能体）**                                     | 能做判断、调用工具、完成任务（例：让 AI 扮演“助手 + 工具人”） |
| 🛠️ **工具集成**                                            | 可集成搜索、计算器、数据库查询等工具供 Agent 调用            |
| 🌐 **多种模型支持**                                        | 不仅支持 OpenAI，还支持 HuggingFace、Anthropic、Azure、Claude 等等 |



LangChain 框架本身由多个开源库组成（v0.2.1版本）：

- langchain-core：基础抽象和 LangChain 表达式语言。
-  langchain-community：第三方集成以及合作伙伴包（如 langchain-openai、langchain-anthropic 等），一些集成已经进一步拆分为自己的轻量级包，只依赖于 langchain-core。
- langchain：构建应用程序认知架构的链、代理和检索策略。
- langgraph：通过将步骤构建为图中的边和节点，使用 LLMs 构建健壮且有状态的多参与者应用程序。
- langserve：将 LangChain 链部署为 REST API。
- langsmith：一个开发平台，可以让你调试、测试、评估和监控 LLM 应用程序，并与 LangChain 无缝衔接



LangChain 文档重构与解读

https://python.langchain.com/docs/introduction/

Diátaxis，这是一种全新的文档组织哲学，旨在将内容划分为四个清晰的类别：**教程、操作指南、参考与解释**。

Diátaxis 官网：https://diataxis.fr/





## Prompt

大多数 LLM 应用程序都不会直接将用户输入传递给 LLM。通常，它们会将用户输入添加到一个更大的文本片段中，称为**提示模板**,该模板提供有关特定任务的附加上下文。

同一个 Prompt 可以支持各种 LLM，在切换 LLM 的时候，无需修改 Prompt。

在 LangChain 中，Prompt 被分成了两大类：

- Prompt Template：将 Prompt 按照 template 进行一定格式化，针对 Prompt 进行变量处理以及提示词的组合。
- Selectors：根据不同条件去选择不同提示词，或者在不同情况下通过 Selector，选择不同示例去进一步提高 Prompt 支持能力。

本质上 Selectors 只是 Prompt Template 的二次封装。





## Model

Model 是 LangChain 的核心组件，但是 LangChain 本身不提供自己的 LLM，而是提供了一个标准接口，用于封装不同类型的 LLM 进行交互，其中 LangChain 为两种类型的模型提供接口和集成：

- LLM：使用纯文本作为输入和输出的大语言模型。
- Chat Model：使用聊天消息列表作为输入并返回聊天消息的聊天模型。



Model 的意义是什么？

LangChain 的 `Model` 模块把底层的 API 差异、请求格式、token 管理等**统一封装**了，让你：

- ✅ 快速切换模型（只需改类名）
- ✅ 支持统一链式调用
- ✅ 和 Prompt、Memory、Tools 等模块无缝集成



## OutputParser

在 **LangChain** 中，`OutputParser` 是一个 **用于解析大模型输出的模块**，它能把 LLM 返回的“自由文本”转为你想要的 **结构化数据**，比如字典、列表、JSON、对象、数值等。



## LCEL表达式

LangChain Expression Language

例子

```python
from langchain_core.prompts import ChatPromptTemplate
from langchain_openai import ChatOpenAI
from langchain_core.output_parsers import StrOutputParser

prompt = ChatPromptTemplate.from_template("你是谁？用 {style} 风格回答")
model = ChatOpenAI()
parser = StrOutputParser()

chain = prompt | model | parser  # LCEL 表达式
```

📌 总结：

- **LCEL 是 LangChain 的表达式语言**
- 通过 `|` 运算符组合组件，类似 Unix 管道
- 能让链式开发更清晰、简洁、声明式
- 背后依赖了 `Runnable` 协议，每个模块都实现了 `invoke()` 等方法

解决的问题——**在构建多步骤 LLM 应用时的代码混乱、结构不清、调试困难等一系列“组合地狱”问题。**

  

## Runnable

`RunnableParallel` 是什么？

✅ 它的作用：

**并行执行多个链**，将同一个输入送入多个子链，最后输出一个字典，包含每个子链的结果。

✅ 用途：

当你需要**多个模型或任务并行执行**时使用，比如：

- 同时翻译、总结、提问
- 多种 LLM 回答进行对比
- 多工具并发调用



`RunnablePassthrough` 是什么？

✅ 它的作用：

**原样返回输入值，不做任何处理。**

✅ 用途：

- 占位符（链还没写完，先打个桩）
- 在调试阶段保留原始输入
- 组合链时对部分字段不处理
- 来简化 invoke 的调用流程



| 项目     | `RunnableParallel`       | `RunnablePassthrough`    |
| -------- | ------------------------ | ------------------------ |
| 功能     | 并行执行多个链           | 什么都不做，直接返回输入 |
| 输入     | 一个输入                 | 任意输入                 |
| 输出     | 字典，包含每条子链输出   | 与输入相同               |
| 应用场景 | 多任务、对比、多模态处理 | 占位、调试、保留输入     |





### lambda、itemgetter解释

```python
chain = {
            "context": lambda x: retrieval(x["query"]),
            "query": itemgetter("query"),
        } | prompt | llm | parser
```

```python
lambda x: retrieval(x["query"]) 解析

# 假设你调用：
chain.invoke({"query": "你好，我是谁？"})
#那传入的 x 就是：{"query": "你好，我是谁？"}
lambda x: retrieval(x["query"]) 就等于 retrieval("你好，我是谁？")
```

```python
itemgetter("query") 解析
itemgetter是来自 operator 模块的一个工具函数：
itemgetter("query") 会返回一个函数，效果和这个一样： lambda x: x["query"]

也就是从输入字典中取出 "query" 字段的值。
```

```python
你调用 chain.invoke({"query": "你好"})，LangChain 会把这个输入映射为：
{
    "context": retrieval("你好"), 
    "query": "你好"
}
```





### RunnablePassthrough解析

```python
RunnablePassthrough.assign(
    context=lambda x: retrieval(x["query"])
)
为什么能替换
{
    "context": lambda x: retrieval(x["query"]),
    "query": itemgetter("query"),
}


输入：{"query": "你好"}
输出：保留原输入，加一个新字段

```

```python
chain = {"query": RunnablePassthrough()} | prompt | llm | StrOutputParser()

"query" 字段的内容会被原样传递下去，没有任何改变。
```



### RunnableLambda

把普通函数变成 LCEL 链条组件

```python
RunnableLambda(memory.load_memory_variables) | itemgetter("history")

# 你不能直接写：
chain = memory.load_memory_variables | itemgetter("history")
# 因为 load_memory_variables 是一个普通函数，不支持 | 运算符（LCEL 语法）。
```





## Callback

Callback 是 LangChain 提供的回调机制，允许我们在 LLM 应用程序的各个阶段使用 hook。
