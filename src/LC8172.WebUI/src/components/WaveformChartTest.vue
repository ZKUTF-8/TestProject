<template>
    <div class="waveform-test">
        <a-card title="波形图测试" size="small">
            <template #extra>
                <a-space>
                    <a-button
                        @click="generateMockData"
                        type="primary"
                        size="small"
                    >
                        生成测试数据
                    </a-button>
                    <a-button @click="addGate" size="small">添加闸门</a-button>
                    <a-button @click="clearGates" danger size="small"
                        >清空闸门</a-button
                    >
                </a-space>
            </template>

            <div ref="chartContainer" class="chart-container"></div>

            <a-divider />

            <div class="gate-list">
                <h4>闸门列表：</h4>
                <a-list size="small" :data-source="gates" bordered>
                    <template #renderItem="{ item }">
                        <a-list-item>
                            <span>{{ item.name }}</span>
                            <span style="margin-left: 16px"
                                >开始: {{ item.start.toFixed(2) }}</span
                            >
                            <span style="margin-left: 16px"
                                >长度: {{ item.length.toFixed(2) }}</span
                            >
                            <template #actions>
                                <a-button
                                    size="small"
                                    danger
                                    @click="removeGate(item.id)"
                                    >删除</a-button
                                >
                            </template>
                        </a-list-item>
                    </template>
                </a-list>
            </div>
        </a-card>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import * as echarts from 'echarts';
import type { EChartsType } from 'echarts';

interface Gate {
    id: string;
    name: string;
    start: number;
    length: number;
    color: string;
}

const chartContainer = ref<HTMLElement | null>(null);
let chart: EChartsType | null = null;

const waveformData = ref<number[][]>([]);
const gates = ref<Gate[]>([]);
let gateCounter = 1;

// 生成模拟波形数据
function generateMockData() {
    const points: number[][] = [];
    const totalPoints = 1000;

    for (let i = 0; i < totalPoints; i++) {
        const x = i / 100;
        const y =
            Math.sin(x) * 0.8 +
            Math.sin(x * 3) * 0.3 +
            (Math.random() - 0.5) * 0.1;
        points.push([x, y]);
    }

    waveformData.value = points;

    // 只更新波形数据，不重新生成闸门图形
    if (chart) {
        chart.setOption({
            series: [
                {
                    type: 'line',
                    data: waveformData.value,
                    showSymbol: false,
                    animation: false,
                    lineStyle: {
                        width: 1,
                        color: '#00ff00',
                    },
                },
            ],
        });
    }
}

// 添加闸门
function addGate() {
    const randomStart = Math.random() * 8;
    const randomLength = Math.random() * 2 + 0.5;

    gates.value.push({
        id: `gate-${Date.now()}`,
        name: `Gate${gateCounter++}`,
        start: randomStart,
        length: randomLength,
        color: `hsl(${Math.random() * 360}, 70%, 50%)`,
    });

    updateChart();
}

// 删除闸门
function removeGate(id: string) {
    gates.value = gates.value.filter((g) => g.id !== id);
    updateChart();
}

// 清空闸门
function clearGates() {
    gates.value = [];
    updateChart();
}

