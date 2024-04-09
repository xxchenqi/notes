# ASM字节码插装

创建buildSrc目录

...



### 插件

```groovy
package com.enjoy.plugin

import com.android.build.gradle.AppExtension
import org.gradle.api.Plugin
import org.gradle.api.Project

class ASMPlugin implements Plugin<Project> {

    @Override
    void apply(Project project) {

        project.extensions.getByType(AppExtension.class).registerTransform(new ASMTransform())
    }
}
```



### Transform

```groovy
package com.cq.plugin

import com.android.build.api.transform.Format
import com.android.build.api.transform.QualifiedContent
import com.android.build.api.transform.Transform
import com.android.build.api.transform.TransformException
import com.android.build.api.transform.TransformInvocation
import com.android.build.api.transform.TransformOutputProvider
import com.android.build.gradle.internal.pipeline.TransformManager
import com.android.utils.FileUtils
import com.google.common.collect.FluentIterable
import jdk.internal.org.objectweb.asm.ClassReader
import jdk.internal.org.objectweb.asm.ClassWriter
import org.gradle.internal.impldep.org.apache.commons.codec.digest.DigestUtils

/**
 * 自定义的Transform的执行时间在所有Transform执行之前 执行
 */
class ASMTransform extends Transform {

    /**
     * Transform名称(例 :app:transformClassesWithAsmForDebug)
     * @return
     */
    @Override
    String getName() {
        return "asm"
    }

    /**
     * Transform处理内容(处理标准资源:Class,Jar,Resource(java的资源))
     * @return
     */
    @Override
    Set<QualifiedContent.ContentType> getInputTypes() {
        //只处理Class
        return TransformManager.CONTENT_CLASS
    }

    /**
     * Transform作用域(接收哪些范围:所有的,第三方模块,app自己)
     * @return
     */
    @Override
    Set<? super QualifiedContent.Scope> getScopes() {
        //只对引入插件的模块src起作用
        return TransformManager.PROJECT_ONLY
    }

    /**
     * 是否增量
     * @return
     */
    @Override
    boolean isIncremental() {
        return false
    }

    @Override
    void transform(TransformInvocation transformInvocation)
            throws TransformException, InterruptedException, IOException {
        //如何获取每一个要处理的class文件?

        //transform的输出
        TransformOutputProvider outputProvider = transformInvocation.outputProvider
        //将上次构建内容进行清理
        outputProvider.deleteAll()

        def inputs = transformInvocation.inputs
        inputs.each {
            //jar包
//            def jarInputs = it.jarInputs
            def directoryInputs = it.directoryInputs
//            directoryInputs.each {
//                /**
//                 * 获取文件状态
//                 * NOTCHANGED 未改变
//                 * ADDED 新增
//                 * CHANGED 改变
//                 * REMOVED 移除
//                 */
//                def changedFiles = it.changedFiles
//                changedFiles.entrySet().each {
//                    println("${it.key.name} ===>${it.value.name()}")
//                }
//            }

            directoryInputs.each {
                //获取目录名
                String dirName = it.name
                //获取目录的文件
                File src = it.getFile()
                println("目录" + src)

                String md5Name = DigestUtils.md5Hex(src.absolutePath)

                //上一个transform的输出会作为下一个transform的输入,
                //处理之后的内容用transform获得的file写到outputProvider
                File dest = outputProvider.getContentLocation(dirName + md5Name,
                        it.contentTypes, it.scopes,
                        Format.DIRECTORY)

                //执行插桩
                processInject(src, dest)
            }
        }
    }

    void processInject(File src, File dest) {
        String dir = src.absolutePath
        FluentIterable<File> allFiles = FileUtils.getAllFiles(dir)
        for (File file : allFiles) {
            FileInputStream fis = new FileInputStream()
            ClassReader cr = new ClassReader(fis)
            ClassWriter cw = new ClassWriter(ClassWriter.COMPUTE_FRAMES)
            cr.accept(new ClassInjectTimeVisitor(cw, file.name), ClassReader.EXPAND_FRAMES)

        }

    }

}
```



### 类访问器

```groovy
package com.cq.plugin

import jdk.internal.org.objectweb.asm.ClassVisitor
import jdk.internal.org.objectweb.asm.MethodVisitor
import org.objectweb.asm.Opcodes

class ClassInjectTimeVisitor extends ClassVisitor {
    String className

    ClassInjectTimeVisitor(ClassVisitor cv, String fileName) {
        super(Opcodes.ASM5, cv);
        className = fileName.substring(0, fileName.lastIndexOf("."))
    }

    /**
 	  * 读到的每一个方法都会调用此api
 	  *		
	  */
    @Override
    MethodVisitor visitMethod(int i, String s, String s1, String s2, String[] strings) {
        org.objectweb.asm.MethodVisitor mv = super.visitMethod(access, name, desc, signature,
                exceptions)
        return new MethodAdapterVisitor(mv, access, name, desc, className)
    }
}
```



### 访问方法类

