# ApkChecker

## 如何使用

```
java -jar matrix-apk-canary-2.0.8.jar --config apk-checker-config.json
```

## 原理:

### 主函数入口:

```java
//ApkChecker.main
public static void main(String... args) {
    if (args.length > 0) {
        ApkChecker m = new ApkChecker();
        m.run(args);
    } else {
        System.out.println(INTRODUCT + HELP);
        System.exit(0);
    }
}
```

```java
private void run(String[] args) {
  ApkJob job = new ApkJob(args);
  try {
      job.run();
  } catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
  }
}
```

### config文件解析，创建对应的task

```java
public void run() throws  Exception {
    // config文件解析，创建对应的task，添加注册表?
    if (parseParams()) {
        //创建解压任务
        ApkTask unzipTask = TaskFactory.factory(TaskFactory.TASK_TYPE_UNZIP, jobConfig, new HashMap<String, String>());
        //添加到preTasks
        preTasks.add(unzipTask);
        for (String format : jobConfig.getOutputFormatList()) {
            //解析--format，创建JobResult，添加到集合
            JobResult result = JobResultFactory.factory(format, jobConfig);
            if (result != null) {
                jobResults.add(result);
            } else {
                Log.w(TAG, "Unknown output format name '%s' !", format);
            }
        }
        //执行任务
        execute();
    } else {
        ApkChecker.printHelp();
    }
}
```

### 重要属性

```
private ExecutorService executor;
private List<ApkTask> preTasks;
private List<ApkTask> taskList;
private List<JobResult> jobResults;
```



### 执行流程

```java
private void execute() throws Exception {
    try {
        // 遍历preTasks
        for (ApkTask preTask : preTasks) {
            //Task初始化流程
            preTask.init();
            //Task调用流程,并返回TASK结果
            TaskResult taskResult = preTask.call();
            if (taskResult != null) {
                TaskResult formatResult = null;
                // 遍历jobResults(JobHtmlResult,JobJsonResult)
                for (JobResult jobResult : jobResults) {
                    //创建TaskResult (反射创建?)
                    //json转HTML/格式化json
                    formatResult = TaskResultFactory.transferTaskResult(taskResult.taskType, taskResult, jobResult.getFormat(), jobConfig);
                    if (formatResult != null) {
                        //添加到 jobResult
                        jobResult.addTaskResult(formatResult);
                    }
                }
            }
        }
        /*
        	遍历taskList 初始化
            0 = {ManifestAnalyzeTask@2880} 
            1 = {ShowFileSizeTask@2881} 
            2 = {MethodCountTask@2882} 
            3 = {ResProguardCheckTask@2883} 
            4 = {FindNonAlphaPngTask@2884} 
            5 = {MultiLibCheckTask@2885} 
            6 = {UncompressedFileTask@2886} 
            7 = {CountRTask@2887} 
            8 = {DuplicateFileTask@2888} 
            9 = {UnusedResourcesTask@2889} 
            10 = {UnusedAssetsTask@2890} 
        */
        for (ApkTask task : taskList) {
            task.init();
        }
        //invokeAll方法会新建一个ArrayList然后foreach循环tasks将每一个tasks add进这个ArrayList。因此如果需要将各个线程的结果按照一定顺序获取，那么可以使用有固定排序的数据结构比如ArrayList来存储每一个task，当方法调用完之后每一个task的结果会按tasks的顺序返回。
        // timeoutSeconds = 超时时间
        List<Future<TaskResult>> futures = executor.invokeAll(taskList, timeoutSeconds, TimeUnit.SECONDS);
        for (Future<TaskResult> future : futures) {
            // 获取结果
            TaskResult taskResult = future.get();
            if (taskResult != null) {
                TaskResult formatResult = null;
                for (JobResult jobResult : jobResults) {
                    formatResult = TaskResultFactory.transferTaskResult(taskResult.taskType, taskResult, jobResult.getFormat(), jobConfig);
                    if (formatResult != null) {
                        jobResult.addTaskResult(formatResult);
                    }
                }
            }
        }
        
        //关闭线程池
        executor.shutdownNow();

        //遍历jobResults 进行输出
        for (JobResult jobResult : jobResults) {
            jobResult.output();
        }
        
        // 删除解压文件    
        Log.d(TAG, "parse apk end, try to delete tmp un zip files");
        FileUtils.deleteDirectory(new File(jobConfig.getUnzipPath()));

    } catch (Exception e) {
        Log.e(TAG, "Task executor execute with error:" + e.getMessage());
        throw e;
    }
}
```







## TASK

### ApkTask

实现了Callable(带返回值的Runnable)

```java
public abstract class ApkTask implements Callable<TaskResult> {

    private static final String TAG = "Matrix.ApkTask";

    protected int type;
    protected JobConfig config;
    protected Map<String, String> params;
    protected List<ApkTaskProgressListener> progressListeners;

    public interface ApkTaskProgressListener {
        void getProgress(int progress, String message);
    }

    public ApkTask(JobConfig config, Map<String, String> params) {
        this.params = params;
        this.config = config;
        progressListeners = new LinkedList<>();
    }

    public int getType() {
        return type;
    }

    public void init() throws TaskInitException {
        if (config == null) {
            throw new TaskInitException(TAG + "---jobConfig can not be null!");
        }

        if (params == null) {
            throw new TaskInitException(TAG + "---params can not be null!");
        }
    }

    public void addProgressListener(ApkTaskProgressListener listener) {
        if (listener != null) {
            progressListeners.add(listener);
        }
    }

    public void removeProgressListener(ApkTaskProgressListener listener) {
        if (listener != null) {
            progressListeners.remove(listener);
        }
    }

    protected void notifyProgress(int progress, String message) {
        for (ApkTaskProgressListener listener : progressListeners) {
            listener.getProgress(progress, message);
        }
    }

    @Override
    public abstract TaskResult call() throws TaskExecuteException;
}
```



