<template>
    <div class="gate-results">
        <div v-if="!results || results.length === 0" class="empty-state">
            <a-empty description="暂无闸门计算结果" />
        </div>

        <div v-else class="results-list">
            <div
                v-for="result in results"
                :key="result.gateId"
                class="result-item"
            >
                <div class="result-header">
                    <h4>{{ result.gateName }}</h4>
                </div>

                <div class="result-content">
                    <!-- 上升沿结果 -->
                    <div v-if="result.risingEdgeFound" class="result-row">
                        <span class="label">Rising Edge:</span>
                        <a-tag color="green">Found</a-tag>
                        <span class="value"
                            >Val={{ result.risingEdgeValue.toFixed(3) }} V,
                            TOF={{ result.risingEdgeTime.toFixed(3) }} μs</span
                        >
                    </div>
                    <div v-else class="result-row">
                        <span class="label">Rising Edge:</span>
                        <a-tag color="error">Not Found</a-tag>
                    </div>

                    <!-- 下降沿结果 -->
                    <div v-if="result.fallingEdgeFound" class="result-row">
                        <span class="label">Falling Edge:</span>
                        <a-tag color="green">Found</a-tag>
                        <span class="value"
                            >Val={{ result.fallingEdgeValue.toFixed(3) }} V,
                            TOF={{ result.fallingEdgeTime.toFixed(3) }} μs</span
                        >
                    </div>
                    <div v-else class="result-row">
                        <span class="label">Falling Edge:</span>
                        <a-tag color="error">Not Found</a-tag>
                    </div>

                    <!-- 正峰值结果 -->
                    <div v-if="result.ppaFound" class="result-row">
                        <span class="label">PPA:</span>
                        <a-tag color="green">Found</a-tag>
                        <span class="value"
                            >Val={{ result.ppaValue.toFixed(3) }} V, TOF={{
                                result.ppaTime.toFixed(3)
                            }}
                            μs</span
                        >
                    </div>

                    <!-- 负峰值结果 -->
                    <div v-if="result.npaFound" class="result-row">
                        <span class="label">NPA:</span>
                        <a-tag color="green">Found</a-tag>
                        <span class="value"
                            >Val={{ result.npaValue.toFixed(3) }} V, TOF={{
                                result.npaTime.toFixed(3)
                            }}
                            μs</span
                        >
                    </div>
                </div>

                <!-- 子闸门结果（递归） -->
                <div v-if="result.childResults && result.childResults.length > 0" class="child-results">
                    <gate-results :results="result.childResults" />
                </div>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import type { GateResult } from '@/types/models';

// ==================== Props ====================
interface Props {
    results?: GateResult[];
}

defineProps<Props>();
</script>

<style scoped>
.gate-results {
    width: 100%;
}

.empty-state {
    padding: 40px 0;
    text-align: center;
}

.results-list {
    display: flex;
    flex-direction: column;
    gap: 16px;
}

.result-item {
    padding: 16px;
    background: #fafafa;
    border-radius: 4px;
    border-left: 3px solid #1890ff;
}

.result-header h4 {
    margin: 0 0 12px 0;
    font-size: 16px;
    font-weight: bold;
    color: #1890ff;
}

.result-content {
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.result-row {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: 14px;
}

.result-row .label {
    font-weight: 500;
    min-width: 110px;
    color: #666;
}

.result-row .value {
    color: #333;
    font-family: 'Consolas', 'Monaco', monospace;
}

.child-results {
    margin-top: 12px;
    padding-left: 20px;
    border-left: 2px dashed #d9d9d9;
}
</style>

