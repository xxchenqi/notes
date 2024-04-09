# CameraX

## 基础知识

### YUV介绍

*YUV*是一种颜色空间,Y表示明亮度（Luminance、Luma），U和V则是色度、浓度（Chrominance、Chroma）

### YUV格式

YUV根据Y、U、V存储方式的不同，可以分成两个格式：

- 紧缩格式（packed）：每个像素点的Y、U、V连续存储，Y1U1V1...YnUnVn。
- 平面格式（planar）：先存储所有像素点的Y分量，再存储所有像素点的UV分量，Y和UV分别连续存储在不同矩阵当中。

平面格式（planar）又分为：

- 平面格式（planar）：先存储所有像素的Y，再存储所有像素点U或者V，最后存储V或者U。其中U、V分别连续存储：Y1...Yn U1...Un V1...Vn 或者 Y1...Yn V1...Vn U1...Un。
- 半平面格式（semi-planar）：先存储所有像素的Y，再存储所有像素点UV或者VU。其中U、V交替存储：Y1...Yn U1V1...UnVn 或者 Y1...Yn V1U1...VnUn。



采样方式采用YUV420、存储方式采用平面格式（planar）称为YUV420P。YUV420P根据U和V顺序不同又分为：

- I420： Y1...Y4n U1...Un V1...Vn (例如：YYYYYYYYUUVV)
- YV12：Y1...Y4n V1...Vn U1...Un (例如：YYYYYYYYVVUU)

采样方式采用YUV420、存储方式采用半平面格式（semi-planar）称为YUV420SP，YUV420SP根据U和V顺序不同又分为：

- NV12： Y1...Y4n U1V1...UnVn (例如：YYYYYYYYUVUV)
- NV21：Y1...Y4n V1U1...VnUn (例如：YYYYYYYYVUVU)



### YUV模型

`YUV模型`是根据一个亮度`(Y分量)`和两个色度`(UV分量)`来定义颜色空间，常见的YUV格式有`YUY2、YUYV、YVYU、UYVY、AYUV、Y41P、Y411、Y211、IF09、IYUV、YV12、YVU9、YUV411、YUV420`等

其中比较常见的`YUV420`分为两种：`YUV420P（Planar平面）和YUV420SP(Semi-Planar半平面)`。



android平台下使用相机默认图像格式是NV21

**android.hardware.Camera**中可以发现`NV21`属于`YUV420SP`格式

```java
private String cameraFormatForPixelFormat(int pixel_format) {
    switch(pixel_format) {
    case ImageFormat.NV16:      return PIXEL_FORMAT_YUV422SP;
    case ImageFormat.NV21:      return PIXEL_FORMAT_YUV420SP; // 默认
    case ImageFormat.YUY2:      return PIXEL_FORMAT_YUV422I;
    case ImageFormat.YV12:      return PIXEL_FORMAT_YUV420P;
    case ImageFormat.RGB_565:   return PIXEL_FORMAT_RGB565;
    case ImageFormat.JPEG:      return PIXEL_FORMAT_JPEG;
    default:                    return null;
    }
}
```

### YUV采样

```
YUV 4:4:4采样，每一个Y对应一组UV分量,一个YUV占8+8+8 = 24bits 3个字节。
YUV 4:2:2采样，每两个Y共用一组UV分量,一个YUV占8+4+4 = 16bits 2个字节。
YUV 4:2:0采样，每四个Y共用一组UV分量,一个YUV占8+2+2 = 12bits 1.5个字节。
```

常见的YUV420P和YUV420SP都是基于4:2:0采样的，所以如果图片的宽为width，高为heigth，

在内存中占的空间为width * height * 3 / 2，其中前width * height的空间存放Y分量，

接着width * height / 4存放U分量，width * height / 4存放V分量。

### YUV420P(YU12和YV12)格式

YUV420P又叫plane平面模式，Y , U , V分别在不同平面，也就是有三个平面，它是YUV标准格式4：2：0，主要分为：YU12和YV12

- YU12格式

> 在android平台下也叫作I420格式，首先是所有Y值，然后是所有U值，最后是所有V值。

```
YU12：亮度(行×列) + U(行×列/4) + V(行×列/4)
```

