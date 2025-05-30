# Vue3

## 基础说明

目录说明

```
your-project/
├── public/
│   └── favicon.ico         # 公共资源，构建时原样拷贝到 dist
├── src/                    # 项目源代码（重点开发目录）
│   ├── assets/             # 图片、字体等静态资源
│   ├── components/         # Vue 组件
│   ├── App.vue             # 应用根组件
├── index.html              # 项目的入口文件 HTML 模板（Vite 会注入 scripts）
├── package.json            # 项目信息及依赖配置
├── tsconfig.json           # TypeScript 配置（如果使用 TS）
├── vite.config.ts          # Vite 配置文件
├── pnpm-lock.yaml          # pnpm 锁定依赖版本
├── env.d.ts                # 作用是让ts认识 .txt .jpg 等这些文件（如果报错，执行npm i）
└── README.md               # 项目说明文档
```

启动

```
npm run dev
此命令在package.json中有配置
```

创建引用

```ts
// 引入createApp用于创建应用
import { createApp } from 'vue'
// 引入App根组件
import App from './App.vue'


// createApp(App) 创建引用，根组件是APP
// mount('#app') 挂载应用
// #app 指的是index.html 中的 <div id="app"></div>
createApp(App).mount('#app')
```

vue 基础结构

```vue
<template>
	写HTML标签
</template>

<script lang="ts">
	写JS或TS
    
    export default {
        name:'App' // 组件名
    }
</script>

<style >
	写CSS样式
</style>
```