### UnzipTask

输入的Apk文件首先会经过UnzipTask处理，解压到指定目录，在这一步还会做一些全局的准备工作，包括反混淆类名（读取mapping.txt）、反混淆资源(读取resMapping.txt)、统计文件大小等。



初始化

```java
public void init() throws TaskInitException {
	inputFile = new File(config.getApkPath());
	outputFile = new File(config.getUnzipPath());
	mappingTxt = new File(config.getMappingFilePath());
	resMappingTxt = new File(config.getResMappingFilePath());
}
```

call调用流程

```java
public TaskResult call() throws TaskExecuteException {
	zipFile = new ZipFile(inputFile);
    // 创建任务结果
    TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
    // apk大小添加到结果
    ((TaskJsonResult) taskResult).add("total-size", inputFile.length());
    //解析mapping文件，存储到proguardClassMap
    readMappingTxtFile();
    config.setProguardClassMap(proguardClassMap);
    //解析ResMapping,存储到resguardMap
    readResMappingTxtFile();
	config.setResguardMap(resguardMap);
    Enumeration entries = zipFile.entries();
    
    //解压流程
    JsonArray jsonArray = new JsonArray();
    String outEntryName = "";
    while (entries.hasMoreElements()) {
        ZipEntry entry = (ZipEntry) entries.nextElement();
        //解压
        outEntryName = writeEntry(zipFile, entry);
        if (!Util.isNullOrNil(outEntryName)) {
            JsonObject fileItem = new JsonObject();
            fileItem.addProperty("entry-name", outEntryName);
            fileItem.addProperty("entry-size", entry.getCompressedSize());
            jsonArray.add(fileItem);
            // 存入对应的文件名和(文件大小与压缩后大小)
            entrySizeMap.put(outEntryName, Pair.of(entry.getSize(), entry.getCompressedSize()));
            entryNameMap.put(entry.getName(), outEntryName);
        }
    }
    
    //赋值
    config.setEntrySizeMap(entrySizeMap);
	config.setEntryNameMap(entryNameMap);
    ((TaskJsonResult) taskResult).add("entries", jsonArray);
    taskResult.setStartTime(startTime);
    taskResult.setEndTime(System.currentTimeMillis());
    return taskResult;
}
```



### ManifestAnalyzeTask

用于读取AndroidManifest.xml中的信息

> 实现方法：利用ApkTool中的 *AXmlResourceParser* 来解析二进制的AndroidManifest.xml文件，并且可以反混淆出AndroidManifest.xml中引用的资源名称。



初始化

```java
public void init() throws TaskInitException {
    String inputPath = config.getUnzipPath();
    inputFile = new File(inputPath, ApkConstants.MANIFEST_FILE_NAME);
    arscFile = new File(inputPath, ApkConstants.ARSC_FILE_NAME);
}
```

call调用流程

```java
public TaskResult call() throws TaskExecuteException {
    try {
        ManifestParser manifestParser = null;
        //创建解析器
        if (!FileUtil.isLegalFile(arscFile)) {
            manifestParser = new ManifestParser(inputFile);
        } else {
            manifestParser = new ManifestParser(inputFile, arscFile);
        }
        TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }
        long startTime = System.currentTimeMillis();
        // 解析sdk版本、权限、app版本、activity、application等
        JsonObject jsonObject = manifestParser.parse();
        ((TaskJsonResult) taskResult).add("manifest", jsonObject);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```

### ShowFileSizeTask

根据文件大小以及文件后缀名来过滤出超过指定大小的文件，并按照升序或降序排列结果。

> 实现方法：直接利用UnzipTask中统计的文件大小来过滤输出结果。

 

```java
public void init() throws TaskInitException {
    String inputPath = config.getUnzipPath();
    inputFile = new File(inputPath);

    if (params.containsKey(JobConstants.PARAM_MIN_SIZE_IN_KB)) {
        try {
            downLimit = Long.parseLong(params.get(JobConstants.PARAM_MIN_SIZE_IN_KB));
        } catch (NumberFormatException e) {
            Log.e(TAG, "DOWN-LIMIT-SIZE '" + params.get(JobConstants.PARAM_MIN_SIZE_IN_KB) + "' is not number format!");
        }
    }

    if (params.containsKey(JobConstants.PARAM_ORDER)) {
        if (JobConstants.ORDER_ASC.equals(params.get(JobConstants.PARAM_ORDER))) {
            order = JobConstants.ORDER_ASC;
        } else if (JobConstants.ORDER_DESC.equals(params.get(JobConstants.PARAM_ORDER))) {
            order = JobConstants.ORDER_DESC;
        } else {
            Log.e(TAG, "ORDER-BY '" + params.get(JobConstants.PARAM_ORDER) + "' is not correct!");
        }
    }

    filterSuffix = new HashSet<>();

    if (params.containsKey(JobConstants.PARAM_SUFFIX) && !Util.isNullOrNil(params.get(JobConstants.PARAM_SUFFIX))) {
        String[]  suffix = params.get(JobConstants.PARAM_SUFFIX).split(",");
        for (String suffixStr : suffix) {
            filterSuffix.add(suffixStr.trim());
        }
    }

    entryList = new ArrayList<Pair<String, Long>>();
}
```



