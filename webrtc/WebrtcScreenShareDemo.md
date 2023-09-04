# WebRTC

## 名词解释

**WebRTC**：（Web 实时通信）是一项开放的标准和技术集合，旨在使 Web 浏览器能够实现实时音视频通信和数据传输的能力，而无需依赖插件或其他第三方软件。

**PeerConnection**：代表了浏览器中的一个对等连接（peer-to-peer connection），用于在浏览器之间建立音视频通信或数据传输的能力。

提供了以下功能和方法：

1. 媒体流管理：可以通过 PeerConnection 对象添加本地音频和视频流，并接收和处理远程音频和视频流。
2. 网络协商：PeerConnection 通过使用 ICE（Interactive Connectivity Establishment）协议来协商对等连接的网络地址和通信协议。
3. 候选项收集：PeerConnection 会自动收集本地的 ICE 候选项，用于建立对等连接。
4. 媒体协商：可以通过 PeerConnection 对象进行媒体协商，以确定音视频编解码格式、传输参数等。
5. 信令交换：PeerConnection 对象需要与远程对等方进行信令交换，以建立连接并交换通信所需的信息。
6. 数据通道：除了音视频传输，PeerConnection 还支持创建数据通道，用于在浏览器之间传输任意数据。

通过使用 PeerConnection 对象，开发者可以在 Web 应用程序中实现实时音视频通信、视频会议、屏幕共享、文件传输等功能，为用户提供丰富的实时交互体验。

**PeerConnectionFactory**：用于建立点对点（或多对多）的实时通信连接，支持音频、视频和数据的传输。

 **ICE**（Interactive Connectivity Establishment）：服务器的配置信息。ICE 用于在对等连接中建立网络连接，并处理 NAT（Network Address Translation）和防火墙等网络障碍。iceServer 包含 ICE 服务器的 URL、用户名和密码等信息。





## 初始化流程

```kotlin
WebrtcService.surfaceView = views.surfaceView // org.webrtc.SurfaceViewRenderer

mainRepository.init(username, surfaceView!!)

fun init(username: String, surfaceView: SurfaceViewRenderer) {
    this.username = username
    this.surfaceView = surfaceView
    initSocket()
    initWebrtcClient()
}
```



### initSocket

MainRepository

```kotlin
private fun initSocket() {
    socketClient.listener = this
    // 用户名自定义
    socketClient.init(username)
}
```

SocketClient

```kotlin
fun init(username:String){
    this.username = username
    // implementation 'org.java-websocket:Java-WebSocket:1.5.3'
    // org.java_websocket.client.WebSocketClient
    webSocket= object : WebSocketClient(URI("ws://10.128.167.171:3000")){
        // WebSocket 连接成功打开时调用的方法。可以在此方法中处理与服务器的握手信息。
        override fun onOpen(handshakedata: ServerHandshake?) {
            // 发送消息
            sendMessageToSocket(
                DataModel(
                    type = DataModelType.SignIn,
                    username = username,
                    null,
                    null
                )
            )
        }
        
        // 接收到服务器发送的文本消息时调用的方法。可以在此方法中处理接收到的消息。
        override fun onMessage(message: String?) {
            val model = try {
                gson.fromJson(message.toString(),DataModel::class.java)
            }catch (e:Exception){
                null
            }
            Log.d("TAG", "onMessage: $model")
            model?.let {
                listener?.onNewMessageReceived(it)
            }
        }
        
        // WebSocket 连接关闭时调用的方法。可以在此方法中处理连接关闭的相关逻辑。
        override fun onClose(code: Int, reason: String?, remote: Boolean) {
            CoroutineScope(Dispatchers.IO).launch {
                delay(5000)
                init(username)
            }
        }
        
        // 发生错误时调用的方法。可以在此方法中处理连接或消息处理过程中的异常情况。
        override fun onError(ex: Exception?) {
        }
    }
    // 连接成功后回调onOpen
    webSocket?.connect()
}
```



