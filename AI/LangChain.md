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
