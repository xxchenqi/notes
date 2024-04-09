# DataBind

布局生成路径

```
build\intermediates\data_binding_layout_info_type_merge\debug\out\activity_main-layout.xml
```

```xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<Layout directory="layout" filePath="app\src\main\res\layout\activity_main.xml"
    isBindingData="true" isMerge="false" layout="activity_main"
    modulePackage="com.derry.databinding_java" rootNodeType="android.widget.LinearLayout">
    <Variables name="user" declared="true" type="com.derry.databinding_java.User">
        <location endLine="12" endOffset="52" startLine="10" startOffset="8" />
    </Variables>
    <Targets>
        <Target tag="layout/activity_main_0" view="LinearLayout">
            <Expressions />
            <location endLine="43" endOffset="18" startLine="23" startOffset="4" />
        </Target>
        
        <!-- tag与activity_main布局中的ViewId进行连接 -->
        <Target id="@+id/tv1" tag="binding_1" view="EditText">
            <Expressions>
                <Expression attribute="android:text" text="user.name">
                    <Location endLine="34" endOffset="39" startLine="34" startOffset="12" />
                    <TwoWay>true</TwoWay>
                    <ValueLocation endLine="34" endOffset="37" startLine="34" startOffset="29" />
                </Expression>
            </Expressions>
            <location endLine="35" endOffset="37" startLine="30" startOffset="8" />
        </Target>
        <Target id="@+id/tv2" tag="binding_2" view="EditText">
            <Expressions>
                <Expression attribute="android:text" text="user.pwd">
                    <Location endLine="41" endOffset="38" startLine="41" startOffset="12" />
                    <TwoWay>true</TwoWay>
                    <ValueLocation endLine="41" endOffset="36" startLine="41" startOffset="29" />
                </Expression>
            </Expressions>
            <location endLine="42" endOffset="37" startLine="37" startOffset="8" />
        </Target>
    </Targets>
</Layout>
```



```
build\intermediates\incremental\mergeDebugResources\stripped.dir\layout\activity_main.xml
```

```xml
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:app="http://schemas.android.com/apk/res-auto"
    xmlns:tools="http://schemas.android.com/tools"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    android:orientation="vertical"
    android:tag="layout/activity_main_0">
	
    <!-- 增加了tag -->
    <EditText
        android:id="@+id/tv1"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:tag="binding_1"
        android:textSize="50sp" />

    <EditText
        android:id="@+id/tv2"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:tag="binding_2"
        android:textSize="50sp" />
</LinearLayout>  
```





## setContentView流程

```
final ActivityMainBinding binding = DataBindingUtil.setContentView(this, R.layout.activity_main);
```

```java
public static <T extends ViewDataBinding> T setContentView(@NonNull Activity activity,
        int layoutId, @Nullable DataBindingComponent bindingComponent) {
    // 调用的activity.setContentView
    activity.setContentView(layoutId);
    View decorView = activity.getWindow().getDecorView();
    ViewGroup contentView = (ViewGroup) decorView.findViewById(android.R.id.content);
    // 将 contentView 传入
    return bindToAddedViews(bindingComponent, contentView, 0, layoutId);
}
```

```java
private static <T extends ViewDataBinding> T bindToAddedViews(DataBindingComponent component,
        ViewGroup parent, int startChildren, int layoutId) {
    final int endChildren = parent.getChildCount();
    final int childrenAdded = endChildren - startChildren;
    if (childrenAdded == 1) {
        final View childView = parent.getChildAt(endChildren - 1);
        return bind(component, childView, layoutId);
    } else {
        final View[] children = new View[childrenAdded];
        for (int i = 0; i < childrenAdded; i++) {
            children[i] = parent.getChildAt(i + startChildren);
        }
        return bind(component, children, layoutId);
    }
}
```

```java
static <T extends ViewDataBinding> T bind(DataBindingComponent bindingComponent, View root,
        int layoutId) {
    // DataBinderMapper sMapper = new DataBinderMapperImpl();
    return (T) sMapper.getDataBinder(bindingComponent, root, layoutId);
}
```



## getDataBinder流程

apt生成 DataBinderMapperImpl

```java
package androidx.databinding;

public class DataBinderMapperImpl extends MergedDataBinderMapper {
  DataBinderMapperImpl() {
    addMapper(new com.derry.databinding_java.DataBinderMapperImpl());
  }
}
```