```kotlin
fun sendMessageToSocket(message:Any?){
    try {
        webSocket?.send(gson.toJson(message))
    }catch (e:Exception){
        e.printStackTrace()
    }
}
```



### initWebrtcClient

```kotlin
private fun initWebrtcClient() {
    webrtcClient.listener = this
    webrtcClient.initializeWebrtcClient(username, surfaceView,
        object : MyPeerObserver() {
            override fun onIceCandidate(p0: IceCandidate?) {
                // 当本地 ICE 候选项可用时，将调用此方法。ICE 候选项用于建立对等连接，并包含网络地址和协议等信息。
                super.onIceCandidate(p0)
                p0?.let { webrtcClient.sendIceCandidate(it, target) }
            }
            override fun onConnectionChange(newState: PeerConnection.PeerConnectionState?) {
                // 处理连接状态变化的逻辑
                super.onConnectionChange(newState)
                Log.d("TAG", "onConnectionChange: $newState")
                if (newState == PeerConnection.PeerConnectionState.CONNECTED){
                    // 已连接 显示disconnectBtn
                    listener?.onConnectionConnected()
                }
            }
            override fun onAddStream(p0: MediaStream?) {
                // // 当远程流（音频或视频）被添加到 PeerConnection 时，将调用此方法。可以在此方法中处理远程流的渲染和显示等操作。
                super.onAddStream(p0)
                Log.d("TAG", "onAddStream: $p0")
                // 显示surfaceview
                p0?.let { listener?.onRemoteStreamAdded(it) }
            }
        })
}
```

WebrtcClient

```kotlin
fun initializeWebrtcClient(
    username: String, view: SurfaceViewRenderer, observer: Observer
) {
    this.username = username
    this.observer = observer
    peerConnection = createPeerConnection(observer)
    initSurfaceView(view)
}
```

```kotlin
private val iceServer = listOf(
    PeerConnection.IceServer(
        "turn:openrelay.metered.ca:443?transport=tcp", "openrelayproject", "openrelayproject"
    )
)

private fun createPeerConnection(observer: Observer): PeerConnection? {
    // 创建 PeerConnection 对象
    return peerConnectionFactory.createPeerConnection(
        
        // ICE（Interactive Connectivity Establishment）服务器的配置信息。ICE 用于在对等连接中建立网络连接，并处理 NAT（Network Address Translation）和防火墙等网络障碍。iceServer 包含 ICE 服务器的 URL、用户名和密码等信息。
        
        // 一个观察者对象，用于监听 PeerConnection 的状态和事件。该观察者对象需要实现 PeerConnection.Observer 接口，并提供相应的回调方法，以便在 PeerConnection 的状态发生变化时进行处理。
        iceServer, observer
    )
}
```

```kotlin
// PeerConnectionFactory 用于建立点对点（或多对多）的实时通信连接，支持音频、视频和数据的传输。
private val peerConnectionFactory by lazy { createPeerConnectionFactory() }

private fun createPeerConnectionFactory(): PeerConnectionFactory {
    return PeerConnectionFactory.builder().setVideoDecoderFactory(
        	// 设置视频解码器, 'com.mesibo.api:webrtc:1.0.5'提供
            DefaultVideoDecoderFactory(eglBaseContext)
        ).setVideoEncoderFactory(
        	// 设置视频编码器, 'com.mesibo.api:webrtc:1.0.5'提供
            DefaultVideoEncoderFactory(
                eglBaseContext, true, true
            )
        ).setOptions(PeerConnectionFactory.Options().apply {
        	// 不禁用加密。
            disableEncryption = false
        	// 不禁用网络监控。
            disableNetworkMonitor = false
        }).createPeerConnectionFactory()
}
```

