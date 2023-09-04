# DDSWebRtcDemo

## 视频通话流程(A -> B)

当用户A拨打用户B视频



```java
初始化
SkyEngineKit.init(new VoipEvent());
gEngineKit = SkyEngineKit.Instance();

请求权限...

添加Video/Audio fragment



// 创建会话
room = UUID.randomUUID().toString() + System.currentTimeMillis();
// 开始呼叫
gEngineKit.startOutCall(getApplicationContext(), room, targetId, audioOnly);

App.getInstance().setRoomId(room);
App.getInstance().setOtherUserId(targetId);
CallSession session = gEngineKit.getCurrentSession();
session.setSessionCallback(this);

```

### 开始呼叫

```java
public boolean startOutCall(Context context, final String room, final String targetId,
                            final boolean audioOnly) {
    // 未初始化
    if (avEngineKit == null) {
        Log.e(TAG, "startOutCall error,please init first");
        return false;
    }
    // 忙线中
    if (mCurrentCallSession != null && mCurrentCallSession.getState() != EnumType.CallState.Idle) {
        Log.i(TAG, "startCall error,currentCallSession is exist");
        return false;
    }
    isAudioOnly = audioOnly;
    // 初始化会话
    mCurrentCallSession = new CallSession(context, room, audioOnly, mEvent);
    mCurrentCallSession.setTargetId(targetId);
    mCurrentCallSession.setIsComing(false);
    mCurrentCallSession.setCallState(EnumType.CallState.Outgoing);
    // 创建房间
    mCurrentCallSession.createHome(room, 2);


    return true;
}
```



### 初始化会话

```java
public CallSession(Context context, String roomId, boolean audioOnly, ISkyEvent event) {
    executor = Executors.newSingleThreadExecutor();
    this.mIsAudioOnly = audioOnly;
    this.mRoomId = roomId;
    this.mEvent = event;
    iEngine = AVEngine.createEngine(new WebRTCEngine(audioOnly, context));
    iEngine.init(this);
}
```

```java
public WebRTCEngine(boolean mIsAudioOnly, Context mContext) {
    this.mIsAudioOnly = mIsAudioOnly;
    this.mContext = mContext;
    audioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
    // 初始化ice地址
    initIceServer();
}
```

```java
// 服务器实例列表
private final List<PeerConnection.IceServer> iceServers = new ArrayList<>();

private void initIceServer() {
    // 初始化一些stun和turn的地址
    PeerConnection.IceServer var1 = PeerConnection.IceServer.builder("stun:stun.l.google.com:19302")
            .createIceServer();
    iceServers.add(var1);
    PeerConnection.IceServer var11 = PeerConnection.IceServer.builder("stun:42.192.40.58:3478")
            .createIceServer();
    PeerConnection.IceServer var12 = PeerConnection.IceServer.builder("turn:42.192.40.58:3478")
            .setUsername("ddssingsong")
            .setPassword("123456")
            .createIceServer();
    iceServers.add(var11);
    iceServers.add(var12);
}
```



### 创建房间

```java
public void createHome(String room, int roomSize) {
    executor.execute(() -> {
        if (mEvent != null) {
            // VoipEvent.createRoom
            mEvent.createRoom(room, roomSize);
        }
    });
}
```

```java
public void createRoom(String room, int roomSize) {
    SocketManager.getInstance().createRoom(room, roomSize);
}
```

```java
public void createRoom(String room, int roomSize) {
    if (webSocket != null) {
        webSocket.createRoom(room, roomSize, myId);
    }
}
```

```java
public void createRoom(String room, int roomSize, String myId) {
    Map<String, Object> map = new HashMap<>();
    map.put("eventName", "__create");
    Map<String, Object> childMap = new HashMap<>();
    childMap.put("room", room);
    childMap.put("roomSize", roomSize);
    childMap.put("userID", myId);
    map.put("data", childMap);
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    
    /*
    {
        "data": {
            "room": "77223c70-a7cd-4196-93af-0d80311dca461692694145283",
            "roomSize": 2,
            "userID": "cq"
        },
        "eventName": "__create"
    }
    */
    send(jsonString);
}
```





### 服务端处理创建房间消息

```
case "__create":
	createRoom(message, data.getData());
```

```java
private void createRoom(String message, Map<String, Object> data) {
    String room = (String) data.get("room");
    String userId = (String) data.get("userID");

    System.out.println(String.format("createRoom:%s ", room));

    RoomInfo roomParam = rooms.get(room);
    // 没有这个房间
    if (roomParam == null) {
        int size = (int) Double.parseDouble(String.valueOf(data.get("roomSize")));
        // 创建房间
        RoomInfo roomInfo = new RoomInfo();
        roomInfo.setMaxSize(size);
        roomInfo.setRoomId(room);
        roomInfo.setUserId(userId);
        // 将房间储存起来
        rooms.put(room, roomInfo);


        CopyOnWriteArrayList<UserBean> copy = new CopyOnWriteArrayList<>();
        // 将自己加入到房间里
        UserBean my = MemCons.userBeans.get(userId);
        copy.add(my);
        rooms.get(room).setUserBeans(copy);

        // 发送__peers进入房间消息给自己
        EventData send = new EventData();
        send.setEventName("__peers");
        Map<String, Object> map = new HashMap<>();
        map.put("connections", "");
        map.put("you", userId);
        map.put("roomSize", size);
        send.setData(map);
        System.out.println(gson.toJson(send));
        sendMsg(my, -1, gson.toJson(send));

    }

}
```



### 客户端进入房间消息处理

```
{
    "data": {
        "connections": "",
        "roomSize": 2,
        "you": "cq"
    },
    "eventName": "__peers"
}
```

```java
if (eventName.equals("__peers")) {
    handlePeers(map);
    return;
}
```

```java
private void handlePeers(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String you = (String) data.get("you");
        String connections = (String) data.get("connections");
        int roomSize = (int) data.get("roomSize");
        this.iEvent.onPeers(you, connections, roomSize);
    }
}
```

```java
public void onPeers(String myId, String connections, int roomSize) {
    handler.post(() -> {
        //自己进入了房间，然后开始发送offer
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.onJoinHome(myId, connections, roomSize);
        }
    });
}
```

