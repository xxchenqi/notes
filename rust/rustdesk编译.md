# rustdesk编译

## 1.环境问题

环境版本版本在CI文件可以查到，安装的版本必须和CI匹配,否则编译各种问题
  CARGO_NDK_VERSION: "3.1.2"
  LLVM_VERSION: "15.0.6"
  FLUTTER_VERSION: "3.13.9"
  FLUTTER_RUST_BRIDGE_VERSION: "1.80.1"
  NDK_VERSION: "r26b"


flutter_rust_bridge_codegen指定版本安装
cargo install flutter_rust_bridge_codegen --version "1.80.1" --features "uuid"

cargo ndk 安装指定版本
cargo install cargo-ndk --version 3.1.2

查看cargo-ndk版本
cargo ndk --version
cargo-ndk 3.4.0

查看flutter版本
flutter --version
Flutter 3.16.9 

升级和降级(可以不操作，目前用最新的没问题)
flutter upgrade
flutter upgrade v3.13.9
flutter downgrade



## 2.libclang-10-dev llvm-10-dev 下载不到

在kali系统中libclang-10-dev llvm-10-dev 下载不到了，可以通过apt search搜索换个版本



## 3.the trait `IntoIntoDart<_>` is not implemented for `EventToUI`

stream.add(EventToUI::Rgba(display));
       ^^^ the trait `IntoIntoDart<_>` is not implemented for `EventToUI`
	   
这个错误就是flutter_rust_bridge_codegen版本不对导致的

 

## 4.aom文件找不到等各种问题

vcpkg中的aom文件找不到等一系列问题
ndk_compat,oboe_wrapper等链接错误

由于文档中下载的压缩包太老，没有aom等文件，所以不用。
用项目中提供vcpkg.json文件来安装
$VCPKG_ROOT/vcpkg install --triplet arm64-android --x-install-root="$VCPKG_ROOT/installed"

安装完成后可以通过vcpkg/installed 目录中查看
也可以通过./vcpkg list查看安装的



vcpkg install aom 此命令安装的是当前平台的aom库，并不会安装其他平台。

可以通过指定平台安装: vcpkg install aom:arm64-android 



## 5.为什么子工作流的value没有，但是父工作流的值有？

```
没有设置 secrets: inherit
```

`secrets: inherit` 的作用是将父工作流中的 `secrets` 传递给子工作流。在 GitHub Actions 中，`secrets` 是安全的敏感信息（例如 API 密钥、私钥等），默认情况下不会传递给子工作流，以确保安全性。

通过在子工作流的配置中添加 `secrets: inherit`，你告诉 GitHub Actions 在子工作流中继承父工作流的 `secrets`。这样，子工作流就可以访问那些敏感信息，而不必单独配置。

这种方法可以确保将敏感信息限制在需要的范围内，提高了安全性。如果不显式声明 `secrets: inherit`，则子工作流默认不会继承父工作流的 `secrets`。