# APP发版配置检查

V1签名:
在 META-INF 文件夹下有三个文件：MANIFEST.MF、CERT.SF、CERT.RSA

V2签名:
v1 版本的 META_INF 的 .SF 文件属性当中有一个 X-Android-APK-Signed 属性：

X-Android-APK-Signed: 2

因此如果想绕过 v2 走 v1 校验是不行的。



查看签名：

```
keytool -list -printcert -jarfile <path>

apksigner verify -v --print-certs <path>
```





查看渠道

```
java -jar VasDolly.jar get -c <path>
```

写入渠道

```
java -jar VasDolly.jar put -c "zhihuiyun" <inapk> <outapk>
```





### 磁场apk信息

sha1

```
SHA1: 6B:54:87:74:33:EA:D3:3B:2F:2D:66:D5:14:3A:D4:1C:11:36:3D:F9
```

```
Signer #1 certificate DN: CN=hj, OU=hj, O=hj, L=shanghai, ST=shanghai, C=zh-cn
Signer #1 certificate SHA-256 digest: 944b297ed9bd0cbf6360882ba8532a7ab2b8e483df3f2dea18ee6cc06d602f8c
Signer #1 certificate SHA-1 digest: 6b54877433ead33b2f2d66d5143ad41c11363df9
Signer #1 certificate MD5 digest: 299b5d706d70fec3e4f3377f13f1aa6f
Signer #1 key algorithm: RSA
Signer #1 key size (bits): 1024
Signer #1 public key SHA-256 digest: 9b079210a665c6b8d584f5a474ad8b96f730bd155a14403759d5c672e2cf58f0
Signer #1 public key SHA-1 digest: a8213a9752dc5f7a78cbda2eda35b6ba07f458d2
Signer #1 public key MD5 digest: 6ddc2d81571bf9651640f778e0ed095c
```

别名

```
hjwordgames
```

password

```
123456
```