```kotlin
// org.webrtc.SurfaceViewRenderer; , 'com.mesibo.api:webrtc:1.0.5'提供
// SurfaceViewRenderer 用于在 Android 平台上进行实时音视频通信的图像渲染和显示。
private fun initSurfaceView(view: SurfaceViewRenderer){
    this.localSurfaceView = view
    view.run {
        // 不对图像进行镜像翻转
        setMirror(false)
        // 启用硬件缩放器
        setEnableHardwareScaler(true)
        // 初始化
        init(eglBaseContext,null)
    }
}
```

观察者方法

```kotlin
open class MyPeerObserver : PeerConnection.Observer{
    override fun onSignalingChange(p0: PeerConnection.SignalingState?) {
        // 当信令状态发生变化时，将调用此方法。可以在此方法中处理信令状态的相关逻辑。
    }

    override fun onIceConnectionChange(p0: PeerConnection.IceConnectionState?) {
        // 当 ICE 连接状态发生变化时，将调用此方法。可以在此方法中处理 ICE 连接状态的相关逻辑，如连接建立或断开等。
    }

    override fun onIceConnectionReceivingChange(p0: Boolean) {
        // 当 ICE 连接接收状态发生变化时，将调用此方法。可以在此方法中处理 ICE 连接接收状态的相关逻辑。
    }

    override fun onIceGatheringChange(p0: PeerConnection.IceGatheringState?) {
        // 当 ICE 收集状态发生变化时，将调用此方法。
    }

    override fun onIceCandidate(p0: IceCandidate?) {
        // 当本地 ICE 候选项可用时，将调用此方法。ICE 候选项用于建立对等连接，并包含网络地址和协议等信息。
    }

    override fun onIceCandidatesRemoved(p0: Array<out IceCandidate>?) {
        // 当 ICE 候选项被移除时，将调用此方法。可以在此方法中处理移除 ICE 候选项的逻辑。
    }

    override fun onAddStream(p0: MediaStream?) {
        // 当远程流（音频或视频）被添加到 PeerConnection 时，将调用此方法。可以在此方法中处理远程流的渲染和显示等操作。
    }

    override fun onRemoveStream(p0: MediaStream?) {
        // 当远程流（音频或视频）被移除时，将调用此方法。可以在此方法中处理远程流的停止渲染和显示等操作。
    }

    override fun onDataChannel(p0: DataChannel?) {
        // 当数据通道被打开时，将调用此方法。可以在此方法中处理数据通道的相关逻辑，如发送和接收数据等。
    }

    override fun onRenegotiationNeeded() {
        // 当需要重新协商连接时，将调用此方法。可以在此方法中处理重新协商连接的逻辑。
    }

    override fun onAddTrack(p0: RtpReceiver?, p1: Array<out MediaStream>?) {
        // 当远程轨道（音频或视频）被添加到 PeerConnection 时，将调用此方法。
    }
}
```





## 请求流程(点击Request)

```kotlin
private fun startScreenCapture(){
    val mediaProjectionManager = application.getSystemService(
        Context.MEDIA_PROJECTION_SERVICE
    ) as MediaProjectionManager
    startActivityForResult(
        mediaProjectionManager.createScreenCaptureIntent(), capturePermissionRequestCode
    )
}

override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
    super.onActivityResult(requestCode, resultCode, data)
    if (requestCode != capturePermissionRequestCode) return
    WebrtcService.screenPermissionIntent = data
    webrtcServiceRepository.requestConnection(
        views.targetEt.text.toString()
    )
}
```

```kotlin
fun requestConnection(target: String){
    val thread = Thread {
        val startIntent = Intent(context, WebrtcService::class.java)
        startIntent.action = "RequestConnectionIntent"
        startIntent.putExtra("target",target)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O){
            context.startForegroundService(startIntent)
        } else {
            context.startService(startIntent)
        }
    }
    thread.start()
}
```

