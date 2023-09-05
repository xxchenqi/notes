# WebRTC基础理论

## WebRTC概念

WebRTC（Web Real-Time Communication，网页即时通信）

WebRTC主要应用在实时通信方面，其优点总结为如下几点。

- 跨平台：可以在Web、Android、iOS、Windows、MacOS、Linux环境下运行WebRTC应用。
- 实时传输：传输速度快，延迟低，适合实时性要求较高的应用场景。
- 音视频引擎：强大的音视频处理能力。
- 免插件：不需要安装任何插件，打开浏览器即可使用。
- 免费：虽然WebRTC技术已经较为成熟，集成了最佳的音视频引擎和十分先进的Codec，但仍是免费的。
- 强大的打洞能力：WebRTC技术包含了使用STUN、ICE、TURN、RTP-over-TCP的关键NAT和防火墙穿透技术，并支持代理。
- 主流浏览器支持：包括Chrome、Safari、Firefox、Edge等。



## 整体架构详解

![](.\assets\webrtc.png)

- Video Conference、Video Call、Remote Education为应用层，指具体的音视频应用，是应用开发人员最关注的。
- Web API部分是Web应用开发者API层，为上层应用层提供API服务，是应用开发者调用的接口。
- C++ API部分是面向浏览器厂商的API层。
- Session Management为信令管理层，可由开发者自行定义实现。
- VoiceEngine、VideoEngine以及Transport为WebRTC的核心内容，可由WebRTC的应用SDK厂商进行优化处理。
- Audio Capture、Video Capture可供浏览器厂商自定义实现。



**1.Web应用**

Web开发者可以基于Web API开发基于视频、音频的实时通信应用，如视频会议、远程教育、视频通话、视频直播、游戏直播、远程协作、互动游戏、实时人脸识别、远程机械手操作等。

**2.Web API**

Web API是面向第三方开发者的WebRTC标准API（JavaScript），使开发者能够容易地开发出类似于网络视频聊天的Web应用，最新的技术进展可以参考W3C的WebRTC文档https://www.w3.org/TR/webrtc/，常用的API如下所示。

- MediaStream：媒体数据流，如音频流、视频流等。
- RTCPeerConnection：该类很重要，提供了应用层的调用接口。
- RTCDataChannel：传输非音视频数据，如文字、图片等。

WebRTC的API接口非常丰富，更多详细的API可以参考网址https://developer.mozilla.org/zh-CN/docs/Web/API/WebRTC_API，该文档提供了中文说明。

**3.C++ API**

底层API使用C++语言编写，使浏览器厂商容易实现WebRTC标准的Web API，抽象地对数字信号过程进行处理。如RTCPeerConnection API是每个浏览器之间点对点连接的核心，RTCPeerConnection是WebRTC组件，用于处理点对点间流数据的稳定和有效通信。

**4.Session Management**

Session Management是一个抽象的会话层，提供会话建立和管理功能。该层协议留给应用开发者自定义实现。对于Web应用，建议使用WebSocket技术来管理信令Session。信令主要用来转发会话双方的媒体信息和网络信息

**5.Transport**

Transport为WebRTC的传输层，涉及音视频的数据发送、接收、网络打洞等内容，可以通过STUN和ICE组件来建立不同类型的网络间的呼叫连接。

**6.VoiceEngine**

VoiceEngine（音频引擎）是包含一系列音频多媒体处理的框架，包括从音频采集到网络传输端等整个解决方案。VoiceEngine是WebRTC极具价值的技术之一，是Google收购GIPS公司后开源的，目前在VoIP技术上处于业界领先地位。下面介绍主要的模块。

- iSAC（Internet Speech Audio Codec）是针对VoIP和音频流的宽带和超宽带音频编解码器，是WebRTC音频引擎的默认编解码器，参数如下所示。
  - 采样频率：16kHz，24kHz，32kHz（默认为16kHz）。
  - 自适应速率为：10kbps~52kbps。·自适应包大小：30ms~60ms。
  - 算法延时：frame+3ms。
