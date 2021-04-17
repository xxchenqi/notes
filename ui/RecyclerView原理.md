# RecyclerView原理解析:



## 缓存集合定义:

- mChangeScrap与 mAttachedScrap 用来缓存还在屏幕内的 ViewHolder
- mCachedViews用来缓存移除屏幕之外的 ViewHolder(保存有数据的ViewHolder)

- mViewCacheExtension 开发给用户的自定义扩展缓存，需要用户自己管理 View 的创建和缓存

- RecycledViewPool ViewHolder 缓存池(保存无数据的ViewHolder,只保存类型)



代码解析:

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
	static class ScrapData {
        final ArrayList<ViewHolder> mScrapHeap = new ArrayList<>();
        int mMaxScrap = DEFAULT_MAX_SCRAP;
	}
	SparseArray<ScrapData> mScrap = new SparseArray<>();
}


```



## 复用流程:

### 在滑动式复用

入口：滑动 Move 事件,RecyclerView.onTouchEvent --> scrollByInternal --> scrollStep --> mLayout.scrollVerticallyBy --> scrollBy  --> fill -->循环调用layoutChunk  --> layoutState.next --> addView(view);



### layoutState.next方法解析:

layoutState.next --> getViewForPosition --> tryGetViewHolderForPositionByDeadline 

**tryGetViewHolderForPositionByDeadline**方法主要是从缓存集合中去获取ViewHolder，分为以下几种情况:

1. getChangedScrapViewForPosition -- mChangeScrap 与动画相关(通过id和postion来查找)
2. getScrapOrHiddenOrCachedHolderForPosition -- mAttachedScrap 、mCachedViews (通过position查找)
3. getScrapOrCachedViewForId  -- mAttachedScrap 、mCachedViews (通过ViewType,itemId查找)
4. mViewCacheExtension.getViewForPositionAndType -- 自定义缓存 -- （使用情况：局部刷新??）
5. getRecycledViewPool().getRecycledView -- 从缓冲池里面获取



以上都没获取到缓存就执行创建流程:

mAdapter.createViewHolder --》 onCreateViewHolder

创建ViewHolder 后 绑定：

 tryBindViewHolderByDeadline--》 mAdapter.bindViewHolder--》onBindViewHolder





## 回收(缓存)流程:

LinearLayoutManager.onLayoutChildren --> detachAndScrapAttachedViews --> scrapOrRecycleView

//如果有效&&没有被移除

--> recycler.recycleViewHolderInternal(viewHolder); -- 处理 CacheView 、RecyclerViewPool 的缓存

​	--> 1.ViewHodler没有发生改变会进来 -- 先判断mCachedViews的大小

​		-->mCachedViews.size 大于 默认大小(2)  ---

​		-->recycleCachedViewAt(0) --- 从mCachedViews转移到RecycledViewPool

​			-->**addViewHolderToRecycledViewPool** --- 缓存池里面的数据都是从mCachedViews里面出来的

​			-->mCachedViews.remove(cachedViewIndex); --- 加入到缓存池后，再将mCachedViews里的数据删除

​	--> 2.**addViewHolderToRecycledViewPool**

​		 --> getRecycledViewPool().putRecycledView(holder);

​				通过itemType来获取集合

​		--> scrap.resetInternal(); ---  ViewHolder 清空(只保存类型，不保存数据)



//失效等情况,主要处理 mAttachedScrap、mChangedScrap

--> recycler.scrapView(view); ->

标记没移除或者失效

mAttachedScrap.add(holder);

其他情况

 mChangedScrap.add(holder);





## 查找缓存和复用的另一种情况 

### 在OnLayout布局时会复用:

入口：复用：RecyclerView.onLayout --> dispatchLayout --》 dispatchLayoutStep2 --》 **onLayoutChildren** --》 fill

### 缓存另外一种情况fill：

fill -->recycleByLayoutState-->

recycleViewsFromStart(相当于向上滑动) --> recycleChildren--> removeAndRecycleViewAt --> recycler.recycleView 
--> recycler.**recycleViewHolderInternal**(viewHolder); -- 只会处理 CacheView 、RecyclerViewPool 的缓存

不会去处理 recycler.scrapView(view) (mAttachedScrap,mChangedScrap)







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