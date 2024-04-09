Activity

```java
public boolean dispatchTouchEvent(MotionEvent ev) {
    if (ev.getAction() == MotionEvent.ACTION_DOWN) {
        onUserInteraction();//屏保
    }
    //调用到PhoneWindow的superDispatchTouchEvent
    if (getWindow().superDispatchTouchEvent(ev)) {
        return true;
    }
    return onTouchEvent(ev);
}
```



PhoneWindow

```java
public boolean superDispatchTouchEvent(MotionEvent event) {
	//调用DecorView的superDispatchTouchEvent
    return mDecor.superDispatchTouchEvent(event);
}
```



DecorView

```java
public boolean superDispatchTouchEvent(MotionEvent event) {
	//调用到ViewGroup的dispatchTouchEvent
    return super.dispatchTouchEvent(event);
}
```



ViewGroup

```java
public boolean dispatchTouchEvent(MotionEvent ev) {
	//详细解析在下面分析
}
```



DOWN处理

```java
if (actionMasked == MotionEvent.ACTION_DOWN || mFirstTouchTarget != null) {
    final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
    //disallowIntercept 为false 就命中
    if (!disallowIntercept) {
        //调用onInterceptTouchEvent
        intercepted = onInterceptTouchEvent(ev);
        ev.setAction(action); 
    } else {
        intercepted = false;
    }
} else {
    intercepted = true;
}

//intercepted为true 下面都不会执行,拦截了
if (!canceled && !intercepted) {
    buildTouchDispatchChildList() //对子View排序
    ArrayList<View> mPreSortedChildren;//存放排序后的集合
    //从最后一个开始循环，父View在最下面，子View在最上面
    for (int i = childrenCount - 1; i >= 0; i--){
        //第一个获取到最上面的子View
        //返回对应的children
        final View child = getAndVerifyPreorderedView(preorderedList, children, childIndex);

        if (!canViewReceivePointerEvents(child) //判断是否显示 || 是否有动画
        || !isTransformedTouchPointInView(x, y, child, null)) 	//判断排除不在点击区域的view
        {
            ev.setTargetAccessibilityFocus(false);
            continue;
        }
        //返回实际还是为null，因为mFirstTouchTarget是null
        newTouchTarget = getTouchTarget(child);
        //分发给谁处理事件
        if (dispatchTransformedTouchEvent(ev, false, child, idBitsToAssign)){
            //赋值mFirstTouchTarget
            //newTouchTarget == mFirstTouchTarget
            //alreadyDispatchedToNewTouchTarget = true
            newTouchTarget = addTouchTarget(child, idBitsToAssign);
			alreadyDispatchedToNewTouchTarget = true;
            //如果处理了就退出循环
            break;
        }
    }
}




if (mFirstTouchTarget == null) {
    //child参数是null，调用自己的dispatchTouchEvent,自己处理
    handled = dispatchTransformedTouchEvent(ev, canceled, null,
        TouchTarget.ALL_POINTER_IDS);
}else{
	//循环只会执行1次,因为是单点触摸不是多点触摸
	while (target != null) {
		//next == null
        final TouchTarget next = target.next;
        if (alreadyDispatchedToNewTouchTarget && target == newTouchTarget) {
            //条件满足
            handled = true;
        } else {
            final boolean cancelChild = resetCancelNextUpFlag(target.child)
                    || intercepted;
            if (dispatchTransformedTouchEvent(ev, cancelChild,
                    target.child, target.pointerIdBits)) {
                handled = true;
            }
            if (cancelChild) {
                if (predecessor == null) {
                    mFirstTouchTarget = next;
                } else {
                    predecessor.next = next;
                }
                target.recycle();
                target = next;
                continue;
            }
        }
        predecessor = target;
        
        //target == null
        target = next;
	}
}

//整个down事件结束
return handled;
```



dispatchTransformedTouchEvent解析

