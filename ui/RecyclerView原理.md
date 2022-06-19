# RecyclerView原理解析:



## 缓存集合定义:

- mChangeScrap与 mAttachedScrap 用来缓存还在屏幕内的 ViewHolder
- mCachedViews用来缓存移除屏幕之外的 ViewHolder(保存有数据的ViewHolder)
  - 通过mCachedViews获取的是不执行onCreateViewHolder和onBindViewHolder

- mViewCacheExtension 开发给用户的自定义扩展缓存，需要用户自己管理 View 的创建和缓存

- RecycledViewPool ViewHolder 缓存池(保存无数据的ViewHolder,只保存类型)
  - 从缓存池复用的显示需要调用onBindViewHolder




## 代码解析:

```java
ArrayList<ViewHolder> mAttachedScrap = new ArrayList<>();
//mCachedViews只存2个   DEFAULT_CACHE_SIZE = 2;
ArrayList<ViewHolder> mCachedViews = new ArrayList<>();
ViewCacheExtension mViewCacheExtension;
//用户自定义抽象方法
abstract View getViewForPositionAndType(Recycler recycler, int position,int type);

//RecycledViewPool(里面保存ViewHolde类型,没有数据)
class RecycledViewPool{
    //每个viewType的最大数量
    private static final int DEFAULT_MAX_SCRAP = 5;
	SparseArray<ScrapData> mScrap = new SparseArray<>();
    
	static class ScrapData {
        final ArrayList<ViewHolder> mScrapHeap = new ArrayList<>();
        int mMaxScrap = DEFAULT_MAX_SCRAP;
	}
}


```



## 入口:

```java
onTouchEvent
case MotionEvent.ACTION_MOVE: 
	scrollByInternal
        scrollStep(x, y, mReusableIntPair);
			consumedY = mLayout.scrollVerticallyBy(dy, mRecycler, mState);//LinearLayoutManager.scrollVerticallyBy
				return scrollBy(dy, recycler, state);
					final int consumed = mLayoutState.mScrollingOffset + fill(recycler, mLayoutState, state, false);
						// fill 内部流程
						//回收流程
						recycleByLayoutState(recycler, layoutState);
						//复用流程
						layoutChunk(recycler, state, layoutState, layoutChunkResult);
							View view = layoutState.next(recycler);
							addView(view);
```



## 复用流程解析:

```java
layoutState.next(recycler);
	final View view = recycler.getViewForPosition(mCurrentPosition);
		return getViewForPosition(position, false);
			return tryGetViewHolderForPositionByDeadline(position, dryRun, FOREVER_NS).itemView;
				if (mState.isPreLayout()) {
                    // 1.mChangeScrap 与动画相关(通过stableId(stableId作用:防止闪烁)和postion来查找)
                    holder = getChangedScrapViewForPosition(position);
                    fromScrapOrHiddenOrCache = holder != null;
                }
				if (holder == null) {
                    // 2.mAttachedScrap 、mCachedViews (通过position查找)
                	holder = getScrapOrHiddenOrCachedHolderForPosition(position, dryRun);
                }
				if (holder == null) {
                    if (mAdapter.hasStableIds()) {
                        // 3.mAttachedScrap 、mCachedViews (通过ViewType,itemId查找)
                        holder = getScrapOrCachedViewForId(mAdapter.getItemId(offsetPosition),type, dryRun);
                    }
                    if (holder == null && mViewCacheExtension != null) {
                        // 4.自定义缓存 -- （使用情况：局部刷新??）
                        final View view = mViewCacheExtension.getViewForPositionAndType(this, position, type);
                    }
                    if (holder == null) { 
                        // 5. 从缓冲池里面获取
                        holder = getRecycledViewPool().getRecycledView(type);
                    }
                    
                    if (holder == null) {
                        // 以上都没获取到缓存就执行创建流程:
                        holder = mAdapter.createViewHolder(RecyclerView.this, type);
                        	// 创建ViewHolder对象
                        	final VH holder = onCreateViewHolder(parent, viewType);
                        	holder.mItemViewType = viewType;
                            return holder;
                    }
                }
				if (mState.isPreLayout() && holder.isBound()) {
                    holder.mPreLayoutPosition = position;
                } else if (!holder.isBound() || holder.needsUpdate() || holder.isInvalid()) {
                    // 创建ViewHolder后绑定：
                    bound = tryBindViewHolderByDeadline(holder, offsetPosition, position, deadlineNs);
                    	mAdapter.bindViewHolder(holder, offsetPosition);
                    		// 处理数据
                    		onBindViewHolder(holder, position, holder.getUnmodifiedPayloads());
                }
```



## 回收(缓存)流程解析

