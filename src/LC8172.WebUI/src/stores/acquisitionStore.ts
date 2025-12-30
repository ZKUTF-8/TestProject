import { defineStore } from 'pinia';
import { ref } from 'vue';
import * as signalR from '@microsoft/signalr';
import type {
    GateConfig,
    WaveformData,
    GateResult,
    AcquisitionConfig
} from '@/types/models';

export const useAcquisitionStore = defineStore('acquisition', () => {
    // ==================== 状态 ====================
    const connection = ref<signalR.HubConnection | null>(null);
    const isConnected = ref(false);
    const isAcquiring = ref(false);
    const waveformData = ref<WaveformData | null>(null);
    const gateResults = ref<GateResult[]>([]);
    const errorMessage = ref('');

    // ==================== 连接管理 ====================
    
    async function connect() {
        if (connection.value) {
            console.warn('SignalR 已连接，跳过重复连接');
            return;
        }

        try {
            connection.value = new signalR.HubConnectionBuilder()
                .withUrl('http://localhost:5000/acquisitionHub')
                .withAutomaticReconnect()
                .configureLogging(signalR.LogLevel.Information)
                .build();

            registerEventHandlers();

            await connection.value.start();
            isConnected.value = true;
            console.log('✅ SignalR 连接成功');
        } catch (err) {
            isConnected.value = false;
            errorMessage.value = `SignalR 连接失败: ${err}`;
            console.error('❌ SignalR 连接失败:', err);
        }
    }

    async function disconnect() {
        if (connection.value) {
            await connection.value.stop();
            connection.value = null;
            isConnected.value = false;
            console.log('SignalR 已断开');
        }
    }

    function registerEventHandlers() {
        if (!connection.value) return;

        connection.value.on('WaveformData', (data: WaveformData) => {
            waveformData.value = data;
            console.log('📊 收到波形数据:', data.points.length, '个点');
        });

        connection.value.on('WaveformChunk', (chunk: number[][]) => {
            if (waveformData.value) {
                waveformData.value.points.push(...chunk);
                const maxPoints = 50000;
                if (waveformData.value.points.length > maxPoints) {
                    waveformData.value.points = waveformData.value.points.slice(-maxPoints);
                }
            }
        });

        connection.value.on('AcquisitionStopped', () => {
            isAcquiring.value = false;
            console.log('⏹️ 采集已停止');
        });

        connection.value.on('GatesConfigured', (gates: GateConfig[]) => {
            console.log('✅ 闸门配置成功:', gates.length, '个闸门');
        });

        connection.value.on('ReceiveMessage', (message: string) => {
            console.log('💬 服务器消息:', message);
        });

        connection.value.on('GateResultsUpdated', (results: GateResult[]) => {
            gateResults.value = results;
            console.log('📈 闸门结果更新');
        });

        connection.value.onreconnecting(() => {
            isConnected.value = false;
            console.warn('🔄 SignalR 重新连接中...');
        });

        connection.value.onreconnected(() => {
            isConnected.value = true;
            console.log('✅ SignalR 重新连接成功');
        });

        connection.value.onclose(() => {
            isConnected.value = false;
            console.error('❌ SignalR 连接关闭');
        });
    }

    // ==================== 采集控制方法 ====================

    async function setGates(gates: GateConfig[]) {
        if (!connection.value || !isConnected.value) {
            throw new Error('SignalR 未连接');
        }

        try {
            await connection.value.invoke('SetGates', gates);
            console.log('📤 发送闸门配置');
        } catch (err) {
            errorMessage.value = `配置闸门失败: ${err}`;
            console.error('❌ 配置闸门失败:', err);
            throw err;
        }
    }

    async function startAcquisition(config: AcquisitionConfig) {
        if (!connection.value || !isConnected.value) {
            throw new Error('SignalR 未连接');
        }

        try {
            isAcquiring.value = true;
            const data = await connection.value.invoke<WaveformData>(
                'StartAcquisition',
                config.channel,
                config.sampleRate
            );
            waveformData.value = data;
            console.log('▶️ 采集已启动');
            return data;
        } catch (err) {
            isAcquiring.value = false;
            errorMessage.value = `启动采集失败: ${err}`;
            console.error('❌ 启动采集失败:', err);
            throw err;
        }
    }

    async function stopAcquisition() {
        if (!connection.value || !isConnected.value) {
            throw new Error('SignalR 未连接');
        }

        try {
            await connection.value.invoke('StopAcquisition');
            isAcquiring.value = false;
            console.log('⏹️ 采集已停止');
        } catch (err) {
            errorMessage.value = `停止采集失败: ${err}`;
            console.error('❌ 停止采集失败:', err);
            throw err;
        }
    }

    async function fetchGateResults() {
        if (!connection.value || !isConnected.value) {
            throw new Error('SignalR 未连接');
        }

        try {
            const results = await connection.value.invoke<GateResult[]>('GetGateResults');
            gateResults.value = results;
            console.log('📊 获取闸门结果成功');
            return results;
        } catch (err) {
            errorMessage.value = `获取闸门结果失败: ${err}`;
            console.error('❌ 获取闸门结果失败:', err);
            throw err;
        }
    }

    // ==================== 返回 ====================
    return {
        // 状态
        isConnected,
        isAcquiring,
        waveformData,
        gateResults,
        errorMessage,

        // 方法
        connect,
        disconnect,
        setGates,
        startAcquisition,
        stopAcquisition,
        fetchGateResults
    };
});

