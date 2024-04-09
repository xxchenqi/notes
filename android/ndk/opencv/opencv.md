# OPENCV

### 1.图片展示

```python
import cv2 as cv

src = cv.imread("F:/project/project_auto/botty/assets/docs/logo.png")
cv.namedWindow("input image", cv.WINDOW_AUTOSIZE)
cv.imshow("input image", src)
cv.waitKey(0)
cv.destroyAllWindows()
print("hi")
```

### 2.获取图片数据，保存图片

```python
import cv2 as cv
import numpy as np

def get_image_info(image):
    # 类型 <class 'numpy.ndarray'>
    print(type(image))
    # (922, 921, 3) 高、宽、通道数
    print(image.shape)
    # 像素数据大小 2547486 = 宽 * 高 * 通道数
    print(image.size)
    # 每个像素点所占用的位数 uint8
    print(image.dtype)
    pixel_data = np.array(image)
    print(pixel_data)


src = cv.imread("F:/project/project_auto/botty/assets/docs/video_thumbnail.png")
cv.namedWindow("input image", cv.WINDOW_AUTOSIZE)
cv.imshow("input image", src)
get_image_info(src)
# 获取灰度图像
gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
# 写入图片
cv.imwrite("E:/cq.png", gray)
cv.waitKey(0)
cv.destroyAllWindows()

```



### 3.图片创建

```python 

def numpy_test():
    # 创建 3*3 类型是uint8的3位数组
    m1 = np.ones([3, 3], np.uint8)
    # 将数组全部填满
    m1.fill(10)
    print(m1)

    # 将3行3列 改成 1行9列
    m2 = m1.reshape([1, 9])
    print(m2)

    m3 = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]], np.int32)
    m3.fill(9)
    print(m3)

def create_image():
    # 多通道
    """
    img = np.zeros([400, 400, 3], np.uint8)
    img[:, :, 1] = np.ones([400, 400])*255
    cv.imshow("new image", img)
    """
    # 单通道
    # img = np.zeros([400, 400, 1], np.uint8)
    # img[:, :, 0] = np.ones([400, 400]) * 127
    # cv.imshow("new image", img)

    img = np.ones([400, 400, 1], np.uint8)
    img = img * 127
    cv.imshow("new image", img)


# 耗时计算
t1 = cv.getTickCount()
numpy_test()
t2 = cv.getTickCount()
time = (t2 - t1) / cv.getTickFrequency()
print("time: %s ms" % (time * 1000))
```



### 4.常见色彩空间

RGB

HSV ： color index 化繁为整，RGB有一堆彩色不好表达，在HSV能很好的表达出来

取值范围：

H：0-180

S：0-255

V：0-255



HIS

YCrCb

YUV





### 5.将图片中绿色的物体提取显示出来

```python
# 视频提取
def extrace_object_dmeo():
    capture = cv.VideoCapture("D:/xxx/xxx.mp4")
    while (True):
        ret, frame = capture.read()
        if ret == False:
            break
        hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
        # 具体的最大和最小值需查表格
        lower_hsv = np.array([35, 43, 46])
        upper_hsv = np.array([77, 255, 255])
        # 二值化
        mask = cv.inRange(hsv, lowerb=lower_hsv, upperb=upper_hsv)
        cv.imshow("video", frame)
        cv.imshow("mask", mask)
        c = cv.waitKey(40)
        if c == 27:
            break

# 图片提取
def extrace_object_dmeo2(img):
    hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
    # 具体的最大和最小值需查表格
    lower_hsv = np.array([35, 43, 46])
    upper_hsv = np.array([77, 255, 255])
    # 二值化
    mask = cv.inRange(hsv, lowerb=lower_hsv, upperb=upper_hsv)
    cv.imshow("video", img)
    cv.imshow("mask", mask)

```



### 6.图片通道的分离和合并

```python
def split_and_merge_image(src):
    b, g, r = cv.split(src)
    # cv.imshow("blue", b)
    # cv.imshow("green", g)
    # cv.imshow("red", r)

    src_merge = cv.merge([b, g, r])
    cv.imshow("merge", src_merge)
```



### 7.图像差,均值和方差计算

```python

def add_demo(m1, m2):
    dst = cv.add(m1, m2)
    cv.imshow("add_demo", dst)


def subtract_demo(m1, m2):
    dst = cv.subtract(m1, m2)
    cv.imshow("add_demo", dst)


def divide_demo(m1, m2):
    dst = cv.divide(m1, m2)
    cv.imshow("divide_demo", dst)


def multiply_demo(m1, m2):
    dst = cv.multiply(m1, m2)
    cv.imshow("multiply_demo", dst)


def other(m1, m2):
    # 计算均值
    # M1 = cv.mean(m1)
    # M2 = cv.mean(m2)
    # print(M1)
    # print(M2)

    # 计算方差
    M1, dev1 = cv.meanStdDev(m1)
    M2, dev2 = cv.meanStdDev(m2)
    print(M1)
    print(M2)
    print("=====")
    print(dev1)
    print(dev2)

```