// 更新图表
function updateChart() {
    if (!chart) return;

    const option: echarts.EChartsOption = {
        grid: {
            left: 60,
            right: 40,
            top: 40,
            bottom: 60,
        },
        xAxis: {
            type: 'value',
            name: '时间 (μs)',
            min: 0,
            max: 10,
        },
        yAxis: {
            type: 'value',
            name: '幅度 (V)',
            min: -1.5,
            max: 1.5,
        },
        tooltip: {
            trigger: 'axis',
            formatter: (params: any) => {
                const point = params[0];
                return `时间: ${point.data[0].toFixed(
                    3
                )} μs<br/>幅度: ${point.data[1].toFixed(3)} V`;
            },
        },
        series: [
            {
                type: 'line',
                data: waveformData.value,
                showSymbol: false,
                animation: false,
                lineStyle: {
                    width: 1,
                    color: '#00ff00',
                },
            },
        ],
        graphic: gates.value
            .map((gate) => {
                return [
                    // 闸门矩形区域
                    {
                        type: 'rect',
                        id: `${gate.id}-rect`,
                        z: 10,
                        shape: {
                            x: 0,
                            y: 0,
                            width: 0,
                            height: 0,
                        },
                        style: {
                            fill: gate.color,
                            opacity: 0.2,
                        },
                        invisible: false,
                        draggable: false,
                        ondrag: null,
                    },
                    // 左边界线（可拖拽调整起始位置）
                    {
                        type: 'line',
                        id: `${gate.id}-left`,
                        z: 20,
                        shape: {
                            x1: 0,
                            y1: 0,
                            x2: 0,
                            y2: 0,
                        },
                        style: {
                            stroke: gate.color,
                            lineWidth: 3,
                        },
                        cursor: 'ew-resize',
                        draggable: 'horizontal',
                        ondrag: echarts.util.curry(onGateLeftDrag, gate.id),
                    },
                    // 右边界线（可拖拽调整长度）
                    {
                        type: 'line',
                        id: `${gate.id}-right`,
                        z: 20,
                        shape: {
                            x1: 0,
                            y1: 0,
                            x2: 0,
                            y2: 0,
                        },
                        style: {
                            stroke: gate.color,
                            lineWidth: 3,
                        },
                        cursor: 'ew-resize',
                        draggable: 'horizontal',
                        ondrag: echarts.util.curry(onGateRightDrag, gate.id),
                    },
                    // 闸门标签
                    {
                        type: 'text',
                        id: `${gate.id}-label`,
                        z: 30,
                        style: {
                            text: gate.name,
                            fill: gate.color,
                            fontSize: 12,
                            fontWeight: 'bold',
                        },
                    },
                ];
            })
            .flat(),
    };

    chart.setOption(option, true);

    // 更新闸门图形的位置
    requestAnimationFrame(() => {
        updateGateGraphics();
    });
}

// 更新闸门图形位置
function updateGateGraphics() {
    if (!chart) return;

    gates.value.forEach((gate) => {
        // 将数据坐标转换为像素坐标
        const leftPixel = chart!.convertToPixel('grid', [gate.start, 0]) as [
            number,
            number
        ];
        const rightPixel = chart!.convertToPixel('grid', [
            gate.start + gate.length,
            0,
        ]) as [number, number];
        const topPixel = chart!.convertToPixel('grid', [0, 1.5]) as [
            number,
            number
        ];
        const bottomPixel = chart!.convertToPixel('grid', [0, -1.5]) as [
            number,
            number
        ];

        // 更新矩形区域
        chart!.setOption({
            graphic: [
                {
                    id: `${gate.id}-rect`,
                    $action: 'merge',
                    shape: {
                        x: leftPixel[0]!,
                        y: topPixel[1]!,
                        width: rightPixel[0]! - leftPixel[0]!,
                        height: bottomPixel[1]! - topPixel[1]!,
                    },
                },
                {
                    id: `${gate.id}-left`,
                    $action: 'merge',
                    shape: {
                        x1: leftPixel[0]!,
                        y1: topPixel[1]!,
                        x2: leftPixel[0]!,
                        y2: bottomPixel[1]!,
                    },
                },
                {
                    id: `${gate.id}-right`,
                    $action: 'merge',
                    shape: {
                        x1: rightPixel[0]!,
                        y1: topPixel[1]!,
                        x2: rightPixel[0]!,
                        y2: bottomPixel[1]!,
                    },
                },
                {
                    id: `${gate.id}-label`,
                    $action: 'merge',
                    x: leftPixel[0]! + 5,
                    y: topPixel[1]! + 5,
                },
            ],
        });
    });
}