```java
private boolean dispatchTransformedTouchEvent(MotionEvent event, boolean cancel,View child, int desiredPointerIdBits) {
	if (child == null) {
        //调用自己的dispatchTouchEvent,自己处理
    	handled = super.dispatchTouchEvent(transformedEvent);
	} else {
        final float offsetX = mScrollX - child.mLeft;
        final float offsetY = mScrollY - child.mTop;
        transformedEvent.offsetLocation(offsetX, offsetY);
        if (! child.hasIdentityMatrix()) {
            transformedEvent.transform(child.getInverseMatrix());
        }
        //调用对应的dispatchTouchEvent
        handled = child.dispatchTouchEvent(transformedEvent);
	}
     return handled;
}
```



MOVE

```java
//第二次进入mFirstTouchTarget不为null
if (actionMasked == MotionEvent.ACTION_DOWN|| mFirstTouchTarget != null) {
    //命中
    final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
    if (!disallowIntercept) {
        intercepted = onInterceptTouchEvent(ev);
        ev.setAction(action); 
    } else {
        intercepted = false;
    }
} else {
    intercepted = true;
}

alreadyDispatchedToNewTouchTarget = false;

if (mFirstTouchTarget == null) {
}else{
	while (target != null) {

        final TouchTarget next = target.next;
        if (alreadyDispatchedToNewTouchTarget && target == newTouchTarget) {
        } else {
            //进入到这里 
            //cancelChild == false
            final boolean cancelChild = resetCancelNextUpFlag(target.child)
                    || intercepted;
            //由子view处理
            if (dispatchTransformedTouchEvent(ev, cancelChild,
                    target.child, target.pointerIdBits)) {
                handled = true;
            }
            if (cancelChild) {
                if (predecessor == null) {
                    mFirstTouchTarget = next;
                } else {
                    predecessor.next = next;
                }
                target.recycle();
                target = next;
                continue;
            }
        }
        predecessor = target;
        
        //target == null
        target = next;
	}
}

//整个事件结束
return handled;

```





内部拦截法:

```java
//父viewpage
@Override
public boolean onInterceptTouchEvent(MotionEvent event) {
    //内部拦截发
    if (event.getAction() == MotionEvent.ACTION_DOWN) {
        super.onInterceptTouchEvent(event);
        return false;
    }
    return true;
}
```





```java
//子listview
@Override
public boolean dispatchTouchEvent(MotionEvent event) {
    int x = (int) event.getX();
    int y = (int) event.getY();

    switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN: {
            getParent().requestDisallowInterceptTouchEvent(true);
            break;
        }
        case MotionEvent.ACTION_MOVE: {
            int deltaX = x - mLastX;
            int deltaY = y - mLastY;
            if (Math.abs(deltaX) > Math.abs(deltaY)) {
                getParent().requestDisallowInterceptTouchEvent(false);
            }
            break;
        }
        case MotionEvent.ACTION_UP: {
            break;

        }
        default:
            break;
    }

    mLastX = x;
    mLastY = y;
    return super.dispatchTouchEvent(event);
}
```





//内部拦截法分析

```java
getParent().requestDisallowInterceptTouchEvent(true);

requestDisallowInterceptTouchEvent->
if (disallowIntercept) {
    mGroupFlags |= FLAG_DISALLOW_INTERCEPT;
} else {
    mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;
}

//为true
final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;

if (!disallowIntercept) {
    //进不去，onInterceptTouchEvent无效
    intercepted = onInterceptTouchEvent(ev);
    ev.setAction(action);
} else {
    intercepted = false;
}

```



拦截不成功的坑

```java
if (actionMasked == MotionEvent.ACTION_DOWN) {
    cancelAndClearTouchTargets(ev);
    resetTouchState();
}
```

```java
private void resetTouchState() {
    clearTouchTargets();
    resetCancelNextUpFlag(this);
    mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;//坑->(mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;导致这个肯定=0
    mNestedScrollAxes = SCROLL_AXIS_NONE;
}
```



