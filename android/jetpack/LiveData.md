# LiveData

## 概念

```
LiveData是一种具有生命周期感知能力的可观察数据持有类
LiveData可以保证屏幕上的显示内容和数据一直保持同步
特点：
1.LiveData了解UI界面的状态，如果activity不在屏幕上显示，livedata不会触发没必要的界面更新，如果activity已经被销毁，会自动清空与observer的连接，意外的调用就不会发生
2.LiveData是一个LifecycleOwner,他可以直接感知activity或fragment的生命周期
```



## 使用

```kotlin
val liveData: MutableLiveData<String> by lazy { MutableLiveData() }
liveData.value = "test"
liveData.postValue("test")
liveData.observe(this, object: Observer<String> {
	override fun onChanged(t: String?) {
		// 更新UI
	}
})
```



## 原理

### 监听流程

```java
@MainThread
public void observe(LifecycleOwner owner,Observer<? super T> observer) {
    if (owner.getLifecycle().getCurrentState() == DESTROYED) {
        return;
    }
    // 包装到 LifecycleBoundObserver
    LifecycleBoundObserver wrapper = new LifecycleBoundObserver(owner, observer);
    // 存入到 mObservers 
    ObserverWrapper existing = mObservers.putIfAbsent(observer, wrapper);
    // LifecycleRegistry.addObserver 流程
    owner.getLifecycle().addObserver(wrapper);
}
```



### 分发流程

```java
public class MutableLiveData<T> extends LiveData<T> {

    public MutableLiveData(T value) {
        super(value);
    }
    
    public MutableLiveData() {
        super();
    }

    @Override
    public void postValue(T value) {
        // 最终通过Handler调用setValue
        super.postValue(value);
    }

    @Override
    public void setValue(T value) {
        super.setValue(value);
    }
}
```

```java
public abstract class LiveData<T> {
	static final int START_VERSION = -1;
    private int mVersion;
    private volatile Object mData;
    private SafeIterableMap<Observer<? super T>, ObserverWrapper> mObservers = new SafeIterableMap<>();
    
    public LiveData() {
        mData = NOT_SET;
        // 初始化 mVersion = -1
        mVersion = START_VERSION;
    }
    
    @MainThread
    protected void setValue(T value) {
        assertMainThread("setValue");
        // mVersion + 1
        mVersion++; 
        mData = value;
        dispatchingValue(null);
    }
    
    void dispatchingValue(ObserverWrapper initiator) {
        if (mDispatchingValue) {
            mDispatchInvalidated = true;
            return;
        }
        mDispatchingValue = true;
        do {
            mDispatchInvalidated = false;
            if (initiator != null) {
                // 此处在 onStateChanged 中会调用到,例如 onStop -> onStart
                considerNotify(initiator);
                initiator = null;
            } else {
                // 正常情况会调用到这,进行循环遍历进行通知
                for (Iterator<Map.Entry<Observer<? super T>, ObserverWrapper>> iterator =
                        mObservers.iteratorWithAdditions(); iterator.hasNext(); ) {
                    // 通知
                    considerNotify(iterator.next().getValue());
                    if (mDispatchInvalidated) {
                        break;
                    }
                }
            }
        } while (mDispatchInvalidated);
        mDispatchingValue = false;
    }
    
    private void considerNotify(ObserverWrapper observer) {
        // onStart,onResume mActive = true 
        // onCreate,onStop,onDestroy mActive = false
        if (!observer.mActive) {
            return;
        }
        // 当前状态至少是 STARTED, (STARTED,RESUMED 返回true)
        if (!observer.shouldBeActive()) {
            observer.activeStateChanged(false);
            return;
        }
        // Version比较
        if (observer.mLastVersion >= mVersion) {
            return;
        }
        observer.mLastVersion = mVersion;
        // 通知更新
        observer.mObserver.onChanged((T) mData);
    }
    

	private abstract class ObserverWrapper {
        void activeStateChanged(boolean newActive) {
            if (newActive == mActive) {
                return;
            }
            mActive = newActive;
            boolean wasInactive = LiveData.this.mActiveCount == 0;
            LiveData.this.mActiveCount += mActive ? 1 : -1;
            if (wasInactive && mActive) {
                onActive(); // 自定义扩展活跃状态
            }
            if (LiveData.this.mActiveCount == 0 && !mActive) {
                onInactive(); // 自定义扩展非活跃状态
            }
            if (mActive) {
                dispatchingValue(this); // 活跃状态下进行分发
            }
        }       
    }
    
    class LifecycleBoundObserver extends ObserverWrapper implements LifecycleEventObserver {
        @Override
		boolean shouldBeActive() {
            return mOwner.getLifecycle().getCurrentState().isAtLeast(STARTED);
        }
        @Override
        public void onStateChanged(@NonNull LifecycleOwner source, @NonNull Lifecycle.Event event) {
            Lifecycle.State currentState = mOwner.getLifecycle().getCurrentState();
            // 状态为 DESTROYED ，则移除观察者
            if (currentState == DESTROYED) {
                removeObserver(mObserver);
                return;
            }
            Lifecycle.State prevState = null;
            while (prevState != currentState) {
                prevState = currentState;
                activeStateChanged(shouldBeActive()); // 状态变更
                currentState = mOwner.getLifecycle().getCurrentState();
            }
        }
     }
}
```



