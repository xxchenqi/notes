```java
CoordinatorLayout

//起始点:
@Override
public void onAttachedToWindow() {
    super.onAttachedToWindow();
    resetTouchBehaviors(false);
    if (mNeedsPreDrawListener) {
        if (mOnPreDrawListener == null) {
            mOnPreDrawListener = new OnPreDrawListener();
        }
        //获取一个观察者来监听视图树
        final ViewTreeObserver vto = getViewTreeObserver();
        //通知观察者绘制即将开始
        vto.addOnPreDrawListener(mOnPreDrawListener);
    }
    if (mLastInsets == null && ViewCompat.getFitsSystemWindows(this)) {
        // We're set to fitSystemWindows but we haven't had any insets yet...
        // We should request a new dispatch of window insets
        ViewCompat.requestApplyInsets(this);
    }
    mIsAttachedToWindow = true;
}

class OnPreDrawListener implements ViewTreeObserver.OnPreDrawListener {
	@Override
	public boolean onPreDraw() {
		//绘制即将开始时会调用
	    onChildViewsChanged(EVENT_PRE_DRAW);
	    return true;
	}
}



final void onChildViewsChanged(@DispatchChangeEvent final int type) {
	final int layoutDirection = ViewCompat.getLayoutDirection(this);
	final int childCount = mDependencySortedChildren.size();
	final Rect inset = acquireTempRect();
	final Rect drawRect = acquireTempRect();
	final Rect lastDrawRect = acquireTempRect();
	
	
	for (int i = 0; i < childCount; i++) {
		//获取子view
	    final View child = mDependencySortedChildren.get(i);
	    final LayoutParams lp = (LayoutParams) child.getLayoutParams();
	    if (type == EVENT_PRE_DRAW && child.getVisibility() == View.GONE) {
	        continue;
	    }

	    // Check child views before for anchor
	    for (int j = 0; j < i; j++) {
	        final View checkChild = mDependencySortedChildren.get(j);

	        if (lp.mAnchorDirectChild == checkChild) {
	            offsetChildToAnchor(child, layoutDirection);
	        }
	    }

	    // Get the current draw rect of the view
	    getChildRect(child, true, drawRect);

	    // Accumulate inset sizes
	    if (lp.insetEdge != Gravity.NO_GRAVITY && !drawRect.isEmpty()) {
	        final int absInsetEdge = GravityCompat.getAbsoluteGravity(
	                lp.insetEdge, layoutDirection);
	        switch (absInsetEdge & Gravity.VERTICAL_GRAVITY_MASK) {
	            case Gravity.TOP:
	                inset.top = Math.max(inset.top, drawRect.bottom);
	                break;
	            case Gravity.BOTTOM:
	                inset.bottom = Math.max(inset.bottom, getHeight() - drawRect.top);
	                break;
	        }
	        switch (absInsetEdge & Gravity.HORIZONTAL_GRAVITY_MASK) {
	            case Gravity.LEFT:
	                inset.left = Math.max(inset.left, drawRect.right);
	                break;
	            case Gravity.RIGHT:
	                inset.right = Math.max(inset.right, getWidth() - drawRect.left);
	                break;
	        }
	    }

	    // Dodge inset edges if necessary
	    if (lp.dodgeInsetEdges != Gravity.NO_GRAVITY && child.getVisibility() == View.VISIBLE) {
	        offsetChildByInset(child, inset, layoutDirection);
	    }

	    if (type != EVENT_VIEW_REMOVED) {
	        // Did it change? if not continue
	        getLastChildRect(child, lastDrawRect);
	        if (lastDrawRect.equals(drawRect)) {
	            continue;
	        }
	        recordLastChildRect(child, drawRect);
	    }

	    // Update any behavior-dependent views for the change
	    for (int j = i + 1; j < childCount; j++) {
	        final View checkChild = mDependencySortedChildren.get(j);
	        final LayoutParams checkLp = (LayoutParams) checkChild.getLayoutParams();
	        final Behavior b = checkLp.getBehavior();
			
			//b.layoutDependsOn(this, checkChild, child)
			//通过2次循环去找到view的依赖关系
	        if (b != null && b.layoutDependsOn(this, checkChild, child)) {
	            if (type == EVENT_PRE_DRAW && checkLp.getChangedAfterNestedScroll()) {
	                checkLp.resetChangedAfterNestedScroll();
	                continue;
	            }

	            final boolean handled;
	            //总共这3个状态，
	            //EVENT_PRE_DRAW, EVENT_NESTED_SCROLL, EVENT_VIEW_REMOVED
	            switch (type) {
	                case EVENT_VIEW_REMOVED:
	                	//如果是移除的type，就执行onDependentViewRemoved
	                    b.onDependentViewRemoved(this, checkChild, child);
	                    handled = true;
	                    break;
	                default:
	                	//其他状态执行onDependentViewChanged
	                    handled = b.onDependentViewChanged(this, checkChild, child);
	                    break;
	            }

	            if (type == EVENT_NESTED_SCROLL) {
	                checkLp.setChangedAfterNestedScroll(handled);
	            }
	        }
	    }
	}

	releaseTempRect(inset);
	releaseTempRect(drawRect);
	releaseTempRect(lastDrawRect);
}

```

```java
behavior在哪里设置的
public static class LayoutParams extends MarginLayoutParams {
	LayoutParams(@NonNull Context context, @Nullable AttributeSet attrs) {
		mBehavior = parseBehavior(context, attrs, a.getString(
        R.styleable.CoordinatorLayout_Layout_layout_behavior));
	}
}
```

```java
为什么要用mDependencySortedChildren来获取子view	
//由于是1对多的，	view ->多个依赖
List<View> mDependencySortedChildren = new ArrayList<>()
//有向无环图
DirectedAcyclicGraph<View> mChildDag = new DirectedAcyclicGraph<>();


protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	prepareChildren();
    ...
}


private void prepareChildren() {
    mDependencySortedChildren.clear();
    mChildDag.clear();
    for (int i = 0, count = getChildCount(); i < count; i++) {
        final View view = getChildAt(i);
        final LayoutParams lp = getResolvedLayoutParams(view);
        lp.findAnchorView(this, view);
        //添加节点
        mChildDag.addNode(view);

        for (int j = 0; j < count; j++) {
            if (j == i) {
                continue;
            }
            final View other = getChildAt(j);
            if (lp.dependsOn(this, view, other)) {
                if (!mChildDag.contains(other)) {
                    //添加节点
                    mChildDag.addNode(other);
                }
                // 添加边缘,相当于有一条边连接过去
                //mChildDag.addEdge(depandency, childview)	
                mChildDag.addEdge(other, view);
            }
        }
    }
    //将图添加到mDependencySortedChildren
    mDependencySortedChildren.addAll(mChildDag.getSortedList());
  	//反转,因为view依赖于depandency
    Collections.reverse(mDependencySortedChildren);
}

```

