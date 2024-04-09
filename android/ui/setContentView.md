# setContentView流程

## Activity流程

```java
Activity.setContentView
PhoneWindow.setContentView
installDecor()// 创建 DecorView ,拿到mContentParent
	mDecor = generateDecor(-1);
		new DecorView()
    mContentParent = generateLayout(mDecor);
		layoutResource = R.layout.screen_simple;// @android:id/content == mContentParent
		mDecor.onResourcesLoaded(mLayoutInflater, layoutResource); //  R.layout.screen_simple 添加到 DecorView（FrameLayout）
		//ID_ANDROID_CONTENT = com.android.internal.R.id.content;
		ViewGroup contentParent = (ViewGroup)findViewById(ID_ANDROID_CONTENT);
		return contentParent
mLayoutInflater.inflate(layoutResID, mContentParent); //  R.layout.activity_main 渲染到 mContentParent
```



## AppcompatActivity

```java
AppcompatActivity.setContentView
AppCompatDelegateImpl.setContentView
ensureSubDecor
    mSubDecor = createSubDecor();
		ensureWindow();// 从Activity里获取PhoneWindow
		mWindow.getDecorView();
    		installDecor// 创建 DecorView ,拿到mContentParent
        
        subDecor = (ViewGroup) inflater.inflate(R.layout.abc_screen_simple, null);
		//获取subDecor中的ContentFrameLayout
		final ContentFrameLayout contentView = (ContentFrameLayout) subDecor.findViewById(R.id.action_bar_activity_content);

		//获取的R.layout.screen_simple里的content 
        final ViewGroup windowContentView = (ViewGroup) mWindow.findViewById(android.R.id.content);    
		//把R.layout.screen_simple里的content的View复制到R.id.action_bar_activity_content
		while (windowContentView.getChildCount() > 0) {
            final View child = windowContentView.getChildAt(0);
            windowContentView.removeViewAt(0);
            contentView.addView(child);
        }
		// 将原始的 content id 置为 NO_ID
    	windowContentView.setId(View.NO_ID); 
		//将subDecor中的ContentFrameLayout的ID(action_bar_activity_content)设置为content
		contentView.setId(android.R.id.content);
		//将subDecor放入到PhoneWindow
		mWindow.setContentView(subDecor);
			mContentParent.addView(view, params);
		return subDecor

//实际获取的就是subDecor中的ContentFrameLayout
ViewGroup contentParent = mSubDecor.findViewById(android.R.id.content);
LayoutInflater.from(mContext).inflate(resId, contentParent);//  R.layout.activity_main 渲染到 contentParent
```



## PhoneWindow创建流程

```
ActivityThread.performLaunchActivity
-->activity.attach
	--> new PhoneWindow()
-->mInstrumentation.callActivityOnCreate

```



## inflate流程

```java
LayoutInflater.from(mContext).inflate(resId, contentParent);
 	// 通过反射创建View
	final View temp = createViewFromTag(root, name, inflaterContext, attrs);
		//是否是sdk
        if (-1 == name.indexOf('.')) {
            //例:<LinearLayout/>
            view = onCreateView(context, parent, name, attrs);
            	PhoneLayoutInflater.onCreateView(name, attrs);//调用的是PhoneLayoutInflater
            		//sClassPrefixList = {"android.widget.","android.webkit.","android.app."};
                    for (String prefix : sClassPrefixList) {
                        //例：这里组装成android.widget.LinearLayout进行反射创建
                     	 View view = createView(name, prefix, attrs);
                         if (view != null) {
                             return view;
                         }
                    }            		
        } else {
            //例:<com.xxx.xxx/>
            view = createView(context, name, null, attrs);
            	// 通过反射创建 View 对象
            	clazz = Class.forName(prefix != null ? (prefix + name) : name, false,
        				mContext.getClassLoader()).asSubclass(View.class);
            	constructor = clazz.getConstructor(mConstructorSignature);
            	final View view = constructor.newInstance(args);
        }
	rInflateChildren(parser, temp, attrs, true);// 创建子View
		final View view = createViewFromTag(parent, name, context, attrs);// 流程同上
```



