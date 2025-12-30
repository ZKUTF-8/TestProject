/**
 * 前后端数据模型
 * ⚠️ 此文件需要与后端 C# 模型保持同步
 * 参考：src/LC8172.WpfHost/Models/
 */

/**
 * 闸门类型
 */
export const enum GateType {
    /** 根闸门（整个采集窗口） */
    Root = 0,
    /** 上升沿检测 */
    Rising = 1,
    /** 下降沿检测 */
    Falling = 2,
    /** 数据门（寻峰） */
    Data = 3
}

/**
 * 闸门配置
 */
export interface GateConfig {
    /** 唯一标识 */
    id: string;
    /** 闸门名称 */
    name: string;
    /** 起始时间（相对父闸门，单位：微秒） */
    start: number;
    /** 闸门长度（单位：微秒） */
    length: number;
    /** 阈值电压（单位：伏特，用于边沿检测） */
    threshold: number;
    /** 闸门类型 */
    type: GateType;
    /** 显示颜色（HEX格式，如 #FFFF00） */
    color: string;
    /** 应用的采集通道号 */
    channel: number;
    /** 父闸门ID（可选） */
    parentId?: string;
    /** 子闸门列表 */
    children: GateConfig[];

    // ===== 前端专用字段（不传给后端） =====
    /** 绝对起始位置（计算得出） */
    absoluteStart?: number;
    /** 像素X坐标（用于ECharts渲染） */
    pixelX?: number;
    /** 像素宽度（用于ECharts渲染） */
    pixelWidth?: number;
    /** 是否被选中 */
    isSelected?: boolean;
    /** 是否展开子节点 */
    isExpanded?: boolean;
}

/**
 * 波形数据
 */
export interface WaveformData {
    /** 数据点列表 [[x1, y1], [x2, y2], ...] */
    points: number[][];
    /** 采样率（单位：MHz） */
    sampleRate: number;
    /** 采集通道号 */
    channel: number;
    /** 时间戳 */
    timestamp: string;
}

/**
 * 闸门计算结果
 */
export interface GateResult {
    /** 闸门ID */
    gateId: string;
    /** 闸门名称 */
    gateName: string;
    
    /** 上升沿是否找到 */
    risingEdgeFound: boolean;
    /** 上升沿电压值 */
    risingEdgeValue: number;
    /** 上升沿时间（TOF） */
    risingEdgeTime: number;
    
    /** 下降沿是否找到 */
    fallingEdgeFound: boolean;
    /** 下降沿电压值 */
    fallingEdgeValue: number;
    /** 下降沿时间（TOF） */
    fallingEdgeTime: number;
    
    /** 正峰值是否找到（PPA - Positive Peak Amplitude） */
    ppaFound: boolean;
    /** 正峰值电压 */
    ppaValue: number;
    /** 正峰值时间（TOF） */
    ppaTime: number;
    
    /** 负峰值是否找到（NPA - Negative Peak Amplitude） */
    npaFound: boolean;
    /** 负峰值电压 */
    npaValue: number;
    /** 负峰值时间（TOF） */
    npaTime: number;
    
    /** 子闸门结果列表 */
    childResults: GateResult[];
}

/**
 * 采集配置
 */
export interface AcquisitionConfig {
    /** 通道号 */
    channel: number;
    /** 采样率（MHz） */
    sampleRate: number;
    /** 采样点数 */
    samplePoints: number;
}

