<template>
    <div class="web-container">
        <!-- 顶部导航栏 -->
        <header class="header">
            <div class="header-content">
                <h1>WPF + Vue3 + SignalR 混合应用</h1>
                <a-tag :color="connected ? 'success' : 'error'">
                    {{ connected ? '已连接' : '未连接' }}
                </a-tag>
            </div>
        </header>

        <!-- 主内容区域 -->
        <main class="main-content">
            <div class="content-section">
                <a-tabs v-model:activeKey="activeTab" size="large">
                    <!-- Tab 1: 性能测试 -->
                    <a-tab-pane key="1" tab="性能测试">
                        <div class="tab-content">
                            <a-row :gutter="24">
                                <a-col :span="8">
                                    <a-card title="基础测试" :bordered="false">
                                        <a-space
                                            direction="vertical"
                                            style="width: 100%"
                                            :size="16"
                                        >
                                            <a-button
                                                type="primary"
                                                size="large"
                                                @click="getCurrentTime"
                                                :loading="loading"
                                                block
                                            >
                                                获取当前时间
                                            </a-button>
                                            <div
                                                v-if="currentTime"
                                                class="result-text"
                                            >
                                                {{ currentTime }}
                                            </div>

                                            <a-divider />

                                            <a-button
                                                type="default"
                                                size="large"
                                                @click="testLatency"
                                                :loading="testingLatency"
                                                block
                                            >
                                                测试通讯延迟
                                            </a-button>
                                            <div
                                                v-if="latencyResult"
                                                class="result-box latency-box"
                                            >
                                                <div class="result-item">
                                                    <span>平均延迟:</span>
                                                    <a-tag color="green"
                                                        >{{
                                                            latencyResult.average
                                                        }}
                                                        ms</a-tag
                                                    >
                                                </div>
                                                <div class="result-item">
                                                    <span>最小/最大:</span>
                                                    <span
                                                        >{{
                                                            latencyResult.min
                                                        }}
                                                        /
                                                        {{
                                                            latencyResult.max
                                                        }}
                                                        ms</span
                                                    >
                                                </div>
                                            </div>
                                        </a-space>
                                    </a-card>
                                </a-col>

                                <a-col :span="8">
                                    <a-card
                                        title="吞吐量测试"
                                        :bordered="false"
                                    >
                                        <a-space
                                            direction="vertical"
                                            style="width: 100%"
                                            :size="16"
                                        >
                                            <a-button
                                                type="primary"
                                                size="large"
                                                @click="testThroughput"
                                                :loading="testingThroughput"
                                                block
                                                danger
                                            >
                                                串行吞吐量 (5秒)
                                            </a-button>
                                            <div
                                                v-if="throughputResult"
                                                class="result-box throughput-box"
                                            >
                                                <div class="result-item">
                                                    <span>QPS:</span>
                                                    <a-tag color="red"
                                                        >{{
                                                            throughputResult.qps
                                                        }}
                                                        req/s</a-tag
                                                    >
                                                </div>
                                                <div class="result-item">
                                                    <span>总请求数:</span>
                                                    <span
                                                        >{{
                                                            throughputResult.totalCalls
                                                        }}
                                                        次</span
                                                    >
                                                </div>
                                            </div>

                                            <a-divider />

                                            <a-button
                                                type="primary"
                                                size="large"
                                                @click="testConcurrent"
                                                :loading="testingConcurrent"
                                                block
                                                danger
                                            >
                                                并发吞吐量 (5万)
                                            </a-button>
                                            <div
                                                v-if="concurrentResult"
                                                class="result-box concurrent-box"
                                            >
                                                <div class="result-item">
                                                    <span>QPS:</span>
                                                    <a-tag color="magenta"
                                                        >{{
                                                            concurrentResult.qps
                                                        }}
                                                        req/s</a-tag
                                                    >
                                                </div>
                                                <div class="result-item">
                                                    <span>耗时:</span>
                                                    <span
                                                        >{{
                                                            concurrentResult.duration
                                                        }}
                                                        秒</span
                                                    >
                                                </div>
                                            </div>
                                        </a-space>
                                    </a-card>
                                </a-col>

                                <a-col :span="8">
                                    <a-card title="带宽测试" :bordered="false">
                                        <a-space
                                            direction="vertical"
                                            style="width: 100%"
                                            :size="16"
                                        >
                                            <a-button
                                                type="primary"
                                                size="large"
                                                @click="testBandwidth"
                                                :loading="testingBandwidth"
                                                block
                                                danger
                                            >
                                                测试带宽 (100MB)
                                            </a-button>
                                            <div
                                                v-if="bandwidthResult"
                                                class="result-box bandwidth-box"
                                            >
                                                <div class="result-item">
                                                    <span>传输速度:</span>
                                                    <a-tag color="blue"
                                                        >{{
                                                            bandwidthResult.speedMBps
                                                        }}
                                                        MB/s</a-tag
                                                    >
                                                </div>
                                                <div class="result-item">
                                                    <span>数据大小:</span>
                                                    <span
                                                        >{{
                                                            bandwidthResult.dataSize
                                                        }}
                                                        MB</span
                                                    >
                                                </div>
                                                <div class="result-item">
                                                    <span>传输时长:</span>
                                                    <span
                                                        >{{
                                                            bandwidthResult.duration
                                                        }}
                                                        秒</span
                                                    >
                                                </div>
                                            </div>
                                        </a-space>
                                    </a-card>
                                </a-col>
                            </a-row>
                        </div>
                    </a-tab-pane>

                    <!-- Tab 2: 实时图表 -->
                    <a-tab-pane key="2" tab="实时图表">
                        <div class="tab-content">
                            <div class="chart-header">
                                <h2>实时数据流 (2通道 x 30000点)</h2>
                                <div class="chart-controls">
                                    <a-button
                                        type="primary"
                                        :danger="chartStreaming"
                                        @click="toggleChartStreaming"
                                        size="large"
                                    >
                                        {{
                                            chartStreaming
                                                ? '停止数据流'
                                                : '启动数据流'
                                        }}
                                    </a-button>
                                </div>
                            </div>
                            <div
                                id="chartContainer"
                                class="chart-container"
                            ></div>
                        </div>
                    </a-tab-pane>

                    <!-- Tab 3: 消息推送 -->
                    <a-tab-pane key="3" tab="消息推送">
                        <div class="tab-content">
                            <h2>接收后端推送消息</h2>
                            <a-divider />
                            <div class="message-list">
                                <div
                                    v-if="messages.length === 0"
                                    class="empty-message"
                                >
                                    暂无消息
                                </div>
                                <div
                                    v-for="(msg, index) in messages"
                                    :key="index"
                                    class="message-item"
                                >
                                    {{ msg }}
                                </div>
                            </div>
                        </div>
                    </a-tab-pane>
                </a-tabs>
            </div>
        </main>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, nextTick, watch } from 'vue';
