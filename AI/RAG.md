# RAG

## 幻觉

### 幻觉的定义及出现的原因

大语言模型在处理自然语言时，有时会出现幻觉，表现为回答不准确或前后不一致的问题。



**幻觉分为两类：**

**事实性幻觉**：指模型生成的内容与可验证的现实事实不一致。而事实性幻觉又分为事实不一致（与现实世界信息相矛盾）和事实捏造（压根没有，无法根据现实信息验证）。

**忠实性幻觉**：指模型生成的内容与用户的指令或上下文不一致。忠实性幻觉可以细分为指令不一致（输出偏离用户指令）、上下文不一致（输出与上下文信息不符）、逻辑不一致（推理步骤以及与最终答案之间不一致）三类。



**产生幻觉的原因:**

**数据源导致的幻觉**

包括数据缺陷和数据中捕获的事实知识的利用率低。

具体来说，数据缺陷分为错误信息和偏见（重复偏见、社会偏见），此外大模型也存在知识边界，所以存在领域知识缺陷和过时的事实知识。即便大模型吃掉了大量的数据，也会在利用时出现问题。

除此之外，大模型可能会过度依赖训练数据中的一些模式，如位置接近性、共现统计数据和相关文档计数，从而导致幻觉，比如：如果训练数据中频繁出现“加拿大”和“多伦多”，那么大模型可能会错误地将多伦多识别为加拿大的首都。



**训练过程导致的幻觉**

在模型的**预训练阶段**（大模型学习通用表示并获取世界知识）、**对齐阶段**（微调大模型使其更好地与人类偏好一致）两个阶段产生的问题也会导致幻觉的发生。

预训练阶段可能会存在：

* **架构缺陷**：基于前一个 token 预测下一个 token，这种单向建模阻碍了模型捕获复杂的上下文关系的能力；自注意力模块存在缺陷，随着 token 长度增加，不同位置的注意力被稀释。

* **暴露偏差**：训练策略也有缺陷，模型推理时依赖于自己生成的 token 进行后续预测，模型生成的错误 token 会在整个后续 token 中产生级联错误。

对齐阶段可能会存在：

- **能力错位**：大模型内在能力与标注数据中描述的功能之间可能存在错位。当对齐数据需求超出这些预定义的能力边界时，大模型会被训练来生成超出其自身知识边界的内容，从而放大幻觉的风险。
- **信念错位**：基于 RLHF 等的微调，使大模型的输出更符合人类偏好，但有时模型会倾向于迎合人类偏好，从而牺牲信息真实性。



**推理导致的幻觉**

1.固有的抽样随机性：在生成内容时根据概率随机生成。

2.不完美的解码表示：上下文关注不足（过度关注相邻文本而忽视了源上下文）和 softmax 瓶颈（输出概率分布的表达能力受限）。





### 大模型幻觉的评估

事实性幻觉，有**检索外部事实**和**不确定性估计**两种方法。

**检索外部事实**是将模型生成的内容与可靠的知识来源进行比较，例如同一个问题利用大语言模型生成内容，同时和外部检索到真实的信息进行对比校验

**不确定性估计**的幻觉检测方法有两类：基于**内部状态**的方法和基于**行为**的方法。

1.基于内部状态的方法主要依赖于大模型的内部状态。例如，通过考虑关键概念的最小标记概率来确定模型的不确定性。

2.基于行为的方法主要依赖观察大模型的行为，不需要访问其内部状态。例如，通过采样多个响应并评估事实陈述的一致性来检测幻觉



而检测 忠实性幻觉 也有 5 种主流方法：

1. **基于事实的度量**：测量生成内容和源内容之间事实的重叠程度来评估忠实性。
2. **分类器度量**：使用训练过的分类器来区分模型生成的忠实内容和幻觉内容。
3. **问答度量**：使用问答系统来验证源内容和生成内容之间的信息一致性。
4. **不确定度估计**：测量模型对其生成输出的置信度来评估忠实性。
5. **提示度量**：让大模型作为评估者，通过特定的提示策略来评估生成内容的忠实性



### 大语言模型幻觉的缓解方案

幻觉和创造/创新/涌现其实只有一线之隔，大模型如果没有幻觉，那就永远无法产生新内容。所以，从涌现/创新的角度来说，大模型的幻觉永远不会被解决，在某些场合下只可能被缓解。

大语言模型幻觉的缓解方案也有**数据、预训练、对齐、推理**相关的方案，与 LLM 应用开发相关的主要在数据方面的处理。



**缓解数据相关幻觉**

减少错误信息和偏见，最直观的方法是收集高质量的事实数据，并进行数据清理以消除偏见。对于大语言模型知识边界的问题，有两种流行方法。一种是**知识编辑**，直接编辑模型参数弥合知识差距。另一种通过**检索增强生成（RAG）**利用非参数知识源。

检索增强生成具体分为三种类型：**一次性检索**、**迭代检索**和**事后检索**。

**一次性检索**是将从单次检索中获得的外部知识直接预置到大模型的提示中；

**迭代检索**允许在整个生成过程中不断收集知识；

**事后检索**是基于检索的修订来完善大模型输出。



**缓解预训练相关幻觉**

**改进模型架构**：使用双向自回归模型和注意力锐化技术，增强模型对上下文的理解。

**优化训练目标**：通过引入事实性增强的训练方法和上下文预训练，提升模型的事实关联和逻辑一致性。

**减少曝光偏差**：采用新的监督信号和解码策略，减少训练与推理过程中的幻觉。



**缓解对齐相关幻觉**

**减少能力错位**：通过改进人类偏好判断，确保模型生成内容在其知识范围内。

**减少信念错位**：聚合人类偏好和调整模型内部激活，减少模型迎合行为，避免生成与模型自身认知相悖的内容。



**缓解推理相关幻觉**

**增强事实性解码**：动态调整解码策略，利用模型内部结构引导事实性回答。

**增强忠实度解码**：通过上下文和逻辑一致性策略，确保模型输出与用户指令或上下文保持一致

 



## RAG概念

检索增强生成（RAG）是指对大型语言模型输出进行优化，使其能够在生成响应之前引用训练数据来源之外的权威知识库。

简单理解：**RAG 就是从外部先检索对应的知识内容，和用户的提问一起构成 Prompt，再让 LLM 生成内容**。



**RAG的重要性及优点**

单纯利用 LLM 进行开发的缺陷：

1. LLM 的训练数据是静态的，这意味着 LLM 掌握的知识是有时间限制的，对于新知识不了解。
2. 当用户需要特定或者即时的数据时，LLM 往往提供通用或者过时的数据。
3. LLM 回答的内容可能是从非权威来源创建响应。
4. 由于术语混淆，不同的培训来源使用相同的术语来谈论不同的事情，因此会产生不确定的响应。



RAG 带来的好处：

1. **经济高效**：预训练和微调模型的成本很高，相比之下，RAG 是一种经济高效将新输入引入 LLM 的方案。
2. **信息即时**：使用 RAG 可以为 LLM 提供最新的研究、统计数据或新闻，确保数据的即时性。
3. **增强用户信任度**：RAG 允许 LLM 通过来源归属来呈现准确的信息。输出可以包括对来源的引文或引用。如果需要进一步说明或更详细的信息，用户也可以自己查找源文档。这可以增加对您的生成式人工智能解决方案的信任和信心。
4. **开发人员拥有更多控制权**：借助 RAG，开发人员可以更高效地测试和改进他们的聊天应用程序。他们可以控制和更改 LLM 的信息来源，以适应不断变化的需求或跨职能使用。开发人员还可以将敏感信息的检索限制在不同的授权级别内，并确保 LLM 生成适当的响应。此外，如果 LLM 针对特定问题引用了错误的信息来源，他们还可以进行故障排除并进行修复。组织可以更自信地为更广泛的应用程序实施生成式人工智能技术。