```java
public TaskResult call() throws TaskExecuteException {
    try {
        // 创建任务结果
        TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }

        long startTime = System.currentTimeMillis();

        Map<String, Pair<Long, Long>> entrySizeMap = config.getEntrySizeMap();
        if (!entrySizeMap.isEmpty()) {                                                        
            for (Map.Entry<String, Pair<Long, Long>> entry : entrySizeMap.entrySet()) {
                final String suffix = getSuffix(entry.getKey());
                Pair<Long, Long> size = entry.getValue();
                // 文件是否超出限制
                if (size.getFirst() >= downLimit * ApkConstants.K1024) {
                    if (filterSuffix.isEmpty() || filterSuffix.contains(suffix)) {
                        // 超出限制就添加进集合中
                        entryList.add(Pair.of(entry.getKey(), size.getFirst()));
                    } else {
                        Log.d(TAG, "file: %s, filter by suffix.", entry.getKey());
                    }
                } else {
                    Log.d(TAG, "file:%s, size:%d B, downlimit:%d KB", entry.getKey(), size.getFirst(), downLimit);
                }
            }
        }
		//排序
        Collections.sort(entryList, new Comparator<Pair<String, Long>>() {
            @Override
            public int compare(Pair<String, Long> entry1, Pair<String, Long> entry2) {
                long file1Len = entry1.getSecond();
                long file2Len = entry2.getSecond();
                if (file1Len < file2Len) {
                    if (order.equals(JobConstants.ORDER_ASC)) {
                        return -1;
                    } else {
                        return 1;
                    }
                } else if (file1Len > file2Len) {
                    if (order.equals(JobConstants.ORDER_DESC)) {
                        return -1;
                    } else {
                        return 1;
                    }
                } else {
                    return 0;
                }
            }
        });

        JsonArray jsonArray = new JsonArray();
        for (Pair<String, Long> sortFile : entryList) {
            JsonObject fileItem = new JsonObject();
            fileItem.addProperty("entry-name", sortFile.getFirst());
            fileItem.addProperty("entry-size", sortFile.getSecond());
            jsonArray.add(fileItem);
        }
        ((TaskJsonResult) taskResult).add("files", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```





### MethodCountTask

可以统计出各个Dex中的方法数，并按照类名或者包名来分组输出结果。

> 实现方法：利用google开源的 *com.android.dexdeps* 类库来读取dex文件，统计方法数。

 

统计数据:

"total-internal-classes" -> {JsonPrimitive@3691} "8245"
"total-internal-methods" -> {JsonPrimitive@3693} "55984"

"total-external-classes" -> {JsonPrimitive@3974} "1259"
"total-external-methods" -> {JsonPrimitive@3977} "8284"



```java
public void init() throws TaskInitException {
    String inputPath = config.getUnzipPath();
    inputFile = new File(inputPath);
    File[] files = inputFile.listFiles();
    try {
        if (files != null) {
            for (File file : files) {
                //获取dex文件,添加进集合中
                if (file.isFile() && file.getName().endsWith(ApkConstants.DEX_FILE_SUFFIX)) {
                    dexFileNameList.add(file.getName());
                    RandomAccessFile randomAccessFile = new RandomAccessFile(file, "rw");
                    dexFileList.add(randomAccessFile);
                }
            }
        }
    } catch (FileNotFoundException e) {
        throw new TaskInitException(e.getMessage(), e);
    }
    
    if (params.containsKey(JobConstants.PARAM_GROUP)) {
        if (JobConstants.GROUP_PACKAGE.equals(params.get(JobConstants.PARAM_GROUP))) {
            group = JobConstants.GROUP_PACKAGE;
        } else if (JobConstants.GROUP_CLASS.equals(params.get(JobConstants.PARAM_GROUP))) {
            group = JobConstants.GROUP_CLASS;
        } else {
            Log.e(TAG, "GROUP-BY '" + params.get(JobConstants.PARAM_GROUP) + "' is not correct!");
        }
    }
}
```



```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }
        long startTime = System.currentTimeMillis();
        JsonArray jsonArray = new JsonArray();
        for (int i = 0; i < dexFileList.size(); i++) {
            RandomAccessFile dexFile = dexFileList.get(i);
            //解析dex文件
            countDex(dexFile);
            dexFile.close();
            // 计算内部方法总数
            int totalInternalMethods = sumOfValue(classInternalMethod);
            // 计算外部方法总数
            int totalExternalMethods = sumOfValue(classExternalMethod);
            JsonObject jsonObject = new JsonObject();
            jsonObject.addProperty("dex-file", dexFileNameList.get(i));
			// 内部类方法统计
            if (JobConstants.GROUP_CLASS.equals(group)) {
                List<String> sortList = sortKeyByValue(classInternalMethod);
                JsonArray classes = new JsonArray();
                for (String className : sortList) {
                    JsonObject classObj = new JsonObject();
                    classObj.addProperty("name", className);
                    classObj.addProperty("methods", classInternalMethod.get(className));
                    classes.add(classObj);
                }
                jsonObject.add("internal-classes", classes);
            } else if (JobConstants.GROUP_PACKAGE.equals(group)) {
                String packageName;
                for (Map.Entry<String, Integer> entry : classInternalMethod.entrySet()) {
                    packageName = ApkUtil.getPackageName(entry.getKey());
                    if (!Util.isNullOrNil(packageName)) {
                        if (!pkgInternalRefMethod.containsKey(packageName)) {
                            pkgInternalRefMethod.put(packageName, entry.getValue());
                        } else {
                            pkgInternalRefMethod.put(packageName, pkgInternalRefMethod.get(packageName) + entry.getValue());
                        }
                    }
                }
                List<String> sortList = sortKeyByValue(pkgInternalRefMethod);
                JsonArray packages = new JsonArray();
                for (String pkgName : sortList) {
                    JsonObject pkgObj = new JsonObject();
                    pkgObj.addProperty("name", pkgName);
                    pkgObj.addProperty("methods", pkgInternalRefMethod.get(pkgName));
                    packages.add(pkgObj);
                }
                jsonObject.add("internal-packages", packages);
            }
            jsonObject.addProperty("total-internal-classes", classInternalMethod.size());
            jsonObject.addProperty("total-internal-methods", totalInternalMethods);
			// 外部类方法统计
            if (JobConstants.GROUP_CLASS.equals(group)) {
                List<String> sortList = sortKeyByValue(classExternalMethod);
                JsonArray classes = new JsonArray();
                for (String className : sortList) {
                    JsonObject classObj = new JsonObject();
                    classObj.addProperty("name", className);
                    classObj.addProperty("methods", classExternalMethod.get(className));
                    classes.add(classObj);
                }
                jsonObject.add("external-classes", classes);

            } else if (JobConstants.GROUP_PACKAGE.equals(group)) {
                String packageName = "";
                for (Map.Entry<String, Integer> entry : classExternalMethod.entrySet()) {
                    packageName = ApkUtil.getPackageName(entry.getKey());
                    if (!Util.isNullOrNil(packageName)) {
                        if (!pkgExternalMethod.containsKey(packageName)) {
                            pkgExternalMethod.put(packageName, entry.getValue());
                        } else {
                            pkgExternalMethod.put(packageName, pkgExternalMethod.get(packageName) + entry.getValue());
                        }
                    }
                }
                List<String> sortList = sortKeyByValue(pkgExternalMethod);
                JsonArray packages = new JsonArray();
                for (String pkgName : sortList) {
                    JsonObject pkgObj = new JsonObject();
                    pkgObj.addProperty("name", pkgName);
                    pkgObj.addProperty("methods", pkgExternalMethod.get(pkgName));
                    packages.add(pkgObj);
                }
                jsonObject.add("external-packages", packages);

            }
            jsonObject.addProperty("total-external-classes", classExternalMethod.size());
            jsonObject.addProperty("total-external-methods", totalExternalMethods);
            jsonArray.add(jsonObject);
        }
        ((TaskJsonResult) taskResult).add("dex-files", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```