### 8.逻辑运算，增加权重

```python
def logic_demo(m1, m2):
    # dst = cv.bitwise_or(m1, m2)
    dst = cv.bitwise_and(m1, m2)
    cv.imshow("logic_demo", dst)


def contrast_brightness_demo(image, c, b):
    h, w, ch = image.shape
    blank = np.zeros([h, w, ch], image.dtype)
    # c = 对比度 b = 亮度
    dst = cv.addWeighted(image, c, blank, 1 - c, b)
    cv.imshow("contract_brightness_demo", dst)


src1 = cv.imread("images/01.jpg")
src2 = cv.imread("images/02.jpg")
# cv.imshow("input images", src)
# logic_demo(src1, src2)
contrast_brightness_demo(src2, 1.2, 100)
cv.waitKey(0)
cv.destroyAllWindows()

```



### 9.ROI,泛洪填充

```python
import cv2 as cv
import numpy as np

#彩色图像填充
def fill_color_demo(image):
    copyImg = image.copy()
    h, w = image.shape[:2]
    # mask大小必须是这么大，才能做mask
    mask = np.zeros([h + 2, w + 2], np.uint8)
	# (30, 30) 位置
    # (0, 255, 255) 填充的颜色
    # (100, 100, 100) 低值,最低的范围是 在(30, 30)中取一个像素 减去 (100, 100, 100)
    # (50, 50, 50) 高值,最高的范围是 在(30, 30)中取一个像素 减去 (100, 100, 100)
    # cv.FLOODFILL_FIXED_RANGE 图像的选择,彩色图像必须是这个，在这个范围内全部填充这个颜色
    cv.floodFill(copyImg, mask, (30, 30), (0, 255, 255), (100, 100, 100), (50, 50, 50), cv.FLOODFILL_FIXED_RANGE)
    cv.imshow("fill_color_demo", copyImg)

#二值图像填充
def fill_binary():
    image = np.zeros([400, 400, 3], np.uint8)
    image[100:300, 100:300, :] = 255
    cv.imshow("fill_binary", image)

    mask = np.ones([402, 402, 1], np.uint8)
    mask[101:301, 101:301] = 0
    cv.floodFill(image, mask, (200, 200), (0, 0, 255), cv.FLOODFILL_MASK_ONLY)
    cv.imshow("filled binary", image)

# roi （region of interest），感兴趣区域
src = cv.imread("images/lena.jpg")
# cv.imshow("src", src)
# face = src[50:250, 100:300]
# gray = cv.cvtColor(face, cv.COLOR_BGR2GRAY)
# backface = cv.cvtColor(gray, cv.COLOR_GRAY2BGR)
# src[50:250, 100:300] = backface
# cv.imshow("src2", src)

# 彩色填充
# fill_color_demo(src)
# 二值填充
fill_binary()

cv.waitKey(0)
cv.destroyAllWindows()

```



### 10.图片模糊

```python
import cv2 as cv
import numpy as np


def blur_demo(image):
    dst = cv.blur(image, (15, 1))
    cv.imshow("blur_demo", dst)


def median_blur_demo(image):
    dst = cv.medianBlur(image, 5)
    cv.imshow("median_blur_demo", dst)


def custom_blur_demo(image):
    # kernel = np.ones([5, 5], np.float32) / 25
    #锐化
    kernel = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]], np.float32)
    dst = cv.filter2D(image, -1, kernel=kernel)
    cv.imshow("custom_blur_demo", dst)


src = cv.imread("images/lena.jpg")
cv.imshow("src", src)
# blur_demo(src)
# median_blur_demo(src)
custom_blur_demo(src)
cv.waitKey(0)
cv.destroyAllWindows()

```



### 11.高斯模糊

```python
import cv2 as cv
import numpy as np


def clamp(pv):
    if pv > 255:
        return 255
    if pv < 0:
        return 0
    else:
        return pv


def gaussian_noise(image):
    h, w, c = image.shape
    for row in range(h):
        for col in range(w):
            s = np.random.normal(0, 20, 3)
            b = image[row, col, 0]
            g = image[row, col, 1]
            r = image[row, col, 2]
            image[row, col, 0] = clamp(b + s[0])
            image[row, col, 1] = clamp(g + s[1])
            image[row, col, 2] = clamp(r + s[2])

    cv.imshow("gaussian_noise", image)


src = cv.imread("images/lena.jpg")
cv.imshow("src", src)
# gaussian_noise(src)
dst = cv.GaussianBlur(src, (0, 0), 15)
cv.imshow("GaussianBlur", dst)
cv.waitKey(0)
cv.destroyAllWindows()

```