```kotlin
"RequestConnectionIntent"->{
    val target= intent.getStringExtra("target")
    target?.let {
        mainRepository.setPermissionIntentToWebrtcClient(screenPermissionIntent!!)
        mainRepository.startScreenCapturing(surfaceView!!)
        // 发送socket消息
        mainRepository.sendScreenShareConnection(it)
    }
}
```

发送屏幕分享消息

```kotlin
fun sendScreenShareConnection(target: String){
    socketClient.sendMessageToSocket(
        DataModel(
            type = StartStreaming,
            username = username,
            target = target,
            null
        )
    )
}
```





```kotlin
fun startScreenCapturing(surfaceView: SurfaceViewRenderer){
    webrtcClient.startScreenCapturing(surfaceView)
}
```

```kotlin
// VideoSource 该对象用于捕获视频数据并生成 VideoTrack，以便进行视频通信或录制等操作。
private val localVideoSource by lazy { peerConnectionFactory.createVideoSource(false) }

fun startScreenCapturing(view:SurfaceViewRenderer){
    // 获取屏幕尺寸
    val displayMetrics = DisplayMetrics()
    val windowsManager = context.getSystemService(Context.WINDOW_SERVICE) as WindowManager
    windowsManager.defaultDisplay.getMetrics(displayMetrics)
    val screenWidthPixels = displayMetrics.widthPixels
    val screenHeightPixels = displayMetrics.heightPixels
    
    // 该对象用于处理 SurfaceTexture 的相关操作，例如渲染视频帧。
    val surfaceTextureHelper = SurfaceTextureHelper.create(
        Thread.currentThread().name,eglBaseContext
    )
    // 创建视频捕获
    screenCapturer = createScreenCapturer()
    // 初始化,surfaceTextureHelper 用于处理捕获到的屏幕内容的渲染和相关操作。
    // localVideoSource.capturerObserver 用于监听屏幕捕获的状态变化和视频帧的传输。
    screenCapturer!!.initialize(surfaceTextureHelper,context,localVideoSource.capturerObserver)
    // 开始屏幕捕获, 屏幕捕获的宽度、屏幕捕获的高度、每秒捕获的视频帧数
    screenCapturer!!.startCapture(screenWidthPixels,screenHeightPixels,15)
    /*
    	创建 VideoTrack 对象，该对象用于处理和传输视频数据。
    	参数1: 用于标识该视频轨道的唯一 ID。
    	参数2: VideoSource 对象，用于提供视频数据给视频轨道。
    */
    localVideoTrack = peerConnectionFactory.createVideoTrack(localTrackId+"_video",localVideoSource)
    // 将 view 作为渲染目标（sink）添加到 localVideoTrack 的方法调用。
    localVideoTrack?.addSink(view)
    // LocalMediaStream 对象用于管理本地媒体流中的音频和视频轨道。
    localStream = peerConnectionFactory.createLocalMediaStream(localStreamId)
    // 将本地音视频轨道添加到本地媒体流，以便在音视频通信中发送本地视频数据。
    localStream?.addTrack(localVideoTrack)
    // 将该流添加到对等连接（PeerConnection），以便与远程端进行音视频通信，并使远程端能够接收和播放本地音视频数据。
    peerConnection?.addStream(localStream)
}
```



```kotlin
private fun createScreenCapturer(): VideoCapturer {
    // 用于在 Android 设备上捕获屏幕内容并生成视频帧。它是用于屏幕共享和屏幕录制等功能的关键组件之一。
    // permissionIntent 参数，用于请求屏幕捕获的权限。
    return ScreenCapturerAndroid(permissionIntent, object : MediaProjection.Callback() {
        // 当屏幕捕获停止时，onStop() 方法会被调用
        override fun onStop() {
            super.onStop()
            Log.d("TAG", "onStop: stopped screen casting permission")
        }
    })
}
```





## 接收流程(接收Request)

MainRepository

