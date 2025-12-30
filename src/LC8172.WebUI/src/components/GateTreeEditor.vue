<template>
    <div class="gate-tree-editor">
        <div class="editor-header">
            <a-space>
                <a-button type="primary" size="small" @click="addRootGate">
                    <template #icon>
                        <PlusOutlined />
                    </template>
                    添加根闸门
                </a-button>
                <a-button size="small" @click="expandAll">
                    展开全部
                </a-button>
                <a-button size="small" @click="collapseAll">
                    收起全部
                </a-button>
            </a-space>
        </div>

        <a-divider style="margin: 12px 0" />

        <div class="tree-content">
            <a-tree
                v-model:expandedKeys="expandedKeys"
                v-model:selectedKeys="selectedKeys"
                :tree-data="treeData"
                :show-line="true"
                :show-icon="false"
                @select="onSelectGate"
            >
                <template #title="{ title, key }">
                    <div class="tree-node-title">
                        <span>{{ title }}</span>
                        <a-space :size="4" class="node-actions">
                            <a-button
                                type="text"
                                size="small"
                                @click.stop="editGate(key as string)"
                            >
                                <EditOutlined />
                            </a-button>
                            <a-button
                                type="text"
                                size="small"
                                danger
                                @click.stop="deleteGate(key as string)"
                            >
                                <DeleteOutlined />
                            </a-button>
                        </a-space>
                    </div>
                </template>
            </a-tree>
        </div>

        <!-- 编辑闸门对话框 -->
        <a-modal
            v-model:open="editModalVisible"
            title="编辑闸门"
            @ok="saveGate"
            @cancel="cancelEdit"
        >
            <a-form :model="editingGate" layout="vertical">
                <a-form-item label="闸门名称">
                    <a-input v-model:value="editingGate.name" />
                </a-form-item>

                <a-form-item label="闸门类型">
                    <a-select v-model:value="editingGate.type">
                        <a-select-option :value="0">Root</a-select-option>
                        <a-select-option :value="1">Rising</a-select-option>
                        <a-select-option :value="2">Falling</a-select-option>
                        <a-select-option :value="3">Data</a-select-option>
                    </a-select>
                </a-form-item>

                <a-row :gutter="16">
                    <a-col :span="12">
                        <a-form-item label="起始时间 (μs)">
                            <a-input-number
                                v-model:value="editingGate.start"
                                :min="0"
                                :step="0.001"
                                style="width: 100%"
                            />
                        </a-form-item>
                    </a-col>
                    <a-col :span="12">
                        <a-form-item label="长度 (μs)">
                            <a-input-number
                                v-model:value="editingGate.length"
                                :min="0.001"
                                :step="0.001"
                                style="width: 100%"
                            />
                        </a-form-item>
                    </a-col>
                </a-row>

                <a-form-item label="阈值 (V)">
                    <a-input-number
                        v-model:value="editingGate.threshold"
                        :step="0.01"
                        style="width: 100%"
                    />
                </a-form-item>

                <a-form-item label="颜色">
                    <input
                        type="color"
                        v-model="editingGate.color"
                        style="width: 100%; height: 32px"
                    />
                </a-form-item>

                <a-form-item label="通道号">
                    <a-input-number
                        v-model:value="editingGate.channel"
                        :min="1"
                        :max="16"
                        style="width: 100%"
                    />
                </a-form-item>
            </a-form>
        </a-modal>
    </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';
import {
    PlusOutlined,
    EditOutlined,
    DeleteOutlined
} from '@ant-design/icons-vue';
import type { GateConfig } from '@/types/models';
import { GateType } from '@/types/models';

// ==================== Props ====================
interface Props {
    gates: GateConfig[];
}

const props = defineProps<Props>();

// ==================== Emits ====================
const emit = defineEmits<{
    update: [gates: GateConfig[]];
    select: [gate: GateConfig | null];
}>();

// ==================== State ====================
const expandedKeys = ref<string[]>([]);
const selectedKeys = ref<string[]>([]);
const editModalVisible = ref(false);
const editingGate = ref<GateConfig>({
    id: '',
    name: '',
    start: 0,
    length: 10,
    threshold: 0,
    type: GateType.Root,
    color: '#FFFF00',
    channel: 1,
    children: []
});

// ==================== Computed ====================

/**
 * 转换为 Ant Design Tree 数据格式
 */
const treeData = computed(() => {
    return convertToTreeData(props.gates);
});

/**
 * 递归转换闸门数据为树形结构
 */
function convertToTreeData(gates: GateConfig[]): any[] {
    return gates.map((gate) => ({
        key: gate.id,
        title: `${gate.name} (${gate.type})`,
        children:
            gate.children && gate.children.length > 0
                ? convertToTreeData(gate.children)
                : undefined
    }));
}

// ==================== Methods ====================

/**
 * 添加根闸门
 */
function addRootGate() {
    editingGate.value = {
        id: Date.now().toString(),
        name: 'Absolute',
        start: 0,
        length: 50,
        threshold: 0,
        type: GateType.Root,
        color: '#00FF00',
        channel: 1,
        children: []
    };
    editModalVisible.value = true;
}

/**
 * 编辑闸门
 */
function editGate(gateId: string) {
    const gate = findGateById(props.gates, gateId);
    if (gate) {
        editingGate.value = { ...gate };
        editModalVisible.value = true;
    }
}

/**
 * 删除闸门
 */
function deleteGate(gateId: string) {
    const newGates = removeGateById(props.gates, gateId);
    emit('update', newGates);
}

/**
 * 保存闸门
 */
function saveGate() {
    // TODO: 实现保存逻辑
    editModalVisible.value = false;
    emit('update', props.gates);
}

/**
 * 取消编辑
 */
function cancelEdit() {
    editModalVisible.value = false;
}

/**
 * 选中闸门
 */
function onSelectGate(selectedKeys: string[]) {
    if (selectedKeys.length > 0 && selectedKeys[0]) {
        const gate = findGateById(props.gates, selectedKeys[0]);
        emit('select', gate || null);
    } else {
        emit('select', null);
    }
}

/**
 * 展开全部
 */
function expandAll() {
    expandedKeys.value = getAllKeys(props.gates);
}

/**
 * 收起全部
 */
function collapseAll() {
    expandedKeys.value = [];
}

/**
 * 获取所有节点的key
 */
function getAllKeys(gates: GateConfig[]): string[] {
    const keys: string[] = [];
    gates.forEach((gate) => {
        keys.push(gate.id);
        if (gate.children && gate.children.length > 0) {
            keys.push(...getAllKeys(gate.children));
        }
    });
    return keys;
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
 * 递归删除闸门
 */
function removeGateById(gates: GateConfig[], id: string): GateConfig[] {
    return gates
        .filter((gate) => gate.id !== id)
        .map((gate) => ({
            ...gate,
            children: removeGateById(gate.children, id)
        }));
}
</script>

<style scoped>
.gate-tree-editor {
    height: 100%;
    display: flex;
    flex-direction: column;
}

.editor-header {
    padding: 8px 0;
}

.tree-content {
    flex: 1;
    overflow-y: auto;
}

.tree-node-title {
    display: flex;
    justify-content: space-between;
    align-items: center;
    width: 100%;
}

.node-actions {
    opacity: 0;
    transition: opacity 0.2s;
}

.tree-node-title:hover .node-actions {
    opacity: 1;
}
</style>