## LayoutInflate的参数的作用原理

```java
public View inflate(XmlPullParser parser, @Nullable ViewGroup root, boolean attachToRoot) {
    final AttributeSet attrs = Xml.asAttributeSet(parser);
    View result = root;
    final String name = parser.getName();
    //<Merge>标签必须要有root,并且attachToRoot必须为true,否则就会抛异常
    if (TAG_MERGE.equals(name)) {
        if (root == null || !attachToRoot) {
            throw new InflateException("<merge /> can be used only with a valid "
                    + "ViewGroup root and attachToRoot=true");
        }
        rInflate(parser, root, inflaterContext, attrs, false);
    } else {
        // 通过反射创建自定义布局view(R.layout.xx),此时是没有布局参数的
        final View temp = createViewFromTag(root, name, inflaterContext, attrs);
        ViewGroup.LayoutParams params = null;
        if (root != null) {
            //root不是null的情况,生成自定义布局的布局参数
            params = root.generateLayoutParams(attrs);
            if (!attachToRoot) {
                //如果attachToRoot为false,将布局参数设置到temp里
                temp.setLayoutParams(params);
            }
        }
        //root不为null,并且attachToRoot为true，将自定义布局view和布局参数添加到root
        if (root != null && attachToRoot) {
            root.addView(temp, params);
        }
        //root为null，或者attachToRoot为fasle，直接返回自定义布局view
        if (root == null || !attachToRoot) {
            result = temp;
        }
    }
	
	return result;
}

```

示例:

```java
// 方式一:将布局添加成功,用的root的布局参数
//params = root.generateLayoutParams(attrs);
//root.addView(temp, params);
//return root;
View view = inflater.inflate(R.layout.inflate_layout, root, true);

// 方式二：报错，一个View只能有一个父亲（The specified child already has a parent.）
//return root;
View view = inflater.inflate(R.layout.inflate_layout, root, true); // 已经addView
ll.addView(view);

// 方式三：布局成功，第三个参数为false
// 目的：想要 inflate_layout 的根节点的属性（宽高）有效，又不想让其处于某一个容器中
//params = root.generateLayoutParams(attrs);
//temp.setLayoutParams(params);
//result = temp;
//return result;
View view = inflater.inflate(R.layout.inflate_layout, root, false);
ll.addView(view);

// 方式四：root = null，这个时候不管第三个参数是什么，显示效果一样
// inflate_layout 的根节点的属性（宽高）设置无效，只是包裹子View，
// 但是子View有效，因为子View是出于容器下的
//result = temp;
//return result
View view = inflater.inflate(R.layout.inflate_layout, null, false);
ll.addView(view);

```

总结:

```
root 为null,布局参数无效，返回自定义布局view，
	 不为null,attachToRoot是true，添加到root，布局参数有效，返回的root
	 不为null,attachToRoot是false，不会添加到root，布局参数有效，返回的自定义布局view
attachToRoot 是否要添加到root里
```



## 面试题



