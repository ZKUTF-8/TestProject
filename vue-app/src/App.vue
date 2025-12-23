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
            <a-row :gutter="24">
                <!-- 左侧：调用后端接口 -->
                <a-col :span="12">
                    <div class="content-section">
                        <h2>调用后端接口</h2>
                        <a-divider />
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
                            <div v-if="currentTime" class="result-box">
                                <a-typography-title :level="4"
                                    >服务器时间</a-typography-title
                                >
                                <a-typography-text strong>{{
                                    currentTime
                                }}</a-typography-text>
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
                                <a-typography-title :level="4"
                                    >延迟测试结果</a-typography-title
                                >
                                <a-descriptions
                                    bordered
                                    :column="1"
                                    size="small"
                                >
                                    <a-descriptions-item label="往返延迟 (RTT)">
                                        <a-tag color="blue"
                                            >{{ latencyResult.rtt }} ms</a-tag
                                        >
                                    </a-descriptions-item>
                                    <a-descriptions-item
                                        label="平均延迟 (10次)"
                                    >
                                        <a-tag color="green"
                                            >{{
                                                latencyResult.average
                                            }}
                                            ms</a-tag
                                        >
                                    </a-descriptions-item>
                                    <a-descriptions-item label="最小延迟">
                                        {{ latencyResult.min }} ms
                                    </a-descriptions-item>
                                    <a-descriptions-item label="最大延迟">
                                        {{ latencyResult.max }} ms
                                    </a-descriptions-item>
                                </a-descriptions>
                            </div>
                        </a-space>
                    </div>
                </a-col>

                <!-- 右侧：接收后端推送 -->
                <a-col :span="12">
                    <div class="content-section">
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
                </a-col>
            </a-row>
        </main>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import * as signalR from '@microsoft/signalr';
import { MessagePackHubProtocol } from '@microsoft/signalr-protocol-msgpack';

const connected = ref(false);
const connectionStatus = ref('正在连接到 SignalR 服务...');
const currentTime = ref('');
const loading = ref(false);
const messages = ref<string[]>([]);
const testingLatency = ref(false);
const latencyResult = ref<{
    rtt: number;
    average: number;
    min: number;
    max: number;
} | null>(null);

let connection: signalR.HubConnection;

onMounted(async () => {
    // 创建 SignalR 连接（使用 MessagePack 二进制协议）
    connection = new signalR.HubConnectionBuilder()
        .withUrl('http://localhost:5000/hub')
        .withHubProtocol(new MessagePackHubProtocol())
        .withAutomaticReconnect()
        .build();

    // 监听服务端推送的消息
    connection.on('ReceiveMessage', (message: string) => {
        messages.value.unshift(
            `[${new Date().toLocaleTimeString()}] ${message}`
        );
    });

    // 启动连接
    try {
        await connection.start();
        connected.value = true;
        connectionStatus.value = '已连接到 SignalR 服务';
        console.log('SignalR 连接成功');
    } catch (err) {
        connected.value = false;
        connectionStatus.value = '连接失败：请确保后端服务已启动';
        console.error('SignalR 连接失败:', err);
    }
});

onUnmounted(async () => {
    if (connection) {
        await connection.stop();
    }
});

// 调用后端方法获取当前时间
const getCurrentTime = async () => {
    if (!connected.value) {
        return;
    }

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

// 测试通讯延迟（使用高精度计时器）
const testLatency = async () => {
    if (!connected.value) {
        return;
    }

    testingLatency.value = true;
    const latencies: number[] = [];

    try {
        // 进行 10 次 ping 测试
        for (let i = 0; i < 10; i++) {
            const startTime = performance.now(); // 使用高精度计时器（微秒级）
            const clientTimestamp = performance.now();

            await connection.invoke<number>('PingTest', clientTimestamp);

            const endTime = performance.now();
            const rtt = endTime - startTime;
            latencies.push(rtt);

            // 每次测试间隔 100ms
            if (i < 9) {
                await new Promise((resolve) => setTimeout(resolve, 100));
            }
        }

        // 计算统计数据（保留3位小数，显示微秒精度）
        const sum = latencies.reduce((a, b) => a + b, 0);
        latencyResult.value = {
            rtt: Math.round((latencies[latencies.length - 1] ?? 0) * 1000) / 1000,
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
</script>

<style scoped>
.web-container {
    min-height: 100vh;
    background: #fff;
}

.header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 24px 0;
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
    font-size: 28px;
    color: white;
}

.main-content {
    margin: 0;
    padding: 40px;
}

.content-section {
    background: #fff;
    padding: 24px;
    border-radius: 8px;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.06);
    min-height: 400px;
}

.content-section h2 {
    margin-top: 0;
    color: #333;
    font-size: 20px;
}

.result-box {
    margin-top: 24px;
    padding: 20px;
    background: #f6ffed;
    border: 1px solid #b7eb8f;
    border-radius: 4px;
}

.latency-box {
    background: #e6f7ff;
    border: 1px solid #91d5ff;
}

.message-list {
    max-height: 300px;
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
</style>
