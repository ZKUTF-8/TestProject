# LC-8172 数据采集上位机

数据采集卡上位机软件，采用 **WPF + WebView2 + Vue3** 混合架构。

## 快速启动

```bash
# 1. 安装前端依赖
cd src/LC8172.WebUI
npm install

# 2. 启动前端开发服务器
npm run dev
# → http://localhost:5173

# 3. Visual Studio 打开 src/LC8172.slnx，按 F5 运行
```

启动后右上角显示 **✅ SignalR 已连接** 即正常。

## 技术栈

- **后端**: WPF + .NET 10.0 + SignalR + WebView2
- **前端**: Vue3 + TypeScript + Ant Design Vue + ECharts
- **通信**: SignalR (MessagePack 协议)

## 项目结构

```
src/
├── LC8172.slnx              # Visual Studio 解决方案
│
├── LC8172.WpfHost/          # C# 后端
│   ├── Program.cs           # 程序入口（SignalR + WPF）
│   ├── MainWindow.xaml      # WebView2 容器
│   ├── Models/              # 数据模型（GateConfig、WaveformData、GateResult）
│   ├── Hubs/                # SignalR Hub
│   ├── Services/            # 业务服务（待开发）
│   └── wwwroot/             # 前端构建输出
│
└── LC8172.WebUI/            # Vue3 前端
    ├── src/
    │   ├── types/models.ts          # TypeScript 类型定义
    │   ├── composables/             # SignalR 通信封装
    │   └── components/              # ECharts 波形图、闸门编辑器
    └── vite.config.ts
```

## 核心功能

- **多通道波形显示**: ECharts Canvas 渲染，支持 30000+ 点流畅显示
- **可交互式闸门配置**: 树状结构，支持 Root/Rising/Falling/Data 四种类型
- **实时数据通信**: SignalR 双向推送，自动重连

## 生产构建

```bash
# 1. 构建前端（输出到 LC8172.WpfHost/wwwroot/）
cd src/LC8172.WebUI
npm run build

# 2. 发布 WPF 应用（保持 sdk\libx64 目录结构）
cd ../LC8172.WpfHost
dotnet publish -c Release -p:PublishProfile=Release
# → 输出到 bin/publish/
```

详细发布说明：[docs/PUBLISH.md](docs/PUBLISH.md)

## 文档

- **详细需求**: [docs/requirements.md](docs/requirements.md)
- **编码规范**: `.cursor/rules/` (自动应用于 AI 对话)

## 待开发

- 闸门拖拽交互逻辑
- 采集卡 DLL 接口封装
- 实时数据流推送优化