### 12.边缘保留滤波(EPF)

```python
import cv2 as cv
import numpy as np


# 高斯双边
def bi_demo(image):
    dst = cv.bilateralFilter(image, 0, 100, 15)
    cv.imshow("bi_demo", dst)


# 均值迁移
def shift_demo(image):
    dst = cv.pyrMeanShiftFiltering(image, 10, 50)
    cv.imshow("shift_demo", dst)


src = cv.imread("images/lena.jpg")
cv.imshow("src", src)
shift_demo(src)
cv.waitKey(0)
cv.destroyAllWindows()

```



### 13.直方图

```python
import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt


def plot_demo(image):
    plt.hist(image.ravel(), 256, [0, 256])
    plt.show()


def image_hist(image):
    color = ('blue', 'green', "red")
    for i, color in enumerate(color):
        hist = cv.calcHist([image], [i], None, [256], [0, 256])
        plt.plot(hist, color=color)
        plt.xlim([0, 256])
    plt.show()


src = cv.imread("images/lena.jpg")
cv.imshow("src", src)
image_hist(src)
cv.waitKey(0)
cv.destroyAllWindows()

```



14.直方图均衡和比较

```python
import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt

# 全局均衡
def equalHist_demo(image):
    gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    dst = cv.equalizeHist(gray)
    cv.imshow("equalHist_demo", dst)

# 局部均衡
def clahe_demo(image):
    gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    clahe = cv.createCLAHE(clipLimit=5.0, tileGridSize=(8, 8))
    dst = clahe.apply(gray)
    cv.imshow("clahe_demo", dst)


def create_rgb_hist(image):
    h, w, c = image.shape
    rgbHist = np.zeros([16 * 16 * 16, 1], np.float32)
    bsize = 256 / 16
    for row in range(h):
        for col in range(w):
            b = image[row, col, 0]
            g = image[row, col, 1]
            r = image[row, col, 2]
            index = np.int64(b / bsize) * 16 * 16 + np.int64(g / bsize) * 16 + np.int64(r / bsize)
            rgbHist[np.int64(index), 0] = rgbHist[np.int64(index), 0] + 1
    return rgbHist


def hist_compare(image1, image2):
    hist1 = create_rgb_hist(image1)
    hist2 = create_rgb_hist(image2)
    match1 = cv.compareHist(hist1, hist2, cv.HISTCMP_BHATTACHARYYA)
    cv.HISTCMP_INTERSECT
    match2 = cv.compareHist(hist1, hist2, cv.HISTCMP_CORREL)
    match3 = cv.compareHist(hist1, hist2, cv.HISTCMP_CHISQR)
    # HISTCMP_BHATTACHARYYA 巴氏距离:完全一样就是0
    # HISTCMP_INTERSECT 相交性
    # HISTCMP_CORREL 相关性：=1说明是一样
    # HISTCMP_CHISQR 卡方：相似度很高说明就越小
    print("巴氏距离:%s,相关性:%s,卡方:%s" % (match1, match2, match3))


src = cv.imread("images/lena.jpg")
src2 = cv.imread("images/blox.jpg")
# cv.imshow("src", src)
# clahe_demo(src)
hist_compare(src, src2)

cv.waitKey(0)
cv.destroyAllWindows()

```

直方图反向投影(略)



### 14.模板匹配

```python
import cv2 as cv
import numpy as np


def template_demo():
    tpl = cv.imread("images/lena.jpg")
    target = cv.imread("images/blox.jpg")
    cv.imshow("template image", tpl)
    cv.imshow("target image", target)
    methods = [cv.TM_SQDIFF_NORMED, cv.TM_CCORR_NORMED, cv.TM_CCOEFF_NORMED]
    th, tw = tpl.shape[:2]
    for md in methods:
        print(md)
        result = cv.matchTemplate(target, tpl, md)
        min_val, max_val, min_loc, max_loc = cv.minMaxLoc(result)
        if md == cv.TM_SQDIFF_NORMED:
            tl = min_loc
        else:
            tl = max_loc
        br = (tl[0] + tw, tl[1] + th)
        cv.rectangle(target, tl, br, (0, 0, 255), 2)
        cv.imshow("match-" + np.str(md), target)


template_demo()
cv.waitKey(0)
cv.destroyAllWindows()

```