```java
package com.derry.databinding_java;

public class DataBinderMapperImpl extends DataBinderMapper {

    @Override
    public ViewDataBinding getDataBinder(DataBindingComponent component, View view, int layoutId) {
        int localizedLayoutId = INTERNAL_LAYOUT_ID_LOOKUP.get(layoutId);
        if (localizedLayoutId > 0) {
            final Object tag = view.getTag();
            if (tag == null) {
                throw new RuntimeException("view must have a tag");
            }
            switch (localizedLayoutId) {
                case LAYOUT_ACTIVITYMAIN: {
                    // 判断tag, 创建 ActivityMainBindingImpl
                    if ("layout/activity_main_0".equals(tag)) {
                        return new ActivityMainBindingImpl(component, view);
                    }
                    throw new IllegalArgumentException("The tag for activity_main is invalid. Received: " + tag);
                }
            }
        }
        return null;
    }
}
```



## mapBindings流程

```java
package com.derry.databinding_java.databinding;

public class ActivityMainBindingImpl extends ActivityMainBinding  {
    public ActivityMainBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root) {
        // 此处的3是 activity_main布局中有3个节点
        this(bindingComponent, root, mapBindings(bindingComponent, root, 3, sIncludes, sViewsWithIds));
    }
    
    public void setUser(com.derry.databinding_java.User User) {
        updateRegistration(0, User);
        this.mUser = User;
        synchronized(this) {
            mDirtyFlags |= 0x1L;
        }
        notifyPropertyChanged(BR.user);
        super.requestRebind();
    }
}
```



```java
public abstract class ViewDataBinding extends BaseObservable implements ViewBinding {
	protected static Object[] mapBindings(DataBindingComponent bindingComponent, View root,
            int numBindings, IncludedLayouts includes, SparseIntArray viewsWithIds) {
        Object[] bindings = new Object[numBindings];
        mapBindings(bindingComponent, root, bindings, includes, viewsWithIds, true);
        return bindings;
    }
    
    private static void mapBindings(DataBindingComponent bindingComponent, View view,
            Object[] bindings, IncludedLayouts includes, SparseIntArray viewsWithIds, boolean isRoot) {
        final int indexInIncludes;
        
        final ViewDataBinding existingBinding = getBinding(view);
        // 判断View是否绑定过ViewDataBinding
        if (existingBinding != null) {
            return;
        }
        Object objTag = view.getTag();
        final String tag = (objTag instanceof String) ? (String) objTag : null;
        boolean isBound = false;
        // 如果tag是根布局 && tag是layout开头
        if (isRoot && tag != null && tag.startsWith("layout")) {
            final int underscoreIndex = tag.lastIndexOf('_');
            if (underscoreIndex > 0 && isNumeric(tag, underscoreIndex + 1)) {
                final int index = parseTagInt(tag, underscoreIndex + 1);
                // 将根布局标签对应的View放在bindings数组中
                if (bindings[index] == null) {
                    bindings[index] = view;
                }
                indexInIncludes = includes == null ? -1 : index;
                isBound = true;
            } else {
                indexInIncludes = -1;
            }
        } 
        ...
    }
    
    static ViewDataBinding getBinding(View v) {
        if (v != null) {
            return (ViewDataBinding) v.getTag(R.id.dataBinding);
        }
        return null;
    }
}
```



## ActivityMainBinding

ActivityMainBindingImpl 父类 ActivityMainBinding  路径

```
build\generated\data_binding_base_class_source_out\debug\out\com\derry\databinding_java\databinding\ActivityMainBinding.java
```

```java
package com.derry.databinding_java.databinding;


public abstract class ActivityMainBinding extends ViewDataBinding {
  @NonNull
  public final EditText tv1;

  @NonNull
  public final EditText tv2;

  @Bindable
  protected User mUser;

  protected ActivityMainBinding(Object _bindingComponent, View _root, int _localFieldCount,
      EditText tv1, EditText tv2) {
    super(_bindingComponent, _root, _localFieldCount);
    this.tv1 = tv1;
    this.tv2 = tv2;
  }

  public abstract void setUser(@Nullable User user);

  @Nullable
  public User getUser() {
    return mUser;
  }
}

```



## set流程

```java
final ActivityMainBinding binding = DataBindingUtil.setContentView(this, R.layout.activity_main);
binding.setUser(user); 
```

```java
public class ActivityMainBindingImpl extends ActivityMainBinding  {
	public void setUser(com.derry.databinding_java.User User) {
        // 更新注册
        updateRegistration(0, User);
        this.mUser = User;
        synchronized(this) {
            mDirtyFlags |= 0x1L;
        }
        // 通知
        notifyPropertyChanged(BR.user);
        super.requestRebind();
    }
}
```

## updateRegistration