## 向量数据库

向量数据库就是一种专门用于存储和处理向量数据的数据库系统，传统的关系型数据库通常不擅长处理向量数据，因为它们需要将数据映射为结构化的表格形式，而**向量数据的维度较高、结构复杂，导致传统数据库存储和查询效率低下**，所以向量数据库应运而生。



**两种数据库的差异**

**搜索**

向量数据库通过相似性搜索的方法来工作。它们可以快速找到与查询向量最相似的数据点，即使这些数据点在数值上并不完全相同。

**数据处理与存储差异**

向量数据库将数据以列形式存储，即每个列都有一个独立的存储空间，这使得向量数据库可以更加灵活地处理复杂的数据结构。向量数据库还可以进行列压缩（**稀疏矩阵**），以减少存储空间和提高数据的访问速度。

并且在向量数据库中，将数据表示为**高维向量**，其中**每个向量对应于数据点**。**这些向量之间的距离表示它们之间的相似性**。这种方式使得非结构化或半结构化数据的存储和检索变得更加高效。



**相似性搜索算法**

**余弦相似度与欧式距离**

在向量数据库中，支持通过多种方式来计算两个向量的相似度，例如：**余弦相似度、欧式距离、曼哈顿距离、闵可夫斯基距离、汉明距离、Jaccard相似度**等多种。其中最常见的就是 余弦相似度 和 欧式距离。

余弦相似度主要用于衡量向量在方向上的相似性，特别适用于文本、图像和高维空间中的向量。它不受向量长度的影响，只考虑方向的相似程度。

欧式距离衡量向量之间的直线距离，得到的值可能很大，最小为 0，通常用于低维空间或需要考虑向量各个维度之间差异的情况。欧氏距离较小的向量被认为更相似。



**相似性搜索加速计算**

在向量数据库中，数据按列进行存储，通常会将多个向量组织成一个 M×N 的矩阵，其中 M 是向量的维度（特征数），N 是向量的数量（数据库中的条目数），这个矩阵可以是稠密或者稀疏的，取决于向量的稀疏性和具体的存储优化策略。

这样计算相似性搜索时，本质上就变成了向量与 M×N 矩阵的每一行进行相似度计算，这里可以用到大量成熟的加速算法：

**1.** **矩阵分解方法**：

**·** **SVD（奇异值分解）**：可以通过奇异值分解将原始矩阵转换为更低秩的矩阵表示，从而减少计算量。

**·** **PCA（主成分分析）**：类似地，可以通过主成分分析将高维矩阵映射到低维空间，减少计算复杂度。

**2.** **索引结构和近似算法**：

**·** **LSH（局部敏感哈希）**：LSH 可以在近似相似度匹配中加速计算，特别适用于高维稀疏向量的情况。

**·** **ANN（近似最近邻）算法**：ANN 算法如KD-Tree、Ball-Tree等可以用来加速对最近邻搜索的计算，虽然主要用于向量空间，但也可以部分应用于相似度计算中。

**3.** **GPU 加速**：使用图形处理单元（GPU）进行并行计算可以显著提高相似度计算的速度，尤其是对于大规模数据和高维度向量。

**4.** **分布式计算**：由于行与行之间独立，所以可以很便捷地支持分布式计算每行与向量的相似度，从而加速整体计算过程。





## Embedding

Embedding 模型是一种在机器学习和自然语言处理中广泛应用的技术，它旨在将高纬度的数据（如文字、图片、视频）映射到低纬度的空间。Embedding 向量是一个 N 维的实值向量，它将输入的数据表示成一个连续的数值空间中的点。

而且通过学习，**Embedding 向量可以更准确地表示对应特征的内在含义，使几何距离相近的向量对应的物体有相近的含义**，甚至对向量进行加减乘除算法都有意义！

一句话理解 Embedding：**一种模型生成方法，可以将非结构化的数据，例如文本/图片/视频等数据映射成有意义的向量数据**。

目前生成 embedding 方法的模型有以下 4 类：

**1.** **Word2Vec（词嵌入模型）**：这个模型通过学习将单词转化为连续的向量表示，以便计算机更好地理解和处理文本。Word2Vec 模型基于两种主要算法 CBOW 和 Skip-gram。

**2.** **Glove**：一种用于自然语言处理的词嵌入模型，它与其他常见的词嵌入模型（如 Word2Vec 和 FastText）类似，可以将单词转化为连续的向量表示。GloVe 模型的原理是通过观察单词在语料库中的共现关系，学习得到单词之间的语义关系。具体来说，GloVe 模型将共现概率矩阵表示为两个词向量之间的点积和偏差的关系，然后通过迭代优化来训练得到最佳的词向量表示。

**GloVe** 模型的优点是它能够在大规模语料库上进行有损压缩，得到较小维度的词向量，同时保持了单词之间的语义关系。这些词向量可以被用于多种自然语言处理任务，如词义相似度计算、情感分析、文本分类等。

**3.** **FastText**：一种基于词袋模型的词嵌入技术，与其他常见的词嵌入模型（如 Word2Vec 和 GloVe）不同之处在于，FastText考虑了单词的子词信息。其核心思想是将单词视为字符的 n-grams 的集合，在训练过程中，模型会同时学习单词级别和n-gram级别的表示。这样可以捕捉到单词内部的细粒度信息，从而更好地处理各种形态和变体的单词。

**4.** **大模型 Embeddings（重点）**：和大模型相关的嵌入模型，如 OpenAI 官方发布的第二代模型：text-embedding-ada-002。它最长的输入是 8191 个tokens，输出的维度是 1536。





**Embedding的价值**

**1.** **降维**：在许多实际问题中，原始数据的维度往往非常高。例如，在自然语言处理中，如果使用 Token 词表编码来表示词汇，其维度等于词汇表的大小，可能达到数十万甚至更高。通过 Embedding，我们可以将这些高维数据映射到一个低维空间，大大减少了模型的复杂度。

**2.** **捕捉语义信息**：Embedding 不仅仅是降维，更重要的是，它能够捕捉到数据的语义信息。例如，在词嵌入中，语义上相近的词在向量空间中也会相近。这意味着Embedding可以保留并利用原始数据的一些重要信息。

**3.** **适应性**： 与一些传统的特征提取方法相比，Embedding 是通过数据驱动的方式学习的。这意味着它能够自动适应数据的特性，而无需人工设计特征。

**4.** **泛化能力**：在实际问题中，我们经常需要处理一些在训练数据中没有出现过的数据。由于Embedding能够捕捉到数据的一些内在规律，因此对于这些未见过的数据，Embedding仍然能够给出合理的表示。

**5.** **可解释性**：尽管 Embedding 是高维的，但我们可以通过一些可视化工具（如t-SNE）来观察和理解 Embedding 的结构。这对于理解模型的行为，以及发现数据的一些潜在规律是非常有用的。





**Embedding 组件使用**

在 LangChain 中，Embeddings 类是一个专为与文本嵌入模型交互而设计的类，这个类为许多嵌入模型提供商（如 OpenAI、Cohere、Hugging Face 等）提供一个标准的接口。

LangChain 中 Embeddings 类提供了两种方法：

1. embed_documents：用于嵌入文档列表，传入一个文档列表，得到这个文档列表对应的向量列表。

2. embed_query：用于嵌入单个查询，传入一个字符串，得到这个字符串对应的向量。