```java
public void onJoinHome(String myId, String users, int roomSize) {
    // 开始计时
    mRoomSize = roomSize;
    startTime = 0;
    handler.post(() -> executor.execute(() -> {
        mMyId = myId;
        List<String> strings;
        if (!TextUtils.isEmpty(users)) {
            String[] split = users.split(",");
            strings = Arrays.asList(split);
            mUserIDList = strings;
        }

        // 发送邀请
        if (!mIsComing) {
            if (roomSize == 2) {
                List<String> inviteList = new ArrayList<>();
                inviteList.add(mTargetId);
                // ====================此时执行在此处
                mEvent.sendInvite(mRoomId, inviteList, mIsAudioOnly);
            }
        } else {
            iEngine.joinRoom(mUserIDList);
        }

        if (!isAudioOnly()) {
            // 画面预览
            if (sessionCallback != null && sessionCallback.get() != null) {
                sessionCallback.get().didCreateLocalVideoTrack();
            }
        }
    }));
}
```



### 发送邀请(A邀请B)

```json
{
    "data": {
        "audioOnly": false,
        "inviteID": "cq",
        "room": "5a34485a-4dcd-4800-9581-1457df3844d71692694622824",
        "userList": "gq"
    },
    "eventName": "__invite"
}
```



```java
public void sendInvite(String room, List<String> userIds, boolean audioOnly) {
    SocketManager.getInstance().sendInvite(room, userIds, audioOnly);
}
```

```java
public void sendInvite(String room, List<String> users, boolean audioOnly) {
    if (webSocket != null) {
        webSocket.sendInvite(room, myId, users, audioOnly);
    }
}
```

```java
public void sendInvite(String room, String myId, List<String> users, boolean audioOnly) {
    Map<String, Object> map = new HashMap<>();
    map.put("eventName", "__invite");

    Map<String, Object> childMap = new HashMap<>();
    childMap.put("room", room);
    childMap.put("audioOnly", audioOnly);
    childMap.put("inviteID", myId);

    String join = StringUtil.listToString(users);
    childMap.put("userList", join);

    map.put("data", childMap);
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    send(jsonString);
}
```



### 服务端处理发送邀请

```
case "__invite":
    invite(message, data.getData());
    break;
```

```java
private void invite(String message, Map<String, Object> data) {
    String userList = (String) data.get("userList");
    String room = (String) data.get("room");
    String inviteId = (String) data.get("inviteID");
    boolean audioOnly = (boolean) data.get("audioOnly");
    String[] users = userList.split(",");
    System.out.println(String.format("room:%s,%s invite %s audioOnly:%b", room, inviteId, userList, audioOnly));
    // 给其他人发送邀请
    for (String user : users) {
        UserBean userBean = MemCons.userBeans.get(user);
        if (userBean != null) {
            sendMsg(userBean, -1, message);
        }
    }
}
```



### 接收邀请(B接收邀请)

```java
if (eventName.equals("__invite")) {
    handleInvite(map);
    return;
}
```

```java
private void handleInvite(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String room = (String) data.get("room");
        boolean audioOnly = (boolean) data.get("audioOnly");
        String inviteID = (String) data.get("inviteID");
        String userList = (String) data.get("userList");
        this.iEvent.onInvite(room, audioOnly, inviteID, userList);
    }
}
```

```java
public void onInvite(String room, boolean audioOnly, String inviteId, String userList) {
    Intent intent = new Intent();
    intent.putExtra("room", room);
    intent.putExtra("audioOnly", audioOnly);
    intent.putExtra("inviteId", inviteId);
    intent.putExtra("userList", userList);
    intent.setAction(Consts.ACTION_VOIP_RECEIVER);
    intent.setComponent(new ComponentName(App.getInstance().getPackageName(), VoipReceiver.class.getName()));
    // 发送广播
    App.getInstance().sendBroadcast(intent);
}
```

广播接收

```java
public void onReceive(Context context, Intent intent) {
    String action = intent.getAction();
    if (Consts.ACTION_VOIP_RECEIVER.equals(action)) {
        String room = intent.getStringExtra("room");
        boolean audioOnly = intent.getBooleanExtra("audioOnly", true);
        String inviteId = intent.getStringExtra("inviteId");
        String inviteUserName = intent.getStringExtra("inviteUserName");
        String userList = intent.getStringExtra("userList");
        String[] list = userList.split(",");
        SkyEngineKit.init(new VoipEvent());
        //todo 处理邀请人名称
        if (inviteUserName == null) {
            inviteUserName = "p2pChat";
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (com.dds.core.util.Utils.isAppRunningForeground()) {
                onForegroundOrBeforeVersionO(room, userList, inviteId, audioOnly, inviteUserName, true);
            } else {
                onBackgroundAfterVersionO(room, userList, inviteId, audioOnly, inviteUserName);
            }
        } else {
            onForegroundOrBeforeVersionO(
                    room,
                    userList,
                    inviteId,
                    audioOnly,
                    inviteUserName,
                    com.dds.core.util.Utils.isAppRunningForeground()
            );
        }
    }
}
```



