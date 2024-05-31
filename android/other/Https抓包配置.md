# Https抓包配置

network_sercurity_config.xml

```xml
<?xml version="1.0" encoding="utf-8"?>
<network-security-config xmlns:android="http://schemas.android.com/apk/res/android">
    <base-config cleartextTrafficPermitted="true" />
    <!--  debug-overrides是只有在客户单的debuggable 属性为true的情况下 才会信任这里的证书  -->
    <debug-overrides>
        <trust-anchors>
            <certificates src="@raw/charles" />
        </trust-anchors>
    </debug-overrides>
</network-security-config>
```



application

```xml
android:networkSecurityConfig="@xml/network_sercurity_config"
```





Charles证书导入

help->SSL Proxying->Save Charles Root Certificat...





mock数据

右键点击url->Map Local->choose选择数据