并且 Embeddings 类并不是一个 Runnable 组件，所以并不能直接接入到 Runnable 序列链中，需要额外的 RunnableLambda 函数进行转换，核心基类代码也非常简单



```python
def cosine_similarity(vec1: list, vec2: list) -> float:
    """计算传入两个向量的余弦相似度"""
    # 1.计算两个向量的点积
    dot_product = np.dot(vec1, vec2)

    # 2.计算向量的长度
    vec1_norm = norm(vec1)
    vec2_norm = norm(vec2)

    # 3.计算余弦相似度
    return dot_product / (vec1_norm * vec2_norm)


# 创建文本嵌入模型
embeddings = OpenAIEmbeddings(model="text-embedding-3-small")

# 嵌入文档列表/字符串列表
documents_vector = embeddings.embed_documents([
    "我叫小奇，我喜欢打篮球",
    "这个喜欢打篮球的人叫小奇",
    "求知若渴，虚心若愚"
])
print(len(documents_vector))

# 计算余弦相似度
print("向量1和向量2的相似度:", cosine_similarity(documents_vector[0], documents_vector[1]))
print("向量1和向量3的相似度:", cosine_similarity(documents_vector[0], documents_vector[2]))
```



## CacheBackEmbedding

通过嵌入模型计算传递数据的向量需要昂贵的算力，对于重复的内容，Embeddings 计算的结果肯定是一致的，如果数据重复仍然二次计算，会导致效率非常低，而且增加无用功。

所以在 LangChain 中提供了一个叫 CacheBackEmbedding 的包装类，来解决此问题。



注：CacheBackEmbedding 默认不缓存 embed_query 生成的向量，如果要缓存，需要设置 query_embedding_cache 的值，另外请尽可能设置 namespace，以避免使用不同嵌入模型嵌入的相同文本发生冲突。

```python
embeddings = OpenAIEmbeddings(model="text-embedding-3-small")
embeddings_with_cache = CacheBackedEmbeddings.from_bytes_store(
    embeddings,
    LocalFileStore("./cache/"),
    namespace=embeddings.model,
    query_embedding_cache=True,
)
```



**运行流程**

本质上就是**封装了一个持久化存储的数据存储仓库**，在每次进行数据嵌入前，会从前数据存储仓库中检索对对应的向量，然后逐个匹配对应的数据是否相等，找到缓存中没有的文本，然后将这些文本调用嵌入生成向量，最后将生成的新向量存储到数据仓库中，从而完成对数据的存储。



## Hugging Face

**本地模型**

在某些对数据保密要求极高的场合下，数据不允许传递到外网，这个时候就可以考虑使用本地的文本嵌入模型

langchain-huggingface 是 langchain 团队基于 huggingface 封装的第三方社区包，

sentence-transformers 是一个用于生成和使用预训练的文本嵌入，基于 transformer 架构，也是目前使用量最大的本地文本嵌入模型。

```python
embeddings = HuggingFaceEmbeddings(
    model_name="sentence-transformers/all-MiniLM-L12-v2",
    cache_folder="./embeddings/"
)
query_vector = embeddings.embed_query("你好，我是小奇，我喜欢打篮球游泳")
```



**远程嵌入模型**

部分模型的文件比较大，如果只是短期内调试，可以考虑使用 HuggingFace 提供的远程嵌入模型



在 Hugging Face 官网（https://huggingface.co/）的 setting 中添加对应的访问秘钥，并配置到 .env 文件中

HUGGINGFACEHUB_API_TOKEN=xxx



```python
embeddings = HuggingFaceEndpointEmbeddings(model="sentence-transformers/all-MiniLM-L12-v2")
query_vector = embeddings.embed_query("你好，我是小奇，我喜欢打篮球游泳")
```



## 向量数据库

Faiss

Pinecone 

TCVectorDB

Weaviate

自定义





## Document

Document 类是 LangChain 中的核心组件，**也是文档加载器、文档分割器、向量数据库、检索器这几个组件之间交互传递的状态数据**。

Document = page_content(页面内容) + metadata(元数据)



在 LangChain 中所有文档加载器的基类为 BaseLoader，封装了统一的 5 个方法：

1. load()/aload()：加载和异步加载文档，返回的数据为文档列表。

2. load_and_split()：传递分割器，加载并将大文档按照传入的分割器进行切割，返回的数据为分割后的文档列表。

3. lazy_load()/alazy_load()：懒加载和异步懒加载文档，返回的是一个迭代器，适用于传递的数据源有多份文档的情况，例如文件夹加载器，可以每次获得最新的加载文档，不需要等到所有文档都加载完毕。



### TextLoader

加载一个文本文件（源码、markdown、text 等存储成文本结构的文件，DOC 并不是文本文件），并把整个文件的内容读入到一个 Document 对象中，同时为文档对象的 metadata 添加 source 字段用于记录源数据的来源信息。

```python
# 1.构建加载器
loader = TextLoader("./test.txt", encoding="utf-8")

# 2.加载数据
documents = loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```



### Markdown 加载器

```python
loader = UnstructuredMarkdownLoader("./test.md")
documents = loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```

### Office 文档加载器

```python
ppt_loader = UnstructuredPowerPointLoader("./test.pptx")
documents = ppt_loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```

### URL 网页加载器

```python
loader = WebBaseLoader("https://imooc.com")
documents = loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```

### 通用文件加载器

对于一些无法判断的数据类型或者想进行通用性文件加载，可以统一使用非结构化文件加载器**UnstructuredFileLoader**来实现对文件的加载。

UnstructuredFileLoader 是所有 UnstructuredXxxLoader 文档类的基类，其核心是将文档划分为元素，当传递一个文件时，库将读取文档，将其分割为多个部分，对这些部分进行分类，然后提取每个部分的文本，然后根据模式决定是否合并。

```python
loader = UnstructuredFileLoader("./test.md")
documents = loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```



### 自定义加载器

在 LangChain 中实现自定义文档加载器只需要继承 BaseLoader 基类，然后实现 lazy_load() 方法即可，如果该文档加载器有异步使用的场景，还需要实现 alazy_load() 方法。

```python
class CustomDocumentLoader(BaseLoader):
    """自定义文档加载器，将文本文件的每一行都解析成Document"""

    def __init__(self, file_path: str) -> None:
        self.file_path = file_path

    def lazy_load(self) -> Iterator[Document]:
        # 1.读取对应的文件
        with open(self.file_path, encoding="utf-8") as f:
            line_number = 0
            # 2.提取文件的每一行
            for line in f:
                # 3.将每一行生成一个Document实例并通过yield返回
                yield Document(
                    page_content=line,
                    metadata={"score": self.file_path, "line_number": line_number}
                )
                line_number += 1

    async def alazy_load(self) -> AsyncIterator[Document]:
        import aiofiles
        async with aiofiles.open(self.file_path, encoding="utf-8") as f:
            line_number = 0
            async for line in f:
                yield Document(
                    page_content=line,
                    metadata={"score": self.file_path, "line_number": line_number}
                )
                line_number += 1


loader = CustomDocumentLoader("./test.txt")
documents = loader.load()

print(documents)
print(len(documents))
print(documents[0].metadata)
```



### Blob

`Blob` 是 LangChain 中用于表示**原始文件或数据块**的一个抽象类型。

传统的数据加载器如 `TextLoader`, `PyPDFLoader`, `CSVLoader` 等，都要求你**提前知道数据格式**，否则你得写多个分支处理各种格式。

Blob 方案通过**抽象数据为统一的 Blob 对象**，再结合“Loader + Parser”机制，实现高度解耦和通用处理。