```java
private void onForegroundOrBeforeVersionO(
        String room, String userList,
        String inviteId, Boolean audioOnly, String inviteUserName, Boolean isForeGround
) {
    String[] strArr = userList.split(",");
    ArrayList<String> list = new ArrayList<>();
    for (String str : strArr)
        list.add(str);
    SkyEngineKit.init(new VoipEvent());
    BaseActivity activity = (BaseActivity) ActivityStackManager.getInstance().getTopActivity();
    // 权限检测
    String[] per;
    if (audioOnly) {
        per = new String[]{Manifest.permission.RECORD_AUDIO};
    } else {
        per = new String[]{Manifest.permission.RECORD_AUDIO, Manifest.permission.CAMERA};
    }
    boolean hasPermission = Permissions.has(activity, per);
    Log.d(TAG, "onForegroundOrBeforeVersionO hasPermission = " + hasPermission + ", isForeGround = " + isForeGround);
    if (hasPermission) {
        onHasPermission(activity, room, list, inviteId, audioOnly, inviteUserName);
    } else {

        ringPlayer = new AsyncPlayer(null);
        shouldStartRing(true); //来电先响铃
        if (isForeGround) {
            Alerter.create(activity).setTitle("来电通知")
                    .setText(
                            "您收到" + inviteUserName + "的来电邀请，请允许"
                                    + (audioOnly ? "录音"
                                    : "录音和相机") + "权限来通话"
                    )
                    .enableSwipeToDismiss()
                    .setBackgroundColorRes(R.color.colorAccent) // or setBackgroundColorInt(Color.CYAN)
                    .setDuration(60 * 1000)
                    .addButton("确定", R.style.AlertButtonBgWhite, v -> {
                        Permissions.request(activity, per, integer -> {
                            shouldStopRing();
                            Log.d(TAG, "Permissions.request integer = " + integer);
                            if (integer == 0) { //权限同意
                                onHasPermission(activity, room, list, inviteId, audioOnly, inviteUserName);
                            } else {
                                onPermissionDenied(room, inviteId);
                            }
                            Alerter.hide();
                        });
                    })
                    .addButton("取消", R.style.AlertButtonBgWhite, v -> {
                        shouldStopRing();
                        onPermissionDenied(room, inviteId);
                        Alerter.hide();
                    }).show();
        } else {
            CallForegroundNotification notification = new CallForegroundNotification(App.getInstance());
            notification.sendRequestIncomingPermissionsNotification(
                    activity,
                    room,
                    userList,
                    inviteId,
                    inviteUserName,
                    audioOnly
            );
        }

    }
}
```



```java
private void onHasPermission(
        Context context, String room, ArrayList<String> list,
        String inviteId, Boolean audioOnly, String inviteUserName
) {
    boolean b = SkyEngineKit.Instance().startInCall(App.getInstance(), room, inviteId, audioOnly);
    Log.d(TAG, "onHasPermission b = " + b);
    if (b) {
        App.getInstance().setOtherUserId(inviteId);
        Log.d(TAG, "onHasPermission list.size() = " + list.size());
        if (list.size() == 1) {
            //以视频电话拨打，切换到音频或重走这里，结束掉上一个，防止对方挂断后，下边还有一个通话界面
            if (context instanceof CallSingleActivity) {
                ((CallSingleActivity) context).finish();
            }
            // ========打开通话页面
            CallSingleActivity.openActivity(context, inviteId, false, inviteUserName, audioOnly, true);
        } else {
            // 群聊
        }
    } else {
        Activity activity = ActivityStackManager.getInstance().getTopActivity();
        activity.finish(); //销毁掉刚才拉起的
    }
}
```



```java
// 接听电话
public boolean startInCall(Context context, final String room, final String targetId,
                           final boolean audioOnly) {
    if (avEngineKit == null) {
        Log.e(TAG, "startInCall error,init is not set");
        return false;
    }
    // 忙线中
    if (mCurrentCallSession != null && mCurrentCallSession.getState() != EnumType.CallState.Idle) {
        // 发送->忙线中...
        Log.i(TAG, "startInCall busy,currentCallSession is exist,start sendBusyRefuse!");
        mCurrentCallSession.sendBusyRefuse(room, targetId);
        return false;
    }
    this.isAudioOnly = audioOnly;
    // 初始化会话
    mCurrentCallSession = new CallSession(context, room, audioOnly, mEvent);
    mCurrentCallSession.setTargetId(targetId);
    mCurrentCallSession.setIsComing(true);
    mCurrentCallSession.setCallState(EnumType.CallState.Incoming);

    // 开始响铃并回复
    mCurrentCallSession.shouldStartRing(); // 播放响铃
    mCurrentCallSession.sendRingBack(targetId, room);


    return true;
}
```



```java
public void sendRingBack(String targetId, String room) {
    executor.execute(() -> {
        if (mEvent != null) {
            mEvent.sendRingBack(targetId, room);
        }
    });
}
```

```java
public void sendRingBack(String targetId, String room) {
    SocketManager.getInstance().sendRingBack(targetId, room);
}
```

```java
public void sendRingBack(String targetId, String room) {
    if (webSocket != null) {
        webSocket.sendRing(myId, targetId, room);
    }
}
```

```java
public void sendRing(String myId, String toId, String room) {
    Map<String, Object> map = new HashMap<>();
    map.put("eventName", "__ring");

    Map<String, Object> childMap = new HashMap<>();
    childMap.put("fromID", myId);
    childMap.put("toID", toId);
    childMap.put("room", room);


    map.put("data", childMap);
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    send(jsonString);
}
```



```json
{
    "data": {
        "fromID": "cq",
        "room": "0c7271a1-1e1d-4948-94d6-016f596e4a5f1692757300432",
        "toID": "gq"
    },
    "eventName": "__ring"
}
```



### 服务端处理响铃消息

```
case "__ring":
    ring(message, data.getData());
    break;
```

```java
private void ring(String message, Map<String, Object> data) {
    String room = (String) data.get("room");
    String inviteId = (String) data.get("toID");
    UserBean userBean = MemCons.userBeans.get(inviteId);
    if (userBean != null) {
        sendMsg(userBean, -1, message);
    }
}
```



### 发送端处理响铃(A处理响铃消息)

```json
{
    "data": {
        "fromID": "gq",
        "room": "3f6e778f-022b-43e3-bebe-d8ecb9ac24611692758115494",
        "toID": "cq"
    },
    "eventName": "__ring"
}
```

```java
// 响铃
if (eventName.equals("__ring")) {
    handleRing(map);
    return;
}
```

```java
private void handleRing(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String fromId = (String) data.get("fromID");
        this.iEvent.onRing(fromId);
    }
}
```

```java
public void onRing(String fromId) {
    handler.post(() -> {
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.onRingBack(fromId);
        }
    });
}
```

```java
public void onRingBack(String userId) {
    if (mEvent != null) {
        mEvent.onRemoteRing();
        //mEvent.shouldStartRing(false);
    }
}
```

```java
@Override
public void onRemoteRing() {
}
```



## 接听流程(B接听A)

### 点击接听加入房间(B加入房间)

```java
// 接听
CallSession session = gEngineKit.getCurrentSession();
if (id == R.id.acceptImageView) {
    if (session != null && session.getState() == CallState.Incoming) {
        session.joinHome(session.getRoomId());
    } else if (session != null) {
        if (callSingleActivity != null) {
            session.sendRefuse();
            callSingleActivity.finish();
        }
    }
}
```