```kotlin
override fun onNewMessageReceived(model: DataModel) {
    when (model.type) {
        // 点击Request接收消息
        StartStreaming -> {
            this.target = model.username
            // 刷新UI，让对方点击是否同意
            listener?.onConnectionRequestReceived(model.username)
        }
        // ...
        else -> Unit
    }
}
```

WebrtcService

```kotlin
override fun onCreate() {
    super.onCreate()
    mainRepository.listener = this
}
```



```kotlin
override fun onConnectionRequestReceived(target: String) {
    listener?.onConnectionRequestReceived(target)
}
```



MainActivity

```kotlin
private fun init(){
	WebrtcService.listener = this
}

override fun onConnectionRequestReceived(target: String) {
    runOnUiThread{
        views.apply {
            notificationTitle.text = "$target is requesting for connection"
            notificationLayout.isVisible = true
            notificationAcceptBtn.setOnClickListener {
                // 同意点击
                webrtcServiceRepository.acceptCAll(target)
                notificationLayout.isVisible = false
            }
            notificationDeclineBtn.setOnClickListener {
                notificationLayout.isVisible = false
            }
        }
    }
}
```



## 同意流程(点击Accept)

```kotlin
fun acceptCAll(target:String){
    val thread = Thread {
        val startIntent = Intent(context, WebrtcService::class.java)
        startIntent.action = "AcceptCallIntent"
        startIntent.putExtra("target",target)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(startIntent)
        } else {
            context.startService(startIntent)
        }
    }
    thread.start()
}
```

```kotlin
"AcceptCallIntent"->{
    val target = intent.getStringExtra("target")
    target?.let {
        mainRepository.startCall(it)
    }
}
```

```kotlin
fun startCall(target: String){
    webrtcClient.call(target)
}
```

```kotlin
private val mediaConstraint = MediaConstraints().apply {
    mandatory.add(MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true"))
}

fun call(target: String) {
    // Offer 描述了当前浏览器可以提供的媒体流和连接配置。
    peerConnection?.createOffer(object : MySdpObserver() {
        override fun onCreateSuccess(desc: SessionDescription?) {
            super.onCreateSuccess(desc)
            peerConnection?.setLocalDescription(object : MySdpObserver() {
                override fun onSetSuccess() {
                    super.onSetSuccess()
                    // 点击同意后，发送Offer消息
                    /*
                    v=0
o=- 4760967798545989454 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE video
a=msid-semantic: WMS
m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 125 100 101 127
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:8mJN
a=ice-pwd:bPu/73Je0ej8iY1o0UhQRSMg
a=ice-options:trickle renomination
a=fingerprint:sha-256 5A:3E:F6:2B:5A:87:22:FF:70:39:E8:B6:50:D3:FE:36:12:33:80:58:86:01:8C:5D:93:EA:A8:95:CB:73:5E:3A
a=setup:actpass
a=mid:video
a=extmap:14 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:13 urn:3gpp:video-orientation
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07
a=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=recvonly
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 goog-remb
a=rtcp-fb:96 transport-cc
a=rtcp-fb:96 ccm fir
a=rtcp-fb:96 nack
a=rtcp-fb:96 nack pli
a=rtpmap:97 rtx/90000
a=fmtp:97 apt=96
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:125 H264/90000
a=rtcp-fb:125 goog-remb
a=rtcp-fb:125 transport-cc
a=rtcp-fb:125 ccm fir
a=rtcp-fb:125 nack
a=rtcp-fb:125 nack pli
a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:100 red/90000
a=rtpmap:101 rtx/90000
a=fmtp:101 apt=100
a=rtpmap:127 ulpfec/90000

                    */
                    listener?.onTransferEventToSocket(
                        DataModel(
                            type = DataModelType.Offer, username, target, desc?.description
                        )
                    )
                }
            }, desc)
        }
    }, mediaConstraint)
}
```

```kotlin
override fun onTransferEventToSocket(data: DataModel) {
    socketClient.sendMessageToSocket(data)
}
```