```java
recycleByLayoutState(recycler, layoutState);
	if (layoutState.mLayoutDirection == LayoutState.LAYOUT_START) {
        // 向下滑动，回收下面
        recycleViewsFromEnd(recycler, scrollingOffset, noRecycleSpace);
    } else {
        // 向上滑动,回收上面
        recycleViewsFromStart(recycler, scrollingOffset, noRecycleSpace);
        	recycleChildren(recycler, 0, i);
        		removeAndRecycleViewAt(i, recycler);
        			recycler.recycleView(view);
        				// 只会处理 CacheView 、RecyclerViewPool 的缓存
        				recycleViewHolderInternal(holder);
    }
```



recycleViewHolderInternal流程

```java
recycleViewHolderInternal(holder);

int cachedViewSize = mCachedViews.size();
// int mViewCacheMax = 2;
if (cachedViewSize >= mViewCacheMax && cachedViewSize > 0) {
    // CachedViews满了,将老的ViewHolder移除并放到Pool中，新的ViewHolder放到CachedViews
    recycleCachedViewAt(0);
    	ViewHolder viewHolder = mCachedViews.get(cachedViewIndex);
    	addViewHolderToRecycledViewPool(viewHolder, true);
    		getRecycledViewPool().putRecycledView(holder);//放入pool
    			final int viewType = scrap.getItemViewType();
                final ArrayList<ViewHolder> scrapHeap = getScrapDataForType(viewType).mScrapHeap;
    			// int mMaxScrap = 5;
                if (mScrap.get(viewType).mMaxScrap <= scrapHeap.size()) {
                    return;
                }
    			// ViewHolder 清空(只保存类型，不保存数据)
    			scrap.resetInternal();
    			// 添加进RecyclerViewPool
                scrapHeap.add(scrap);
    	mCachedViews.remove(cachedViewIndex);
    cachedViewSize--;
}
int targetCacheIndex = cachedViewSize;
// 添加进mCachedViews
mCachedViews.add(targetCacheIndex, holder);
```

布局入口-回收(缓存)流程:

```java
RecyclerView.onLayout
	dispatchLayout();
		dispatchLayoutStep1();// PreLayout  动画前布局
		dispatchLayoutStep2();
			mLayout.onLayoutChildren(mRecycler, mState);
		dispatchLayoutStep3();// postLayout 动画后布局
```



```java
LinearLayoutManager.onLayoutChildren
	detachAndScrapAttachedViews(recycler);
		scrapOrRecycleView(recycler, i, v);
			final ViewHolder viewHolder = getChildViewHolderInt(view);
 
            if (viewHolder.isInvalid() && !viewHolder.isRemoved() && !mRecyclerView.mAdapter.hasStableIds()) {
                removeViewAt(index);
                // 同上 recycleViewHolderInternal 回收流程
                recycler.recycleViewHolderInternal(viewHolder);
            } else {
                detachViewAt(index);
                // 失效等情况,主要处理 mAttachedScrap、mChangedScrap 回收流程
                recycler.scrapView(view);
                	// 标记没移除或者失效
                	if (holder.hasAnyOfTheFlags(ViewHolder.FLAG_REMOVED | ViewHolder.FLAG_INVALID)|| !holder.isUpdated() || canReuseUpdatedViewHolder(holder)) {
                        // 添加到mAttachedScrap
                        mAttachedScrap.add(holder);
                    } else {
                        if (mChangedScrap == null) {
                            mChangedScrap = new ArrayList<ViewHolder>();
                        }
                        holder.setScrapContainer(this, true);
                        // 添加到mChangedScrap
                        mChangedScrap.add(holder);
                    }
                mRecyclerView.mViewInfoStore.onViewDetached(viewHolder);
            }
	fill //fill流程...
```





## 相关问题:

#### 回收什么？复用什么？

ViewHolder



#### 回收到哪里去？从哪里获得复用？

从4级缓存里回收和复用
1.mCahngeScrap、mAttachedScrap 和动画相关。在屏幕内的
2.mCacheView 屏幕外的
3.用户自定义缓存
4.RecyclerViewPool 

#### 什么时候回收？什么时候复用？

滑动时候 和 布局时候

#### 如何复用?

mChangeScrap 与动画相关(通过id和postion来查找)
mAttachedScrap 、mCachedViews (通过position查找)
mAttachedScrap 、mCachedViews (通过ViewType,itemId查找)
自定义缓存
从缓冲池里面获取

#### 如何缓存?

屏幕内
mAttachedScrap，mChangedScrap

屏幕外
如果mCacheView没超过默认大小就存到里面去
超过了 将mCacheView最先存进去的拿出来，通过itemType，找到缓冲池里对应的list，在移动缓冲池里去，
并且将ViewHolder的清空，只存类型。移动到RecyclerViewPool缓冲池里去，并且将ViewHolder的数据清空，只存类型。



在只有一种itemType时候，

onCreateViewHolder为什么会执行这么多次，

而不是只执行1次，其他时候都应该执行onBindViewHolder