```java
public void joinHome(String roomId) {
    executor.execute(() -> {
        _callState = EnumType.CallState.Connecting;
        Log.d(TAG, "joinHome mEvent = " + mEvent);
        setIsComing(true);
        if (mEvent != null) {
            mEvent.sendJoin(roomId);
        }
    });
}
```

```java
public void sendJoin(String room) {
    SocketManager.getInstance().sendJoin(room);
}
```

```java
public void sendJoin(String room) {
    if (webSocket != null) {
        webSocket.sendJoin(room, myId);
    }
}
```

```java
public void sendJoin(String room, String myId) {
    Map<String, Object> map = new HashMap<>();
    map.put("eventName", "__join");
    Map<String, String> childMap = new HashMap<>();
    childMap.put("room", room);
    childMap.put("userID", myId);
    map.put("data", childMap);
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    send(jsonString);
}
```

```json
{
    "data": {
        "room": "f890ff5e-1d94-4bf8-866c-0a05288987f11692760821384",
        "userID": "cq"
    },
    "eventName": "__join"
}
```



### 服务端处理加入房间消息

```
case "__join":
    join(message, data.getData());
    break;
```

```java
private void join(String message, Map<String, Object> data) {
    String room = (String) data.get("room");
    String userID = (String) data.get("userID");

    RoomInfo roomInfo = rooms.get(room);

    int maxSize = roomInfo.getMaxSize();
    CopyOnWriteArrayList<UserBean> roomUserBeans = roomInfo.getUserBeans();

    //房间已经满了
    if (roomUserBeans.size() >= maxSize) {
        return;
    }
    UserBean my = MemCons.userBeans.get(userID);
    // 1. 將我加入到房间
    roomUserBeans.add(my);
    roomInfo.setUserBeans(roomUserBeans);
    rooms.put(room, roomInfo);

    // 2. 返回房间里的所有人信息
    EventData send = new EventData();
    send.setEventName("__peers");
    Map<String, Object> map = new HashMap<>();

    String[] cons = new String[roomUserBeans.size()];
    for (int i = 0; i < roomUserBeans.size(); i++) {
        UserBean userBean = roomUserBeans.get(i);
        if (userBean.getUserId().equals(userID)) {
            continue;
        }
        cons[i] = userBean.getUserId();
    }
    StringBuilder sb = new StringBuilder();
    for (int i = 0; i < cons.length; i++) {
        if (cons[i] == null) {
            continue;
        }
        sb.append(cons[i]).append(",");
    }
    if (sb.length() > 0) {
        sb.deleteCharAt(sb.length() - 1);
    }

    // 给自己发送消息
    map.put("connections", sb.toString());
    map.put("you", userID);
    map.put("roomSize", roomInfo.getMaxSize());
    send.setData(map);
    sendMsg(my, -1, gson.toJson(send));


    // 3. 给房间里的其他人发送消息
    EventData newPeer = new EventData();
    newPeer.setEventName("__new_peer");
    Map<String, Object> sendMap = new HashMap<>();
    sendMap.put("userID", userID);
    newPeer.setData(sendMap);
    for (UserBean userBean : roomUserBeans) {
        if (userBean.getUserId().equals(userID)) {
            continue;
        }
        sendMsg(userBean, -1, gson.toJson(newPeer));
    }
}
```



### 处理自己进入房间消息(B进入房间 joinRoom)



```
{
    "data": {
        "connections": "gq",
        "roomSize": 2,
        "you": "cq"
    },
    "eventName": "__peers"
}
```

```java
if (eventName.equals("__peers")) {
    handlePeers(map);
    return;
}
```

```java
private void handlePeers(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String you = (String) data.get("you");
        String connections = (String) data.get("connections");
        int roomSize = (int) data.get("roomSize");
        this.iEvent.onPeers(you, connections, roomSize);
    }
}
```

```java
public void onPeers(String myId, String connections, int roomSize) {
    handler.post(() -> {
        //自己进入了房间，然后开始发送offer
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.onJoinHome(myId, connections, roomSize);
        }
    });
}
```

```java
public void onJoinHome(String myId, String users, int roomSize) {
    // 开始计时
    mRoomSize = roomSize;
    startTime = 0;
    handler.post(() -> executor.execute(() -> {
        mMyId = myId;
        List<String> strings;
        if (!TextUtils.isEmpty(users)) {
            String[] split = users.split(",");
            strings = Arrays.asList(split);
            mUserIDList = strings;
        }

        // 发送邀请
        if (!mIsComing) {
            if (roomSize == 2) {
                List<String> inviteList = new ArrayList<>();
                inviteList.add(mTargetId);
                mEvent.sendInvite(mRoomId, inviteList, mIsAudioOnly);
            }
        } else {
            // 此时执行在此处
            iEngine.joinRoom(mUserIDList);
        }

        if (!isAudioOnly()) {
            // 画面预览
            if (sessionCallback != null && sessionCallback.get() != null) {
                sessionCallback.get().didCreateLocalVideoTrack();
            }
        }
    }));
}
```

```java
public void joinRoom(List<String> userIds) {
    if (iEngine == null) {
        return;
    }
    iEngine.joinRoom(userIds);
}
```

```java
public void joinRoom(List<String> userIds) {
    Log.d(TAG, "joinRoom: " + userIds.toString());
    for (String id : userIds) {
        // create Peer
        Peer peer = new Peer(_factory, iceServers, id, this);
        peer.setOffer(false);
        // add localStream
        // peer.addLocalStream(_localStream);
        List<String> mediaStreamLabels = Collections.singletonList("ARDAMS");
        if (_localVideoTrack != null) {
            peer.addVideoTrack(_localVideoTrack, mediaStreamLabels);
        }
        if (_localAudioTrack != null) {
            peer.addAudioTrack(_localAudioTrack, mediaStreamLabels);
        }
        // 添加列表
        peers.put(id, peer);
    }
    if (mCallback != null) {
        mCallback.joinRoomSucc();
    }
	
    // 是否插入耳机
    if (isHeadphonesPlugged()) {
        toggleHeadset(true);
    } else {
        if (mIsAudioOnly)
            toggleSpeaker(false);
        else {
            // 设置通信模式
            audioManager.setMode(AudioManager.MODE_IN_COMMUNICATION);
        }
    }
}
```