所以在down的情况下onInterceptTouchEvent肯定会执行



解决:在父view

```java
public boolean onInterceptTouchEvent(MotionEvent event) {
    //内部拦截发
    if (event.getAction() == MotionEvent.ACTION_DOWN) {
        super.onInterceptTouchEvent(event);
        return false;
    }
    return true;
}
```





MOVE分析  水平滑动

```java
//第二次进入mFirstTouchTarget不为null
if (actionMasked == MotionEvent.ACTION_DOWN|| mFirstTouchTarget != null) {
    //修改了所以disallowIntercept = false
    final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
    if (!disallowIntercept) {
        //命中
        //此时 intercepted == true
        intercepted = onInterceptTouchEvent(ev);
        ev.setAction(action); 
    } else {
        intercepted = false;
    }
} else {
    //第二次进来命中这里
    intercepted = true;
}

alreadyDispatchedToNewTouchTarget = false;
//此时由子view拿着此事件
if (mFirstTouchTarget == null) {
    //第二次进来执行这里,实际调用的父view的dispatchTouchEvent
    handled = dispatchTransformedTouchEvent(ev, canceled, null,
                        TouchTarget.ALL_POINTER_IDS);
}else{
	while (target != null) {
        final TouchTarget next = target.next;
        if (alreadyDispatchedToNewTouchTarget && target == newTouchTarget) {
        } else {
            //进入到这里 
            //cancelChild == true 
            final boolean cancelChild = resetCancelNextUpFlag(target.child)
                    || intercepted;
            //由子view处理
            //处理后由于是cancel事件，所以if没命中
            if (dispatchTransformedTouchEvent(ev, cancelChild,
                    target.child, target.pointerIdBits)) {
                handled = true;
            }
            if (cancelChild) {//命中
                if (predecessor == null) {
                    mFirstTouchTarget = next;//mFirstTouchTarget = null
                } else {
                    predecessor.next = next;
                }
                target.recycle();
                target = next;
                continue;
            }
        }
        predecessor = target;
        
        //target == null
        target = next;
	}
}

//整个事件结束
return handled;

```

```java
private boolean dispatchTransformedTouchEvent(MotionEvent event, boolean cancel,View child, int desiredPointerIdBits) {
    //此时cancel 为 true
    final int oldAction = event.getAction();
	if (cancel || oldAction == MotionEvent.ACTION_CANCEL) {
    	event.setAction(MotionEvent.ACTION_CANCEL);
    	if (child == null) {
    	    handled = super.dispatchTouchEvent(event);
    	} else {
            //命中->执行cancel事件
    	    handled = child.dispatchTouchEvent(event);
    	}
    	event.setAction(oldAction);
    	return handled;
	}
    if (child == null) {
     	//第二次进来调用这里
    	handled = super.dispatchTouchEvent(transformedEvent);
	} else {
        final float offsetX = mScrollX - child.mLeft;
        final float offsetY = mScrollY - child.mTop;
        transformedEvent.offsetLocation(offsetX, offsetY);
        if (! child.hasIdentityMatrix()) {
            transformedEvent.transform(child.getInverseMatrix());
        }
      
        handled = child.dispatchTouchEvent(transformedEvent);
	}
    return handled;
}
```





外部拦截发

父:

```java
@Override
public boolean onInterceptTouchEvent(MotionEvent event) {
    int x = (int) event.getX();
    int y = (int) event.getY();

    switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN: {
            mLastX = (int) event.getX();
            mLastY = (int) event.getY();
            break;
        }
        case MotionEvent.ACTION_MOVE: {
            int deltaX = x - mLastX;
            int deltaY = y - mLastY;
            if (Math.abs(deltaX) > Math.abs(deltaY)) {
                return true;
            }
            break;
        }
        case MotionEvent.ACTION_UP: {
            break;
        }
        default:
            break;
    }
    return super.onInterceptTouchEvent(event);
}
```