```java
private void countDex(RandomAccessFile dexFile) throws IOException {
    classInternalMethod.clear();
    classExternalMethod.clear();
    pkgInternalRefMethod.clear();
    pkgExternalMethod.clear();
    DexData dexData = new DexData(dexFile);
    // 加载dex
    dexData.load();
    // 获取方法
    MethodRef[] methodRefs = dexData.getMethodRefs();
    // 获取
    ClassRef[] externalClassRefs = dexData.getExternalReferences();
    Map<String, String> proguardClassMap = config.getProguardClassMap();
    String className = null;
    // 解析系统类
    for (ClassRef classRef : externalClassRefs) {
        className = ApkUtil.getNormalClassName(classRef.getName());
        //className在proguardClassMap 说明是混淆的，通过proguardClassMap来获取具体类名
        if (proguardClassMap.containsKey(className)) {
            className = proguardClassMap.get(className);
        }
        if (className.indexOf('.') == -1) {
            continue;
        }
        classExternalMethod.put(className, 0);
    }
    // 解析方法
    for (MethodRef methodRef : methodRefs) {
        className = ApkUtil.getNormalClassName(methodRef.getDeclClassName());
        if (proguardClassMap.containsKey(className)) {
            className = proguardClassMap.get(className);
        }
        if (!Util.isNullOrNil(className)) {
            if (className.indexOf('.') == -1) {
                continue;
            }
            if (classExternalMethod.containsKey(className)) {
                classExternalMethod.put(className, classExternalMethod.get(className) + 1);
            } else if (classInternalMethod.containsKey(className)) {
                classInternalMethod.put(className, classInternalMethod.get(className) + 1);
            } else {
                classInternalMethod.put(className, 1);
            }
        }
    }

    // 移除0方法的引用类
    Iterator<String> iterator = classExternalMethod.keySet().iterator();
    while (iterator.hasNext()) {
        if (classExternalMethod.get(iterator.next()) == 0) {
            iterator.remove();
        }
    }
}
```





### ResProguardCheckTask

可以判断apk是否经过了资源混淆

> 实现方法：资源混淆之后的res文件夹会重命名成r，直接判断是否存在文件夹r即可判断是否经过了资源混淆。

```java
public TaskResult call() throws TaskExecuteException {
    File resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_PROGUARD_NAME);
    try {
        TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }
        long startTime = System.currentTimeMillis();
        // 判断是否资源混淆
        if (resDir.exists() && resDir.isDirectory()) {
            Log.i(TAG, "find resource directory " + resDir.getAbsolutePath());
            ((TaskJsonResult) taskResult).add("hasResProguard", true);
        } else {
            resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_NAME);
            if (resDir.exists() && resDir.isDirectory()) {
                File[] dirs = resDir.listFiles();
                boolean hasProguard = true;
                for (File dir : dirs) {
                    // 判断是否资源混淆
                    if (dir.isDirectory() && !fileNamePattern.matcher(dir.getName()).matches()) {
                        hasProguard = false;
                        Log.i(TAG, "directory " + dir.getName() + " has a non-proguard name!");
                        break;
                    }
                }
                ((TaskJsonResult) taskResult).add("hasResProguard", hasProguard);
            } else {
                throw new TaskExecuteException(TAG + "---No resource directory found!");
            }
        }
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```



### FindNonAlphaPngTask 

检测出apk中非透明的png文件

> 实现方法：通过 *java.awt.BufferedImage* 类读取png文件并判断是否有alpha通道。

```java
public TaskResult call() throws TaskExecuteException {
    File resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_PROGUARD_NAME);
    TaskResult taskResult = null;
    try {
        taskResult = TaskResultFactory.factory(getType(), TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        long startTime = System.currentTimeMillis();
        if (resDir.exists() && resDir.isDirectory()) {
            findNonAlphaPng(resDir);
        } else {
            resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_NAME);
            if (resDir.exists() && resDir.isDirectory()) {
                findNonAlphaPng(resDir);
            }
        }

        Collections.sort(nonAlphaPngList, new Comparator<Pair<String, Long>>() {
            @Override
            public int compare(Pair<String, Long> entry1, Pair<String, Long> entry2) {
                long file1Len = entry1.getSecond();
                long file2Len = entry2.getSecond();
                if (file1Len < file2Len) {
                    return 1;
                } else if (file1Len > file2Len) {
                    return -1;
                } else {
                    return 0;
                }
            }
        });

        JsonArray jsonArray = new JsonArray();
        for (Pair<String, Long> entry : nonAlphaPngList) {
            if (!Util.isNullOrNil(entry.getFirst())) {
                JsonObject jsonObject = new JsonObject();
                jsonObject.addProperty("entry-name", entry.getFirst());
                jsonObject.addProperty("entry-size", entry.getSecond());
                jsonArray.add(jsonObject);
            }
        }
        ((TaskJsonResult) taskResult).add("files", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```