```java
public void joinRoomSucc() {
    // 关闭响铃
    if (mEvent != null) {
        mEvent.shouldStopRing();
    }
    // 更换界面
    _callState = EnumType.CallState.Connected;
    //Log.d(TAG, "joinRoomSucc, sessionCallback.get() = " + sessionCallback.get());
    if (sessionCallback != null && sessionCallback.get() != null) {
        startTime = System.currentTimeMillis();
        sessionCallback.get().didChangeState(_callState);
    }
}
```





### 处理房间其他人消息(A接收到新人进入房间消息 userIn)

```json
{
    "data": {
        "userID": "cq"
    },
    "eventName": "__new_peer"
}
```



```java
// 新人入房间
if (eventName.equals("__new_peer")) {
    handleNewPeer(map);
    return;
}
```

```java
private void handleNewPeer(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String userID = (String) data.get("userID");
        this.iEvent.onNewPeer(userID);
    }
}
```

```java
public void onNewPeer(String userId) {
    handler.post(() -> {
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.newPeer(userId);
        }
    });
}
```

```java
public void newPeer(String userId) {
    handler.post(() -> executor.execute(() -> {
        // 其他人加入房间
        iEngine.userIn(userId);

        // 关闭响铃
        if (mEvent != null) {
            mEvent.shouldStopRing();
        }
        // 更换界面
        _callState = EnumType.CallState.Connected;

        if (sessionCallback != null && sessionCallback.get() != null) {
            startTime = System.currentTimeMillis();
            sessionCallback.get().didChangeState(_callState);

        }
    }));

}
```

```java
public void userIn(String userId) {
    if (iEngine == null) {
        return;
    }
    iEngine.userIn(userId);
}
```

```java
public void userIn(String userId) {
    Log.d(TAG, "userIn: " + userId);
    // create Peer
    Peer peer = new Peer(_factory, iceServers, userId, this);
    peer.setOffer(true);
    // add localStream
    List<String> mediaStreamLabels = Collections.singletonList("ARDAMS");
    if (_localVideoTrack != null) {
        peer.addVideoTrack(_localVideoTrack, mediaStreamLabels);
    }
    if (_localAudioTrack != null) {
        peer.addAudioTrack(_localAudioTrack, mediaStreamLabels);
    }
    // 添加列表
    peers.put(userId, peer);
    // createOffer
    peer.createOffer();
}
```



### 创建offer(A创建offer)

```java
public void createOffer() {
    if (pc == null) return;
    Log.d(TAG, "createOffer");
    pc.createOffer(this, offerOrAnswerConstraint());
}
```

建立媒体约束

```java
private MediaConstraints offerOrAnswerConstraint() {
    MediaConstraints mediaConstraints = new MediaConstraints();
    ArrayList<MediaConstraints.KeyValuePair> keyValuePairs = new ArrayList<>();
    keyValuePairs.add(new MediaConstraints.KeyValuePair("OfferToReceiveAudio", "true"));
    keyValuePairs.add(new MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true"));
    mediaConstraints.mandatory.addAll(keyValuePairs);
    return mediaConstraints;
}
```

offer创建成功回调

```java
public void onCreateSuccess(SessionDescription origSdp) {
    Log.d(TAG, "sdp创建成功       " + origSdp.type);
    String sdpString = origSdp.description;
    final SessionDescription sdp = new SessionDescription(origSdp.type, sdpString);
    localSdp = sdp;
    setLocalDescription(sdp);
}
```

sdp设置成功回调

```java
public void onSetSuccess() {
    Log.d(TAG, "sdp连接成功   " + pc.signalingState().toString());
    if (pc == null) return;
    // 发送者
    if (isOffer) {
        if (pc.getRemoteDescription() == null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                // =====================此时执行到此处
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }
        } else {
            Log.d(TAG, "Remote SDP set succesfully");

            drainCandidates();
        }

    } else {
        if (pc.getLocalDescription() != null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }

            drainCandidates();
        } else {
            Log.d(TAG, "Remote SDP set succesfully");
        }
    }


}
```

```java
public void onSendOffer(String userId, SessionDescription description) {
    if (mCallback != null) {
        mCallback.onSendOffer(userId, description);
    }
}
```

```java
public void onSendOffer(String userId, SessionDescription description) {
    executor.execute(() -> {
        if (mEvent != null) {
            Log.d(TAG, "onSendOffer");
            mEvent.sendOffer(userId, description.description);
        }
    });
}
```

```java
public void sendOffer(String userId, String sdp) {
    SocketManager.getInstance().sendOffer(userId, sdp);
}
```

```java
public void sendOffer(String userId, String sdp) {
    if (webSocket != null) {
        webSocket.sendOffer(myId, userId, sdp);
    }
}
```

```java
public void sendOffer(String myId, String userId, String sdp) {
    Map<String, Object> map = new HashMap<>();
    Map<String, Object> childMap = new HashMap<>();
    childMap.put("sdp", sdp);
    childMap.put("userID", userId);
    childMap.put("fromID", myId);
    map.put("data", childMap);
    map.put("eventName", "__offer");
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    send(jsonString);
}
```



