# LC-8172 数据采集上位机软件需求文档

## 项目概述

**项目代号**: LC-8172  
**项目类型**: 数据采集卡上位机软件  
**技术架构**: WPF (WebView2) + Vue3 + SignalR + ECharts

---

## 一、核心功能

### 1.1 基础功能
- 采集卡配置管理
- 启动/停止采集
- 多通道数据采集支持
- 实时波形显示（XY轴曲线图）

### 1.2 波形显示
- 显示多通道波形数据
- 支持缩放、平移等基本交互
- 时间轴单位：微秒 (μs)
- 电压轴单位：伏特 (V)

---

## 二、闸门系统（核心功能）

### 2.1 闸门概念
闸门是在波形图上标记的时间区域，用于指定采集卡进行特定计算的范围。

### 2.2 闸门数据结构

#### 闸门属性
| 属性 | 类型 | 说明 |
|------|------|------|
| id | string | 唯一标识 |
| name | string | 闸门名称（如 Absolute, FSF, Data1） |
| start | number | 起始时间（相对父闸门，单位：μs） |
| length | number | 闸门长度（单位：μs） |
| threshold | number | 阈值电压（单位：V，用于边沿检测） |
| type | GateType | 闸门类型 |
| color | string | 显示颜色（HEX格式） |
| channel | number | 应用的采集通道号 |
| parentId | string? | 父闸门ID |
| children | GateConfig[] | 子闸门列表 |

#### 闸门类型 (GateType)
- **Root**: 根闸门（整个采集窗口）
- **Rising**: 上升沿检测
- **Falling**: 下降沿检测
- **Data**: 数据门（寻峰）

### 2.3 闸门树状结构
闸门以树状层级组织：
```
Absolute (根闸门)
├── FSF (子闸门 - Rising上升沿)
│   ├── Data1 (孙闸门 - Data数据门)
│   ├── Data2 (孙闸门 - Data数据门)
│   └── 信息 (孙闸门 - Data数据门)
└── 其他子闸门...
```

### 2.4 位置计算规则（关键）
- **相对位置**: 每个闸门的 `start` 是相对于父闸门的偏移量
- **绝对位置**: 闸门的实际位置 = 所有祖先闸门的 `start` 之和
- **示例**:
  ```
  Absolute.start = 0
  FSF.start = 2.3 (相对Absolute)
  Data1.start = 1.5 (相对FSF)
  
  → Data1的绝对位置 = 0 + 2.3 + 1.5 = 3.8 μs
  ```

### 2.5 闸门可视化
- 在波形图上显示为**矩形区域**或**垂直标记线**
- 显示闸门名称标签
- 使用指定颜色区分不同闸门
- 半透明背景以不遮挡波形

---

## 三、交互需求（核心难点）

### 3.1 闸门拖拽操作
1. **整体拖拽**
   - 鼠标拖动闸门矩形，改变闸门的起始位置
   - 拖拽时实时更新 `start` 值
   - 拖拽时显示当前时间值

2. **调整长度**
   - 闸门左右两侧有**拖拽手柄**（小圆点）
   - 拖动左侧手柄：改变 `start`，保持右边界不变
   - 拖动右侧手柄：改变 `length`，保持左边界不变

3. **父子联动**
   - 拖动父闸门时，所有子孙闸门**跟随移动**
   - 子闸门的相对位置保持不变
   - 拖动子闸门时，只改变它相对父闸门的偏移量

### 3.2 闸门与波形相交
- 当闸门起始线穿过波形时，计算交点坐标
- 用途：
  - 辅助用户对齐波形特征点
  - 可选：吸附功能（拖拽时自动对齐波形）

### 3.3 闸门管理界面
- 树状结构显示所有闸门
- 可选中、编辑闸门属性
- 支持添加、删除、重命名闸门
- 改变闸门父子关系（拖拽重组）

---

## 四、采集卡通信

### 4.1 通信协议
- 前端 ↔ 后端：SignalR（双向实时通信）
- 后端 ↔ 采集卡：PInvoke调用DLL

### 4.2 数据流向
```
┌─────────────┐  SignalR   ┌─────────────┐  DLL调用  ┌─────────────┐
│   Vue前端   │ ←────────→ │  WPF后端    │ ←──────→ │   采集卡    │
│  (图表交互) │            │ (硬件通信)  │           │  (数据计算) │
└─────────────┘            └─────────────┘           └─────────────┘
```

### 4.3 SignalR接口（后端实现）
#### 前端调用的方法
- `SetGates(gates: GateConfig[])` - 配置闸门参数
- `StartAcquisition(channel: int, sampleRate: double)` - 开始采集
- `StopAcquisition()` - 停止采集
- `GetGateResults()` - 获取闸门计算结果

#### 后端推送的事件
- `WaveformData` - 推送完整波形数据
- `WaveformChunk` - 推送实时数据流（增量）
- `AcquisitionStopped` - 采集已停止
- `GateResultsUpdated` - 闸门计算结果更新