- iLBC（Internet Low Bitrate Codec）是VoIP音频流的窄带语音编解码器，参数如下所示。
  - 采样频率：8kHz。
  - 20ms帧比特率为15.2kbps。
  - 30ms帧比特率为13.33kbps。
- NetEQ For Voice是针对音频软件实现的语音信号处理元件。NetEQ算法是自适应抖动控制算法以及语音包丢失隐藏算法，该算法能够快速且高解析度地适应不断变化的网络环境，确保音质优美且缓冲延迟最小，是GIPS公司独特的技术，能够有效地处理网络抖动和语音包丢失时对语音质量产生的影响。NetEQ也是WebRTC中一个极具价值的技术，对于提高VoIP质量有明显效果，与AEC、NR、AGC等模块集成使用效果更好。
- Acoustic Echo Canceler（AEC，回声消除器）是一个基于软件的信号处理元件，能实时地去除Mic采集到的回声。
- Noise Reduction（NR，噪声抑制）也是一个基于软件的信号处理元件，用于消除与相关VoIP的某些类型的背景噪声（如嘶嘶声、风扇噪音等）。

**7.VideoEngine**

VideoEngine是WebRTC视频处理引擎，包含一系列视频处理的整体框架，从摄像头采集视频到视频信息网络传输再到视频显示，是一个完整过程的解决方案。下面介绍主要的模块。

- VP8是视频图像编解码器，也是WebRTC视频引擎默认的编解码器。VP8适合实时通信应用场景，因为它主要是针对低延时而设计的编解码器。VPx编解码器是Google收购ON2公司后开源的，现在是WebM项目的一部分，而WebM项目是Google致力于推动的HTML5标准之一。
- Video Jitter Buffer（视频抖动缓冲器）模块可以降低由于视频抖动和视频信息包丢失带来的不良影响。
- Image Enhancements（图像质量增强）模块对网络摄像头采集到的视频图像进行处理，包括明暗度检测、颜色增强、降噪处理等功能，用来提升视频质量。

## WebRTC 通话原理

![](.\assets\通话原理基本流程.png)

双方要建立起通话，主要步骤如下所示。

步骤1　媒体协商。Peer-A与Peer-B通过信令服务器进行媒体协商，如双方使用的音视频编码格式。双方交换的媒体数据由SDP（Session Description Protocol，会话描述协议）描述。

步骤2　网络协商。Peer-A与Peer-B通过STUN服务器获取到各自的网络信息，如IP和端口。然后通过信令服务器转发，互相交换各种网络信息。这样双方就知道对方的IP和端口了，即P2P打洞成功建立直连。这个过程涉及NAT及ICE协议。

步骤3　建立连接。Peer-A与Peer-B如果没有建立起直连，则通过TURN中转服务器转发音视频数据，最终完成音视频通话。



### 媒体协商

在WebRTC中，参与视频通信的双方必须先交换SDP信息，这样双方才能“知根知底”，这一过程也称为“媒体协商”。

SDP从哪来？

一般来说，在建立连接之前，连接双方需要先通过API来指定自己要传输什么数据（如Audio、Video、DataChannel），以及自己希望接受什么数据，然后Peer-A调用CreateOffer()方法，获取offer类型的SessionDescription，通过公共服务器传递给Peer-B，同样，Peer-B通过调用CreateAnswer()，获取answer类型的SessionDescription，通过公共服务器传递给Peer-A。在这个过程中，由哪一方创建Offer（Answer）都可以，但是要保证连接双方创建的SessionDescription类型是相互对应的。

信令服务器可以用来交换双方的SDP信息，一般是通过创建Socket连接进行交互处理。



### SDP协议

