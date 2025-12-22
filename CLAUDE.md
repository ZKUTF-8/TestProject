# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 提供在此代码库中工作时的指导说明。

## 项目概述

WPF (Windows Presentation Foundation) 桌面应用程序，目标框架为 .NET 10.0-windows。

## 常用命令

### 构建项目
```bash
dotnet build WpfWebApp/WpfWebApp.csproj
```

### 运行项目
```bash
dotnet run --project WpfWebApp/WpfWebApp.csproj
```

### 清理项目
```bash
dotnet clean WpfWebApp/WpfWebApp.csproj
```

### 还原包
```bash
dotnet restore WpfWebApp/WpfWebApp.csproj
```

## 架构说明

这是一个标准的 WPF MVVM 应用程序，结构如下：

- **App.xaml/App.xaml.cs**: 应用程序入口点和全局资源
- **MainWindow.xaml/MainWindow.xaml.cs**: 主应用程序窗口
- **XAML 文件**: 使用 WPF 标记语言定义 UI 布局
- **代码隐藏文件 (.cs)**: 处理 UI 逻辑和事件处理程序

### 关键设置

- **目标框架**: net10.0-windows
- **可空引用类型**: 已启用
- **隐式 Using**: 已启用
- **使用 WPF**: true

### WPF 特定说明

- UI 在 XAML 文件中声明式定义
- 代码隐藏文件是与 XAML 定义互补的分部类
- 应用程序启动通过 `App.xaml` 配置（StartupUri 指向 MainWindow）
- 添加新的 Window/UserControl 时，需要同时创建 .xaml 和 .xaml.cs 文件
