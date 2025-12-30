---
description: "LC-8172 数据采集上位机项目规则 - WPF + WebView2 + Vue3 混合架构"
alwaysApply: true
---

# LC-8172 数据采集上位机项目规则

## 项目概述

LC-8172 是数据采集卡上位机软件，采用 **WPF + WebView2 混合架构**。

**核心功能**：
- 多通道波形显示（ECharts Canvas 渲染）
- 可交互式闸门配置（树状结构，支持拖拽）
- 实时数据通信（SignalR 双向推送）

**技术栈**：
- 后端：WPF + .NET 10.0 + SignalR Server
- 前端：Vue3 + TypeScript + Ant Design Vue + ECharts
- 通信：SignalR (MessagePack 协议)

## 项目结构

```
src/
├── LC8172.slnx                      # Visual Studio 解决方案
├── LC8172.WpfHost/                  # C# 后端
│   ├── Program.cs                   # 程序入口（启动 SignalR + WPF）
│   ├── MainWindow.xaml              # WebView2 容器
│   ├── Models/                      # 数据模型
│   │   ├── GateConfig.cs           # 闸门配置
│   │   ├── WaveformData.cs         # 波形数据
│   │   └── GateResult.cs           # 闸门计算结果
│   ├── Services/                    # 业务服务（待开发）
│   └── Hubs/
│       └── AcquisitionHub.cs        # SignalR Hub
└── LC8172.WebUI/                    # Vue3 前端
    ├── src/
    │   ├── types/models.ts          # TypeScript 类型定义
    │   ├── stores/
    │   │   └── acquisitionStore.ts  # Pinia 状态管理（SignalR 通信）
    │   └── components/
    │       ├── WaveformChart.vue    # ECharts 波形图
    │       ├── GateTreeEditor.vue   # 闸门树编辑器
    │       └── GateResults.vue      # 结果显示
    └── vite.config.ts
```

---

## C# 编码规范

### 使用 C# 10+ 现代语法（必须）

**1. 文件范围命名空间**
```csharp
// ✅ 正确
namespace LC8172.WpfHost.Models;

public class GateConfig { }

// ❌ 错误
namespace LC8172.WpfHost.Models
{
    public class GateConfig { }
}
```

**2. 集合表达式**
```csharp
// ✅ 正确
public List<GateConfig> Children { get; set; } = [];
private static readonly Dictionary<string, int> _cache = [];

// ❌ 错误
public List<GateConfig> Children { get; set; } = new();
public List<GateConfig> Children { get; set; } = new List<GateConfig>();
```

**3. Target-typed new 表达式**
```csharp
// ✅ 正确
List<double[]> points = [];
return [ new() { Id = "1" }, new() { Id = "2" } ];

// ❌ 错误
List<double[]> points = new List<double[]>();
return new List<GateResult> { new GateResult { ... } };
```

**4. 可空引用类型（已启用）**
```csharp
public string Name { get; set; } = string.Empty;  // 非空
public string? ParentId { get; set; }             // 可空
```

### 命名规范

- 类名：`PascalCase`（如 `GateConfig`）
- 方法名：`PascalCase`（如 `StartAcquisition`）
- 私有字段：`_camelCase`（如 `_random`）
- 属性：`PascalCase`（如 `GateId`）
- 参数/局部变量：`camelCase`（如 `gateId`）

### XML 文档注释（必须）

所有公共类和方法必须有 XML 文档注释：
```csharp
/// <summary>
/// 闸门配置
/// ⚠️ 修改此类时，同步更新前端：src/LC8172.WebUI/src/types/models.ts
/// </summary>
public class GateConfig { }
```

---

## 前端编码规范

### TypeScript 严格模式

- 启用严格类型检查
- 避免使用 `any`，使用具体类型或 `unknown`
- Props 和 Emits 必须显式定义类型

### Vue3 组合式 API

```typescript
// ✅ 正确：使用 setup script
<script setup lang="ts">
import { ref } from 'vue';

interface Props {
  data: WaveformData;
}

const props = defineProps<Props>();
const emit = defineEmits<{
  update: [value: string];
}>();
</script>

// ❌ 错误：使用 Options API
export default {
  data() { return {}; }
}
```

### 组件命名

- 文件名：`PascalCase.vue`（如 `WaveformChart.vue`）
- 组件名：与文件名一致
- Props：`camelCase`
- Events：`kebab-case`

---

## 前后端数据同步规则（重要）

### 手动同步数据模型

前后端数据模型需要手动保持一致：