// 拖拽左边界（调整起始位置）
function onGateLeftDrag(gateId: string, params: any) {
    const gate = gates.value.find((g) => g.id === gateId);
    if (!gate || !chart || !params.target) return;

    // 获取拖拽后的图形位置
    const shape = params.target.shape;
    if (!shape) return;

    // 将像素坐标转换为数据坐标
    const dataCoord = chart.convertFromPixel('grid', [shape.x1, 0]) as [
        number,
        number
    ];
    if (!dataCoord) return;

    const newStart = Math.max(
        0,
        Math.min(dataCoord[0], gate.start + gate.length - 0.1)
    );
    const deltaStart = newStart - gate.start;

    gate.start = newStart;
    gate.length = gate.length - deltaStart;

    // 实时更新图形
    updateSingleGate(gate);
}

// 拖拽右边界（调整长度）
function onGateRightDrag(gateId: string, params: any) {
    const gate = gates.value.find((g) => g.id === gateId);
    if (!gate || !chart || !params.target) return;

    // 获取拖拽后的图形位置
    const shape = params.target.shape;
    if (!shape) return;

    // 将像素坐标转换为数据坐标
    const dataCoord = chart.convertFromPixel('grid', [shape.x1, 0]) as [
        number,
        number
    ];
    if (!dataCoord) return;

    const newEnd = Math.max(gate.start + 0.1, dataCoord[0]);
    gate.length = newEnd - gate.start;

    // 实时更新图形
    updateSingleGate(gate);
}

// 更新单个闸门图形（优化性能）
function updateSingleGate(gate: Gate) {
    if (!chart) return;

    const leftPixel = chart.convertToPixel('grid', [gate.start, 0]) as [
        number,
        number
    ];
    const rightPixel = chart.convertToPixel('grid', [
        gate.start + gate.length,
        0,
    ]) as [number, number];
    const topPixel = chart.convertToPixel('grid', [0, 1.5]) as [number, number];
    const bottomPixel = chart.convertToPixel('grid', [0, -1.5]) as [
        number,
        number
    ];

    chart.setOption({
        graphic: [
            {
                id: `${gate.id}-rect`,
                $action: 'merge',
                shape: {
                    x: leftPixel[0]!,
                    y: topPixel[1]!,
                    width: rightPixel[0]! - leftPixel[0]!,
                    height: bottomPixel[1]! - topPixel[1]!,
                },
            },
            {
                id: `${gate.id}-left`,
                $action: 'merge',
                shape: {
                    x1: leftPixel[0]!,
                    y1: topPixel[1]!,
                    x2: leftPixel[0]!,
                    y2: bottomPixel[1]!,
                },
            },
            {
                id: `${gate.id}-right`,
                $action: 'merge',
                shape: {
                    x1: rightPixel[0]!,
                    y1: topPixel[1]!,
                    x2: rightPixel[0]!,
                    y2: bottomPixel[1]!,
                },
            },
            {
                id: `${gate.id}-label`,
                $action: 'merge',
                x: leftPixel[0]! + 5,
                y: topPixel[1]! + 5,
            },
        ],
    });
}

onMounted(() => {
    if (chartContainer.value) {
        chart = echarts.init(chartContainer.value);

        // 监听窗口大小变化
        window.addEventListener('resize', () => {
            chart?.resize();
            updateGateGraphics();
        });

        // 生成初始数据
        generateMockData();

        // 添加一个初始闸门
        gates.value.push({
            id: 'gate-initial',
            name: 'Gate1',
            start: 2,
            length: 2,
            color: '#ff6b6b',
        });
        gateCounter = 2;

        updateChart();
    }
});

onUnmounted(() => {
    chart?.dispose();
});
</script>

<style scoped>
.waveform-test {
    padding: 16px;
}

.chart-container {
    width: 100%;
    height: 500px;
    background: #1a1a1a;
    border-radius: 4px;
}

.gate-list {
    margin-top: 16px;
}

.gate-list h4 {
    margin-bottom: 8px;
}
</style>