import * as signalR from '@microsoft/signalr';
import { MessagePackHubProtocol } from '@microsoft/signalr-protocol-msgpack';
import { lightningChart, emptyFill, Themes, LegendBoxBuilders, SolidLine, SolidFill, ColorRGBA } from '@lightningchart/lcjs';

const activeTab = ref('1');
const connected = ref(false);
const connectionStatus = ref('正在连接到 SignalR 服务...');
const currentTime = ref('');
const loading = ref(false);
const messages = ref<string[]>([]);

// 延迟测试
const testingLatency = ref(false);
const latencyResult = ref<{
    rtt: number;
    average: number;
    min: number;
    max: number;
} | null>(null);

// 吞吐量测试
const testingThroughput = ref(false);
const throughputResult = ref<{
    totalCalls: number;
    qps: number;
    duration: number;
} | null>(null);

// 并发测试
const testingConcurrent = ref(false);
const concurrentResult = ref<{
    totalCalls: number;
    qps: number;
    duration: number;
} | null>(null);

// 带宽测试
const testingBandwidth = ref(false);
const bandwidthResult = ref<{
    dataSize: number;
    duration: number;
    speedMBps: number;
} | null>(null);

// 图表相关
const chartStreaming = ref(false);
const currentFPS = ref(0);
let chart: any = null;
let series1: any = null;
let series2: any = null;
let lastFrameTime = 0;
let frameCount = 0;
let fpsCheckInterval: any = null;
let localRenderLoop: any = null;

let connection: signalR.HubConnection;

onMounted(async () => {
    connection = new signalR.HubConnectionBuilder()
        .withUrl('http://localhost:5000/hub')
        .withHubProtocol(new MessagePackHubProtocol())
        .withAutomaticReconnect()
        .build();

    connection.on('ReceiveMessage', (message: string) => {
        messages.value.unshift(
            `[${new Date().toLocaleTimeString()}] ${message}`
        );
    });

    connection.on(
        'StreamData',
        (data: { channel1: number[]; channel2: number[] }) => {
            if (chart && series1 && series2) {
                // series1.clear();
                // series1.appendSamples({
                //     yValues: data.channel1,
                //     step: 1
                // });
                // series2.clear();
                // series2.appendSamples({
                //     yValues: data.channel2,
                //     step: 1
                // });
                frameCount++;
                const now = performance.now();
                if (now - lastFrameTime >= 1000) {
                    currentFPS.value = frameCount;
                    chart.setTitle(`实时数据流 (2通道 x 30000点) - FPS: ${frameCount}`);
                    frameCount = 0;
                    lastFrameTime = now;
                }
            }
        }
    );

    try {
        await connection.start();
        connected.value = true;
        console.log('SignalR 连接成功');
    } catch (err) {
        connected.value = false;
        console.error('SignalR 连接失败:', err);
    }
});