SDP是一个描述多媒体连接内容的协议，例如分辨率、格式、编码、加密算法等，便于在数据传输时两端都能够理解彼此的数据。本质上，这些描述内容的元数据并不是媒体流本身。

从技术上讲，SDP并不是一个真正的协议，而是一种数据格式，用于描述在设备之间共享媒体的连接。SDP包含的内容非常多，如下所示即为一个SDP信息。

```
//版本
v=0
//<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
o=- 3089712662142082488 2 IN IP4 127.0.0.1
//会话名
s=-
//会话的起始时间和结束时间，0代表没有限制
t=0 0
//表示音频传输和data channel传输共用一个传输通道，通过id区分不同的流
a=group:BUNDLE audio data
//WebRTC Media Stream
a=msid-semantic: WMS
//m=audio说明本会话包含音频，9代表音频使用端口9来传输，但是在WebRTC中现在一般不使用，如果设置为0，代表不传输音频
//使用UDP来传输RTP包，并使用TLS加密。SAVPF代表使用srtcp的反馈机制来控制通信过程
//111 103 104 9 0 8 106 105 13 110 112 113 126表示支持的编码，和后面的a=rtpmap对应
m=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 110 112 113 126
//表示你要用来接收或者发送音频时使用的IP地址，WebRTC使用ICE传输，不使用这个地址
c=IN IP4 0.0.0.0
//用来传输rtcp的地址和端口，WebRTC中不使用
a=rtcp:9 IN IP4 0.0.0.0
//ICE协商过程中的安全验证信息
a=ice-ufrag:ubhd
a=ice-pwd:l82NnsGm5i7pucQRchNdjA6B
//支持trickle，即SDP里面只描述媒体信息，ICE候选项的信息另行通知
a=ice-options:trickle
//DTLS协商过程中需要的认证信息
a=fingerprint:sha-256 CA:83:D0:0F:3B:27:4C:8F:F4:DB:34:58:AC:A6:5D:36:01:07:9F: 2B:1D:95:29:AD:0C:F8:08:68:34:D8:62:A7
a=setup:active
//前面BUNDLE行中用到的媒体标识
a=mid:audio
//指出要在rtp头部中加入音量信息
a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level
//当前客户端只接收数据，不发送数据，如recvonly、sendonly、inactive、sendrecv
a=recvonly
//rtp、rtcp包使用同一个端口来传输
a=rtcp-mux
//下面都是对m=audio这一行媒体编码的补充说明，指出了编码采用的编号、采样率、声道等
a=rtpmap:111 opus/48000/2
a=rtcp-fb:111 transport-cc
//对opus编码可选的补充说明，minptime代表最小打包时长是10ms，useinbandfec=1代表使用opus编码内置fec特性
a=fmtp:111 minptime=10;useinbandfec=1
a=rtpmap:103 ISAC/16000
a=rtpmap:104 ISAC/32000
a=rtpmap:9 G722/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:106 CN/32000
a=rtpmap:105 CN/16000
a=rtpmap:13 CN/8000
a=rtpmap:110 telephone-event/48000
a=rtpmap:112 telephone-event/32000
a=rtpmap:113 telephone-event/16000
a=rtpmap:126 telephone-event/8000
//下面是对Data Channel的描述，基本和上面的audio描述类似，使用DTLS加密，使用SCTP传输
m=application 9 DTLS/SCTP 5000
c=IN IP4 0.0.0.0
//可以是CT或AS，CT方式是设置整个会议的带宽，AS是设置单个会话的带宽。默认带宽是kbps级别
b=AS:30
a=ice-ufrag:ubhd
a=ice-pwd:l82NnsGm5i7pucQRchNdjA6B
a=ice-options:trickle
a=fingerprint:sha-256 CA:83:D0:0F:3B:27:4C:8F:F4:DB:34:58:AC:A6:5D:36:01:07:9F: 2B:1D:95:29:AD:0C:F8:08:68:34:D8:62:A7
a=setup:active
//前面BUNDLE行中用到的媒体标识
a=mid:data
//使用端口5000，一个消息的大小是1024b
a=sctpmap:5000 webrtc-datachannel 1024
```