```java
private void findNonAlphaPng(File file) throws IOException {
    if (file != null) {
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (File tempFile : files) {
                findNonAlphaPng(tempFile);
            }
        } else if (file.isFile() && file.getName().endsWith(ApkConstants.PNG_FILE_SUFFIX) && !file.getName().endsWith(ApkConstants.NINE_PNG)) {
            BufferedImage bufferedImage = ImageIO.read(file);
            // 判断是否有Alpha通道
            if (bufferedImage != null && bufferedImage.getColorModel() != null && !bufferedImage.getColorModel().hasAlpha()) {
                String filename = file.getAbsolutePath().substring(inputFile.getAbsolutePath().length() + 1);
                if (entryNameMap.containsKey(filename)) {
                    filename = entryNameMap.get(filename);
                }
                long size = file.length();
                if (entrySizeMap.containsKey(filename)) {
                    size = entrySizeMap.get(filename).getFirst();
                }
                if (size >= downLimitSize * ApkConstants.K1024) {
                    // 添加进没有透明通道图片集合中
                    nonAlphaPngList.add(Pair.of(filename, file.length()));
                }
            }
        }
    }
}
```





### MultiLibCheckTask

可以判断apk中是否有针对多个ABI的so

> 实现方法：直接判断lib文件夹下是否包含多个目录。

```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(getType(), TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }
        long startTime = System.currentTimeMillis();
        JsonArray jsonArray = new JsonArray();
        if (libDir.exists() && libDir.isDirectory()) {
            File[] dirs = libDir.listFiles();
            for (File dir : dirs) {
                if (dir.isDirectory()) {
                    jsonArray.add(dir.getName());
                }
            }
        }
        ((TaskJsonResult) taskResult).add("lib-dirs", jsonArray);
        if (jsonArray.size() > 1) {
            ((TaskJsonResult) taskResult).add("multi-lib", true);
        } else {
            ((TaskJsonResult) taskResult).add("multi-lib", false);
        }
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```





### UncompressedFileTask

可以检测出未经压缩的文件类型

> 实现方法：直接利用UnzipTask中统计的各个文件的压缩前和压缩后的大小，判断压缩前和压缩后大小是否相等。

```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(type, TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        if (taskResult == null) {
            return null;
        }
        long startTime = System.currentTimeMillis();
        JsonArray jsonArray = new JsonArray();
        Map<String, Pair<Long, Long>> entrySizeMap = config.getEntrySizeMap();
        if (!entrySizeMap.isEmpty()) {                                                          
            for (Map.Entry<String, Pair<Long, Long>> entry : entrySizeMap.entrySet()) {
                final String suffix = getSuffix(entry.getKey());
                Pair<Long, Long> size = entry.getValue();
                if (filterSuffix.isEmpty() || filterSuffix.contains(suffix)) {
                    // 未压缩资源
                    if (!uncompressSizeMap.containsKey(suffix)) {
                        uncompressSizeMap.put(suffix, size.getFirst());
                    } else {
                        uncompressSizeMap.put(suffix, uncompressSizeMap.get(suffix) + size.getFirst());
                    }
                    if (!compressSizeMap.containsKey(suffix)) {
                        compressSizeMap.put(suffix, size.getSecond());
                    } else {
                        compressSizeMap.put(suffix, compressSizeMap.get(suffix) + size.getSecond());
                    }
                } else {
                    Log.d(TAG, "file: %s, filter by suffix.", entry.getKey());
                }
            }
        }

        for (String suffix : uncompressSizeMap.keySet()) {
            if (uncompressSizeMap.get(suffix).equals(compressSizeMap.get(suffix))) {
                JsonObject fileItem = new JsonObject();
                // 未压缩资源的后缀
                fileItem.addProperty("suffix", suffix);
                // 获取未压缩的总大小
                fileItem.addProperty("total-size", uncompressSizeMap.get(suffix));
                jsonArray.add(fileItem);
            }
        }
        ((TaskJsonResult) taskResult).add("files", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```



### CountRTask

统计R类以及R类的中的field数目

> 实现方法：利用 *com.android.dexdeps* 类库来读取dex文件，找出R类以及field数目。

 

```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(type, TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        long startTime = System.currentTimeMillis();
        Map<String, String> classProguardMap = config.getProguardClassMap();
        for (RandomAccessFile dexFile : dexFileList) {
            DexData dexData = new DexData(dexFile);
            // 加载dex
            dexData.load();
            dexFile.close();
            // 获取内部引用
            ClassRef[] defClassRefs = dexData.getInternalReferences();
            for (ClassRef classRef : defClassRefs) {
                // 获取类名 Landroidx/activity/R$attr; ->  androidx.activity.R$attr 
                String className = ApkUtil.getNormalClassName(classRef.getName());
                // 判断是否存在混淆
                if (classProguardMap.containsKey(className)) {
                    className = classProguardMap.get(className);
                }
                // 获取纯类名 androidx.activity.R$attr -> androidx.activity.R
                String pureClassName = getOuterClassName(className);
                // 判断是否是R类
                if (pureClassName.endsWith(".R") || "R".equals(pureClassName)) {
                    if (!classesMap.containsKey(pureClassName)) {
                        classesMap.put(pureClassName, classRef.getFieldArray().length);
                    } else {
                        classesMap.put(pureClassName, classesMap.get(pureClassName) + classRef.getFieldArray().length);
                    }
                }
            }
        }

        JsonArray jsonArray = new JsonArray();
        long totalSize = 0;
        Map<String, String> proguardClassMap = config.getProguardClassMap();
        for (Map.Entry<String, Integer> entry : classesMap.entrySet()) {
            JsonObject jsonObject = new JsonObject();
            if (proguardClassMap.containsKey(entry.getKey())) {
                jsonObject.addProperty("name", proguardClassMap.get(entry.getKey()));
            } else {
                jsonObject.addProperty("name", entry.getKey());
            }
            jsonObject.addProperty("field-count", entry.getValue());
            totalSize += entry.getValue();
            jsonArray.add(jsonObject);
        }
        ((TaskJsonResult) taskResult).add("R-count", jsonArray.size());
        ((TaskJsonResult) taskResult).add("Field-counts", totalSize);

        ((TaskJsonResult) taskResult).add("R-classes", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}

private String getOuterClassName(String className) {
    if (!Util.isNullOrNil(className)) {
        int index = className.indexOf('$');
        if (index >= 0) {
            className = className.substring(0, index);
        }
    }
    return className;
}
```