**Blob  解析器**

```python
class CustomParser(BaseBlobParser):
    """自定义解析器，用于将传入的文本二进制数据的每一行解析成Document组件"""


    def lazy_parse(self, blob: Blob) -> Iterator[Document]:
        line_number = 0
        with blob.as_bytes_io() as f:
            for line in f:
                yield Document(
                    page_content=line,
                    metadata={"source": blob.source, "line_number": line_number}
                )
                line_number += 1


# 1.加载blob数据
blob = Blob.from_path("./test.txt")
parser = CustomParser()

# 2.解析得到文档数据
documents = list(parser.lazy_parse(blob))

# 3.输出相应的信息
print(documents)
print(len(documents))
print(documents[0].metadata)
```



### 字符分割器

```python
# 1.加载对应的文档
loader = UnstructuredMarkdownLoader("./test.md")
documents = loader.load()

# 2.创建文本分割器
text_splitter = CharacterTextSplitter(
    separator="\n\n",
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

# 3.分割文本
chunks = text_splitter.split_documents(documents)

for chunk in chunks:
    print(f"块大小:{len(chunk.page_content)}, 元数据:{chunk.metadata}")

print(len(chunks))
```

使用 CharacterTextSplitter 进行分割时，虽然传递了 chunk_size 为 500，但是仍然没法确保分割出来的文档一直保持在这个范围内，这是因为在底层 CharacterTextSplitter 是先按照分割符号拆分整个文档，然后循环遍历拆分得到的列表，将每个列表逐个相加，直到最接近 chunk_size 窗口大小时则完成一个 Document 的组装。

但是如果基于分割符号得到的文本，本身长度已经超过了 chunk_size，则会直接进行警告，并且将对应的文本单独变成一个块。



### 递归字符文本分割器

RecursiveCharacterTextSplitter 分割器可以传递 一组分隔符 和 设定块内容大小，根据分隔符的优先顺序对文本进行预分割，然后将小块进行合并，将大块进行递归分割，直到获得所需块的大小，最终这些文档块的大小并不能完全相同，但是仍然会逼近指定长度。

RecursiveCharacterTextSplitter 的分隔符参数默认为 ["\n\n", "\n", " ", ""]，即优先使用换两行的数据进行分割，然后在使用单个换行符，如果块内容还是太大，则使用空格，最后再拆分成单个字符。

所以如果使用默认参数，这个字符文本分割器最后得到的文档块长度一定不会超过预设的大小，但是仍然会有小概率出现远小于的情况（目前也没有很好的解决方案）。

对比普通的字符文本分割器， 递归字符文本分割器可以传递多个分隔符，并且根据不同分隔符的优先级来执行相应的分割。



```python
loader = UnstructuredMarkdownLoader("./test.md")
documents = loader.load()

text_splitter = RecursiveCharacterTextSplitter(
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

chunks = text_splitter.split_documents(documents)

for chunk in chunks:
    print(f"块大小: {len(chunk.page_content)}, 元数据: {chunk.metadata}")
```



**代码分割器**

```python
loader = UnstructuredFileLoader("./test.py")
documents = loader.load()

text_splitter = RecursiveCharacterTextSplitter.from_language(
    language=Language.PYTHON,
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

chunks = text_splitter.split_documents(documents)

for chunk in chunks:
    print(f"块大小: {len(chunk.page_content)}, 元数据: {chunk.metadata}")
```



**中文场景下的递归分割**

RecursiveCharacterTextSplitter 默认配置的分隔符均是英文场合下的，在中文场合下，除了换行/空格，一般还有更加复杂的语句结束判断标识，例如：。、！、？等标识符，如果想更好去切割 中英文文档，可以考虑重设分隔符列表（或者继承该类进行重写）。

```python
# 1.创建加载器和文本分割器
loader = UnstructuredMarkdownLoader("./test.md")
separators = [
    "\n\n",
    "\n",
    "。|！|？",
    "\.\s|\!\s|\?\s",  # 英文标点符号后面通常需要加空格
    "；|;\s",
    "，|,\s",
    " ",
    ""
]
text_splitter = RecursiveCharacterTextSplitter(
    separators=separators,
    is_separator_regex=True,
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

# 2.加载文档与分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

for chunk in chunks:
    print(f"块大小: {len(chunk.page_content)}, 元数据: {chunk.metadata}")
```



### 语义分割器使

```python
# 1.构建加载器和文本分割器
loader = UnstructuredFileLoader("./test.txt")
text_splitter = SemanticChunker(
    embeddings=OpenAIEmbeddings(model="text-embedding-3-small"),
    number_of_chunks=10,
    add_start_index=True,
    sentence_split_regex=r"(?<=[。？！.?!])"
)

# 2.加载文本与分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

# 3.循环打印
for chunk in chunks:
    print(f"块大小: {len(chunk.page_content)}, 元数据: {chunk.metadata}")

```



### 其他文档分割器

**HTMLHeaderTextSplitter**

在 HTML 文档中按照元素级别进行分割，查找出每一块文本的内容与其所有关联的标题，并为每个相关的标题块提供元数据（顺序往上逐层查找，直到找到所有嵌套层级的标题）。

**HTMLSectionSplitter**

HTMLSectionSplitter：在 HTML 文档中按照元素级别进行分割，查找出每一块文本的内容及其副标题（顺序往上查找，找到最近的副标题则停止）。



**递归 JSON 分割器**

RecursiveJsonSplitter分割器会按照深度优先的方式遍历 JSON 数据，并构建较小的 JSON 块，而且尽可能保持嵌套 JSON 对象完整，但如果需要保持文档块大小在最小块大小和最大块大小之间，则会将它们拆分。

```python
text_splitter = RecursiveJsonSplitter(max_chunk_size=300)

json_chunks = text_splitter.split_json(json_data)
chunks = text_splitter.create_documents(json_chunks)

count = 0
for chunk in chunks:
    count += len(chunk.page_content)

print(count)
```



**基于标记的分割器**

```python
def calculate_token_count(query: str) -> int:
    """计算传入文本的token数"""
    encoding = tiktoken.encoding_for_model("text-embedding-3-large")
    return len(encoding.encode(query))


# 1.定义加载器和文本分割器
loader = UnstructuredFileLoader("./test.txt")
text_splitter = RecursiveCharacterTextSplitter(
    separators=[
        "\n\n",
        "\n",
        "。|！|？",
        "\.\s|\!\s|\?\s",  # 英文标点符号后面通常需要加空格
        "；|;\s",
        "，|,\s",
        " ",
        ""
    ],
    is_separator_regex=True,
    chunk_size=500,
    chunk_overlap=50,
    length_function=calculate_token_count,
)

# 2.加载文档并执行分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

# 3.循环打印分块内容
for chunk in chunks:
    print(f"块大小: {len(chunk.page_content)}, 元数据: {chunk.metadata}")

```



### 自定义文档分割器

```python
class CustomTextSplitter(TextSplitter):
    """自定义文本分割器"""

    def __init__(self, seperator: str, top_k: int = 10, **kwargs):
        """构造函数，传递分割器还有需要提取的关键词数，默认为10"""
        super().__init__(**kwargs)
        self._seperator = seperator
        self._top_k = top_k

    def split_text(self, text: str) -> List[str]:
        """传递对应的文本执行分割并提取分割数据的关键词，组成文档列表返回"""
        # 1.根据传递的分隔符分割传入的文本
        split_texts = text.split(self._seperator)

        # 2.提取分割出来的每一段文本的关键词，数量为self._top_k个
        text_keywords = []
        for split_text in split_texts:
            text_keywords.append(jieba.analyse.extract_tags(split_text, self._top_k))

        # 3.将关键词使用逗号进行拼接组成字符串列表并返回
        return [",".join(keywords) for keywords in text_keywords]


# 1.创建加载器与分割器
loader = UnstructuredFileLoader("./test.txt")
text_splitter = CustomTextSplitter("\n\n", 10)

# 2.加载文档并分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

# 3.循环遍历文档信息
for chunk in chunks:
    print(chunk.page_content)
```