```
a 代表属性 
m 代表媒体信息
c 代表网络信息
```

```
a=ssrc:xxx cname:xxx 媒体源的唯一标识,cname代表别名
```





### 网络协商

通信双方彼此要了解对方的网络情况，这样才有可能找到一条通信链路。需要做以下两个处理。

- 获取外网IP地址映射。
- 通过信令服务器（signal server）交换“网络信息”。

实际情况是我们的计算机都是在某个局域网中并且有防火墙，需要进行网络地址转换（Network Address Translation，NAT）

#### NAT

NAT(Network Address Translation)是为了解决IPv4下的IP地址匮乏而出现的一种技术。

NAT技术会保护内网地址的安全性，所以这就会引发一个问题，就是当我们采用P2P中的连接方式时，NAT会阻止外网地址的访问，这时我们就得采用NAT穿透技术了。

解决思路：借助一个公网IP服务器，Peer-A与Peer-B都往公网IP/PORT发包，公网服务器就可以获知Peer-A与Peer-B的IP/PORT，又由于Peer-A与Peer-B主动给公网IP服务器发包，所以公网服务器可以穿透NAT-A与NAT-B并发送包给Peer-A与Peer-B。所以只要公网IP将Peer-B的IP/PORT发给Peer-A，将Peer-A的IP/PORT发给Peer-B，这样下次Peer-A与Peer-B互相发送消息时，就不会被NAT阻拦了。

在WebRTC中采用ICE框架来保证RTCPeerConnection能实现NAT穿透。

#### ICE

ICE（Interactive Connectivity Establishment，互动式连接建立）是一种框架，使各种NAT穿透技术（如STUN、TURN等）可以实现统一，该技术可以让客户端成功地穿透远程用户与网络之间可能存在的各类防火墙。



#### STUN

STUN是指简单UDP穿透NAT（Simple Traversal of UDP Through NAT），这项技术允许位于NAT（或多重NAT）后的客户端找出自己的公网IP地址，以及查出自己位于哪种类型的NAT及NAT所绑定的Internet端口。这些信息可用于将两个同时处于NAT路由器之后的主机之间建立UDP通信

即使通过STUN服务器取得了公网IP位址，也不一定能建立连接。因为不同的NAT类型处理传入的UDP分组的方式是不同的，四种主要类型中有三种可以使用STUN穿透：完全圆锥型NAT、受限圆锥型NAT和端口受限圆锥型NAT。但大型公司网络中经常采用的对称型NAT（又称为双向NAT）则不能使用，这类路由器会透过NAT部署所谓的“Symmetric NAT限制”，也就是说，路由器只会接受你之前连线过的节点所建立的连线，这类网络就需要用到TURN技术。



**TURN**

TURN是指使用中继穿透NAT（Traversal Using Relays around NAT），是STUN的一个扩展（在RFC5389中定义），主要添加了中继功能。如果终端在进行NAT之后，在特定的情景下有可能使得终端无法和其他终端进行直接的通信，这时就需要将公网的服务器作为一个中继，对来往的数据进行转发。这个转发采用的协议就是TURN。

在STUN服务器的基础上，再架设几台TURN服务器。在STUN分配公网IP失败后，可以通过TURN服务器请求公网IP地址作为中继地址，将媒体数据由TURN服务器中转。

当媒体数据进入TURN服务器中转，这种方式的带宽由服务器端承担。所以在架设中转服务器时要考虑硬件及带宽。

以上是WebRTC中经常用到的协议，STUN服务器和TURN服务器我们使用coturn开源项目来搭建，地址为https://github.com/coturn/coturn。也可以使用以Golang技术开发的服务器来搭建，地址为https://github.com/pion/turn。



#### 信令服务器