```groovy
package com.cq.plugin

import org.objectweb.asm.AnnotationVisitor
import org.objectweb.asm.MethodVisitor
import org.objectweb.asm.Opcodes
import org.objectweb.asm.Type
import org.objectweb.asm.commons.AdviceAdapter

class MethodAdapterVisitor extends AdviceAdapter {

    private String className;
    private String methodName;
    private boolean inject;
    private int index;
    private int start, end;

    protected MethodAdapterVisitor(MethodVisitor mv, int access, String name, String desc,
                                   String className) {
        super(Opcodes.ASM5, mv, access, name, desc);
        methodName = name;
        this.className = className;
    }

    @Override
    AnnotationVisitor visitAnnotation(String desc, boolean visible) {
        //记录方法是不是被注解
        if ("Lcom/enjoy/asminject/InjectTime;".equals(desc)) {
            inject = true;
        }
        return super.visitAnnotation(desc, visible);
    }


    @Override
    protected void onMethodEnter() {
        if (inject) {
//                0: invokestatic #2 // Method java/lang/System.currentTimeMillis:()J
//                3: lstore_1
            //储备本地变量备用
            mv.visitMethodInsn(INVOKESTATIC, "java/lang/System", "currentTimeMillis", "()J", false);
            index = newLocal(Type.LONG_TYPE);
            start = index;
            mv.visitVarInsn(LSTORE, start);
        }

    }


    @Override
    protected void onMethodExit(int opcode) {
        if (inject) {
            mv.visitMethodInsn(INVOKESTATIC, "java/lang/System", "currentTimeMillis", "()J", false);
            index = newLocal(Type.LONG_TYPE);
            end = index;
            mv.visitVarInsn(LSTORE, end);

            // getstatic #3 // Field java/lang/System.out:Ljava/io/PrintStream;
            //获得静态成员 out
            mv.visitFieldInsn(GETSTATIC, "java/lang/System", "out", "Ljava/io/PrintStream;");

            // new #4 // class java/lang/StringBuilder
            // 引入类型 分配内存 并dup压入栈顶让下面的INVOKESPECIAL 知道执行谁的构造方法
            mv.visitTypeInsn(NEW, "java/lang/StringBuilder");
            mv.visitInsn(DUP);

            //invokevirtual #7   // Method java/lang/StringBuilder.append:
            // (Ljava/lang/String;)Ljava/lang/StringBuilder;
            // 执行构造方法
            mv.visitMethodInsn(INVOKESPECIAL, "java/lang/StringBuilder", "<init>",
                    "()V", false);

            // ldc #6 // String execute:
            // 把常量压入栈顶 后面使用
            mv.visitLdcInsn("==========>" + className + " execute " + methodName + ": ");

            //invokevirtual #7 // Method java/lang/StringBuilder.append: (Ljava/lang/String;)
            // Ljava/lang/StringBuilder;
            // 执行append方法，使用栈顶的值作为参数
            mv.visitMethodInsn(INVOKEVIRTUAL, "java/lang/StringBuilder", "append",
                    "(Ljava/lang/String;)Ljava/lang/StringBuilder;", false);

            // lload_3 获得存储的本地变量
            // lload_1
            // lsub   减法指令
            mv.visitVarInsn(LLOAD, end);
            mv.visitVarInsn(LLOAD, start);
            mv.visitInsn(LSUB);

            // invokevirtual #8 // Method java/lang/StringBuilder.append:(J)
            // Ljava/lang/StringBuilder;
            // 把减法结果append
            mv.visitMethodInsn(INVOKEVIRTUAL, "java/lang/StringBuilder", "append",
                    "(J)Ljava/lang/StringBuilder;", false);

            //append "ms."
            mv.visitLdcInsn("ms.");
            mv.visitMethodInsn(INVOKEVIRTUAL, "java/lang/StringBuilder", "append",
                    "(Ljava/lang/String;)Ljava/lang/StringBuilder;", false);

            //tostring
            mv.visitMethodInsn(INVOKEVIRTUAL, "java/lang/StringBuilder", "toString",
                    "()Ljava/lang/String;", false);

            mv.visitMethodInsn(INVOKEVIRTUAL, "java/io/PrintStream", "println",
                    "(Ljava/lang/String;)V", false);
        }
    }
}
```





### 临时