```kotlin
fun sendMessageToSocket(message:Any?){
    try {
        webSocket?.send(gson.toJson(message))
    }catch (e:Exception){
        e.printStackTrace()
    }
}
```

接收消息

```kotlin
override fun onNewMessageReceived(model: DataModel) {
    when (model.type) {
        // 对方点击同意后，接收到此消息,然后发送Answer消息
        Offer -> {
            webrtcClient.onRemoteSessionReceived(
                SessionDescription(
                    // 表示SDP消息的类型为Offer,Offer是由发起方创建的SDP消息，描述了发起方可提供的媒体流和连接配置。
                    SessionDescription.Type.OFFER, model.data
                        .toString()
                )
            )
            this.target = model.username
            // 发送answer消息
            webrtcClient.answer(target)
        }
        // 接收到的Answer消息
        Answer -> {
            /*
v=0
o=- 3891713929923074185 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE video
a=msid-semantic: WMS local_stream
m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 125 100 101 127
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:b6Uh
a=ice-pwd:vUex+8JwXR6xY5g4K0zmHO4q
a=ice-options:trickle renomination
a=fingerprint:sha-256 3D:4F:AD:6E:09:8B:46:C4:CB:EE:7B:AC:1A:72:17:6C:88:F5:0F:B6:31:F9:07:6D:A5:71:00:68:B6:E8:04:99
a=setup:active
a=mid:video
a=extmap:14 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:13 urn:3gpp:video-orientation
a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07
a=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=sendonly
a=rtcp-mux
a=rtcp-rsize
a=rtpmap:96 VP8/90000
a=rtcp-fb:96 goog-remb
a=rtcp-fb:96 transport-cc
a=rtcp-fb:96 ccm fir
a=rtcp-fb:96 nack
a=rtcp-fb:96 nack pli
a=rtpmap:97 rtx/90000
a=fmtp:97 apt=96
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:125 H264/90000
a=rtcp-fb:125 goog-remb
a=rtcp-fb:125 transport-cc
a=rtcp-fb:125 ccm fir
a=rtcp-fb:125 nack
a=rtcp-fb:125 nack pli
a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:100 red/90000
a=rtpmap:101 rtx/90000
a=fmtp:101 apt=100
a=rtpmap:127 ulpfec/90000
a=ssrc-group:FID 3790643291 3832602576
a=ssrc:3790643291 cname:qrHYJgfAHGB/dK9b
a=ssrc:3790643291 msid:local_stream local_track_video
a=ssrc:3790643291 mslabel:local_stream
a=ssrc:3790643291 label:local_track_video
a=ssrc:3832602576 cname:qrHYJgfAHGB/dK9b
a=ssrc:3832602576 msid:local_stream local_track_video
a=ssrc:3832602576 mslabel:local_stream
a=ssrc:3832602576 label:local_track_video

            */
            webrtcClient.onRemoteSessionReceived(
                // 表示SDP消息的类型为Answer（应答）,Answer是由接收方创建的SDP消息，作为对收到的Offer进行回应，描述了接收方可接受的媒体流和连接配置。
                SessionDescription(SessionDescription.Type.ANSWER, model.data.toString())
            )
        }
    }
}
```



```kotlin
fun answer(target: String) {
    peerConnection?.createAnswer(object : MySdpObserver() {
        override fun onCreateSuccess(desc: SessionDescription?) {
            super.onCreateSuccess(desc)
            peerConnection?.setLocalDescription(object : MySdpObserver() {
                override fun onSetSuccess() {
                    super.onSetSuccess()
                    // 发送answer消息
                    listener?.onTransferEventToSocket(
                        DataModel(
                            type = DataModelType.Answer,
                            username = username,
                            target = target,
                            data = desc?.description
                        )
                    )
                }
            }, desc)
        }
    }, mediaConstraint)
}
```



## 服务端转发流程