### DuplicateFileTask 

检测出冗余的文件

> 实现方法：通过比较文件的MD5是否相等来判断文件内容是否相同。

 

```java
private File inputFile;
// md5值，重复md5的文件名
private Map<String, List<String>> md5Map;
// md5,文件大小
private List<Pair<String, Long>> fileSizeList;

// 文件名,原始大小和压缩后大小
private Map<String, Pair<Long, Long>> entrySizeMap;
private Map<String, String> entryNameMap;
```



```java
public TaskResult call() throws TaskExecuteException {
    TaskResult taskResult = null;
    try {
        taskResult = TaskResultFactory.factory(getType(), TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        long startTime = System.currentTimeMillis();
        JsonArray jsonArray = new JsonArray();
		// 计算md5
        computeMD5(inputFile);
		// 按照大小排序
        Collections.sort(fileSizeList, new Comparator<Pair<String, Long>>() {
            @Override
            public int compare(Pair<String, Long> entry1, Pair<String, Long> entry2) {
                long file1Len = entry1.getSecond();
                long file2Len = entry2.getSecond();
                if (file1Len < file2Len) {
                    return 1;
                } else if (file1Len > file2Len) {
                    return -1;
                } else {
                    return 0;
                }
            }
        });
		
        for (Pair<String, Long> entry : fileSizeList) {
            if (md5Map.get(entry.getFirst()).size() > 1) {
                JsonObject jsonObject = new JsonObject();
                jsonObject.addProperty("md5", entry.getFirst());
                jsonObject.addProperty("size", entry.getSecond());
                JsonArray jsonFiles = new JsonArray();
                for (String filename : md5Map.get(entry.getFirst())) {
                    jsonFiles.add(filename);
                }
                jsonObject.add("files", jsonFiles);
                jsonArray.add(jsonObject);
            }
        }
        ((TaskJsonResult) taskResult).add("files", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
    return taskResult;
}
```



```java
private void computeMD5(File file) throws NoSuchAlgorithmException, IOException {
    if (file != null) {
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (File resFile : files) {
                computeMD5(resFile);
            }
        } else {
            MessageDigest msgDigest = MessageDigest.getInstance("MD5");
            BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(file));
            byte[] buffer = new byte[512];
            int readSize = 0;
            long totalRead = 0;
            while ((readSize = inputStream.read(buffer)) > 0) {
                msgDigest.update(buffer, 0, readSize);
                totalRead += readSize;
            }
            inputStream.close();
            if (totalRead > 0) {
                // 获取md5
                final String md5 = Util.byteArrayToHex(msgDigest.digest());
                String filename = file.getAbsolutePath().substring(inputFile.getAbsolutePath().length() + 1);
                if (entryNameMap.containsKey(filename)) {
                    filename = entryNameMap.get(filename);
                }
                
                if (!md5Map.containsKey(md5)) {
                    md5Map.put(md5, new ArrayList<String>());
                    if (entrySizeMap.containsKey(filename)) {
                        fileSizeList.add(Pair.of(md5, entrySizeMap.get(filename).getFirst()));
                    } else {
                        fileSizeList.add(Pair.of(md5, totalRead));
                    }
                }
                // 存入md5集合
                md5Map.get(md5).add(filename);
            }
        }
    }
}
```



### UnusedResourcesTask

可以检测出apk中未使用的资源，对于getIdentifier获取的资源可以加入白名单

> 实现方法： 
>
> （1）读取R.txt获取apk中声明的所有资源得到declareResourceSet； 
>
> （2）通过读取smali文件中引用资源的指令（包括通过reference和直接通过资源id引用资源）得出class中引用的资源classRefResourceSet； 
>
> （3）通过ApkTool解析res目录下的xml文件、*AndroidManifest.xml* 以及 *resource.arsc* 得出资源之间的引用关系； 
>
> （4）根据上述几步得到的中间数据即可确定出apk中未使用到的资源。



```
R文件 (所有)
    id -> 名称
    0x7f0908e5 -> R.id.switch_btn

smali文件
	名称
	R.id.top_overlay

xml文件
	
AndroidManifest.xml
	获取到引用 @ref/0x7f1203f5

resource.arsc
	id 名称 默认值



解析R文件  resourceDefMap(14075)
将R文件解析的资源 添加进未使用的资源 unusedResSet(14075)
解析smali文件 resourceRefSet(9947)

解析资源文件(res,arsc,AndroidManifest) 
	解析资源      添加进 resourceRefSet (9947 -> 11619)
	将R文件，添加忽略限制后 添加进 resourceRefSet (11619 -> 12689) ???
	解析子引用     移除unusedResSet中的自引用

// R文件
unusedResSet.removeAll(resourceRefSet);
```





