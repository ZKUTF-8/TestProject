<template>
    <a-config-provider 
        :theme="{ token: { colorPrimary: '#1890ff' } }"
        :wave="{ disabled: true }"
    >
        <div class="app-container">
            <!-- 顶部导航栏 -->
            <header class="app-header">
                <div class="header-content">
                    <h1>LC-8172 数据采集上位机</h1>
                    <a-space>
                        <a-tag :color="isConnected ? 'success' : 'error'">
                            {{
                                isConnected
                                    ? 'SignalR 已连接'
                                    : 'SignalR 未连接'
                            }}
                        </a-tag>
                        <a-button @click="showTest = !showTest" size="small">
                            {{ showTest ? '返回主界面' : '测试图表' }}
                        </a-button>
                    </a-space>
                </div>
            </header>

            <!-- 主内容区域 -->
            <main class="app-main" v-if="!showTest">
                <a-layout style="height: 100%">
                    <!-- 左侧闸门编辑器 -->
                    <a-layout-sider
                        width="320"
                        theme="light"
                        style="
                            background: #fff;
                            border-right: 1px solid #f0f0f0;
                        "
                    >
                        <div style="padding: 16px">
                            <h3>闸门配置</h3>
                            <a-divider style="margin: 12px 0" />
                            <a-empty description="闸门编辑器开发中..." />
                        </div>
                    </a-layout-sider>

                    <!-- 右侧内容区 -->
                    <a-layout-content
                        style="padding: 16px; background: #f5f5f5"
                    >
                        <a-space
                            direction="vertical"
                            style="width: 100%"
                            :size="16"
                        >
                            <!-- 控制面板 -->
                            <a-card title="采集控制" size="small">
                                <a-space>
                                    <a-button
                                        type="primary"
                                        :loading="!!isAcquiring"
                                    >
                                        开始采集
                                    </a-button>
                                    <a-button danger :disabled="!isAcquiring">
                                        停止采集
                                    </a-button>
                                    <a-button>配置</a-button>
                                </a-space>
                            </a-card>

                            <!-- 波形显示区 -->
                            <a-card
                                title="波形显示"
                                size="small"
                                style="height: 500px"
                            >
                                <div
                                    style="
                                        height: 100%;
                                        display: flex;
                                        align-items: center;
                                        justify-content: center;
                                    "
                                >
                                    <a-empty
                                        description="ECharts 波形图开发中..."
                                    />
                                </div>
                            </a-card>

                            <!-- 闸门结果显示 -->
                            <a-card title="闸门计算结果" size="small">
                                <a-empty description="结果显示组件开发中..." />
                            </a-card>
                        </a-space>
                    </a-layout-content>
                </a-layout>
            </main>

            <!-- 测试界面 -->
            <main class="app-main" v-if="showTest">
                <WaveformChartTest />
            </main>
        </div>
    </a-config-provider>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import { storeToRefs } from 'pinia';
import { useAcquisitionStore } from './stores/acquisitionStore';
import WaveformChartTest from './components/WaveformChartTest.vue';

const acquisitionStore = useAcquisitionStore();
const { isConnected, isAcquiring } = storeToRefs(acquisitionStore);
const showTest = ref(true);

onMounted(async () => {
    await acquisitionStore.connect();
});

onUnmounted(async () => {
    await acquisitionStore.disconnect();
});
</script>

<style scoped>
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

.app-container {
    width: 100vw;
    height: 100vh;
    display: flex;
    flex-direction: column;
    background: #f5f5f5;
}

.app-header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 16px 24px;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.header-content {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.header-content h1 {
    margin: 0;
    font-size: 20px;
    font-weight: 500;
    color: white;
}

.app-main {
    flex: 1;
    overflow: hidden;
}
</style>