```json
{
    "data": {
        "fromID": "gq",
        "sdp": "v=0
o=- 2169028190071036930 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE 0 1
a=extmap-allow-mixed
a=msid-semantic: WMS ARDAMS
m=video 9 UDP/TLS/RTP/SAVPF 96 97 39 40 98 99 127 103 104 105 106 107 108
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:N/sJ
a=ice-pwd:TMlDPtUlFOpAZgPHKt9Zd6QA
a=ice-options:trickle renomination
a=fingerprint:sha-256 DB:CC:10:07:9F:54:2F:B8:D8:71:08:E8:8E:5D:A9:AE:97:B3:70:B6:2A:72:E2:4D:E5:17:98:DA:59:75:70:CE
a=setup:actpass
a=mid:0
a=extmap:1 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:3 urn:3gpp:video-orientation
a=extmap:4 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=extmap:9 urn:ietf:params:rtp-hdrext:sdes:mid
a=extmap:10 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
a=extmap:11 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
a=sendrecv
a=msid:ARDAMS ARDAMSv0
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
a=rtpmap:39 AV1/90000
a=rtcp-fb:39 goog-remb
a=rtcp-fb:39 transport-cc
a=rtcp-fb:39 ccm fir
a=rtcp-fb:39 nack
a=rtcp-fb:39 nack pli
a=rtpmap:40 rtx/90000
a=fmtp:40 apt=39
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=fmtp:98 profile-id=0
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:127 H264/90000
a=rtcp-fb:127 goog-remb
a=rtcp-fb:127 transport-cc
a=rtcp-fb:127 ccm fir
a=rtcp-fb:127 nack
a=rtcp-fb:127 nack pli
a=fmtp:127 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:103 rtx/90000
a=fmtp:103 apt=127
a=rtpmap:104 H265/90000
a=rtcp-fb:104 goog-remb
a=rtcp-fb:104 transport-cc
a=rtcp-fb:104 ccm fir
a=rtcp-fb:104 nack
a=rtcp-fb:104 nack pli
a=rtpmap:105 rtx/90000
a=fmtp:105 apt=104
a=rtpmap:106 red/90000
a=rtpmap:107 rtx/90000
a=fmtp:107 apt=106
a=rtpmap:108 ulpfec/90000
a=ssrc-group:FID 3616667243 1415571918
a=ssrc:3616667243 cname:/wlY5+fy0vt8TGWx
a=ssrc:3616667243 msid:ARDAMS ARDAMSv0
a=ssrc:1415571918 cname:/wlY5+fy0vt8TGWx
a=ssrc:1415571918 msid:ARDAMS ARDAMSv0
m=audio 9 UDP/TLS/RTP/SAVPF 111 63 9 102 0 8 13 110 126
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:N/sJ
a=ice-pwd:TMlDPtUlFOpAZgPHKt9Zd6QA
a=ice-options:trickle renomination
a=fingerprint:sha-256 DB:CC:10:07:9F:54:2F:B8:D8:71:08:E8:8E:5D:A9:AE:97:B3:70:B6:2A:72:E2:4D:E5:17:98:DA:59:75:70:CE
a=setup:actpass
a=mid:1
a=extmap:14 urn:ietf:params:rtp-hdrext:ssrc-audio-level
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:4 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:9 urn:ietf:params:rtp-hdrext:sdes:mid
a=sendrecv
a=msid:ARDAMS ARDAMSa0
a=rtcp-mux
a=rtpmap:111 opus/48000/2
a=rtcp-fb:111 transport-cc
a=fmtp:111 minptime=10;useinbandfec=1
a=rtpmap:63 red/48000/2
a=fmtp:63 111/111
a=rtpmap:9 G722/8000
a=rtpmap:102 ILBC/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:13 CN/8000
a=rtpmap:110 telephone-event/48000
a=rtpmap:126 telephone-event/8000
a=ssrc:635684422 cname:/wlY5+fy0vt8TGWx
a=ssrc:635684422 msid:ARDAMS ARDAMSa0
",
        "userID": "cq"
    },
    "eventName": "__offer"
}
```



### 服务端转发offer

```java
case "__offer":
    offer(message, data.getData());
    break;
```

```java
private void offer(String message, Map<String, Object> data) {
    String userId = (String) data.get("userID");
    UserBean userBean = MemCons.userBeans.get(userId);
    sendMsg(userBean, -1, message);
}
```



### 客户端接收offer处理(B处理接收到的offer)

```java
if (eventName.equals("__offer")) {
    handleOffer(map);
    return;
}
```

```java
private void handleOffer(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String sdp = (String) data.get("sdp");
        String userID = (String) data.get("fromID");
        this.iEvent.onOffer(userID, sdp);
    }
}
```

```java
public void onOffer(String userId, String sdp) {
    handler.post(() -> {
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.onReceiveOffer(userId, sdp);
        }
    });
}
```

```java
public void onReceiveOffer(String userId, String description) {
    executor.execute(() -> {
        iEngine.receiveOffer(userId, description);
    });
}
```

```java
public void receiveOffer(String userId, String description) {
    if (iEngine == null) {
        return;
    }
    iEngine.receiveOffer(userId, description);
}
```

```java
public void receiveOffer(String userId, String description) {
    Peer peer = peers.get(userId);
    if (peer != null) {
        SessionDescription sdp = new SessionDescription(SessionDescription.Type.OFFER, description);
        peer.setOffer(false);
        peer.setRemoteDescription(sdp);
        peer.createAnswer();
    }
}
```

### 创建Answer

```java
public void createAnswer() {
    if (pc == null) return;
    Log.d(TAG, "createAnswer");
    // this 是 SdpObserver
    pc.createAnswer(this, offerOrAnswerConstraint());
}
```

创建成功回调

```java
public void onCreateSuccess(SessionDescription origSdp) {
    Log.d(TAG, "sdp创建成功       " + origSdp.type);
    String sdpString = origSdp.description;
    final SessionDescription sdp = new SessionDescription(origSdp.type, sdpString);
    localSdp = sdp;
    setLocalDescription(sdp);
}
```

设置sdp成功回调

```java
public void onSetSuccess() {
    Log.d(TAG, "sdp连接成功   " + pc.signalingState().toString());
    if (pc == null) return;
    // 发送者
    if (isOffer) {
        if (pc.getRemoteDescription() == null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }
        } else {
            Log.d(TAG, "Remote SDP set succesfully");

            drainCandidates();
        }

    } else {
        if (pc.getLocalDescription() != null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                // =====================执行到此处
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }

            drainCandidates();
        } else {
            Log.d(TAG, "Remote SDP set succesfully");
        }
    }
}
```

```java
public void onSendAnswer(String userId, SessionDescription description) {
    if (mCallback != null) {
        mCallback.onSendAnswer(userId, description);
    }
}
```

```java
public void onSendAnswer(String userId, SessionDescription description) {
    executor.execute(() -> {
        if (mEvent != null) {
            Log.d(TAG, "onSendAnswer");
            mEvent.sendAnswer(userId, description.description);
        }
    });
}
```

```java
public void sendAnswer(String userId, String sdp) {
    SocketManager.getInstance().sendAnswer(userId, sdp);
}
```