// 监听标签页切换，切换到实时图表时初始化图表
watch(activeTab, async (newTab) => {
    if (newTab === '2' && !chart && connected.value) {
        await nextTick();
        await initChart();
    }
});

onUnmounted(async () => {
    if (fpsCheckInterval) {
        clearInterval(fpsCheckInterval);
    }
    if (chartStreaming.value) {
        await toggleChartStreaming();
    }
    if (chart) {
        chart.dispose();
    }
    if (connection) {
        await connection.stop();
    }
});

const initChart = async () => {
    await nextTick();

    const lc = lightningChart({
        license: '0002-n/X8/32N46BkrWb9wJ1FF88Jy0h8JwD0/P1plemwtDO/8iFxGtXV2ktvhOEkc+pqj5W5BnhPvaobCGEQZE8=-MEUCIQCZiQhNP4C0hfnk9HZm9RTgaA61CgPFcdER0kPjaCf+qQIgLeZ/5wp5H852W+OpVa8ZpOb+r4YZP9T1t4KQaZA509M=',
        licenseInformation: {
            appTitle: 'ETest',
            company: 'Beijing Kiyun Hengda Technology Co., Ltd.'
        }
    });

    chart = lc.ChartXY({
        container: 'chartContainer',
        animationsEnabled: false,
        theme: Themes.darkGold
    });

    chart.setTitle('实时数据流 (2通道 x 30000点)');
    chart.getDefaultAxisX().setTitle('采样点');
    chart.getDefaultAxisY().setTitle('幅值');
    chart.getDefaultAxisX().setInterval({ start: 0, end: 30000 });

    // 添加图例
    const legend = chart
        .addLegendBox(LegendBoxBuilders.HorizontalLegendBox)
        .setTitle('通道');

    // 通道 1 - 橙色
    series1 = chart
        .addPointLineAreaSeries({ dataPattern: 'ProgressiveX' })
        .setAreaFillStyle(emptyFill)
        .setName('通道 1')
        .setStrokeStyle(new SolidLine({
            thickness: 2,
            fillStyle: new SolidFill({ color: ColorRGBA(255, 140, 0) })
        }));
    legend.add(series1);

    // 通道 2 - 蓝色
    series2 = chart
        .addPointLineAreaSeries({ dataPattern: 'ProgressiveX' })
        .setAreaFillStyle(emptyFill)
        .setName('通道 2')
        .setStrokeStyle(new SolidLine({
            thickness: 2,
            fillStyle: new SolidFill({ color: ColorRGBA(30, 144, 255) })
        }));
    legend.add(series2);
};

const toggleChartStreaming = async () => {
    if (!connected.value) return;
    if (chartStreaming.value) {
        await connection.invoke('StopStreaming');
        chartStreaming.value = false;
        currentFPS.value = 0;
        frameCount = 0;
        if (fpsCheckInterval) {
            clearInterval(fpsCheckInterval);
            fpsCheckInterval = null;
        }
        if (chart) {
            chart.setTitle('实时数据流 (2通道 x 30000点) - FPS: 0');
        }
    } else {
        await connection.invoke('StartStreaming');
        chartStreaming.value = true;
        lastFrameTime = performance.now();

        // 监控数据流是否停止
        fpsCheckInterval = setInterval(() => {
            if (chart && performance.now() - lastFrameTime > 1300) {
                chart.setTitle('实时数据流 (2通道 x 30000点) - FPS: 0');
                currentFPS.value = 0;
            }
        }, 100);
    }
};

const getCurrentTime = async () => {
    if (!connected.value) return;
    loading.value = true;
    try {
        const time = await connection.invoke<string>('GetCurrentTime');
        currentTime.value = time;
    } catch (err) {
        console.error('调用失败:', err);
    } finally {
        loading.value = false;
    }
};

const testLatency = async () => {
    if (!connected.value) return;
    testingLatency.value = true;
    const latencies: number[] = [];
    try {
        for (let i = 0; i < 10; i++) {
            const startTime = performance.now();
            await connection.invoke<number>('PingTest', startTime);
            const rtt = performance.now() - startTime;
            latencies.push(rtt);
            if (i < 9) await new Promise((resolve) => setTimeout(resolve, 100));
        }
        const sum = latencies.reduce((a, b) => a + b, 0);
        latencyResult.value = {
            rtt:
                Math.round((latencies[latencies.length - 1] ?? 0) * 1000) /
                1000,
            average: Math.round((sum / latencies.length) * 1000) / 1000,
            min: Math.round(Math.min(...latencies) * 1000) / 1000,
            max: Math.round(Math.max(...latencies) * 1000) / 1000,
        };
    } catch (err) {
        console.error('延迟测试失败:', err);
    } finally {
        testingLatency.value = false;
    }
};