### 总结

目前 RAG 分块 Chunk 的 4 种策略：

1. 固定大小分块：这是最常见的分块方法，通过设定块的大小和是否有重叠来决定分块。这种方法简单直接，不需要使用任何NLP库，因此计算成本低且易于使用，例如 CharacterTextSplitter，亦或者直接循环遍历固定大小拆分。

2. 基于结构的分块：常见的 HTML、MARKDOWN 格式，或者其他可以有明确结构格式的文档。这种可以借助“结构感知”对文档分块，充分利用文档文本以外的信息，类似 LangChain 中的 HTMLHeaderTextSplitter 等。

3. 基于语义的分块：这种策略旨在确保每个分块包含尽可能多的语义独立信息。可以采用不同的方法，如标点符号、自然段落、或者NLTK、Spicy 等工具包来实现语义分块，或者 Embedding-based 方法，例如 LangChain 中的 SemanticChunker 等。

4. 递归分块：递归分块使用一组分隔符，以分层和迭代的方式将输入文本划分为更小的块。如果最初分割文本没有产生所需大小或结构的块，则该方法会继续递归地分割直到满足条件，例如 LangChain 中的 RecursiveCharacterTextSplitter 等。

这些策略各有优势和适用场景，选择合适的分块策略取决于具体的应用需求和数据特性。



> 注:
>
> 对于一个 RAG 场景，分成四个主要阶段：预检索、检索、后检索 和 生成，其中 分块 是 预检索 阶段的策略，如果在 分块 阶段尝试了上述 4 种策略均没有很好的效果，或许就不应该采用 RAG 的策略，而是使用 微调 的方式，让这部分知识成为模型永久的记忆，效果可能会更好





### 转换器

**问答转换器**

```python
documents = [Document(page_content=page_content)]

qa_transformer = DoctranQATransformer(openai_api_model="gpt-3.5-turbo-16k")
transformer_documents = qa_transformer.transform_documents(documents)

for qa in transformer_documents[0].metadata.get("questions_and_answers"):
    print("问答数据:", qa)
```



**翻译转换器**

```python
documents = [Document(page_content=page_content)]

text_translator = DoctranTextTranslator(openai_api_model="gpt-3.5-turbo-16k")
translator_documents = text_translator.transform_documents(documents)

print(translator_documents[0].page_content)
```





### VectorStore

作用：封装了数据库的操作方式公共特征，方法划分成 6 种：相似性搜索、最大边际相关性搜索、通用搜索、添加删除精确查找数据、检索器、创建数据库

**带得分阈值的相似性搜索**

```python
embedding = OpenAIEmbeddings(model="text-embedding-3-small")

documents = [
    Document(page_content="test1", metadata={"page": 1}),
    Document(page_content="test2", metadata={"page": 2}),
]
db = FAISS.from_documents(documents, embedding)

print(db.similarity_search_with_relevance_scores("test", score_threshold=0.4))
```



**as_retriever() 检索器**

```python
# 1.构建加载器与分割器
loader = UnstructuredMarkdownLoader("./test.md")
text_splitter = RecursiveCharacterTextSplitter(
    separators=["\n\n", "\n", "。|！|？", "\.\s|\!\s|\?\s", "；|;\s", "，|,\s", " ", "", ],
    is_separator_regex=True,
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

# 2.加载文档并分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

# 3.将数据存储到向量数据库
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="url",
        auth_credentials=AuthApiKey("apikey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
db.add_documents(chunks)

# 4.转换检索器（带阈值的相似性搜索，数据为10条，得分阈值为0.5）
retriever = db.as_retriever(
    search_type="similarity_score_threshold",
    search_kwargs={"k": 10, "score_threshold": 0.5},
)

# 5.检索结果
documents = retriever.invoke("test")

print(list(document.page_content[:50] for document in documents))
print(len(documents))
```





**MMR 最大边际相关性**

最大边际相关性（MMR，max_marginal_relevance_search）的基本思想是同时考量查询与文档的 相关度，以及文档之间的 相似度。

MMR 就是在一大堆最相似的文档中查找最不相似的，从而保证 结果多样化。

所以 MMR 在保证查询准确的同时，尽可能提供 多样化结果，以增加信息检索的有效性和多样性



```python
loader = UnstructuredMarkdownLoader("./test.md")
text_splitter = RecursiveCharacterTextSplitter(
    separators=["\n\n", "\n", "。|！|？", "\.\s|\!\s|\?\s", "；|;\s", "，|,\s", " ", "", ],
    is_separator_regex=True,
    chunk_size=500,
    chunk_overlap=50,
    add_start_index=True,
)

# 2.加载文档并分割
documents = loader.load()
chunks = text_splitter.split_documents(documents)

# 3.将数据存储到向量数据库
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("apikey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)

# 4.执行最大边际相关性搜索
# search_documents = db.similarity_search("test")
search_documents = db.max_marginal_relevance_search("test")

# 5.打印搜索的结果
for document in search_documents:
    print(document.page_content[:100])
    print("===========")
```

 80% 的场合使用相似性搜索都可以得到不错的效果，对于一些追求创新/创意/多样性的 RAG 场景，可以考虑使用 最大边际相关性搜索。

并且在执行 MMR 搜索时，如果向量数据库的规模越大，一般 fetch_k 设置的值越大，在 k 的大概 2~3 倍左右，如果添加了 filter 对数据进行筛选，则可以考虑在将 fetch_k 扩大到 k 的 4~6 倍。

在使用 相似性搜索 时，尽可能使用 similarity_search_with_relevance_scores() 方法并传递阈值信息，确保在向量数据库数据较少的情况下，不将一些不相关的数据也检索出来，并且着重调试 得分阈值(score_threshold)，对于不同的文档/分割策略/向量数据库，得分阈值并不一致，需要经过调试才能得到一个相对比较正确的值（阈值过大检索不到内容，阈值过小容易检索到不相关内容）。



**VectorStoreRetriever 检索器**

```python
# 1.构建向量数据库
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("apikey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)

# 2.转换检索器
retriever = db.as_retriever(
    search_type="similarity_score_threshold",
    search_kwargs={"k": 10, "score_threshold": 0.5},
).configurable_fields(
    search_type=ConfigurableField(id="db_search_type"),
    search_kwargs=ConfigurableField(id="db_search_kwargs"),
)

# 3.修改运行时配置执行MMR搜索，并返回4条数据
mmr_documents = retriever.with_config(
    configurable={
        "db_search_type": "mmr",
        "db_search_kwargs": {
            "k": 4,
        }
    }
).invoke("test")
print("相似性搜索: ", mmr_documents)
print("内容长度:", len(mmr_documents))

print(mmr_documents[0].page_content[:20])
print(mmr_documents[1].page_content[:20])

```





## RAG优化策略

### 优化策略

RAG 开发 6 个阶段：查询转换、路由、查询构建、索引、检索 和 生成。

目前市面上常见的优化方案有：问题转换、多路召回、混合检索、搜索重排、动态路由、图查询、问题重建、自检索 等数十种优化策略