```java
private final List<String> dexFileNameList;
private final Map<String, String> rclassProguardMap;

//0x7f0908e5 -> R.id.switch_btn
// 所有资源
private final Map<String, String> resourceDefMap;
/*
key = "R.styleable.AlertDialog"
value = {HashSet@3921}  size = 9
 0 = "0x7f040754"
 1 = "0x010100f2"
*/
//名称 -> [id]
private final Map<String, Set<String>> styleableMap;
//有效的资源 R.attr.yearStyle
private final Set<String> resourceRefSet;
//未使用资源
private final Set<String> unusedResSet;
//忽略资源
private final Set<String> ignoreSet;

/*
key = "R.layout.plv_image_container_layout"
value = {HashSet@3243}  size = 2
0 = "R.id.iv_chat_img"
1 = "R.id.cpv_img_loading"
*/
// 反混淆后的value里资源
private final Map<String, Set<String>> nonValueReferences;

private Stack<String> visitPath;
```



```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(type, TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        long startTime = System.currentTimeMillis();
        // 解析mapping文件，存入rclassProguardMap集合中
        readMappingTxtFile();
        // 解析R.txt文件，获取到所有资源，分别存入resourceDefMap 和 styleableMap
        readResourceTxtFile();
        // 将所有的资源 添加进未使用资源
        unusedResSet.addAll(resourceDefMap.values());
        Log.i(TAG, "find resource declarations %d items.", unusedResSet.size());
        // 解析dex文件,读取smali文件中引用资源,添加进resourceRefSet 有效资源集合中
        decodeCode();
        Log.i(TAG, "find resource references in classes: %d items.", resourceRefSet.size());
        // 解析res目录下的xml文件、AndroidManifest.xml 以及 resource.arsc 
        decodeResources();
        Log.i(TAG, "find resource references %d items.", resourceRefSet.size());
        // 将所有资源中移除有效的资源 = 未使用的资源
        unusedResSet.removeAll(resourceRefSet);
        Log.i(TAG, "find unused references %d items", unusedResSet.size());
        Log.d(TAG, "find unused references %s", unusedResSet.toString());
        JsonArray jsonArray = new JsonArray();
        for (String name : unusedResSet) {
            jsonArray.add(name);
        }
        ((TaskJsonResult) taskResult).add("unused-resources", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```



```java
private void decodeCode() throws IOException {
    // 遍历dex
    for (String dexFileName : dexFileNameList) {
        // 加载dex容器
        MultiDexContainer<? extends DexBackedDexFile> dexFiles = DexFileFactory.loadDexContainer(new File(inputFile, dexFileName), Opcodes.forApi(15));

        for (String dexEntryName : dexFiles.getDexEntryNames()) {
            // 获取 dexEntry
            MultiDexContainer.DexEntry<? extends DexBackedDexFile> dexEntry = dexFiles.getEntry(dexEntryName);
            BaksmaliOptions options = new BaksmaliOptions();
            // 拷贝class
            List<? extends ClassDef> classDefs = Ordering.natural().sortedCopy(dexEntry.getDexFile().getClasses());
			// 遍历class
            for (ClassDef classDef : classDefs) {
                // 反汇编class
                String[] lines = ApkUtil.disassembleClass(classDef, options);
                if (lines != null) {
                    // 读取Smali
                    readSmaliLines(lines);
                }
            }
        }

    }
}
```



```java
private void readSmaliLines(String[] lines) {
    if (lines == null) {
        return;
    }
    boolean arrayData = false;
    // 解析smali,将资源引用存入resourceRefSet 有效资源集合中
    for (String line : lines) {
        line = line.trim();
        if (!Util.isNullOrNil(line)) {
            if (line.startsWith("const")) {
                String[] columns = line.split(" ");
                if (columns.length >= 3) {
                    final String resId = parseResourceId(columns[2].trim());
                    if (!Util.isNullOrNil(resId) && resourceDefMap.containsKey(resId)) {
                        // 存入
                        resourceRefSet.add(resourceDefMap.get(resId));
                    }
                }
            } else if (line.startsWith("sget")) {
                String[] columns = line.split(" ");
                if (columns.length >= 3) {
                    final String resourceRef = parseResourceNameFromProguard(columns[2].trim());
                    if (!Util.isNullOrNil(resourceRef)) {
                        Log.d(TAG, "find resource reference %s", resourceRef);
                        if (styleableMap.containsKey(resourceRef)) {
                            //reference of R.styleable.XXX
                            for (String attr : styleableMap.get(resourceRef)) {
                                // 存入
                                resourceRefSet.add(resourceDefMap.get(attr));
                            }
                        } else {
                            // 存入
                            resourceRefSet.add(resourceRef);
                        }
                    }
                }
            } else if (line.startsWith(".array-data 4")) {
                arrayData = true;
            } else if (line.startsWith(".end array-data")) {
                arrayData = false;
            } else  {
                if (arrayData) {
                    String[] columns = line.split(" ");
                    if (columns.length > 0) {
                        final String resId = parseResourceId(columns[0].trim());
                        if (!Util.isNullOrNil(resId) && resourceDefMap.containsKey(resId)) {
                            Log.d(TAG, "array field resource, %s", resId);
                            // 存入
                            resourceRefSet.add(resourceDefMap.get(resId));
                        }
                    }
                }
            }
        }
    }
}
```