信令服务器（signal server）转发彼此的媒体信息和网络信息。

我们在基于WebRTC API开发应用（App）时，可以将彼此的App连接到信令服务器，一般搭建在公网或者两端都可以访问到的局域网，借助信令服务器，就可以实现SDP（媒体信息）及Candidate（网络信息）交换。

信令服务器不只是交换SDP和Candidate，还有其他功能，比如房间管理、用户列表、用户进入、用户退出等IM功能。



### 连接建立的流程

1）连接双方（Peer）通过第三方服务器来交换（signaling）各自的SDP数据。

2）连接双方通过STUN协议从STUN服务器那里获取到自己的NAT结构、子网IP和公网IP、端口，即Candidate信息。

3）连接双方通过第三方服务器来交换各自的Candidate，如果连接双方在同一个NAT下，那它们仅通过内网Candidate就能建立起连接；如果它们处于不同NAT下，就需要通过STUN服务器识别出的公网Candidate进行通信。

4）如果仅通过STUN服务器发现的公网Candidate仍然无法建立连接，这就需要寻求TURN服务器提供的转发服务，然后将转发形式的Candidate共享给对方。

5）连接双方向目标IP端口发送报文，通过SDP数据中涉及的密钥以及期望传输的内容建立起加密长连接。



### 连接建立处理

1）A创建Offer

2）A保存Offer（设置本地描述）

3）A发送Offer给B

4）B保存Offer（设置远端描述）

5）B创建Answer

6）B保存Answer（设置本地描述）

7）B发送Answer给A

8）A保存Answer（设置远端描述）

9）A发送ICE Candidate给B

10）B发送ICE Candidate给A

11）A、B收到对方的媒体流并播放



## RTP

RTP（Real-time Transport Protocol，实时传输协议）

RTP是一种协议，用于实时传输音频和视频数据。

RTP在数据传输过程中负责分包、排序和时间戳，以确保数据能够正确传输。

RTP使用UDP协议，因为它比TCP协议更适合实时传输应用，因为它不会在数据传输时引入延迟



PS:

RTMP是基于TCP，一旦网络出问题，就会出现延迟

RTP考虑到网络通畅，会将包丢弃

那么为什么RTP数据还能正常组装？因为里面有分包排序时间戳



RTP协议的具体格式如下所示：

```
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                           timestamp                           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |           synchronization source (SSRC) identifier            |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |      contributing source (CSRC) identifiers (optional)        |
 |                             ....                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                           payload ...                         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

上述格式中的各个字段含义如下：

- Version (V)：2位，指示RTP协议的版本号。
- Padding (P)：1位，指示是否在RTP数据包的末尾添加填充字节。
- Extension (X)：1位，指示是否存在RTP头部扩展。
- CSRC Count (CC)：4位，指示CSRC（Contributing Source）标识符的数量。
- Marker (M)：1位，由应用程序使用，可以用于标识重要的数据包。
- Payload Type (PT)：7位，指示RTP数据包中承载的媒体类型。
- Sequence Number：16位，每个RTP数据包都有一个唯一的序列号，用于数据包的排序和丢包检测。
- Timestamp：32位，用于指示媒体数据的时间戳，用于同步和播放控制。
- Synchronization Source (SSRC) Identifier：32位，用于唯一标识RTP数据包的源。
- Contributing Source (CSRC) Identifiers：可选字段，用于标识对媒体数据贡献的其他源。
- Payload：变长字段，包含实际的媒体数据。



## RTCP

Real-Time Control Protocol (RTCP)

RTCP是RTP的伴随协议，用于控制数据传输和收集统计信息。

RTCP在数据传输过程中负责收集数据传输的质量和性能信息，以便对传输进行调整和优化。

RTCP使用UDP协议，但是它不同于RTP，因为它不会传输实际的音频或视频数据。相反，它传输控制信息和统计信息。

webRTC使用这些协议来支持实时音频和视频通信，例如视频会议、实时游戏和实时音频聊天等应用

RTCP协议用于在实时通信会话中提供控制信息，例如参与者的接收质量反馈、会话控制和统计信息等。它与RTP协议一起使用，以提供完整的实时传输解决方案。

具体格式和字段说明：

```
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|    RC   |   PT=SR=200   |             length            |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         SSRC of sender                        |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |              NTP timestamp (most significant word)            |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |              NTP timestamp (least significant word)           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         RTP timestamp                         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                     sender's packet count                     |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      sender's octet count                     |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         report block(s)                       |
 |                            ....                               |

