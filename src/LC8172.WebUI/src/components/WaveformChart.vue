<template>
    <div class="waveform-chart-container">
        <v-chart
            ref="chartRef"
            :option="chartOption"
            :autoresize="true"
            style="width: 100%; height: 100%"
            @click="onChartClick"
        />
    </div>
</template>

<script setup lang="ts">
import { ref, computed, watch } from 'vue';
import VChart from 'vue-echarts';
import { use } from 'echarts/core';
import { CanvasRenderer } from 'echarts/renderers';
import { LineChart } from 'echarts/charts';
import {
    GridComponent,
    TooltipComponent,
    LegendComponent,
    DataZoomComponent
} from 'echarts/components';
import type { GateConfig, WaveformData } from '@/types/models';

// 注册 ECharts 组件
// 使用 Canvas 渲染器，适合大数据量波形显示（10000+点）
// Canvas 渲染在大数据场景下性能优于 SVG
use([
    CanvasRenderer,
    LineChart,
    GridComponent,
    TooltipComponent,
    LegendComponent,
    DataZoomComponent
]);

// ==================== Props ====================
interface Props {
    /** 波形数据 */
    data?: WaveformData;
    /** 闸门配置列表 */
    gates?: GateConfig[];
    /** 图表高度 */
    height?: number;
}

const props = withDefaults(defineProps<Props>(), {
    data: undefined,
    gates: () => [],
    height: 500
});

// ==================== Emits ====================
const emit = defineEmits<{
    /** 闸门拖拽事件 */
    gateDrag: [gate: GateConfig];
    /** 闸门点击事件 */
    gateClick: [gate: GateConfig];
    /** 图表点击事件 */
    chartClick: [params: any];
}>();

// ==================== State ====================
const chartRef = ref();

// ==================== Computed ====================

/**
 * ECharts 配置选项
 */
const chartOption = computed(() => {
    // 准备波形数据
    const seriesData = props.data?.points || [];

    return {
        animation: false, // 关闭动画以提升性能
        backgroundColor: '#fff',
        
        // 网格配置
        grid: {
            left: 80,
            right: 40,
            top: 60,
            bottom: 80,
            containLabel: true
        },

        // X轴配置
        xAxis: {
            type: 'value',
            name: '时间 (μs)',
            nameLocation: 'middle',
            nameGap: 30,
            nameTextStyle: {
                fontSize: 14,
                fontWeight: 'bold'
            },
            axisLine: {
                lineStyle: { color: '#666' }
            },
            splitLine: {
                show: true,
                lineStyle: { color: '#e0e0e0', type: 'dashed' }
            }
        },

        // Y轴配置
        yAxis: {
            type: 'value',
            name: '电压 (V)',
            nameLocation: 'middle',
            nameGap: 50,
            nameTextStyle: {
                fontSize: 14,
                fontWeight: 'bold'
            },
            axisLine: {
                lineStyle: { color: '#666' }
            },
            splitLine: {
                show: true,
                lineStyle: { color: '#e0e0e0', type: 'dashed' }
            }
        },

        // 工具提示
        tooltip: {
            trigger: 'axis',
            formatter: (params: any) => {
                if (!params || params.length === 0) return '';
                const point = params[0];
                return `
                    时间: ${point.value[0].toFixed(3)} μs<br/>
                    电压: ${point.value[1].toFixed(4)} V
                `;
            },
            axisPointer: {
                type: 'cross',
                label: {
                    backgroundColor: '#6a7985'
                }
            }
        },

        // 数据缩放
        dataZoom: [
            {
                type: 'inside', // 鼠标滚轮缩放
                xAxisIndex: 0,
                start: 0,
                end: 100
            },
            {
                type: 'slider', // 滑块缩放
                xAxisIndex: 0,
                start: 0,
                end: 100,
                height: 20,
                bottom: 10
            }
        ],

        // 系列数据（波形）
        series: [
            {
                type: 'line',
                name: `通道 ${props.data?.channel || 1}`,
                data: seriesData,
                smooth: false,
                symbol: 'none', // 不显示数据点标记
                lineStyle: {
                    width: 1.5,
                    color: '#00ff00' // 绿色波形
                },
                sampling: 'lttb', // LTTB降采样算法，大幅提升性能
                large: true, // 启用大数据优化模式
                largeThreshold: 1000, // 超过1000个点启用大数据模式
                progressive: 5000, // 渐进式渲染，每次渲染5000个点
                progressiveThreshold: 10000, // 超过10000个点启用渐进式渲染
                progressiveChunkMode: 'sequential' // 顺序渲染模式
            }
        ],

        // 图形元素（用于绘制闸门）
        graphic: generateGateGraphics()
    };
});

/**
 * 生成闸门图形元素
 */
function generateGateGraphics() {
    if (!props.gates || props.gates.length === 0) return [];

    const graphics: any[] = [];
    const chart = chartRef.value?.chart;
    if (!chart) return [];

    props.gates.forEach((gate) => {
        // 计算绝对起始位置
        const absoluteStart = calculateAbsoluteStart(gate);
        // const absoluteEnd = absoluteStart + gate.length;

        // TODO: 将数据坐标转换为像素坐标
        // const pixelStart = chart.convertToPixel('grid', [absoluteStart, 0]);
        // const pixelEnd = chart.convertToPixel('grid', [absoluteEnd, 0]);

        // 闸门矩形背景
        graphics.push({
            type: 'rect',
            id: `gate-rect-${gate.id}`,
            shape: {
                x: absoluteStart,
                y: 0,
                width: gate.length,
                height: 100 // 百分比
            },
            style: {
                fill: gate.color + '20', // 半透明
                stroke: gate.color,
                lineWidth: 2
            },
            z: 10
        });

        // 闸门名称标签
        graphics.push({
            type: 'text',
            id: `gate-label-${gate.id}`,
            style: {
                text: gate.name,
                x: absoluteStart + 5,
                y: 10,
                fill: gate.color,
                fontSize: 14,
                fontWeight: 'bold'
            },
            z: 11
        });
    });

    return graphics;
}

/**
 * 计算闸门的绝对起始位置（递归计算父闸门）
 */
function calculateAbsoluteStart(gate: GateConfig): number {
    let totalStart = gate.start;
    
    // 如果有父闸门ID，需要找到父闸门并累加位置
    if (gate.parentId && props.gates) {
        const parent = findGateById(props.gates, gate.parentId);
        if (parent) {
            totalStart += calculateAbsoluteStart(parent);
        }
    }
    
    return totalStart;
}

/**
 * 递归查找闸门
 */
function findGateById(gates: GateConfig[], id: string): GateConfig | null {
    for (const gate of gates) {
        if (gate.id === id) return gate;
        if (gate.children && gate.children.length > 0) {
            const found = findGateById(gate.children, id);
            if (found) return found;
        }
    }
    return null;
}

/**
 * 图表点击事件
 */
function onChartClick(params: any) {
    emit('chartClick', params);
}

// ==================== Watch ====================

/**
 * 监听数据变化，更新图表
 */
watch(
    () => props.data,
    () => {
        // 数据更新后，图表会自动重绘
    },
    { deep: true }
);

watch(
    () => props.gates,
    () => {
        // 闸门更新后，图表会自动重绘
    },
    { deep: true }
);

// ==================== Expose ====================
defineExpose({
    chartRef
});
</script>

<style scoped>
.waveform-chart-container {
    width: 100%;
    height: 100%;
    position: relative;
}
</style>