```java
private void decodeResources() throws IOException, InterruptedException, AndrolibException, XmlPullParserException {
    // AndroidManifest.xml
    File manifestFile = new File(inputFile, ApkConstants.MANIFEST_FILE_NAME);
    // resources.arsc
    File arscFile = new File(inputFile, ApkConstants.ARSC_FILE_NAME);
    // res
    File resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_NAME);
    if (!resDir.exists()) {
        resDir = new File(inputFile, ApkConstants.RESOURCE_DIR_PROGUARD_NAME);
    }

    /*
         key = "R.layout.plv_image_container_layout"
         value = {HashSet@3243}  size = 2
          0 = "R.id.iv_chat_img"
          1 = "R.id.cpv_img_loading"
    */
    // xml中的资源
    Map<String, Set<String>> fileResMap = new HashMap<>();

    // values目录下 以及 AndroidManifest中的资源
    Set<String> valuesReferences = new HashSet<>();
    
	// 解析
    ApkResourceDecoder.decodeResourcesRef(manifestFile, arscFile, resDir, fileResMap, valuesReferences);

    Map<String, String> resguardMap = config.getResguardMap();
    
	// 混淆处理
    for (String resource : fileResMap.keySet()) {
        Set<String> result = new HashSet<>();
        
        for (String resName : fileResMap.get(resource)) {
            // value 混淆处理
           if (resguardMap.containsKey(resName)) {
               result.add(resguardMap.get(resName));
           } else {
               result.add(resName);
           }
        }
        // key 混淆处理
        if (resguardMap.containsKey(resource)) {
            nonValueReferences.put(resguardMap.get(resource), result);
        } else {
            nonValueReferences.put(resource, result);
        }
    }
    
	// 遍历 valuesReferences 存入 在使用资源中
    for (String resource : valuesReferences) {
        if (resguardMap.containsKey(resource)) {
            resourceRefSet.add(resguardMap.get(resource));
        } else {
            resourceRefSet.add(resource);
        }
    }
	// 将白名单集合从unusedResSet中进行移除
    for (String resource : unusedResSet) {
        if (ignoreResource(resource)) {
            resourceRefSet.add(resource);
        }
    }

    // 遍历resourceRefSet中的资源
    for (String resource : resourceRefSet) {
        readChildReference(resource);
    }
}
```



```java
private void readChildReference(String resource) throws IllegalStateException {
    // 如果nonValueReferences中 包含 resourceRefSet里的资源
    if (nonValueReferences.containsKey(resource)) {
        visitPath.push(resource);
        // 获取引用资源 (例:layout下引用的资源)
        Set<String> childReference = nonValueReferences.get(resource);
        //  从unusedResSet中删除此资源
        unusedResSet.removeAll(childReference);
        for (String reference : childReference) {
            if (!visitPath.contains(reference)) {
                // 递归资源(例：A资源有效，那个A所引用的资源集合B也是有效的)
                readChildReference(reference);
            } else {
                visitPath.push(reference);
                throw new IllegalStateException("Found resource cycle! " + visitPath.toString());
            }
        }
        visitPath.pop();
    }
}
```





### UnusedAssetsTask

检测出apk中未使用的assets文件

> 实现方法：搜索smali文件中引用字符串常量的指令，判断引用的字符串常量是否某个assets文件的名称

```kotlin
private final Set<String> ignoreSet;
private final Set<String> assetsPathSet;
private final Set<String> assetRefSet;
```



```java
public TaskResult call() throws TaskExecuteException {
    try {
        TaskResult taskResult = TaskResultFactory.factory(type, TaskResultFactory.TASK_RESULT_TYPE_JSON, config);
        long startTime = System.currentTimeMillis();
        File assetDir = new File(inputFile, ApkConstants.ASSETS_DIR_NAME);
        // 查找所有asset资源,添加到assetsPathSet
        findAssetsFile(assetDir);
        // assetsPathSet 内容改为相对路径，忽略资源添加到assetRefSet
        generateAssetsSet(assetDir.getAbsolutePath());
        Log.i(TAG, "find all assets count: %d", assetsPathSet.size());
        // 解析smali文件,有效资源添加到assetRefSet
        decodeCode();
        Log.i(TAG, "find reference assets count: %d", assetRefSet.size());
        // 所有资源中移除有效资源 = 无效资源
        assetsPathSet.removeAll(assetRefSet);
        JsonArray jsonArray = new JsonArray();
        for (String name : assetsPathSet) {
            jsonArray.add(name);
        }
        ((TaskJsonResult) taskResult).add("unused-assets", jsonArray);
        taskResult.setStartTime(startTime);
        taskResult.setEndTime(System.currentTimeMillis());
        return taskResult;
    } catch (Exception e) {
        throw new TaskExecuteException(e.getMessage(), e);
    }
}
```











### MultiSTLCheckTask

检测apk中的so是否静态链接STL

> 实现方法：通过nm工具来读取so的符号表，如果出现 *std::* 即表示so静态链接了STL。

 

```java
private boolean isStlLinked(File libFile) throws IOException, InterruptedException {
    ProcessBuilder processBuilder = new ProcessBuilder(toolnmPath, "-D", "-C", libFile.getAbsolutePath());
    Process process = processBuilder.start();
    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
    String line = reader.readLine();
    while (line != null) {
        String[] columns = line.split(" ");
        Log.d(TAG, "%s", line);
        // 判断是否是静态链接STL
        if (columns.length >= 3 && columns[1].equals("T") && columns[2].startsWith("std::")) {
            return true;
        }
        line = reader.readLine();
    }
    reader.close();
    process.waitFor();
    return false;
}
```





### UnStrippedSoCheckTask

检测出apk中未经裁剪的动态库文件

> 实现方法：使用nm工具读取动态库文件的符号表，若输出结果中包含*no symbols*字样则表示该动态库已经过裁剪



```java
private boolean isSoStripped(File libFile) throws IOException, InterruptedException {
    ProcessBuilder processBuilder = new ProcessBuilder(toolnmPath, libFile.getAbsolutePath());
    Process process = processBuilder.start();
    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
    String line = reader.readLine();
    boolean result = false;
    if (!Util.isNullOrNil(line)) {
        Log.d(TAG, "%s", line);
        String[] columns = line.split(":");
        // 判断是否裁剪
        if (columns.length == 3 && columns[2].trim().equalsIgnoreCase("no symbols")) {
            result = true;
        }
    }
    reader.close();
    process.waitFor();
    return result;
}
```