```

上述格式中的各个字段含义如下：

- Version (V)：2位，指示RTCP协议的版本号。
- Padding (P)：1位，指示是否在RTCP数据包的末尾添加填充字节。
- Reception Report Count (RC)：5位，指示接收报告（Reception Report）块的数量。
- Packet Type (PT)：8位，指示RTCP数据包的类型。200表示发送者报告（Sender Report）。
- Length：16位，指示RTCP数据包的长度（以32位字为单位）。
- SSRC of Sender：32位，用于唯一标识发送RTCP数据包的源。
- NTP Timestamp：64位，用于指示发送RTCP数据包的时间戳（使用NTP格式）。
- RTP Timestamp：32位，用于指示媒体数据的时间戳，与RTP数据包中的时间戳对应。
- Sender's Packet Count：32位，指示发送者发送的RTP数据包数量。
- Sender's Octet Count：32位，指示发送者发送的总字节数。
- Report Block(s)：可选字段，包含接收报告块，用于描述接收者对发送者的接收质量情况。





接收报告块的格式如下：

```
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                 SSRC_1 (SSRC of first receiver)               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | fraction lost |       cumulative number of packets lost       |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |           extended highest sequence number received           |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      interarrival jitter                      |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                         last SR (LSR)                         |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                   delay since last SR (DLSR)                  |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                                                               |
 | additional optional feedback fields, such as                  |
 |   reception quality indicators (RQIs) or                       |
 |   application-specific information (ASIs),                    |
 |   depending on the feedback format specified                  |
 |                                                               |

