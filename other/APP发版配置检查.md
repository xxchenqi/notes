# APP发版配置检查

V1签名:
在 META-INF 文件夹下有三个文件：MANIFEST.MF、CERT.SF、CERT.RSA

V2签名:
v1 版本的 META_INF 的 .SF 文件属性当中有一个 X-Android-APK-Signed 属性：

X-Android-APK-Signed: 2

因此如果想绕过 v2 走 v1 校验是不行的。



查看签名：

```
keytool -list -printcert -jarfile <path apk>

apksigner verify -v --print-certs <path apk>

keytool -v -list -keystore <path jks> -storepass <password>
```





# VasDolly

查看渠道

```
java -jar VasDolly.jar get -c <path>
```

写入渠道

```
java -jar VasDolly.jar put -c "zhihuiyun" <inapk> <outapk>
```