在 RAG 应用开发中，使用的优化策略越多，单次响应成本越高，性能越差，需要合理使用。



### Multi-Query 多查询策略

多查询策略 也被称为 子查询，是一种用于生成子问题的技术，其核心思想是在问答过程中，为了更好地理解和回答主问题，系统会自动生成并提出与主问题相关的子问题，这些子问题通常具有更具体的细节，可以帮助大语言模型更深入地理解主问题，从而进行更加准确的检索并提供正确的答案。

多查询策略 会从多个角度重写用户问题，为每个重写的问题执行检索，然后将检索到的文档列表进行合并后去重，返回唯一文档。

```python
# 1.构建向量数据库与检索器
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("AuthApiKey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
retriever = db.as_retriever(search_type="mmr")

# 2.创建多查询检索器
multi_query_retriever = MultiQueryRetriever.from_llm(
    retriever=retriever,
    llm=ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0),
    include_original=True,
)

# 3.执行检索
docs = multi_query_retriever.invoke("test")
print(docs)
print(len(docs))
```



### 多查询结果融合策略及 RRF

RAG融合主要思想是在 Multi-Query 的基础上，对其检索结果进行重新排序（即 reranking）后输出 Top K 个结果，最后再将这 Top K 个结果喂给 LLM 并生成最终答案。

在 RAG融合 中，对文档列表进行排序&去重合并的算法为 RRF(Reciprocal Rank Fusion)，即倒排序排名算法。

```python

class RAGFusionRetriever(MultiQueryRetriever):
    """RAG多查询结果融合策略检索器"""
    k: int = 4

    def retrieve_documents(
            self, queries: List[str], run_manager: CallbackManagerForRetrieverRun
    ) -> List[List]:
        """重写检索文档函数，返回值变成一个嵌套的列表"""
        documents = []
        for query in queries:
            docs = self.retriever.invoke(
                query, config={"callbacks": run_manager.get_child()}
            )
            documents.append(docs)
        return documents

    def unique_union(self, documents: List[List]) -> List[Document]:
        """使用RRF算法来去重合并对应的文档，参数为嵌套列表，返回值为文档列表"""
        # 1.定义一个变量存储每个文档的得分信息
        fused_result = {}

        # 2.循环两层获取每一个文档信息
        for docs in documents:
            for rank, doc in enumerate(docs):
                # 3.使用dumps函数将类示例转换成字符串
                doc_str = dumps(doc)
                # 4.判断下该文档的字符串是否已经计算过得分
                if doc_str not in fused_result:
                    fused_result[doc_str] = 0
                # 5.计算新的分
                fused_result[doc_str] += 1 / (rank + 60)

        # 6.执行排序操作，获取相应的数据，使用的是降序
        reranked_results = [
            (loads(doc), score)
            for doc, score in sorted(fused_result.items(), key=lambda x: x[1], reverse=True)
        ]

        return [item[0] for item in reranked_results[:self.k]]


# 1.构建向量数据库与检索器
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("auth_credentials"),
    ),
    index_name="DatasetDemo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
retriever = db.as_retriever(search_type="mmr")

rag_fusion_retriever = RAGFusionRetriever.from_llm(
    retriever=retriever,
    llm=ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0),
)

# 3.执行检索
docs = rag_fusion_retriever.invoke("test")
print(docs)
print(len(docs))

```





### 复杂问题检索

对于复杂问题分解策略 可以划分成两种方案：`迭代式回答` 与 `并行式回答`

**迭代式回答**

迭代式回答，会将上一次的 提问+答案，还有这一次的 检索上下文 一起传递给 LLM，让其生成答案，迭代到最后一次，就是最终答案。而 并行式回答 则会同时检索，并同时调用 LLM 生成答案，最后在将答案进行汇总，让 LLM 整理生成最终答案。

```python
def format_qa_pair(question: str, answer: str) -> str:
    """格式化传递的问题+答案为单个字符串"""
    return f"Question: {question}\nAnswer: {answer}\n\n".strip()

# 1.定义分解子问题的prompt
decomposition_prompt = ChatPromptTemplate.from_template(
    "你是一个乐于助人的AI助理，可以针对一个输入问题生成多个相关的子问题。\n"
    "目标是将输入问题分解成一组可以独立回答的子问题或者子任务。\n"
    "生成与一下问题相关的多个搜索查询：{question}\n"
    "并使用换行符进行分割，输出（3个子问题/子查询）："
)

# 2.构建分解问题链
decomposition_chain = (
        {"question": RunnablePassthrough()}
        | decomposition_prompt
        | ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
        | StrOutputParser()
        | (lambda x: x.strip().split("\n"))
)

# 3.构建向量数据库与检索器
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("AuthApiKey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
retriever = db.as_retriever(search_type="mmr")

# 4.执行提问获取子问题
question = "LLM文档有哪些"
sub_questions = decomposition_chain.invoke(question)

# 5.构建迭代问答链：提示模板+链
prompt = ChatPromptTemplate.from_template("""这是你需要回答的问题：
---
{question}
---

这是所有可用的背景问题和答案对：
---
{qa_pairs}
---

这是与问题相关的额外背景信息：
---
{context}
---""")
chain = (
        {
            "question": itemgetter("question"),
            "qa_pairs": itemgetter("qa_pairs"),
            "context": itemgetter("question") | retriever,
        }
        | prompt
        | ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
        | StrOutputParser()
)

# 5.循环遍历所有子问题进行检索并获取答案
qa_pairs = ""
for sub_question in sub_questions:
    answer = chain.invoke({"question": sub_question, "qa_pairs": qa_pairs})
    qa_pair = format_qa_pair(sub_question, answer)
    qa_pairs += "\n---\n" + qa_pair
    print(f"问题: {sub_question}")
    print(f"答案: {answer}")
```



### 少量示例提示模板

在与 LLM 的对话中，提供少量的示例被称为 少量示例，这是一种简单但强大的指导生成的方式，在某些情况下可以显著提高模型性能（与之对应的是零样本），少量示例可以降低 Prompt 的复杂度，快速告知 LLM 生成内容的规范。

```python

# 1.构建示例模板与示例
example_prompt = ChatPromptTemplate.from_messages([
    ("human", "{question}"),
    ("ai", "{answer}"),
])
examples = [
    # 少量示例提示模板 ...
]

# 2.构建少量示例提示模板
few_shot_prompt = FewShotChatMessagePromptTemplate(
    example_prompt=example_prompt,
    examples=examples,
)
# 3.构建最终提示模板
prompt = ChatPromptTemplate.from_messages([
    ("system", ""),
    few_shot_prompt,
    ("human", "{question}"),
])

# 4.创建大语言模型与链
llm = ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
chain = prompt | llm | StrOutputParser()

# 5.调用链获取结果
print(chain.invoke("test"))
```



### Step-Back 回答回退策略的优点

为复杂问题生成一个前置问题，通过前置问题来执行相应的检索，这就是 Setp-Back 回答回退策略（后退提示），这是一种用于增强语言模型的推理和问题解决能力的技巧，它鼓励 LLM 从一个给定的问题或问题后退一步，提出一个更抽象、更高级的问题，涵盖原始查询的本质。

后退提示背后的概念是，许多复杂的问题或任务包含很多复杂的细节和约束，这使 LLM 难以直接检索和应用相关信息。通过引入一个后退问题，这个问题通常更容易回答，并且围绕一个更广泛的概念或原则，让 LLM 可以更有效地构建它们的推理。