```java
---

1.为什么requestWindowFeature()要在setContentView()之前调用
	requestWindowFeature 实际调用的是 PhoneWindow.requestFeature,
	在这个方法里面会判断如果变量 mContentParentExplicitlySet 为true则报错，
	而这个变量会在 `PhoneWindow.setContentView` 调用的时候设置为true。
2. 为什么这么设计呢？
	DecorView的xml布局是通过设置的窗口特征进行选择的。
3. 为什么 requestWindowFeature(Window.FEATURE_NO_TITLE);设置无效？
	需要用 supportRequestWindowFeature(Window.FEATURE_NO_TITLE);，因为继承的是AppCompatActivity，这个类里面会覆盖设置。

---


2.LayoutInflate几个参数的作用？


LayoutInflater inflater = LayoutInflater.from(this);
// 方式一:布局添加成功，里面执行了 ll.addView(view)
View view = inflater.inflate(R.layout.inflate_layout, ll, true);

// 方式二：报错，一个View只能有一个父亲（The specified child already has a parent.）
View view = inflater.inflate(R.layout.inflate_layout, ll, true);
ll.addView(view);

// 方式三：布局成功，第三个参数为false
// 目的：想要 inflate_layout 的根节点的属性（宽高）有效，又不想让其处于某一个容器中
View view = inflater.inflate(R.layout.inflate_layout, ll, false);
ll.addView(view);

// 方式四：root = null，这个时候不管第三个参数是什么，显示效果一样
// inflate_layout 的根节点的属性（宽高）设置无效，只是包裹子View，
// 但是子View（Button）有效，因为Button是处于容器下的
View view = inflater.inflate(R.layout.inflate_layout, null, false);
ll.addView(view);

---

3.描述下merge、include、ViewStub标签的特点
include:
1. 不能作为根元素，需要放在 ViewGroup中
2. findViewById查找不到目标控件，这个问题出现的前提是在使用include时设置了id，而在findViewById时却用了被include进来的布局的根元素id。
   1. 为什么会报空指针呢？
   如果使用include标签时设置了id，这个id就会覆盖 layout根view中设置的id，从而找不到这个id
   代码：LayoutInflate.parseInclude 
   			--》final int id = a.getResourceId(R.styleable.Include_id, View.NO_ID);
   			--》if (id != View.NO_ID) {
                    view.setId(id);
                } 

merge:
1. merge标签必须使用在根布局
2. 因为merge标签并不是View,所以在通过LayoutInflate.inflate()方法渲染的时候,第二个参数必须指定一个父容器,
	且第三个参数必须为true,也就是必须为merge下的视图指定一个父亲节点.
3. 由于merge不是View所以对merge标签设置的所有属性都是无效的.
4. 通过设置tools:parentTag="android.widget.LinearLayout"来显示布局效果
        
        
ViewStub:就是一个宽高都为0的一个View，它默认是不可见的
1. 类似include，但是一个不可见的View类，用于在运行时按需懒加载资源，只有在代码中调用了viewStub.inflate()
	或者viewStub.setVisible(View.visible)方法时才内容才变得可见。
2. 这里需要注意的一点是，当ViewStub被inflate到parent时，ViewStub就被remove掉了，即当前view hierarchy中不再存在ViewStub，
	而是使用对应的layout视图代替。

ViewStub.java
public ViewStub(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
	// 获取 InflatedId
	mInflatedId = a.getResourceId(R.styleable.ViewStub_inflatedId, NO_ID);
	// 设置不显示
    setVisibility(GONE);
}
protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	// 设置宽高为0
    setMeasuredDimension(0, 0);
}

public View inflate() {
    final ViewParent viewParent = getParent();

    if (viewParent != null && viewParent instanceof ViewGroup) {
        if (mLayoutResource != 0) {
            final ViewGroup parent = (ViewGroup) viewParent;
            // 设置ID
            final View view = inflateViewNoAdd(parent);
            // 替换自己和View
            replaceSelfWithView(view, parent);

            mInflatedViewRef = new WeakReference<>(view);
            if (mInflateListener != null) {
                mInflateListener.onInflate(this, view);
            }

            return view;
        } else {
            throw new IllegalArgumentException("ViewStub must have a valid layoutResource");
        }
    } else {
        throw new IllegalStateException("ViewStub must have a non-null ViewGroup viewParent");
    }
}

private View inflateViewNoAdd(ViewGroup parent) {
    if (mInflatedId != NO_ID) {
        view.setId(mInflatedId);
    }
}

private void replaceSelfWithView(View view, ViewGroup parent) {
    final int index = parent.indexOfChild(this);
    // 移除自己
    parent.removeViewInLayout(this);

    final ViewGroup.LayoutParams layoutParams = getLayoutParams();
    // 添加View
    if (layoutParams != null) {
        parent.addView(view, index, layoutParams);
    } else {
        parent.addView(view, index);
    }
}


```