```

- SSRC_1：32位，表示第一个接收者的SSRC（同RTCP数据包中的SSRC字段）。
- Fraction Lost：8位，表示丢包比例的分数部分。
- Cumulative Number of Packets Lost：24位，表示累计丢失的数据包数量。
- Extended Highest Sequence Number Received：32位，表示接收者收到的最高序列号（对应RTP数据包的序列号）。
- Interarrival Jitter：32位，表示到达接收者的RTP数据包之间的抖动。
- Last SR (LSR)：32位，表示上一个发送者报告（SR）的时间戳。
- Delay since Last SR (DLSR)：32位，表示自上一个发送者报告（SR）以来的延迟。



## P2P流程图

![](.\assets\p2p.png)



## Mesh

![](.\assets\Mesh.png)



Mesh方案是一种点对点通信的拓扑结构，其中每个参与者（也称为节点）都直接与其他参与者建立连接，形成一个网状结构。在Mesh方案中，所有节点之间都可以直接通信，而不需要通过中心服务器转发数据。

在Mesh方案中，每个节点都可以与其他节点直接建立连接，这种连接通常是通过WebRTC提供的PeerConnection对象来实现的。每个节点可以充当发送者和接收者，可以将数据直接发送到其他节点，并从其他节点接收数据。这种直接的点对点通信可以提供较低的延迟和更高的带宽利用率。

Mesh方案适用于小规模的通信场景，其中参与者数量相对较少。由于每个节点都需要与其他节点建立连接，因此随着参与者数量的增加，网络中的连接数量将呈指数级增长，这可能会导致网络资源消耗过多。因此，在大规模的通信场景中，通常会使用其他拓扑结构，如Selective Forwarding Unit（SFU）或Multipoint Control Unit（MCU）来管理连接和转发数据。



优点：节省流量

缺点：支持人数少

适合场景：小班教学、几个人开会(6人以内)



```
A to-b to-c to-d 3M b-a c-a d-a 3M
B to-a to-c to-d 3M a-b c-b d-b 3M
C to-a to-b to-d 3M a-c b-c d-c 3M
D to-a to-b to-c 3M a-d b-d c-d 3M
```





## MCU

![](.\assets\MCU.png)

MCU（Multipoint Control Unit）方案是一种多点控制单元方案，用于管理多个参与者之间的实时通信。MCU方案通常用于大规模的视频会议或多方通话场景。

在MCU方案中，参与者（也称为终端）不直接与其他参与者建立点对点连接，而是通过与MCU服务器建立连接来进行通信。MCU服务器充当中心节点，负责接收来自每个参与者的音视频流，并将其转发给其他参与者。这种转发过程可以通过混流、解码和重新编码等技术实现。

MCU方案的优点之一是它可以有效地管理和控制多个参与者之间的通信。它可以提供更好的可扩展性，因为参与者只需要与MCU服务器建立连接，而不需要与其他参与者直接建立连接。此外，MCU还可以提供一些额外的功能，如混音、录制和流量控制。

然而，MCU方案也存在一些限制。由于所有的音视频流都经过MCU服务器，这可能会增加延迟并消耗服务器资源。此外，MCU服务器也可能成为单点故障，一旦服务器出现故障，整个通信将中断。



适合场景：同时开会人多的会议场景

优点：支持更多的端的会议，节省带宽

缺点：服务器资源消耗太大，需要较高的配置

```
A to-mcu 1M mcu-A 1M
B to-mcu 1M mcu-B 1M
C to-mcu 1M mcu-C 1M
D to-mcu 1M mcu-D 1M

连麦:
A---WebRTC---B---> MCU合成---> RTMP/CDN---> RTMP/HLS
```



## SFU(推荐方案)

![](.\assets\SFU.png)

SFU（Selective Forwarding Unit）方案是一种选择性转发单元方案，用于实现多方实时通信。SFU方案在多人视频会议或多方通话场景中广泛应用。

在SFU方案中，参与者（也称为终端）通过与SFU服务器建立连接来进行通信。与MCU方案不同，SFU服务器不会对音视频流进行混流或重新编码。相反，SFU服务器接收来自每个参与者的音视频流，并将其转发给其他参与者。每个参与者都会接收到其他参与者的音视频流，并根据自己的需求进行解码和播放。

SFU方案的优点之一是减少了延迟和服务器资源的消耗。由于SFU服务器只是简单地转发音视频流，而不进行混流或重新编码，因此可以更快地将流转发给其他参与者。此外，SFU方案还提供了更好的可扩展性，因为每个参与者只需要处理自己感兴趣的音视频流，而不需要处理其他参与者的流。

然而，SFU方案也有一些限制。由于每个参与者都需要接收和解码其他参与者的音视频流，这可能会增加终端设备的负担。此外，由于SFU服务器不对流进行混流或重新编码，所以无法提供一些额外的功能，如混音或录制。

SFU方案与MCU方案相比，更适合于大规模的实时通信，特别是在需要低延迟和高可扩展性的场景中。它已经成为WebRTC中常用的多点通信方案之一。



优点：服务器压力小，不需要太高配置

缺点：带宽要求较高，流量消耗大

```
A to-sfu 1M  sfu-b-a 1M  sfu-c-a 1M  sfu-d-a 1M
B to-sfu 1M  sfu-a-b 1M  sfu-c-b 1M  sfu-d-b 1M
C to-sfu 1M  sfu-a-c 1M  sfu-b-c 1M  sfu-d-C 1M
D to-sfu 1M  sfu-a-d 1M  sfu-b-d 1M  sfu-c-d 1M

1上行 n-1下行
```