```java
public abstract class ViewDataBinding extends BaseObservable implements ViewBinding {
	// 使用BR属性的值做index，存储每个BR的属性对应的监听器
    private WeakListener[] mLocalFieldObservers;
    
    // 创建属性监听器
    private static final CreateWeakListener CREATE_PROPERTY_LISTENER = new CreateWeakListener() {
        @Override
        public WeakListener create(ViewDataBinding viewDataBinding, int localFieldId) {
            return new WeakPropertyListener(viewDataBinding, localFieldId).getListener();
        }
    };
    
    protected boolean updateRegistration(int localFieldId, Observable observable) {
    	return updateRegistration(localFieldId, observable, CREATE_PROPERTY_LISTENER);
	}
    
    // localFieldId对应的BR文件中的ID , observable就是观察者User对象
    private boolean updateRegistration(int localFieldId, Object observable,
            CreateWeakListener listenerCreator) {
        if (observable == null) {
            return unregisterFrom(localFieldId);
        }
        // 通过 localFieldId 获取对应的监听器
        WeakListener listener = mLocalFieldObservers[localFieldId];
        if (listener == null) {
            registerTo(localFieldId, observable, listenerCreator);
            return true;
        }
        if (listener.getTarget() == observable) {
            return false;//nothing to do, same object
        }
        unregisterFrom(localFieldId);
        registerTo(localFieldId, observable, listenerCreator);
        return true;
    }
    
    protected void registerTo(int localFieldId, Object observable, CreateWeakListener listenerCreator) {
        if (observable == null) {
            return;
        }
        WeakListener listener = mLocalFieldObservers[localFieldId];
        if (listener == null) {
            // 创建监听器
            listener = listenerCreator.create(this, localFieldId);
            mLocalFieldObservers[localFieldId] = listener;
            if (mLifecycleOwner != null) {
                // 将监听器与观察者(activity)绑定
                listener.setLifecycleOwner(mLifecycleOwner);
            }
        }
        // 将监听器与被观察者(user)绑定
        listener.setTarget(observable);
    }
}
```



```java
private static class WeakListener<T> extends WeakReference<ViewDataBinding> {
    // WeakPropertyListener
    private final ObservableReference<T> mObservable;
    
	public void setTarget(T object) {
        unregister();
        mTarget = object;
        if (mTarget != null) {
            mObservable.addListener(mTarget);
        }
    }
}
```



```java
private static class WeakPropertyListener extends Observable.OnPropertyChangedCallback implements ObservableReference<Observable> {
	@Override
    public void addListener(Observable target) {
        target.addOnPropertyChangedCallback(this);
    }
    
    @Override
    public void onPropertyChanged(Observable sender, int propertyId) {
        // 属性发生变化就会回调
        ViewDataBinding binder = mListener.getBinder();
        if (binder == null) {
            return;
        }
        Observable obj = mListener.getTarget();
        if (obj != sender) {
            return; 
        }
        
        //处理
        binder.handleFieldChange(mListener.mLocalFieldId, sender, propertyId);
    }
}
```



## 通知

```java
public void notifyPropertyChanged(int fieldId) {
    synchronized (this) {
        if (mCallbacks == null) {
            return;
        }
    }
    //最终会调用到 WeakPropertyListener.onPropertyChanged
    mCallbacks.notifyCallbacks(this, fieldId, null);
}
```



## 处理

```java
public abstract class ViewDataBinding extends BaseObservable implements ViewBinding {
    private void handleFieldChange(int mLocalFieldId, Object object, int fieldId) {
        if (mInLiveDataRegisterObserver) {
            return;
        }
        boolean result = onFieldChange(mLocalFieldId, object, fieldId);
        if (result) {
            requestRebind();
        }
    }
    
    protected void requestRebind() {
        if (mContainingBinding != null) {
            mContainingBinding.requestRebind();
        } else {
            final LifecycleOwner owner = this.mLifecycleOwner;
            if (owner != null) {
                Lifecycle.State state = owner.getLifecycle().getCurrentState();
                if (!state.isAtLeast(Lifecycle.State.STARTED)) {
                    return; 
                }
            }
            synchronized (this) {
                if (mPendingRebind) {
                    return;
                }
                mPendingRebind = true;
            }
            // 通知改变 ...
            if (USE_CHOREOGRAPHER) {
                mChoreographer.postFrameCallback(mFrameCallback);
            } else {
                mUIThreadHandler.post(mRebindRunnable);
            }
        }
    }
}
```



```java
public class ActivityMainBindingImpl extends ActivityMainBinding  {
    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
            case 0 :
                return onChangeUser((com.derry.databinding_java.User) object, fieldId);
        }
        return false;
    }
    private boolean onChangeUser(com.derry.databinding_java.User User, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x1L;
            }
            return true;
        }
        else if (fieldId == BR.name) {
            synchronized(this) {
                    mDirtyFlags |= 0x2L;
            }
            return true;
        }
        else if (fieldId == BR.pwd) {
            synchronized(this) {
                    mDirtyFlags |= 0x4L;
            }
            return true;
        }
        return false;
    }
}
```

