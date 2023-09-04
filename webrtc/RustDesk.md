# RustDesk

## flutter_rust_bridge介绍

[flutter_rust_bridge](https://github.com/fzyzcjy/flutter_rust_bridge)

它是用于 `Flutter` 和 `Rust` 的高级内存安全绑定生成器。这个库只是一个代码生成器，帮助你的 `Flutter / Dart` 调用 `Rust` 函数。它只是生成了一些模板代码，代替了手工编写。



```shell
# flutter_ffi.rs 生成 generated_bridge.dart
# 创建的一个 C 头文件，里面列出了 Rust 库导出的所有符号，我们需要使用它确保 Xcode 不会将符号去除。
~/.cargo/bin/flutter_rust_bridge_codegen --rust-input ../src/flutter_ffi.rs --dart-output ./lib/generated_bridge.dart --c-output ./macos/Runner/bridge_generated.h

```





## RustDesk So初始化流程

main.dart文件中

```dart
Future<void> main(List<String> args) async {
    runMainApp(true);
}

void runMainApp(bool startService) async {
  // register uni links
  await initEnv(kAppTypeMain);
}

Future<void> initEnv(String appType) async {
  // global shared preference
  await platformFFI.init(appType);
  // 初始化全局FFI  
  await initGlobalFFI();
}

```

native_model.dart

```dart
typedef F2 = Pointer<Utf8> Function(Pointer<Utf8>, Pointer<Utf8>);

class PlatformFFI {
    Future<void> init(String appType) async {
        // 加载so
        final dylib = Platform.isAndroid ? DynamicLibrary.open('librustdesk.so');
        // 查找动态链接库中的函数
        _translate = dylib.lookupFunction<F2, F2>('translate');
        // 初始化 _ffiBind
        _ffiBind = RustdeskImpl(dylib);
        //...
    }
}
```



## FFI初始化流程

```dart
//FFI 是 Flutter状态管理器和与Rust核心的数据通信。

// Global FFI object
late FFI _globalFFI;

FFI get gFFI => _globalFFI;

Future<void> initGlobalFFI() async {
  debugPrint("_globalFFI init");
  _globalFFI = FFI(null);
  debugPrint("_globalFFI init end");
  // after `put`, can also be globally found by Get.find<FFI>();
  Get.put(_globalFFI, permanent: true);
}
```



## 文案获取流程:

```dart
translate("Connection");
```

common.dart 

```dart
String translate(String name) {
  if (name.startsWith('Failed to') && name.contains(': ')) {
    return name.split(': ').map((x) => translate(x)).join(': ');
  }
  return platformFFI.translate(name, localeName);
}
```

native_model.dart

```dart
String translate(String name, String locale) {
  if (_translate == null) return name;
  var a = name.toNativeUtf8();
  var b = locale.toNativeUtf8();
  // 调用so中的方法获取文案
  // _translate = dylib.lookupFunction<F2, F2>('translate');
  var p = _translate!(a, b);
  assert(p != nullptr);
  final res = p.toDartString();
  calloc.free(p);
  calloc.free(a);
  calloc.free(b);
  return res;
}
```



## 连接流程

home_page.dart

```dart
class WebHomePage extends StatelessWidget {
	final connectionPage = ConnectionPage();
}
```

connection_page.dart

```dart
SizedBox(
  width: 60,
  height: 60,
  child: IconButton(
    icon: const Icon(Icons.arrow_forward,
        color: MyTheme.darkGray, size: 45),
    // 点击事件
    onPressed: onConnect,
  ),
),


void onConnect() {
  var id = _idController.id;
  connect(context, id);
}


connect(
  BuildContext context,
  String id, {
  bool isFileTransfer = false,
  bool isTcpTunneling = false,
  bool isRDP = false,
}) async {
    
    // ....
    
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (BuildContext context) => RemotePage(id: id),
      ),
    );
}

```



remote_page.dart

```dart
class RemotePage extends StatefulWidget {
  RemotePage({Key? key, required this.id}) : super(key: key);

  final String id;

  @override
  State<RemotePage> createState() => _RemotePageState();
}

```

```dart
class _RemotePageState extends State<RemotePage> {
    @override
    void initState() {
      super.initState();
      gFFI.start(widget.id);
    }
}
```



```dart
void start(String id,
    {bool isFileTransfer = false,
    bool isPortForward = false,
    bool isRdp = false,
    String? switchUuid,
    String? password,
    bool? forceRelay}) {

    if (!isSessionAdded) {
      final addRes = bind.sessionAddSync(
        sessionId: sessionId,
        id: id,
        isFileTransfer: isFileTransfer,
        isPortForward: isPortForward,
        isRdp: isRdp,
        switchUuid: switchUuid ?? '',
        forceRelay: forceRelay ?? false,
        password: password ?? '',
      );
    }
    // TODO sessionStart
    final stream = bind.sessionStart(sessionId: sessionId, id: id);
    final cb = ffiModel.startEventListener(sessionId, id);
    final useTextureRender = bind.mainUseTextureRender();
    final SimpleWrapper<bool> isToNewWindowNotified = SimpleWrapper(false);

    // 监听
    stream.listen((message) {
      if (closed) return;
      if (isSessionAdded && !isToNewWindowNotified.value) {
        bind.sessionReadyToNewWindow(sessionId: sessionId);
        isToNewWindowNotified.value = true;
      }
      () async {
        /*
        	连接错误时的message :
        	EventToUI.event(field0: {"type":"error","title":"Connection Error","name":"msgbox","text":"ID does not exist","hasRetry":"","link":""})
        	
        	连接成功时的message :
        	EventToUI.event(field0: {"secure":"true","direct":"false","name":"connection_ready"})
        	
        	EventToUI.event(field0: {"fingerprint":"f6ee b3d7 e7a0 a3e4 a323 f276 7720 a721 2399 878d c3e0 7b51 32bb 1cde 1412 033d","name":"fingerprint"})
        	
        	EventToUI.event(field0: {"type":"input-password","hasRetry":"","name":"msgbox","link":"","title":"Password Required","text":""})
        	
        	EventToUI.event(field0: {"audio":"false","name":"permission"})
        	
        	EventToUI.event(field0: {"speed":"0.03kB/s","target_bitrate":"","delay":"","codec_format":"","fps":"0","name":"update_quality_status"})
			....
			EventToUI.event(field0: {"name":"update_privacy_mode"})
			
			EventToUI.event(field0: {"platform_additions":"","version":"1.2.2","current_display":"0","name":"peer_info","displays":"[{\"y\":0,\"x\":0,\"cursor_embedded\":0,\"width\":800,\"original_width\":800,\"original_height\":1280,\"height\":1280}]","platform":"Android","username":"android","resolutions":"[]","sas_enabled":"false","features":"{\"privacy_mode\":0}","hostname":"HUAWEI-SCM-W09"})
			
			EventToUI.event(field0: {"type":"success","title":"Successful","text":"Connected, waiting for image...","link":"","name":"msgbox","hasRetry":""})


			
			关闭事件
			EventToUI.event(field0: close)
			
			
			输入密码错误事件
			EventToUI.event(field0: {"link":"","hasRetry":"","name":"msgbox","title":"Wrong Password","type":"re-input-password","text":"Do you want to enter again?"})
        */
        if (message is EventToUI_Event) {
          if (message.field0 == "close") {
            closed = true;
            debugPrint('Exit session event loop');
            return;
          }
          Map<String, dynamic>? event;
          try {
            event = json.decode(message.field0);
          } catch (e) {
            debugPrint('json.decode fail1(): $e, ${message.field0}');
          }
          if (event != null) {
            /*
            回调方法 startEventListener()
            cb = ffiModel.startEventListener(sessionId, id);
            StreamEventHandler startEventListener(SessionID sessionId, String peerId) {...}
            
            */
            await cb(event);
          }
        } else if (message is EventToUI_Rgba) {
          if (useTextureRender) {
            onEvent2UIRgba();
          } else {
            final sz = platformFFI.getRgbaSize(sessionId);
            if (sz == null || sz == 0) {
              return;
            }
            final rgba = platformFFI.getRgba(sessionId, sz);
            if (rgba != null) {
              onEvent2UIRgba();
              imageModel.onRgba(rgba);
            }
          }
        }
      }();
    });
    // every instance will bind a stream
    this.id = id;
}
```



```dart
StreamEventHandler startEventListener(SessionID sessionId, String peerId) {
    return (evt) async {
		var name = evt['name'];
        if (name == 'msgbox') {
          // 打开错误弹窗
          handleMsgBox(evt, sessionId, peerId);
        } else if (name == 'connection_ready') {
            // 设置连接类型(设置是否安全和直连)
            setConnectionType(
                peerId, evt['secure'] == 'true', evt['direct'] == 'true');
      	} else if (name == 'fingerprint') {	
            // 设置指纹
        	FingerprintState.find(peerId).value = evt['fingerprint'] ?? '';
        } else if (name == 'permission') {
        	updatePermission(evt, peerId);
        } else if (name == 'update_quality_status') {
        	parent.target?.qualityMonitorModel.updateQualityStatus(evt);
        } else if (name == 'update_privacy_mode') {
        	updatePrivacyMode(evt, sessionId, peerId);
        } else if (name == 'peer_info') {
            handlePeerInfo(evt, peerId);
        }
        // ... 还有很多

    }
}
```



```dart
setConnectionType(String peerId, bool secure, bool direct) {
  _secure = secure;
  _direct = direct;
  try {
    // 通过ID找到连接类型 (peerId是对方的连接ID )
    var connectionType = ConnectionTypeState.find(peerId);
    connectionType.setSecure(secure);
    connectionType.setDirect(direct);
  } catch (e) {
    //
  }
}
```



```dart
// TOOD
notifyListeners
```



### 输入密码Dialog流程

```dart
handleMsgBox(Map<String, dynamic> evt, SessionID sessionId, String peerId) {
    if (type == 're-input-password') {
        // 输入密码错误弹窗
    	wrongPasswordDialog(sessionId, dialogManager, type, title, text);
    } else if (type == 'input-password') {
        // 输入密码弹窗
		enterPasswordDialog(sessionId, dialogManager);
    }
}
```

```dart
void enterPasswordDialog(
    SessionID sessionId, OverlayDialogManager dialogManager) async {
  await _connectDialog(
    sessionId,
    dialogManager,
    passwordController: TextEditingController(),
  );
}
```



```dart
_connectDialog(
  SessionID sessionId,
  OverlayDialogManager dialogManager, {
  TextEditingController? osUsernameController,
  TextEditingController? osPasswordController,
  TextEditingController? passwordController,
}) async {
  var rememberPassword = false;
  if (passwordController != null) {
    rememberPassword =
        await bind.sessionGetRemember(sessionId: sessionId) ?? false;
  }
  var rememberAccount = false;
  if (osUsernameController != null) {
    rememberAccount =
        await bind.sessionGetRemember(sessionId: sessionId) ?? false;
  }
  dialogManager.dismissAll();
  dialogManager.show((setState, close, context) {

    // 确认点击事件
    submit() {
      final osUsername = osUsernameController?.text.trim() ?? '';
      final osPassword = osPasswordController?.text.trim() ?? '';
      final password = passwordController?.text.trim() ?? '';
      if (passwordController != null && password.isEmpty) return;
      if (rememberAccount) {
        bind.sessionPeerOption(
            sessionId: sessionId, name: 'os-username', value: osUsername);
        bind.sessionPeerOption(
            sessionId: sessionId, name: 'os-password', value: osPassword);
      }
      
      /*
      	调用登录
		late FFI _globalFFI
		FFI get gFFI => _globalFFI;
      */
      gFFI.login(
        osUsername,
        osPassword,
        sessionId,
        password,
        rememberPassword,
      );
      close();
      dialogManager.showLoading(translate('Logging in...'),
          onCancel: closeConnection);
    }

	// ...
    return CustomAlertDialog(...)
      
}

```





#### 登录

model.dart

```dart
void login(String osUsername, String osPassword, SessionID sessionId,
    String password, bool remember) {
  // RustdeskImpl get bind => platformFFI.ffiBind;
  bind.sessionLogin(
      sessionId: sessionId,
      osUsername: osUsername,
      osPassword: osPassword,
      password: password,
      remember: remember);
}
```

generated_bridge.dart

```dart
Future<void> sessionLogin(
    {required UuidValue sessionId,
    required String osUsername,
    required String osPassword,
    required String password,
    required bool remember,
    dynamic hint}) {
  var arg0 = _platform.api2wire_Uuid(sessionId);
  var arg1 = _platform.api2wire_String(osUsername);
  var arg2 = _platform.api2wire_String(osPassword);
  var arg3 = _platform.api2wire_String(password);
  var arg4 = remember;
  return _platform.executeNormal(FlutterRustBridgeTask(
    // 登录 wire_session_login
    callFfi: (port_) => _platform.inner
        .wire_session_login(port_, arg0, arg1, arg2, arg3, arg4),
    parseSuccessData: _wire2api_unit,
    constMeta: kSessionLoginConstMeta,
    argValues: [sessionId, osUsername, osPassword, password, remember],
    hint: hint,
  ));
}
```

```dart
FlutterRustBridgeTaskConstMeta get kSessionLoginConstMeta =>
    const FlutterRustBridgeTaskConstMeta(
      debugName: "session_login",
      argNames: [
        "sessionId",
        "osUsername",
        "osPassword",
        "password",
        "remember"
      ],
    );
```



```dart
// Execute a normal ffi call. Usually called by generated code instead of manually called.
Future<S> executeNormal<S>(FlutterRustBridgeTask<S> task) {
  final completer = Completer<dynamic>();
  final sendPort = singleCompletePort(completer);
  // 调用了_platform.inner.wire_session_login(port_, arg0, arg1, arg2, arg3, arg4)
  task.callFfi(sendPort.nativePort);
  // TODO
  return completer.future.then((dynamic raw) =>
      _transformRust2DartMessage(raw, task.parseSuccessData));
}
```





```dart
void wire_session_login(
  int port_,
  ffi.Pointer<wire_uint_8_list> session_id,
  ffi.Pointer<wire_uint_8_list> os_username,
  ffi.Pointer<wire_uint_8_list> os_password,
  ffi.Pointer<wire_uint_8_list> password,
  bool remember,
) {
  return _wire_session_login(
    port_,
    session_id,
    os_username,
    os_password,
    password,
    remember,
  );
}
```

```dart
late final _wire_session_login = _wire_session_loginPtr.asFunction<
    void Function(
        int,
        ffi.Pointer<wire_uint_8_list>,
        ffi.Pointer<wire_uint_8_list>,
        ffi.Pointer<wire_uint_8_list>,
        ffi.Pointer<wire_uint_8_list>,
        bool)>();
```

```dart
late final _wire_session_loginPtr = _lookup<
    ffi.NativeFunction<
        ffi.Void Function(
            ffi.Int64,
            ffi.Pointer<wire_uint_8_list>,
            ffi.Pointer<wire_uint_8_list>,
            ffi.Pointer<wire_uint_8_list>,
            ffi.Pointer<wire_uint_8_list>,
            ffi.Bool)>>('wire_session_login');
```



flutter_ffi.rs

```rust
pub fn session_login(
    session_id: SessionID,
    os_username: String,
    os_password: String,
    password: String,
    remember: bool,
) {
    if let Some(session) = SESSIONS.read().unwrap().get(&session_id) {
        session.login(os_username, os_password, password, remember);
    }
}
```

retmots.rs

```rust
sciter::dispatch_script_call! {
	fn login(String, String, String, bool);
}
```

ui_session_interface.rs

```rust
pub fn login(
    &self,
    os_username: String,
    os_password: String,
    password: String,
    remember: bool,
) {
    self.send(Data::Login((os_username, os_password, password, remember)));
}
```



```rust
#[async_trait]
impl<T: InvokeUiSession> Interface for Session<T> {
    fn send(&self, data: Data) {
        if let Some(sender) = self.sender.read().unwrap().as_ref() {
            sender.send(data).ok();
        }
    }
}

```





### 窗口渲染流程



### 菜单栏点击逻辑





### 连接失败错误弹窗

model.dart

```dart
showMsgBox(SessionID sessionId, String type, String title, String text,
    String link, bool hasRetry, OverlayDialogManager dialogManager,
    {bool? hasCancel}) {
  // reconnect: reconnect 重连
  msgBox(sessionId, type, title, text, link, dialogManager,
      hasCancel: hasCancel, reconnect: reconnect);
  _timer?.cancel();
  if (hasRetry) {
    _timer = Timer(Duration(seconds: _reconnects), () {
      reconnect(dialogManager, sessionId, false);
    });
    _reconnects *= 2;
  } else {
    _reconnects = 1;
  }
}
```

common.dart

```dart
void msgBox(SessionID sessionId, String type, String title, String text,
    String link, OverlayDialogManager dialogManager,
    {bool? hasCancel, ReconnectHandle? reconnect}) {
    // ... ...
}
```



#### 点击确定(关闭弹窗)

```dart
submit() {
  dialogManager.dismissAll();
  if (!type.contains("custom") && desktopType != DesktopType.portForward) {
    closeConnection();
  }
}
```



```dart
closeConnection({String? id}) {
  if (isAndroid || isIOS) {
    gFFI.chatModel.hideChatOverlay();
    Navigator.popUntil(globalKey.currentContext!, ModalRoute.withName("/"));
  } else {
    final controller = Get.find<DesktopTabController>();
    controller.closeBy(id);
  }
}
```



#### 点击重连

```dart
if (reconnect != null && title == "Connection Error") {
  buttons.insert(
      0,
      dialogButton('Reconnect', isOutline: true, onPressed: () {
        reconnect(dialogManager, sessionId, false);
      }));
}
```

model.dart

```dart
void reconnect(OverlayDialogManager dialogManager, SessionID sessionId,
    bool forceRelay) {
  // bind = RustdeskImpl
  bind.sessionReconnect(sessionId: sessionId, forceRelay: forceRelay);
  clearPermissions();
  dialogManager.showLoading(translate('Connecting...'),
      onCancel: closeConnection);
}
```



generated_bridge.dart

```dart
Future<void> sessionReconnect(
    {required UuidValue sessionId, required bool forceRelay, dynamic hint}) {
  var arg0 = _platform.api2wire_Uuid(sessionId);
  var arg1 = forceRelay;
  // _platform = RustdeskPlatform
  return _platform.executeNormal(FlutterRustBridgeTask(
    // TODO wire_session_reconnect 重连
    callFfi: (port_) =>
        _platform.inner.wire_session_reconnect(port_, arg0, arg1),
    parseSuccessData: _wire2api_unit,
    constMeta: kSessionReconnectConstMeta,
    argValues: [sessionId, forceRelay],
    hint: hint,
  ));
}
```