![](.\res\YU12格式.png)



- YV12格式

> YV12格式与YU12基本相同，首先是所有Y值，然后是所有V值，最后是所有U值。只要注意从适当的位置提取U和V值，YU12和YV12都可以使用相同的算法进行处理。

```
YV12：亮度Y(行×列) + V(行×列/4) + U(行×列/4)
```

![](.\res\YV12格式.png)



```
YU12: YYYYYYYY UUVV    =>    YUV420P
YV12: YYYYYYYY VVUU    =>    YUV420P
```



###  YUV420SP(NV21和NV12)格式

`YUV420SP`格式的图像阵列，首先是所有`Y值`，然后是`UV`或者`VU`交替存储，`NV12和NV21属于YUV420SP`格式，是一种`two-plane模式`(或semi-planar模式)，即`Y和UV分为两个plane`，但是`UV(CbCr)`为交错存储，而不是分为三个平面。



- NV21格式

> android手机从摄像头采集的预览数据一般都是NV21，存储顺序是先存Y，再VU交替存储，`NV21`存储顺序是先存`Y值`，再`VU`交替存储：`YYYYVUVUVU`，以 `4 X 4` 图片为例子，占用内存为 `4 X 4 X 3 / 2 = 24` 个字节

![](.\res\NV21格式.png)



- NV12格式

> NV12与NV21类似，也属于`YUV420SP`格式，`NV12`存储顺序是先存`Y值`，再`UV`交替存储：`YYYYUVUVUV`，以 `4 X 4` 图片为例子，占用内存为 `4 X 4 X 3 / 2 = 24` 个字节

![](.\res\NV12格式.png)



```
NV12: YYYYYYYY UVUV    =>YUV420SP
NV21: YYYYYYYY VUVU    =>YUV420SP
```









## Camera、Camera2、CameraX图片格式说明

Camera预览监控的就是NV21图片格式

Camera2返回的是Image对象，Image里面有宽高、步长、getPlanes YUV的数据通道、图片格式等。

CameraX监听Analyzer可以拿到实时预览的ImageProxy YUV_420_888的图片。CameraX只返回YUV_420_888格式的图片





## YUV420_888

### 简介