**对比 问题分解策略**，回答回退策略 仅仅多调用一次 LLM，所以相应速度更快，性能更高，并且复杂度更低，对于一些参数量较小的模型，也可以实现不错的效果，对于 问题分解策略-迭代式回答，再一些极端的情况下，模型输出了有偏差的内容，每次都在有偏差的 问题+答案 生成新内容，很有可能会导致最后的输出完全偏离开始的预设。





### HyDE 混合策略

利用 LLM 将问题转换为回答问题的假设性文档/假回答，然后使用嵌入的 假设性文档 去检索真实文档，前提是因为 doc-doc 这个模式执行相似性搜索可以尝试更多的匹配项。

先根据 query 生成一个 doc，然后根据 doc 生成对应的 embedding，再执行相应的检索。



```python
class HyDERetriever(BaseRetriever):
    """HyDE混合策略检索器"""
    retriever: BaseRetriever
    llm: BaseLanguageModel

    def _get_relevant_documents(
            self, query: str, *, run_manager: CallbackManagerForRetrieverRun
    ) -> List[Document]:
        """传递检索query实现HyDE混合策略检索"""
        # 1.构建生成假设性文档的prompt
        prompt = ChatPromptTemplate.from_template(
            "请写一篇科学论文来回答这个问题。\n"
            "问题: {question}\n"
            "文章: "
        )

        # 2.构建HyDE混合策略检索链
        chain = (
                {"question": RunnablePassthrough()}
                | prompt
                | self.llm
                | StrOutputParser()
                | self.retriever
        )

        return chain.invoke(query)


# 1.构建向量数据库与检索器
db = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("AuthApiKey"),
    ),
    index_name="demo",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
retriever = db.as_retriever(search_type="mmr")

# 2.创建HyDE检索器
hyde_retriever = HyDERetriever(
    retriever=retriever,
    llm=ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0),
)

# 3.检索文档
documents = hyde_retriever.invoke("检索文档？")
print(documents)
print(len(documents))
```

HyDE 是一个无监督的方法，可以帮助 RAG 提高效果。但是因为它不完全依赖于 embedding 而是强调问题的答案和查找内容的相似性，也存在一定的局限性。比如如果 LLM 无法理解用户问题，自然不会产生最佳结果，也可能导致错误增加。因此，需要根据场景决定是否选用此方法





### 集成检索器的优势与使用

集成检索器可以利用不同算法的优势，从而获得比任何单一算法更好的性能。集成检索器一个常见的案例是将 稀疏检索器(如BM25) 和 密集检索器(如嵌入相似度) 结合起来，因为它们的优势是互补的，这种检索方式也被称为混合检索（稀疏检索器擅长基于关键词检索，密集检索器擅长基于语义相似性检索）。

```python
# 1.创建文档列表
documents = [
    Document(page_content="test1", metadata={"page": 1}),
    Document(page_content="test2", metadata={"page": 2}),
]

# 2.构建BM25关键词检索器
bm25_retriever = BM25Retriever.from_documents(documents)
bm25_retriever.k = 4

# 3.创建FAISS向量数据库检索
faiss_db = FAISS.from_documents(documents, embedding=OpenAIEmbeddings(model="text-embedding-3-small"))
faiss_retriever = faiss_db.as_retriever(search_kwargs={"k": 4})

# 4.初始化集成检索器
ensemble_retriever = EnsembleRetriever(
    retrievers=[bm25_retriever, faiss_retriever],
    weights=[0.5, 0.5],
)

# 5.执行检索
docs = ensemble_retriever.invoke("test")
print(docs)
print(len(docs))
```



### 总结

**多查询重写**：实现简单，使用参数较小的模型也可以完成对查询的转换（不涉及回答），因为多查询可以并行检索，所以性能较高，但是在合并的时候，没有考虑到不同文档的权重，仅仅按照顺序进行合并，会让某些高权重的文档在使用时可能被剔除

**多查询融合** : 在 多查询重写 的基础上引入了 RRF 算法，对不同査询检索到的文档列表的每一篇文档计算对应的权重，将排名靠前亦或者是出现次数更多的文档放置到合并文档的靠前部分。

**问题分解策略**：通过将复杂问题/数学问题分解成多个子问题，从而实现对每个子问题的 检索-生成 流程，最后再将所有子问题的 答案 进行合并，在转换环节涉及到对子问题的回答，所以对于中间 LLM 的要求比较高，性能相对也比较差，在上下文长度不足的情况下，拆分问题并迭代回答，可能会让最终答案偏离原始的提问

**回答回退策略**：通过提出一个前置问题/通用问题用于优化原始的复杂问题，从而获得更大的搜索范围，提升检索到相关性高的文档的概率，因为中间 LLM 没涉及到回答，所以可以使用参数量较小的模型来实现，性能相对较高

**HyDE混合策略**:通过将 query 转换成 doc的思想，并执行 doc-doc 实现对称性检索，从而提升找到相似性文档的概率，但是对于上下文不足，或者是开放性问题， HyDE混合策略 的效果相对较差

**集成检索器策略** : 目前使用频率最高的检索器策略，集成多种不同的检索方式，充分利用不同算法的优点，最后再使用 RRF 算法对不同检索器检索到的数据进行合并，从而得到最终文档列表。







## 大模型的函数回调与规范化输出

利用 函数回调 这个功能来执行相应的规范化输出

```python
class RouteQuery(BaseModel):
    """将用户查询映射到对应的数据源上"""
    datasource: Literal["python", "js", "golang"] = Field(
        description="根据用户的问题，选择哪个数据源最相关以回答用户的问题"
    )

# 1.创建绑定结构化输出的大语言模型
llm = ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
structured_llm = llm.with_structured_output(RouteQuery)

# 2.构建一个问题
question = """请检查以下问题是哪种类型：

var cq = "ccc"
"""
res: RouteQuery = structured_llm.invoke(question)

print(res)
print(type(res))
print(res.datasource)

```





根据不同的问题检索不同的 检索器/向量数据库

```python
class RouteQuery(BaseModel):
    """将用户查询映射到最相关的数据源"""
    datasource: Literal["python", "js", "golang"] = Field(
        description="根据给定用户问题，选择哪个数据源最相关以回答他们的问题"
    )


def choose_route(result: RouteQuery) -> str:
    """根据传递的路由结果选择不同的检索器"""
    if "python" in result.datasource:
        return "chain in python"
    elif "js" in result.datasource:
        return "chain in js"
    else:
        return "golang"


# 1.构建大语言模型并进行结构化输出
llm = ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
structured_llm = llm.with_structured_output(RouteQuery)

# 2.创建路由逻辑链
prompt = ChatPromptTemplate.from_messages([
    ("system", "你是一个擅长将用户问题路由到适当的数据源的专家。\n请根据问题涉及的编程语言，将其路由到相关数据源"),
    ("human", "{question}")
])
router = {"question": RunnablePassthrough()} | prompt | structured_llm | choose_route

# 3.执行相应的提问，检查映射的路由
question = """帮我检查下：

prompt = ChatPromptTemplate.from_messages(["human", "speak in {language}"])
prompt.invoke("中文")"""

# 4.选择不同的数据库
print(router.invoke(question))
```





## 语义路由选择不同的Prompt模板