## other

### 数据倒灌:

```
UnPeekLiveData
```





```kotlin
object OKLiveDataBusKT {

    // 存放订阅者
    private val bus : MutableMap<String, BusMutableLiveData<Any>> by lazy { HashMap<String, BusMutableLiveData<Any>>() }

    @Synchronized
    fun <T> with(key: String, type: Class<T>, isStick: Boolean = true) : BusMutableLiveData<T> {
        if (!bus.containsKey(key)) {
            bus[key] = BusMutableLiveData(isStick)
        }
        return bus[key] as BusMutableLiveData<T>
    }

    class BusMutableLiveData<T> private constructor() : MutableLiveData<T>() {

        var isStick: Boolean = false

        constructor(isStick: Boolean) : this() {
            this.isStick = isStick
        }

        override fun observe(owner: LifecycleOwner, observer: Observer<in T>) {
            super.observe(owner, observer)
            if (!isStick) {
                hook(observer = observer)
            }
        }

        private fun hook(observer: Observer<in T>) {
            // TODO 1.得到mLastVersion
            // 获取到LivData的类中的mObservers对象
            val liveDataClass = LiveData::class.java

            val mObserversField: Field = liveDataClass.getDeclaredField("mObservers")
            mObserversField.isAccessible = true // 私有修饰也可以访问

            // 获取到这个成员变量的对象  Any == Object
            val mObserversObject: Any = mObserversField.get(this)

            // 得到map对象的class对象   private SafeIterableMap<Observer<? super T>, ObserverWrapper> mObservers =
            val mObserversClass: Class<*> = mObserversObject.javaClass

            // 获取到mObservers对象的get方法   protected Entry<K, V> get(K k) {
            val get: Method = mObserversClass.getDeclaredMethod("get", Any::class.java)
            get.isAccessible = true // 私有修饰也可以访问

            // 执行get方法
            val invokeEntry: Any = get.invoke(mObserversObject, observer)

            // 取到entry中的value   is "AAA" is String    is是判断类型 as是转换类型
            var observerWraper: Any? = null
            if (invokeEntry != null && invokeEntry is Map.Entry<*, *>) {
                observerWraper = invokeEntry.value
            }
            if (observerWraper == null) {
                throw NullPointerException("observerWraper is null")
            }

            // 得到observerWraperr的类对象
            val supperClass: Class<*> = observerWraper.javaClass.superclass
            val mLastVersion: Field = supperClass.getDeclaredField("mLastVersion")
            mLastVersion.isAccessible = true

            // TODO 2.得到mVersion
            val mVersion: Field = liveDataClass.getDeclaredField("mVersion")
            mVersion.isAccessible = true

            // TODO 3.mLastVersion=mVersion
            val mVersionValue: Any = mVersion.get(this)
            mLastVersion.set(observerWraper, mVersionValue)
        }
    }
}
```



### 同一个页面多次创建，其中一个页面销毁后导致LiveData也被销毁

```
/*
    此处用Pair解决同一个页面多次创建，其中一个页面销毁后导致LiveData也被销毁
 */
private val eventMap = ConcurrentHashMap<String, Pair<StickyLiveData<*>, Int>>()
```