CameraX 使用 [YUV420_888](https://developer.android.google.cn/reference/android/graphics/ImageFormat#YUV_420_888)来生成图像，该格式有 8 位的 Luma(Y)、Chroma(U, V) 和 Paddings(P) 三个通道。

YUV_420_888 是一种通用的 YCbCr 格式，它能够表示任何 4:2:0 色度二次采样的平面或半平面缓冲区 (但不完全交错)，每个颜色样本有 8 位。且能够保证 Y 平面不会与 U/V 平面交错 (且像素步长始终为 1)，以及 U/V 平面总是具有相同的行步长和像素步长。

YUV_420_888这种图像的格式使用3个独立的Buffer表示，这三个Plane分别是代表Y，U，V三个平面，每个平面还有rowStride、pixelStride对应的属性。

![](.\res\yuv420888.jpg)





YUV_420_888是可以兼容所有YUV420P和YUV420SP格式的。也就是说上面提到的I420、YV12、NV12、NV21都可以是YUV_420_888的具体实现。

```
使用Image的getPlanes()获取plane数组: Image.Plane[] planes = image.getPlanes();
它保证planes[0] 总是Y ，planes[1] 总是U(Cb)， planes[2]总是V(Cr)。
并保证Y-Plane永远不会和U/V交叉(像素步幅始终为1。yPlane.getPixelStride()总是返回 1)。
U/V-Plane总是有相同的rowStride和 pixelStride()（即：uPlane.getRowStride() == vPlane.getRowStride() 和 uPlane.getPixelStride() == vPlane.getPixelStride();）。

getPixelStride() 像素步长，获取行内连续两个颜色值之间的距离(步长)。
getRowStride() 行步长，获取行间像素之间的距离。
```



### 格式分类

YUV_420_888的存储又分YUV420分为Planar格式（P）和Semi-Planar格式（SP）两大类，最主要的区别是：

Planar格式（P）按平面分开放，先把U存放完后，再存放V。U是连续的，V也是连续的
即：YYYYYUUUUUVVVV

Semi-Planar格式（SP）只有Y数据一个平面，UV数据合用一个平面。
即：YYYYYUVUVUV…



对于SP格式，得到的图像有两种典型类型：rowStride = Width的，（以6*4的图像进行说明）即：

![](.\res\YUV420_888_1.png)

另一种是rowStride != Width的，即：

![](.\res\YUV420_888_2.png)



### 通道长度计算

```
Y通道长度是（width * height）
image.getPlanes()[0].getBuffer().remaining() == width * height

U通道长度是（width * height / 2 -1）
image.getPlanes()[1].getBuffer().remaining() == width * height / 2 -1

V通道长度是（width * height / 2 -1）
image.getPlanes()[2].getBuffer().remaining() == width * height / 2 -1
```





### 判断YUV420_888图像的 UV 平面缓冲区是否为 NV21 格式

YUV_420_888图像保存数据的格式可以分为两种：

```
1.YYYYYYYYYYYYYYYYY(...)UUUUU(...)VVVVV(...)
2.YYYYYYYYYYYYYYYYY(...)VUVUVUVUVUVU(...)
```

PlaneProxy[]数组中YUV数据的保存方式。

1. YYYYYYYYYYYYYYYYY(...)UUUUU(...)VVVV(...)

```
Y缓冲区 PlaneProxy[0] YYYYYYYYY
U缓冲区 PlaneProxy[1] UUUUU
V缓冲区 PlaneProxy[2] VVVVV
```

2. YYYYYYYYYYYYYYYYY(...)VUVUVUVUVUVU(...)

```
Y缓冲区 PlaneProxy[0]  YYYYYYYYY(...)
U缓冲区 PlaneProxy[1]  UVUVUVUV(...)U
V缓冲区 PlaneProxy[2]  VUVUVUVU(...)V 

PS：PlaneProxy[1]去掉最后一个U PlaneProxy[2]去掉第一个V，则两个缓冲区完全一样
```



```java
/**
 * 检查 YUV_420_888 图像的 UV 平面缓冲区是否为 NV21 格式。
 */
private static boolean areUVPlanesNV21(Plane[] planes, int width, int height) {
    int imageSize = width * height;
    ByteBuffer uBuffer = planes[1].getBuffer();
    ByteBuffer vBuffer = planes[2].getBuffer();
    // 备份缓冲区属性。
    int vBufferPosition = vBuffer.position();
    int uBufferLimit = uBuffer.limit();
    // 将 V 缓冲区推进 1 个字节，因为 U 缓冲区将不包含第一个 V 值。
    vBuffer.position(vBufferPosition + 1);
    // 切掉 U 缓冲区的最后一个字节，因为 V 缓冲区将不包含最后一个 U 值。
    uBuffer.limit(uBufferLimit - 1);
    // 检查缓冲区是否相等并具有预期的元素数量。
    /**
    	公式：
    	2 * imageSize / 4 - 1 这个公式实际就是UV的长度, -2 是因为 V缓冲区推进了1个字节 
		是否是NV21 = V通道的剩余字节 == (2 * imageSize / 4 - 2)
    */
    boolean areNV21 = (vBuffer.remaining() == (2 * imageSize / 4 - 2)) && (vBuffer.compareTo(uBuffer) == 0);
    // 将缓冲区恢复到初始状态。
    vBuffer.position(vBufferPosition);
    uBuffer.limit(uBufferLimit);
    return areNV21;
}
```





### Google YUV_420_888 图像转换为Bitmap 代码解析

https://github.com/googlesamples/mlkit/blob/master/android/vision-quickstart/app/src/main/java/com/google/mlkit/vision/demo/BitmapUtils.java#L82

```java
public class BitmapUtils {
 
    /**
     * 将 NV21 格式字节缓冲区转换为Bitmap。
     */
    @Nullable
    public static Bitmap getBitmap(ByteBuffer data, int width, int height, int rotation) {
        data.rewind();
        byte[] imageInBuffer = new byte[data.limit()];
        data.get(imageInBuffer, 0, imageInBuffer.length);
        try {
            YuvImage image = new YuvImage(imageInBuffer, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            image.compressToJpeg(new Rect(0, 0, width, height), 80, stream);
 
            Bitmap bmp = BitmapFactory.decodeByteArray(stream.toByteArray(), 0, stream.size());
 
            stream.close();
            return rotateBitmap(bmp, rotation, true, false);
        } catch (Exception e) {
            Log.e("VisionProcessorBase", "Error: " + e.getMessage());
        }
        return null;
    }
 
    /**
     * 将来自 CameraX API 的 YUV_420_888 图像转换为Bitmap。
     */
    @RequiresApi(VERSION_CODES.LOLLIPOP)
    @Nullable
    @ExperimentalGetImage
    public static Bitmap getBitmap(ImageProxy imageProxy) {
        if (imageProxy.getImage() == null)
            return null;
        ByteBuffer nv21Buffer = yuv420ThreePlanesToNV21(imageProxy.getImage().getPlanes(), imageProxy.getWidth(), imageProxy.getHeight());
        return getBitmap(nv21Buffer, imageProxy.getWidth(), imageProxy.getHeight(), imageProxy.getImageInfo().getRotationDegrees());
    }
 
    /**
     * bitmap旋转或者翻转
     */
    private static Bitmap rotateBitmap(Bitmap bitmap, int rotationDegrees, boolean flipX, boolean flipY) {
        Matrix matrix = new Matrix();
 
        // 图像旋转
        matrix.postRotate(rotationDegrees);
 
        // flipY垂直或者flipX水平镜像翻转
        matrix.postScale(flipX ? -1.0f : 1.0f, flipY ? -1.0f : 1.0f);
        Bitmap rotatedBitmap =
                Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
 
        // 如果旧bitmap已更改，则回收。
        if (rotatedBitmap != bitmap) {
            bitmap.recycle();
        }
        return rotatedBitmap;
    }
    
    /**
     * YUV_420_888格式转换成NV21.
     *
     * NV21 格式由一个包含 Y、U 和 V 值的单字节数组组成。
     * 对于大小为 S 的图像，数组的前 S 个位置包含所有 Y 值。其余位置包含交错的 V 和 U 值。
     * U 和 V 在两个维度上都进行了 2 倍的二次采样，因此有 S/4 U 值和 S/4 V 值。
     * 总之，NV21 数组将包含 S 个 Y 值，后跟 S/4 + S/4 VU 值: YYYYYYYYYYYYYY(...)YVUVUVUVU(...)VU
     *
     * YUV_420_888 是一种通用格式，可以描述任何 YUV 图像，其中 U 和 V 在两个维度上都以 2 倍的因子进行二次采样。
     * {@link Image#getPlanes} 返回一个包含 Y、U 和 V 平面的数组
     * Y 平面保证不会交错，因此我们可以将其值复制到 NV21 数组的第一部分。U 和 V 平面可能已经具有 NV21 格式的表示。
     * 如果平面共享相同的缓冲区，则会发生这种情况，V 缓冲区位于 U 缓冲区之前的一个位置，并且平面的 pixelStride 为 2。
     * 如果是这种情况，我们可以将它们复制到 NV21 阵列中。
     */
    @RequiresApi(VERSION_CODES.KITKAT)
    private static ByteBuffer yuv420ThreePlanesToNV21(
            Plane[] yuv420888planes, int width, int height) {
        int imageSize = width * height;
        byte[] out = new byte[imageSize + 2 * (imageSize / 4)];
 
        if (areUVPlanesNV21(yuv420888planes, width, height)) {
            // YYYYYYYYYYYYYYYYY(...)VUVUVUVUVUVU(...)
            /**
            Y缓冲区 PlaneProxy[0]  YYYYYYYYY(...)
            U缓冲区 PlaneProxy[1]  UVUVUVUV(...)U
            V缓冲区 PlaneProxy[2]  VUVUVUVU(...)V 
            */
            
            // 复制 Y 的值
            // 复制完成后的结果为：YYYYYYYYYYYYYYYYY(...)
            yuv420888planes[0].getBuffer().get(out, 0, imageSize);
            // 从 V 缓冲区获取第一个 V 值，因为 U 缓冲区不包含它。
            // 复制完成后的结果为：YYYYYYYYYYYYYYYYY(...)V
            yuv420888planes[2].getBuffer().get(out, imageSize, 1);
            // 从 U 缓冲区复制第一个 U 值和剩余的 VU 值。
            //  2 * imageSize / 4 - 1 == imageSize /2 -1 ,
            // 复制完成后的结果为：YYYYYYYYYYYYYYYYY(...)VUVUVUVUVUVU(...)
            yuv420888planes[1].getBuffer().get(out, imageSize + 1, 2 * imageSize / 4 - 1);
        } else {
            // YYYYYYYYYYYYYYYYY(...)UUUUU(...)VVVV(...)
            
            // 复制 Y
            // 结果YYYY
            unpackPlane(yuv420888planes[0], width, height, out, 0, 1);
            // 复制 U，这里需要 +1，是因为NV21格式是V开头的，所以U起始位置要向后移动1位
            // 结果YYYY 0U0U0U...
            unpackPlane(yuv420888planes[1], width, height, out, imageSize + 1, 2);
            // 取 V.
            // 结果YYYY VUVUVU...
            unpackPlane(yuv420888planes[2], width, height, out, imageSize, 2);
        }
 
        return ByteBuffer.wrap(out);
    }
 
    /**
     * 检查 YUV_420_888 图像的 UV 平面缓冲区是否为 NV21 格式。
     */
    @RequiresApi(VERSION_CODES.KITKAT)
    private static boolean areUVPlanesNV21(Plane[] planes, int width, int height) {
        int imageSize = width * height;
 
        ByteBuffer uBuffer = planes[1].getBuffer();
        ByteBuffer vBuffer = planes[2].getBuffer();
 
        // 备份缓冲区属性。
        int vBufferPosition = vBuffer.position();
        int uBufferLimit = uBuffer.limit();
 
        // 将 V 缓冲区推进 1 个字节，因为 U 缓冲区将不包含第一个 V 值。
        vBuffer.position(vBufferPosition + 1);
        // 切掉 U 缓冲区的最后一个字节，因为 V 缓冲区将不包含最后一个 U 值。
        uBuffer.limit(uBufferLimit - 1);
 
        // 检查缓冲区是否相等并具有预期的元素数量。
        boolean areNV21 = (vBuffer.remaining() == (2 * imageSize / 4 - 2)) && (vBuffer.compareTo(uBuffer) == 0);
 
        // 将缓冲区恢复到初始状态。
        vBuffer.position(vBufferPosition);
        uBuffer.limit(uBufferLimit);
 
        return areNV21;
    }
 
    /**
     * 将图像平面解压缩为字节数组。
     *
     * 输入平面数据将被复制到“out”中，从“offset”开始，每个像素将被“pixelStride”隔开。 请注意，输出上没有行填充。
     */
    @TargetApi(VERSION_CODES.KITKAT)
    private static void unpackPlane(Plane plane, int width, int height, byte[] out, int offset, int pixelStride) {
        ByteBuffer buffer = plane.getBuffer();
        buffer.rewind();
 
        // 计算当前平面的大小。假设它的纵横比与原始图像相同。
        // 计算行数
        int numRow = (buffer.limit() + plane.getRowStride() - 1) / plane.getRowStride();
        if (numRow == 0) {
            return;
        }
        // 缩放因子，如果行数和高度相同的话就是1
        int scaleFactor = height / numRow;
        // 列数
        int numCol = width / scaleFactor;
 
        // 提取输出缓冲区中的数据。
        int outputPos = offset;
        int rowStart = 0;
        for (int row = 0; row < numRow; row++) {
            int inputPos = rowStart;
            for (int col = 0; col < numCol; col++) {
                out[outputPos] = buffer.get(inputPos);
                outputPos += pixelStride;
                inputPos += plane.getPixelStride();
            }
            rowStart += plane.getRowStride();
        }
    }
}
```



### MLKit代码解析

https://github.com/jenly1314/MLKit/blob/1f411a1776a2188e0ba68e8f9bfa5778e51e8069/mlkit-common/src/main/java/com/king/mlkit/vision/common/analyze/CommonAnalyzer.java#L107

```java
int imageSize = imageProxy.getWidth() * imageProxy.getHeight();
byte[] bytes = new byte[imageSize + 2 * (imageSize / 4)];

/**
  * YUV420_888转NV21
  *
  * @param image
  * @param nv21
  */
private void yuv_420_888toNv21(@NonNull ImageProxy image, byte[] nv21) {
    ImageProxy.PlaneProxy yPlane = image.getPlanes()[0];
    ImageProxy.PlaneProxy uPlane = image.getPlanes()[1];
    ImageProxy.PlaneProxy vPlane = image.getPlanes()[2];

    ByteBuffer yBuffer = yPlane.getBuffer();
    ByteBuffer uBuffer = uPlane.getBuffer();
    ByteBuffer vBuffer = vPlane.getBuffer();
    yBuffer.rewind();
    uBuffer.rewind();
    vBuffer.rewind();

    int ySize = yBuffer.remaining();

    int position = 0;

    // 向数组中添加完整的y缓冲区。如果rowStride > 1，可能会跳过一些填充。
    for (int row = 0; row < image.getHeight(); row++) {
        // 逐行添加
        yBuffer.get(nv21, position, image.getWidth());
        position += image.getWidth();
        // 移动到y缓冲区的行首位置
        // ySize 和 yBuffer.position() - imageProxy.getWidth() + yPlane.getRowStride() 取最小值?
        // 需要移动的位置 = yBuffer当前位置(这个位置每次执行yBuffer.get都会更新) - 图片宽度 + y行步长 
        yBuffer.position(Math.min(ySize, yBuffer.position() - image.getWidth() + yPlane.getRowStride()));
    }
	
    // uv的高度
    int chromaHeight = image.getHeight() / 2;
    // uv的宽度
    int chromaWidth = image.getWidth() / 2;
    // 行步长
    int vRowStride = vPlane.getRowStride();
    int uRowStride = uPlane.getRowStride();
    // 像素步长
    int vPixelStride = vPlane.getPixelStride();
    int uPixelStride = uPlane.getPixelStride();

    // 交叉u和v帧，填充缓冲区的其余部分。使用两行缓冲区可以从字节缓冲区执行更快的批量获取。
    byte[] vLineBuffer = new byte[vRowStride];
    byte[] uLineBuffer = new byte[uRowStride];
    for (int row = 0; row < chromaHeight; row++) {
        vBuffer.get(vLineBuffer, 0, Math.min(vRowStride, vBuffer.remaining()));
        uBuffer.get(uLineBuffer, 0, Math.min(uRowStride, uBuffer.remaining()));
        int vLineBufferPosition = 0;
        int uLineBufferPosition = 0;
        for (int col = 0; col < chromaWidth; col++) {
            // 填充数组，先V后U
            nv21[position++] = vLineBuffer[vLineBufferPosition];
            nv21[position++] = uLineBuffer[uLineBufferPosition];
            // 更新下一次添加的位置
            vLineBufferPosition += vPixelStride;
            uLineBufferPosition += uPixelStride;
        }
    }
}
```



### 2台设备信息记录

```
设备1：
getFormat: 35
width: 1920
height: 1080
yPlane-limit: 2073600
yPlane-getRowStride: 1920
yPlane-getPixelStride: 1
uPlane-limit: 1036799
uPlane-getRowStride: 1920
uPlane-getPixelStride: 2
vPlane-limit: 1036799
vPlane-getRowStride: 1920
vPlane-getPixelStride: 2
areUVPlanesNV21: true

设备2：
getFormat: 35
width: 1920
height: 1080
yPlane-limit: 2211712
yPlane-getRowStride: 2048
yPlane-getPixelStride: 1
uPlane-limit: 1105791
uPlane-getRowStride: 2048
uPlane-getPixelStride: 2
vPlane-limit: 1105791
vPlane-getRowStride: 2048
vPlane-getPixelStride: 2
areUVPlanesNV21: false

```





## 相关文档

https://blog.csdn.net/weixin_41937380/article/details/127758173

[https://developer.android.google.cn/training/camerax](https://developer.android.google.cn/training/camerax)

https://101.dev/t/camerax-imageanalysis-yuv-rgb/522

https://www.jianshu.com/p/944ede616261

https://blog.csdn.net/mozushixin_1/article/details/123892476

https://developer.android.google.cn/reference/android/graphics/ImageFormat#YUV_420_888