```java
public void sendAnswer(String userId, String sdp) {
    if (webSocket != null) {
        webSocket.sendAnswer(myId, userId, sdp);
    }
}
```

```java
public void sendAnswer(String myId, String userId, String sdp) {
    Map<String, Object> map = new HashMap<>();
    Map<String, Object> childMap = new HashMap<>();
    childMap.put("sdp", sdp);
    childMap.put("fromID", myId);
    childMap.put("userID", userId);
    map.put("data", childMap);
    map.put("eventName", "__answer");
    JSONObject object = new JSONObject(map);
    final String jsonString = object.toString();
    Log.d(TAG, "send-->" + jsonString);
    send(jsonString);
}
```



```json
{
    "data": {
        "fromID": "cq",
        "sdp": "v=0
o=- 9158379573334196205 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE 0 1
a=extmap-allow-mixed
a=msid-semantic: WMS ARDAMS
m=video 9 UDP/TLS/RTP/SAVPF 96 97 39 40 98 99 127 103 106 107 108
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:1gZv
a=ice-pwd:ZgXjEEvuG3yJa/aUU+99fZD4
a=ice-options:trickle renomination
a=fingerprint:sha-256 42:14:81:C5:70:E4:C6:FC:0C:48:19:47:7B:C5:E3:F6:7E:70:79:39:D5:5D:61:AE:CD:78:E8:3E:71:FB:CA:6C
a=setup:active
a=mid:0
a=extmap:1 urn:ietf:params:rtp-hdrext:toffset
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:3 urn:3gpp:video-orientation
a=extmap:4 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:5 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay
a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type
a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing
a=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/color-space
a=extmap:9 urn:ietf:params:rtp-hdrext:sdes:mid
a=extmap:10 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id
a=extmap:11 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id
a=sendrecv
a=msid:ARDAMS ARDAMSv0
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
a=rtpmap:39 AV1/90000
a=rtcp-fb:39 goog-remb
a=rtcp-fb:39 transport-cc
a=rtcp-fb:39 ccm fir
a=rtcp-fb:39 nack
a=rtcp-fb:39 nack pli
a=rtpmap:40 rtx/90000
a=fmtp:40 apt=39
a=rtpmap:98 VP9/90000
a=rtcp-fb:98 goog-remb
a=rtcp-fb:98 transport-cc
a=rtcp-fb:98 ccm fir
a=rtcp-fb:98 nack
a=rtcp-fb:98 nack pli
a=fmtp:98 profile-id=0
a=rtpmap:99 rtx/90000
a=fmtp:99 apt=98
a=rtpmap:127 H264/90000
a=rtcp-fb:127 goog-remb
a=rtcp-fb:127 transport-cc
a=rtcp-fb:127 ccm fir
a=rtcp-fb:127 nack
a=rtcp-fb:127 nack pli
a=fmtp:127 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
a=rtpmap:103 rtx/90000
a=fmtp:103 apt=127
a=rtpmap:106 red/90000
a=rtpmap:107 rtx/90000
a=fmtp:107 apt=106
a=rtpmap:108 ulpfec/90000
a=ssrc-group:FID 3983659415 1590197850
a=ssrc:3983659415 cname:rtrDazpbCWvWpU2V
a=ssrc:1590197850 cname:rtrDazpbCWvWpU2V
m=audio 9 UDP/TLS/RTP/SAVPF 111 63 9 102 0 8 13 110 126
c=IN IP4 0.0.0.0
a=rtcp:9 IN IP4 0.0.0.0
a=ice-ufrag:1gZv
a=ice-pwd:ZgXjEEvuG3yJa/aUU+99fZD4
a=ice-options:trickle renomination
a=fingerprint:sha-256 42:14:81:C5:70:E4:C6:FC:0C:48:19:47:7B:C5:E3:F6:7E:70:79:39:D5:5D:61:AE:CD:78:E8:3E:71:FB:CA:6C
a=setup:active
a=mid:1
a=extmap:14 urn:ietf:params:rtp-hdrext:ssrc-audio-level
a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time
a=extmap:4 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01
a=extmap:9 urn:ietf:params:rtp-hdrext:sdes:mid
a=sendrecv
a=msid:ARDAMS ARDAMSa0
a=rtcp-mux
a=rtpmap:111 opus/48000/2
a=rtcp-fb:111 transport-cc
a=fmtp:111 minptime=10;useinbandfec=1
a=rtpmap:63 red/48000/2
a=fmtp:63 111/111
a=rtpmap:9 G722/8000
a=rtpmap:102 ILBC/8000
a=rtpmap:0 PCMU/8000
a=rtpmap:8 PCMA/8000
a=rtpmap:13 CN/8000
a=rtpmap:110 telephone-event/48000
a=rtpmap:126 telephone-event/8000
a=ssrc:1703416566 cname:rtrDazpbCWvWpU2V
",
        "userID": "gq"
    },
    "eventName": "__answer"
}
```









```java
private void drainCandidates() {
    Log.i(TAG, "drainCandidates");
    synchronized (Peer.class) {
        if (queuedRemoteCandidates != null) {
            Log.d(TAG, "Add " + queuedRemoteCandidates.size() + " remote candidates");
            for (IceCandidate candidate : queuedRemoteCandidates) {
                pc.addIceCandidate(candidate);
            }
            queuedRemoteCandidates = null;
        }
    }
}
```



### 服务端转发Answer

```java
case "__offer":
    offer(message, data.getData());
    break;
```

```
private void offer(String message, Map<String, Object> data) {
    String userId = (String) data.get("userID");
    UserBean userBean = MemCons.userBeans.get(userId);
    sendMsg(userBean, -1, message);
}
```



### 客户端处理Answer(A处理接收到的Answer)

```java
if (eventName.equals("__answer")) {
    handleAnswer(map);
    return;
}
```

```java
private void handleAnswer(Map map) {
    Map data = (Map) map.get("data");
    if (data != null) {
        String sdp = (String) data.get("sdp");
        String userID = (String) data.get("fromID");
        this.iEvent.onAnswer(userID, sdp);
    }
}
```

```java
public void onAnswer(String userId, String sdp) {
    handler.post(() -> {
        CallSession currentSession = SkyEngineKit.Instance().getCurrentSession();
        if (currentSession != null) {
            currentSession.onReceiverAnswer(userId, sdp);
        }
    });
}
```