```java
package com.cq.asmapp;

import org.junit.Test;
import org.objectweb.asm.AnnotationVisitor;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassVisitor;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.MethodVisitor;
import org.objectweb.asm.Opcodes;
import org.objectweb.asm.Type;
import org.objectweb.asm.commons.AdviceAdapter;
import org.objectweb.asm.commons.Method;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class ASMUnitTest {
    @Test
    public void test() throws IOException {
        /**
         * 1、获得待插桩的字节码数据
         */
        FileInputStream fis = new FileInputStream("D:\\AndroidStudioProjects\\ASMApp\\app\\src\\test\\java\\com\\cq\\asmapp\\InjectTest.class");

        /**
         * 2、执行插桩
         */
        ClassReader cr = new ClassReader(fis);  // class解析器
        //执行解析
        ClassWriter classWriter = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
        //ASM7 扩展之前的版本,EXPAND_FRAMES 扩展帧
        cr.accept(new MyClassVisitor(Opcodes.ASM7, classWriter), ClassReader.EXPAND_FRAMES);

        /**
         * 3、输出结果
         */
        byte[] bytes = classWriter.toByteArray();
        FileOutputStream fos = new FileOutputStream("D:\\AndroidStudioProjects\\ASMApp\\app\\src\\test\\java\\com\\cq\\asmapp\\InjectTest2.class");
        fos.write(bytes);
        fos.close();
    }


    //类访问器
    static class MyClassVisitor extends ClassVisitor {

        public MyClassVisitor(int api, ClassVisitor classVisitor) {
            super(api, classVisitor);
        }

        /**
         * 读到的每一个方法都会调用此api
         *
         * @param access
         * @param name
         * @param descriptor
         * @param signature
         * @param exceptions
         * @return
         */
        @Override
        public MethodVisitor visitMethod(int access, String name, String descriptor, String signature, String[] exceptions) {
            MethodVisitor methodVisitor = super.visitMethod(access, name, descriptor, signature, exceptions);
            System.out.println(name);
            return new MyMethodVisitor(api, methodVisitor, access, name, descriptor);
        }
    }


    /**
     * 这个类用来访问方法
     */
    static class MyMethodVisitor extends AdviceAdapter {
        //局部变量表
        int s;
        int e;

        protected MyMethodVisitor(int api, MethodVisitor methodVisitor, int access, String name, String descriptor) {
            super(api, methodVisitor, access, name, descriptor);
        }

        boolean inject;

        @Override
        public AnnotationVisitor visitAnnotation(String descriptor, boolean visible) {
            System.out.println(getName() + "--->" + descriptor);
//            Lcom/cq/asmapp/ASMTest;
            if ("Lcom/cq/asmapp/ASMTest;".equals(descriptor)) {
                inject = true;
            }
            return super.visitAnnotation(descriptor, visible);
        }


        /**
         * 方法进入时执行
         */
        @Override
        protected void onMethodEnter() {
            super.onMethodEnter();
            System.out.println("onMethodEnter");
            if (!inject) {
                return;
            }


            //       long var1 = System.currentTimeMillis();
//            0: invokestatic  #2                  // Method java/lang/System.currentTimeMillis:()J
//            3: lstore_1

            invokeStatic(Type.getType("Ljava/lang/System;"), new Method("currentTimeMillis", "()J"));
            s = newLocal(Type.LONG_TYPE);
            storeLocal(s);
        }

        /**
         * 方法退出时执行
         *
         * @param opcode
         */
        @Override
        protected void onMethodExit(int opcode) {
            super.onMethodExit(opcode);
            System.out.println("onMethodExit");
            if (!inject) {
                return;
            }

            invokeStatic(Type.getType("Ljava/lang/System;"), new Method("currentTimeMillis", "()J"));
            e = newLocal(Type.LONG_TYPE);
            storeLocal(e);
//            14: getstatic     #6                  // Field java/lang/System.out:Ljava/io/PrintStream;
            getStatic(Type.getType("Ljava/lang/System;"), "out", Type.getType("Ljava/io/PrintStream;"));
//            17: new           #7                  // class java/lang/StringBuilder
            newInstance(Type.getType("Ljava/lang/StringBuilder;"));
//            20: dup
            dup();
//            21: invokespecial #8                  // Method java/lang/StringBuilder."<init>":()V
            invokeConstructor(Type.getType("Ljava/lang/StringBuilder;"), new Method("<init>", "()V"));
//            24: ldc           #9                  // String execute:
            visitLdcInsn("execute");
//            26: invokevirtual #10                 // Method java/lang/StringBuilder.append:(Ljava/lang/String;)Ljava/lang/StringBuilder;
            invokeVirtual(Type.getType("Ljava/lang/StringBuilder;"), new Method("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;"));
//            29: lload_3
            loadLocal(e);
//            30: lload_1
            loadLocal(s);
//            31: lsub
            math(SUB, Type.LONG_TYPE);
//            32: invokevirtual #11                 // Method java/lang/StringBuilder.append:(J)Ljava/lang/StringBuilder;
            invokeVirtual(Type.getType("Ljava/lang/StringBuilder;"), new Method("append", "(J)Ljava/lang/StringBuilder;"));
//            35: invokevirtual #12                 // Method java/lang/StringBuilder.toString:()Ljava/lang/String;
            invokeVirtual(Type.getType("Ljava/lang/StringBuilder;"), new Method("toString", "()Ljava/lang/String;"));
//            38: invokevirtual #13                 // Method java/io/PrintStream.println:(Ljava/lang/String;)V
            invokeVirtual(Type.getType("Ljava/io/PrintStream;"), new Method("println", "(Ljava/lang/String;)V"));
        }
    }

}

```