**后端 C# 模型**：`src/LC8172.WpfHost/Models/*.cs`
**前端 TypeScript 类型**：`src/LC8172.WebUI/src/types/models.ts`

**修改流程**：
1. 修改后端 C# 类时，必须同步更新前端 TypeScript 类型
2. 在 C# 类的 XML 注释中添加提醒：
   ```csharp
   /// <summary>
   /// ⚠️ 修改此类时，同步更新前端：src/LC8172.WebUI/src/types/models.ts
   /// </summary>
   ```

### 字段映射规则

| C# 命名 | TypeScript 命名 | 示例 |
|---------|----------------|------|
| `PascalCase` | `camelCase` | `GateId` → `gateId` |
| `List<T>` | `T[]` | `List<GateConfig>` → `GateConfig[]` |
| `string?` | `string \| undefined` | `ParentId?` → `parentId?: string` |
| `DateTime` | `string` (ISO 8601) | `Timestamp` → `timestamp: string` |

### 核心数据模型

**GateConfig（闸门配置）**：
- 树状结构，支持无限层级嵌套
- `Start` 是相对父闸门的偏移量
- 绝对位置 = 所有祖先的 Start 之和

**WaveformData（波形数据）**：
- `Points` 格式：`[[x1, y1], [x2, y2], ...]`
- `SampleRate` 单位：MHz
- 支持 30000+ 数据点

**GateResult（闸门计算结果）**：
- 由采集卡计算返回
- 包含上升沿/下降沿/峰值检测结果

---

## SignalR 通信规范

### Hub 方法命名

- 使用 `PascalCase`
- 动词开头（如 `StartAcquisition`、`SetGates`）

### 核心方法

```csharp
// 后端 Hub
public async Task SetGates(List<GateConfig> gates)
public async Task<WaveformData> StartAcquisition(int channel, double sampleRate)
public async Task StopAcquisition()
public async Task<List<GateResult>> GetGateResults()
```

```typescript
// 前端调用
await connection.invoke('SetGates', gates);
await connection.invoke<WaveformData>('StartAcquisition', channel, sampleRate);
await connection.invoke('StopAcquisition');
```

### 服务器推送事件

```typescript
connection.on('WaveformData', (data: WaveformData) => { });
connection.on('WaveformChunk', (chunk: number[][]) => { });
connection.on('AcquisitionStopped', () => { });
connection.on('GatesConfigured', (gates: GateConfig[]) => { });
```

---

## 环境配置

### DEBUG vs RELEASE

- **DEBUG**：WebView2 加载 `http://localhost:5173`（Vite dev server）
- **RELEASE**：WebView2 加载 `http://localhost:5000`（本地静态文件）

### 端口分配

- Vite dev server：`5173`
- SignalR server：`5000`

---

## 性能优化

### ECharts 配置

```typescript
{
  animation: false,              // 关闭动画
  sampling: 'lttb',             // LTTB 降采样
  large: true,                  // 大数据优化
  largeThreshold: 1000,         // 1000点阈值
  progressive: 5000,            // 渐进式渲染
  progressiveThreshold: 10000   // 10000点启用
}
```

### 数据传输优化

- 使用 LTTB 算法在前端降采样
- 大数据流使用增量推送（WaveformChunk）
- 限制保留的历史数据点数

---

## 常见任务

### 添加新的数据模型

1. 在 `src/LC8172.WpfHost/Models/` 创建 C# 类
2. 使用文件范围命名空间和集合表达式
3. 添加 XML 文档注释和同步提醒
4. 在 `src/LC8172.WebUI/src/types/models.ts` 添加对应类型

### 添加新的 SignalR 方法

1. 在 `AcquisitionHub.cs` 添加方法
2. 在 `stores/acquisitionStore.ts` 添加调用方法
3. 更新 README.md 的方法列表

### 修改前端组件

1. 使用 `<script setup lang="ts">`
2. 显式定义 Props 和 Emits 类型
3. 使用 Composition API
4. 使用 Ant Design Vue 组件库

---

## 调试技巧

### 查看 SignalR 日志

```typescript
// 前端
const connection = new HubConnectionBuilder()
  .withUrl('http://localhost:5000/acquisitionHub')
  .configureLogging(LogLevel.Debug)  // 启用调试日志
  .build();
```

### 后端控制台输出

```csharp
Console.WriteLine($"收到闸门配置，共 {gates.Count} 个闸门");
```

### WebView2 DevTools

在 DEBUG 模式下自动打开，或按 F12 打开。

** 注意事项
- 在没有特别说明的情况下不要输出总结说明文档