```python
# 1.定义两份不同的prompt模板(物理模板、数学模板)
physics_template = """你是一位非常聪明的物理教程。
你擅长以简洁易懂的方式回答物理问题。
当你不知道问题的答案时，你会坦率承认自己不知道。

这是一个问题：
{query}"""
math_template = """你是一位非常优秀的数学家。你擅长回答数学问题。
你之所以如此优秀，是因为你能将复杂的问题分解成多个小步骤。
并且回答这些小步骤，然后将它们整合在一起回来更广泛的问题。

这是一个问题：
{query}"""

# 2.创建文本嵌入模型，并执行嵌入
embeddings = OpenAIEmbeddings(model="text-embedding-3-small")
prompt_templates = [physics_template, math_template]
prompt_embeddings = embeddings.embed_documents(prompt_templates)


def prompt_router(input) -> ChatPromptTemplate:
    """根据传递的query计算返回不同的提示模板"""
    # 1.计算传入query的嵌入向量
    query_embedding = embeddings.embed_query(input["query"])

    # 2.计算相似性
    similarity = cosine_similarity([query_embedding], prompt_embeddings)[0]
    most_similar = prompt_templates[similarity.argmax()]
    print("使用数学模板" if most_similar == math_template else "使用物理模板")

    # 3.构建提示模板
    return ChatPromptTemplate.from_template(most_similar)


chain = (
        {"query": RunnablePassthrough()}
        | RunnableLambda(prompt_router)
        | ChatOpenAI(model="gpt-3.5-turbo-16k")
        | StrOutputParser()
)

print(chain.invoke("test"))
```





## 查询构建与自查询检索器

利用大模型从用户自然语言问题中提取出结构化筛选条件（如年份、分类等），并将其应用到向量检索或数据库查询中，以提升检索准确性。

```python
# 1. 创建简单的电影文档列表
docs = [
    Document(page_content="肖申克的救赎", metadata={"year": 1994, "rating": 9.7}),
    Document(page_content="阿甘正传", metadata={"year": 1994, "rating": 9.5}),
    Document(page_content="千与千寻", metadata={"year": 2001, "rating": 9.4}),
    Document(page_content="星际穿越", metadata={"year": 2014, "rating": 9.4}),
]

# 2. 嵌入模型
embedding = OpenAIEmbeddings(model="text-embedding-3-small")

# 3. 使用 Chroma 本地内存向量数据库
vectorstore = Chroma.from_documents(docs, embedding)

# 4. 定义元数据字段信息
metadata_field_info = [
    AttributeInfo(name="year", description="电影的上映年份", type="integer"),
    AttributeInfo(name="rating", description="电影评分", type="float"),
]

# 5. 创建 SelfQueryRetriever
retriever = SelfQueryRetriever.from_llm(
    llm=ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0),
    vectorstore=vectorstore,
    document_contents="电影名",
    metadata_field_info=metadata_field_info,
    enable_limit=True
)

# 6. 测试查询
results = retriever.invoke("找出评分大于9.5的电影")
for doc in results:
    print(f"{doc.page_content} - 评分: {doc.metadata['rating']} 年份: {doc.metadata['year']}")
```



## MultiVector 实现多向量检索文档

原始文档 和 摘要文档 中都在元数据中设置了 唯一标识，从向量数据库中找到符合规则的数据后，通过查找其 元数据 的唯一标识，即可在 文档数据库 中匹配出原文档，完成整个多表征/向量的检索。

```python
# 1.创建加载器、文本分割器并处理文档
loader = UnstructuredFileLoader("./test.txt")
text_splitter = RecursiveCharacterTextSplitter(chunk_size=500, chunk_overlap=50)
docs = loader.load_and_split(text_splitter)

# 2.定义摘要生成链
summary_chain = (
        {"doc": lambda x: x.page_content}
        | ChatPromptTemplate.from_template("请总结以下文档的内容：\n\n{doc}")
        | ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
        | StrOutputParser()
)

# 3.批量生成摘要与唯一标识
summaries = summary_chain.batch(docs, {"max_concurrency": 5})
doc_ids = [str(uuid.uuid4()) for _ in summaries]

# 4.构建摘要文档
summary_docs = [
    Document(page_content=summary, metadata={"doc_id": doc_ids[idx]})
    for idx, summary in enumerate(summaries)
]

# 5.构建文档数据库与向量数据库
byte_store = LocalFileStore("./multy-vector")
db = FAISS.from_documents(
    summary_docs,
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)

# 6.构建多向量检索器
retriever = MultiVectorRetriever(
    vectorstore=db,
    byte_store=byte_store,
    id_key="doc_id",
)

# 7.将摘要文档和原文档存储到数据库中
retriever.docstore.mset(list(zip(doc_ids, docs)))

# 8.执行检索
search_docs = retriever.invoke("电影推荐")
print(search_docs)
print(len(search_docs))
```





假设性查询检索原文档

```python
class HypotheticalQuestions(BaseModel):
    """生成假设性问题"""
    questions: List[str] = Field(
        description="假设性问题列表，类型为字符串列表",
    )

# 1.构建一个生成假设性问题的prompt
prompt = ChatPromptTemplate.from_template("生成一个包含3个假设性问题的列表，这些问题可以用于回答下面的文档:\n\n{doc}")

# 2.创建大语言模型，并绑定对应的规范化输出结构
llm = ChatOpenAI(model="gpt-3.5-turbo-16k", temperature=0)
structured_llm = llm.with_structured_output(HypotheticalQuestions)

# 3.创建链应用
chain = (
        {"doc": lambda x: x.page_content}
        | prompt
        | structured_llm
)

hypothetical_questions: HypotheticalQuestions = chain.invoke(
    Document(page_content="test")
)
print(hypothetical_questions)
```





## 父文档检索器

```python
# 1.创建加载器与文档列表，并加载文档
loaders = [
    UnstructuredFileLoader("./test.txt"),
    UnstructuredFileLoader("./test.md"),
]
docs = []
for loader in loaders:
    docs.extend(loader.load())

# 2.创建文本分割器
text_splitter = RecursiveCharacterTextSplitter(
    chunk_size=500,
    chunk_overlap=50,
)

# 3.创建向量数据库与文档数据库
vector_store = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("AuthApiKey"),
    ),
    index_name="ParentDocument",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
byte_store = LocalFileStore("./parent-document")

# 4.创建父文档检索器
retriever = ParentDocumentRetriever(
    vectorstore=vector_store,
    byte_store=byte_store,
    child_splitter=text_splitter,
)

# 5.添加文档
retriever.add_documents(docs, ids=None)

# 6.检索并返回内容
search_docs = retriever.invoke("分享关于LLMOps的一些应用配置")
print(search_docs)
print(len(search_docs))
```

## 小文档块检索大文档块

```python
# 1.创建加载器与文档列表，并加载文档
loaders = [
    UnstructuredFileLoader("./test.txt"),
    UnstructuredFileLoader("./test.md"),
]
docs = []
for loader in loaders:
    docs.extend(loader.load())

# 2.创建文本分割器
parent_splitter = RecursiveCharacterTextSplitter(chunk_size=2000)
child_splitter = RecursiveCharacterTextSplitter(chunk_size=500, chunk_overlap=50)

# 3.创建向量数据库与文档数据库
vector_store = WeaviateVectorStore(
    client=weaviate.connect_to_wcs(
        cluster_url="cluster_url",
        auth_credentials=AuthApiKey("AuthApiKey"),
    ),
    index_name="ParentDocument",
    text_key="text",
    embedding=OpenAIEmbeddings(model="text-embedding-3-small"),
)
store = LocalFileStore("./parent-document")

# 4.创建父文档检索器
retriever = ParentDocumentRetriever(
    vectorstore=vector_store,
    byte_store=store,
    parent_splitter=parent_splitter,
    child_splitter=child_splitter,
)

# 5.添加文档
retriever.add_documents(docs, ids=None)

# 6.检索并返回内容
search_docs = retriever.invoke("test")
print(search_docs)
print(len(search_docs))
```

