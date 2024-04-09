# View绘制流程

## WindowManager.addView流程

```java
ActivityThread.handleResumeActivity
	performResumeActivity//执行activity.onResume
		r.activity.performResume
			mInstrumentation.callActivityOnResume
    			activity.onResume();
	wm.addView(decor, l);//WindowManagerImpl.addView
		mGlobal.addView//WindowManagerGlobal.addView
            root = new ViewRootImpl(view.getContext(), display);//创建ViewRootImpl
			mViews.add(view);// DecorView
			mRoots.add(root);// ViewRootImpl
			mParams.add(wparams);// WindowManager.LayoutParams
			root.setView(view, wparams, panelParentView, userId);
```





## ViewRootImpl.setView流程

```java
ViewRootImpl.setView
	requestLayout
    	scheduleTraversals
    		mChoreographer.postCallback(Choreographer.CALLBACK_TRAVERSAL, mTraversalRunnable, null);
				TraversalRunnable.run
                    doTraversal
                    	performTraversals// 绘制View 
	res = mWindowSession.addToDisplayAsUser// 将窗口添加到WMS上面
    mInputEventReceiver = new WindowInputEventReceiver(inputChannel,Looper.myLooper());//接收事件处理
    view.assignParent(this)//给view设置ViewRootImpl(view可以通过getParent来获取到ViewRootImpl)
```

## ViewRootImpl 构造方法

```java
mThread = Thread.currentThread(); // 拿到创建它的线程，MainThread --- 默认
mDirty = new Rect(); // 脏区域(需要改变的区域会存在里面)
mAttachInfo = new View.AttachInfo(); // 保存当前窗口的一些信息
```



## performTraversals绘制流程

```java
ViewRootImpl.performTraversals()
    windowSizeMayChange |= measureHierarchy()// 预测量
    relayoutResult = relayoutWindow(params, viewVisibility, insetsPending);// 布局窗口
    performMeasure(childWidthMeasureSpec, childHeightMeasureSpec);// 控件树测量
    performLayout // 布局
    performDraw // 绘制
```



## TODO 预测量

```java
measureHierarchy() //最多三次测量
1. 设置一个值，进行第一次测量，MEASURED_STATE_TOO_SMALL
2. 获取一个状态值
3. 改变大小 baseSize = (baseSize+desiredWindowWidth)/2;
4. 进行第二次测量
5. 如果还不满意，直接给自己的最大值，然后第三次测量 -- 不确定
如果 windowSizeMayChange = true; --》 表示还需要测量

```

## performMeasure

```java
performMeasure
	mView.measure
		onMeasure // 如果重写了onMeasure,一定要调用 setMeasuredDimension();,否则会报错IllegalStateException
```

## MeasureSpec

```
32位 int
低30位 --> 值
高2位 --> 模式  
```

## performLayout

```java
performLayout
	host.layout 
		onLayout(changed, l, t, r, b);
			child.layout
```

## performDraw

```java
performDraw
    draw
    	scrollToRectOrFocus// 弹出输入框View上移过程
	    mAttachInfo.mThreadedRenderer.draw()// 硬件加速绘制
    	drawSoftware()// 软件绘制
    		mView.draw(canvas)
    			onDraw(canvas);
				dispatchDraw(canvas);
```



## WindowManagerImpl、WindowManagerGlobal、ViewRootImpl职责

```
WindowManagerImpl：确定 View  属于哪个屏幕，哪个父窗口
WindowManagerGlobal：管理整个进程 所有的窗口信息
ViewRootImpl：WindowManagerGlobal 实际操作者，操作自己的窗口
```



## UI 刷新只能在主线程进行吗？

```java
ViewRootImpl.requestLayout()中会去调用checkThread来检查当前线程
invalidate 中会去调用checkThread来检查当前线程
View.invalidate
	p.invalidateChild(this, damage);// ViewGroup.invalidateChild
		parent.invalidateChildInParent
        	ViewRootImpl.invalidateChildInParent
            	checkThread();
    
void checkThread() {
    if (mThread != Thread.currentThread()) {
        throw new CalledFromWrongThreadException(
                "Only the original thread that created a view hierarchy can touch its views.");
    }
}


所以在ViewRootImpl创建之前可以在子线程刷新UI
```



## 如何实现在子线程刷新Ui？

1. 在ViewRootImpl 创建之前调用
2. 在需要刷新Ui的子线程 创建ViewRootImpl



```java
    // 设置权限
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        if (!Settings.canDrawOverlays(this)) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, OVERLAY_PERMISSION_CODE);
        }
    }

        
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == OVERLAY_PERMISSION_CODE) {
            if (Settings.canDrawOverlays(this)) {
                // 在子线程中创建ViewRootImpl
                childThreadChangeUI();
            } else {
            }
        }
    }

    public void childThreadChangeUI() {
        new Thread() {
            @RequiresApi(api = Build.VERSION_CODES.R)
            @Override
            public void run() {
                Looper.prepare();
                WindowManager wm = (WindowManager) getApplicationContext().getSystemService(WINDOW_SERVICE);
                View view = View.inflate(MainActivity.this, R.layout.item, null);
                TextView tv = view.findViewById(R.id.tv);
                WindowManager.LayoutParams params = new WindowManager.LayoutParams();
                params.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
                // 设置不拦截焦点
                params.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
                params.width = (int) (60 * getResources().getDisplayMetrics().density);
                params.height = (int) (60 * getResources().getDisplayMetrics().density);
                params.gravity = Gravity.LEFT | Gravity.TOP;// 且设置坐标系 左上角
                params.format = PixelFormat.TRANSPARENT;
                int width = wm.getDefaultDisplay().getWidth();
                int height = wm.getDefaultDisplay().getHeight();
                params.y = height / 2 - params.height / 2;
                wm.addView(view, params);

                view.setOnTouchListener(new View.OnTouchListener() {
                    private int y;
                    private int x;

                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        switch (event.getAction()) {
                            case MotionEvent.ACTION_DOWN:
                                x = (int) event.getRawX();
                                y = (int) event.getRawY();
                                break;
                            case MotionEvent.ACTION_MOVE:
                                int minX = (int) (event.getRawX() - x);
                                int minY = (int) (event.getRawY() - y);
                                params.x = Math.min(width - params.width, Math.max(0, minX + params.x));
                                params.y = Math.min(height - params.height, Math.max(0, minY + params.y));
                                wm.updateViewLayout(view, params);
                                x = (int) event.getRawX();
                                y = (int) event.getRawY();
                                break;
                            case MotionEvent.ACTION_UP:
                                if (params.x > 0 && params.x < width - params.width) {
                                    int x = params.x;
                                    if (x > (width - params.width) / 2) {
                                        params.x = width - params.width;
                                    } else {
                                        params.x = 0;
                                    }
                                    wm.updateViewLayout(view, params);

                                } else if (params.x == 0 || params.x == (width - params.width)) {
                                    Toast.makeText(MainActivity.this, "被电击了", Toast.LENGTH_SHORT).show();
                                    tv.setText("更改了item的TextView值");
                                }
                                break;
                        }
                        return true;
                    }
                });
                Looper.loop();
            }
        }.start();
    }

}
```