```java
public void onReceiverAnswer(String userId, String sdp) {
    executor.execute(() -> {
        iEngine.receiveAnswer(userId, sdp);
    });
}
```

```java
public void receiveAnswer(String userId, String sdp) {
    if (iEngine == null) {
        return;
    }
    iEngine.receiveAnswer(userId, sdp);
}
```

```java
public void receiveAnswer(String userId, String sdp) {
    Log.d(TAG, "receiveAnswer--" + userId);
    Peer peer = peers.get(userId);
    if (peer != null) {
        SessionDescription sessionDescription = new SessionDescription(SessionDescription.Type.ANSWER, sdp);
        peer.setRemoteDescription(sessionDescription);
    }
}
```

设置远程sdp成功的回调

```java
public void onSetSuccess() {
    Log.d(TAG, "sdp连接成功   " + pc.signalingState().toString());
    if (pc == null) return;
    // 发送者
    if (isOffer) {
        if (pc.getRemoteDescription() == null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }
        } else {
            Log.d(TAG, "Remote SDP set succesfully");
			// ================== 执行到此处
            drainCandidates();
        }

    } else {
        if (pc.getLocalDescription() != null) {
            Log.d(TAG, "Local SDP set succesfully");
            if (!isOffer) {
                //接收者，发送Answer
                mEvent.onSendAnswer(mUserId, localSdp);
            } else {
                //发送者,发送自己的offer
                mEvent.onSendOffer(mUserId, localSdp);
            }

            drainCandidates();
        } else {
            Log.d(TAG, "Remote SDP set succesfully");
        }
    }
}
```









## 挂断流程(B挂断A) TODO





## 语音 TODO



## 创建房间 TODO





## 其他

```
FragmentVideo

全屏-远程屏幕
fullscreen_video_view
fullscreenRenderer = view.findViewById(R.id.fullscreen_video_view);

View surfaceView = gEngineKit.getCurrentSession().setupRemoteVideo(userId, false);
remoteSurfaceView = (SurfaceViewRenderer) surfaceView;
fullscreenRenderer.removeAllViews();
if (remoteSurfaceView.getParent() != null) {
    ((ViewGroup) remoteSurfaceView.getParent()).removeView(remoteSurfaceView);
}
fullscreenRenderer.addView(remoteSurfaceView);





小屏-本地屏幕
pip_video_view
pipRenderer = view.findViewById(R.id.pip_video_view);

View surfaceView = gEngineKit.getCurrentSession().setupLocalVideo(false);
if (surfaceView != null) {
    localSurfaceView = (SurfaceViewRenderer) surfaceView;
    localSurfaceView.setZOrderMediaOverlay(false);
    fullscreenRenderer.addView(localSurfaceView);
}
pipRenderer.addView(localSurfaceView);
```



创建本地屏幕

```java
public void createLocalStream() {
    // 音频
    audioSource = _factory.createAudioSource(createAudioConstraints());
    _localAudioTrack = _factory.createAudioTrack(AUDIO_TRACK_ID, audioSource);
    // 视频
    if (!mIsAudioOnly) {
        captureAndroid = createVideoCapture();
        surfaceTextureHelper = SurfaceTextureHelper.create("CaptureThread", mRootEglBase.getEglBaseContext());
        videoSource = _factory.createVideoSource(captureAndroid.isScreencast());
        captureAndroid.initialize(surfaceTextureHelper, mContext, videoSource.getCapturerObserver());
        captureAndroid.startCapture(VIDEO_RESOLUTION_WIDTH, VIDEO_RESOLUTION_HEIGHT, FPS);
        _localVideoTrack = _factory.createVideoTrack(VIDEO_TRACK_ID, videoSource);
    }
}
```



```java
public View setupLocalPreview(boolean isOverlay) {
    if (mRootEglBase == null) {
        return null;
    }
    localRenderer = new SurfaceViewRenderer(mContext);
    localRenderer.init(mRootEglBase.getEglBaseContext(), null);
    localRenderer.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FIT);
    localRenderer.setMirror(true);
    localRenderer.setZOrderMediaOverlay(isOverlay);
    ProxyVideoSink localSink = new ProxyVideoSink();
    localSink.setTarget(localRenderer);
    if (_localVideoTrack != null) {
        _localVideoTrack.addSink(localSink);
    }
    return localRenderer;
}
```



创建远程屏幕

```java
public View setupRemoteVideo(String userId, boolean isOverlay) {
    if (TextUtils.isEmpty(userId)) {
        Log.e(TAG, "setupRemoteVideo userId is null ");
        return null;
    }
    Peer peer = peers.get(userId);
    if (peer == null) return null;
    if (peer.renderer == null) {
        peer.createRender(mRootEglBase, mContext, isOverlay);
    }
    return peer.renderer;
}
```

```java
public void createRender(EglBase mRootEglBase, Context context, boolean isOverlay) {
    renderer = new SurfaceViewRenderer(context);
    renderer.init(mRootEglBase.getEglBaseContext(), new RendererCommon.RendererEvents() {
        @Override
        public void onFirstFrameRendered() {
            Log.d(TAG, "createRender onFirstFrameRendered");

        }

        @Override
        public void onFrameResolutionChanged(int videoWidth, int videoHeight, int rotation) {
            Log.d(TAG, "createRender onFrameResolutionChanged");
        }
    });
    renderer.setScalingType(RendererCommon.ScalingType.SCALE_ASPECT_FILL);
    renderer.setMirror(true);
    renderer.setZOrderMediaOverlay(isOverlay);
    sink = new ProxyVideoSink();
    sink.setTarget(renderer);
    if (_remoteStream != null && _remoteStream.videoTracks.size() > 0) {
        _remoteStream.videoTracks.get(0).addSink(sink);
    }

}
```



_remoteStream???

```java
public void onAddStream(MediaStream stream) {
    Log.i(TAG, "onAddStream:");
    stream.audioTracks.get(0).setEnabled(true);
    _remoteStream = stream;
    if (mEvent != null) {
        mEvent.onRemoteStream(mUserId, stream); // 添加远端surfaceView添加到本地
    }
}
```