const testThroughput = async () => {
    if (!connected.value) return;
    testingThroughput.value = true;
    let totalCalls = 0;
    const testDuration = 5000;
    try {
        const startTime = performance.now();
        const endTime = startTime + testDuration;
        while (performance.now() < endTime) {
            await connection.invoke<boolean>('ThroughputTest');
            totalCalls++;
        }
        const actualDuration = (performance.now() - startTime) / 1000;
        const qps = Math.round((totalCalls / actualDuration) * 100) / 100;
        throughputResult.value = {
            totalCalls,
            qps,
            duration: Math.round(actualDuration * 100) / 100,
        };
    } catch (err) {
        console.error('吞吐量测试失败:', err);
    } finally {
        testingThroughput.value = false;
    }
};

const testConcurrent = async () => {
    if (!connected.value) return;
    testingConcurrent.value = true;
    const concurrentCount = 50000;
    try {
        const startTime = performance.now();
        const promises: Promise<boolean>[] = [];
        for (let i = 0; i < concurrentCount; i++) {
            promises.push(connection.invoke<boolean>('ThroughputTest'));
        }
        await Promise.all(promises);
        const actualDuration = performance.now() - startTime;
        const durationSeconds = Math.round(actualDuration / 10) / 100;
        const qps = Math.round((concurrentCount / durationSeconds) * 100) / 100;
        concurrentResult.value = {
            totalCalls: concurrentCount,
            qps,
            duration: durationSeconds,
        };
    } catch (err) {
        console.error('并发吞吐量测试失败:', err);
    } finally {
        testingConcurrent.value = false;
    }
};

const testBandwidth = async () => {
    if (!connected.value) return;
    testingBandwidth.value = true;
    const dataSizeInMB = 100;
    try {
        const startTime = performance.now();
        const data = await connection.invoke<ArrayBuffer>(
            'BandwidthTest',
            dataSizeInMB
        );
        const uint8Array = new Uint8Array(data);
        let checksum = 0;
        for (let i = 0; i < uint8Array.length; i++) {
            checksum += uint8Array[i] ?? 0;
        }
        const actualDuration = performance.now() - startTime;
        const durationSeconds = Math.round(actualDuration / 10) / 100;
        const speedMBps =
            Math.round((dataSizeInMB / durationSeconds) * 100) / 100;
        bandwidthResult.value = {
            dataSize: dataSizeInMB,
            duration: durationSeconds,
            speedMBps,
        };
    } catch (err) {
        console.error('带宽测试失败:', err);
    } finally {
        testingBandwidth.value = false;
    }
};
</script>

<style scoped>
.web-container {
    min-height: 100vh;
    background: #f0f2f5;
}

.header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 20px 0;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.header-content {
    margin: 0;
    padding: 0 40px;
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.header h1 {
    margin: 0;
    font-size: 24px;
    color: white;
}

.main-content {
    margin: 0;
    padding: 24px 40px;
    height: calc(100vh - 80px);
}

.content-section {
    background: #fff;
    padding: 16px;
    border-radius: 8px;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.06);
    height: 100%;
}

.tab-content {
    padding: 16px 0;
    height: calc(100vh - 180px);
    overflow: hidden;
}

.result-text {
    padding: 12px;
    background: #f6ffed;
    border: 1px solid #b7eb8f;
    border-radius: 4px;
    text-align: center;
    font-weight: bold;
}

.result-box {
    padding: 12px;
    background: #f6ffed;
    border: 1px solid #b7eb8f;
    border-radius: 4px;
}

.result-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 4px 0;
}

.latency-box {
    background: #e6f7ff;
    border: 1px solid #91d5ff;
}

.throughput-box {
    background: #fff2e8;
    border: 1px solid #ffbb96;
}

.concurrent-box {
    background: #f9f0ff;
    border: 1px solid #d3adf7;
}

.bandwidth-box {
    background: #e6fffb;
    border: 1px solid #87e8de;
}

.chart-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 16px;
}

.chart-header h2 {
    margin: 0;
    font-size: 18px;
    color: #333;
}

.chart-controls {
    display: flex;
    align-items: center;
}

.chart-container {
    width: 100%;
    height: calc(100% - 60px);
}

.message-list {
    height: calc(100vh - 280px);
    overflow-y: auto;
}

.message-item {
    padding: 12px;
    margin-bottom: 8px;
    background: #fafafa;
    border-left: 3px solid #1890ff;
    border-radius: 4px;
    color: #333;
}

.empty-message {
    text-align: center;
    color: #999;
    padding: 40px;
}

:deep(.ant-card) {
    height: 100%;
}

:deep(.ant-card-body) {
    height: calc(100% - 57px);
}
</style>