### 4.4 闸门计算结果格式
采集卡根据闸门配置计算后，返回结果格式如下：

```
FSF
  Rising Edge Found = True, Val = 0.366, TOF = 2.500
  Falling Edge Found = False, Val = 0.000, TOF = 0.000

信息
  PPA Found = False
  NPA Found = True, Val = -0.488, TOF = 3.633

Data1
  PPA Found = True, Val = 0.496, TOF = 6.132
  NPA Found = True, Val = -0.496, TOF = 8.727

Data2
  PPA Found = True, Val = 0.496, TOF = 6.366
  NPA Found = True, Val = -0.504, TOF = 13.686
```

**字段说明**：
- **PPA**: Positive Peak Amplitude（正峰值）
- **NPA**: Negative Peak Amplitude（负峰值）
- **TOF**: Time of Flight（飞行时间，即峰值出现的时间）
- **Val**: 峰值电压
- **Found**: 是否找到

---

## 五、技术架构

### 5.1 技术栈
- **前端**: Vue3 + TypeScript + Ant Design Vue + ECharts + SignalR Client
- **后端**: WPF + WebView2 + SignalR Server + 采集卡DLL封装
- **构建工具**: Vite (前端) + MSBuild (后端)

### 5.2 目录结构
```
LC-8172/
├── docs/                           # 文档
│   └── requirements.md             # 本需求文档
│
├── src/
│   ├── LC8172.WpfHost/             # WPF宿主程序
│   │   ├── MainWindow.xaml         # 主窗口（WebView2）
│   │   ├── Models/                 # 数据模型
│   │   ├── Services/               # 业务服务
│   │   ├── Hubs/                   # SignalR Hub
│   │   └── wwwroot/                # 前端构建输出
│   │
│   └── LC8172.WebUI/               # Vue前端
│       ├── src/
│       │   ├── components/         # Vue组件
│       │   ├── composables/        # 组合式函数
│       │   └── types/              # TypeScript类型
│       └── vite.config.ts
│
├── .gitignore
└── README.md
```

### 5.3 开发环境配置
- **开发模式**: 
  - 前端: `npm run dev` (http://localhost:5173)
  - 后端: Visual Studio F5，WebView2加载 localhost:5173
- **生产模式**:
  - 前端: `npm run build` → dist/
  - 后端: 加载本地 wwwroot/ 静态文件

---

## 六、UI设计要求

### 6.1 整体布局
```
┌─────────────────────────────────────────────────┐
│  标题栏：LC-8172 数据采集上位机                   │
├──────────┬──────────────────────────────────────┤
│          │  控制栏：[开始采集] [停止] [配置]     │
│  闸门树  ├──────────────────────────────────────┤
│  编辑器  │                                        │
│          │         波形显示区域                   │
│  (300px) │         (ECharts 图表)                │
│          │                                        │
│          ├──────────────────────────────────────┤
│          │  闸门结果显示区域                      │
└──────────┴──────────────────────────────────────┘
```

### 6.2 UI美观性要求
- 现代化Material Design / Fluent Design风格
- 使用Ant Design Vue组件库
- 卡片式布局，适当阴影和圆角
- 配色：科技蓝 + 深色模式可选

### 6.3 图表交互体验
- 流畅的拖拽反馈（60fps）
- 拖拽时显示实时数值
- 悬停时高亮对应闸门
- 右键菜单：编辑、删除、复制闸门

---

## 七、非功能性需求

### 7.1 性能要求
- 波形渲染帧率：≥30fps（常规模式）/ ≥10fps（高采样率）
- 闸门拖拽延迟：<50ms
- SignalR消息往返延迟：<10ms（局域网）
- 支持最大数据点数：30000点/通道

### 7.2 兼容性
- 操作系统：Windows 10/11
- .NET版本：.NET 10.0
- WebView2 Runtime：自动安装（Win10/11通常已内置）

### 7.3 可维护性
- 前后端数据模型保持同步（手动维护TypeScript类型）
- 清晰的代码注释
- Git版本控制

---

## 八、已知技术难点

### 8.1 WebView2本地文件加载
- ❌ 直接使用 `file://` 协议会遇到CORS问题
- ✅ 开发环境：加载Vite dev server (http://localhost:5173)
- ✅ 生产环境：使用虚拟主机或本地HTTP服务器

### 8.2 ECharts拖拽实现
- 使用 `graphic.draggable` 实现闸门拖拽
- 坐标转换：像素坐标 ↔ 数据坐标
- 递归处理父子闸门联动

### 8.3 SignalR双向通信
- 自托管SignalR服务器在WPF中
- CORS配置允许WebView2访问
- 自动重连机制

---

## 九、待确认事项

- [ ] 采集卡具体型号和DLL接口文档
- [ ] 采集卡是否支持实时流式传输
- [ ] 闸门最大嵌套层级限制
- [ ] 是否需要暗色主题支持
- [ ] 是否需要配置文件导入/导出

---

**文档版本**: v1.0  
**最后更新**: 2025-01-29  
**维护人**: Claude AI & 用户

