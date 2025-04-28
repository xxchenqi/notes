# QA

## 跨域

在浏览器中，当一个请求 URL 的**协议、域名、端口**三者之间任意一个与当前页面 URL 不同即为跨域。

| 来源地址            | 请求地址                | 是否跨域         |
| ------------------- | ----------------------- | ---------------- |
| `http://a.com`      | `http://a.com/api`      | ❌ 否             |
| `http://a.com`      | `https://a.com/api`     | ✅ 是（协议不同） |
| `http://a.com`      | `http://b.com/api`      | ✅ 是（域名不同） |
| `http://a.com:3000` | `http://a.com:8000/api` | ✅ 是（端口不同） |

跨域问题本质上就是浏览器的一种保护机制（同源策略限制），所谓同源（即指在同一个域）就是两个页面具有相同的协议（protocol），主机（host）和端口号（port）。它诞生的初衷是为了保护用户的安全，防止恶意网站窃取数据，但是这个保护机制也带来了新的问题，它使得不同站点之间的正常调用，也会遇到阻碍。



解决方式：

**1.跨域资源共享（后端）**

浏览器将 CORS 分成两类：

- 非简单请求：包括 put 和 delete 以及请求头中 Content-Type=application/json 的请求。
- 简单请求：除非非简单请求，其他的都是简单请求。

其实 非简单请求 只比 简单请求 多了一步，即在正式请求前，增加一次 HTTP 查询请求（OPTIONS)，称为“预检/prefligh”请求。浏览器先询问服务器，当前网页所在的域名是否在服务器的许可名单之中，以及可以使用哪些 HTTP 动词和头信息字段。只有得到肯定答复，浏览器才会发出正式的请求，否则就报错。

对于 简单请求，浏览器会直接发起 CORS 请求。具体来说，就是在Header中增加一个Origin字段。如果浏览器发现跨源AJAX请求是简单请求，就自动在头信息之中，添加一个Origin字段。服务器根据Origin的值决定是否同意这次请求。如果同意，则会在响应头中加上一个属性 **Access-Control-Allow-Origin**。

所以第一种方案需要在后端下手，告知前端浏览器的“预检”是许可的，也就是在每一个响应接口的请求头上添加对应的信息，涵盖：

- Access-Control-Allow-Origin：允许的源，例如 ``。
- Access-Control-Allow-Headers：允许的响应头，例如 Content-Type 等。
- Access-Control-Allow-Methods：允许的方法，例如：GET, POST 等。
- Access-Control-Allow-Credentials：是否允许携带授权凭据信息，例如：true、false 等。



**2.中间件代理（前端）**

当我们的后端没有配置 CORS 时，可以在前端中使用代理服务器来解决跨域问题，即前端不直接向后端 API 发起请求，而是向代理服务器发起请求，代理服务器由于不是浏览器端，并没有跨域的概念

在 Vue.js 中，可以很容易使用 Vite 搭建代理服务器，参考链接：[https://cn.vitejs.dev/config/server-options.html#server-proxy](#server-proxy)