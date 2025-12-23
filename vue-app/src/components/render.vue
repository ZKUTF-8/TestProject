<template>
    <div class="form_item">
        <slot name="label-input" />
        <div :id="domeID" class="canvas" style="width: 100%; height: 100%" @contextmenu="onContextmenu" />
    </div>
</template>

<script setup>
import { onMounted, watch, defineModel, defineProps, onBeforeUnmount } from 'vue'
import { lightningChart, emptyFill, Themes, LegendBoxBuilders } from '@lightningchart/lcjs'
import { nanoid } from 'nanoid'

const param = defineProps({ config: Object, hidden: Boolean, disabled: Boolean })

const emit = defineEmits(['event'])

function onContextmenu($event) {
    emit('contextmenu', $event)
}
function onEvent(eventType, $event) {
    emit('event', eventType, $event)
}

// let lc = lightningChart({
//     license:
//         '0002-n/X8/32N46BkrWb9wJ1FF88Jy0h8JwD0/P1plemwtDO/8iFxGtXV2ktvhOEkc+pqj5W5BnhPvaobCGEQZE8=-MEUCIQCZiQhNP4C0hfnk9HZm9RTgaA61CgPFcdER0kPjaCf+qQIgLeZ/5wp5H852W+OpVa8ZpOb+r4YZP9T1t4KQaZA509M=',
//     licenseInformation: {
//         appTitle: 'ETest',
//         company: 'Beijing Kiyun Hengda Technology Co., Ltd.'
//     },
//     // LightningChartOptions
//     overrideInteractionMouseButtons: {
//         chartXYPanMouseButton: 0, // LMB
//         chartXYRectangleZoomFitMouseButton: 2, // RMB
//         axisXYPanMouseButton: 0, // LMB
//         axisXYZoomMouseButton: 2 // RMB
//     }
// })

let lc = null

// const domeID = new Date().getTime().toString()
const domeID = nanoid(8)

let chart
let lineSeries = []
let legend = null

function InitCanvas() {
    const { layout, settings } = param.config.uiConfig
    if (!lc) {
        lc = lightningChart({
            license:
                '0002-n/X8/32N46BkrWb9wJ1FF88Jy0h8JwD0/P1plemwtDO/8iFxGtXV2ktvhOEkc+pqj5W5BnhPvaobCGEQZE8=-MEUCIQCZiQhNP4C0hfnk9HZm9RTgaA61CgPFcdER0kPjaCf+qQIgLeZ/5wp5H852W+OpVa8ZpOb+r4YZP9T1t4KQaZA509M=',
            licenseInformation: {
                appTitle: 'ETest',
                company: 'Beijing Kiyun Hengda Technology Co., Ltd.'
            },
            // LightningChartOptions
            overrideInteractionMouseButtons: {
                chartXYPanMouseButton: 0, // LMB
                chartXYRectangleZoomFitMouseButton: 2, // RMB
                axisXYPanMouseButton: 0, // LMB
                axisXYZoomMouseButton: 2 // RMB
            }
        })
    }
    chart = lc
        .ChartXY({
            container: domeID,
            theme: Themes[layout.theme], // theme, //Themes.darkGold,
            animationsEnabled: false
        })
        .setTitle(`${layout.title.text}`)
    // .setCursor((cursor) => cursor.setTickMarkerYVisible(false).setGridStrokeYStyle(emptyLine))
    // debugger
    chart.axisX.setTitle(layout.xaxis.title.text)
    chart.axisY.setTitle(layout.yaxis.title.text)

    // Remove default "zoom out" interaction, activated when dragging with right mouse button
    chart.setMouseInteractionRectangleFit(false)

    // Fit data to view when user double clicks on series area
    chart.onSeriesBackgroundMouseDoubleClick(() => {
        chart.forEachAxis((axis) => axis.fit())
    })

    legend = chart.addLegendBox(LegendBoxBuilders.HorizontalLegendBox).setTitle('通道').setAutoDispose({
        type: 'max-width',
        maxWidth: 0.3
    })
    legend.setAutoDispose(false)
    // const legend = chart.addLegendBox(LegendBoxBuilders.VerticalLegendBox).setTitle('通道')
    // 初始化线条时跳过X轴通道
    const xAxisChannel = settings.xAxisChannel
    let lineIndex = 0
    settings.line.forEach((item, index) => {
        // 如果当前通道是X轴数据源，跳过
        if (xAxisChannel !== undefined && xAxisChannel !== null && index === xAxisChannel) {
            return
        }
        addLine(lineIndex, item.name)
        lineIndex++
    })
}

function addLine(key, name) {
    let lineSerie = chart
        .addPointLineAreaSeries({ dataPattern: 'ProgressiveX' })
        // .setStrokeStyle((stroke) => stroke.setThickness(1))
        .setAreaFillStyle(emptyFill)
        .setName(name || 'CH' + key)

    if (!legend) {
        legend = chart.addLegendBox(LegendBoxBuilders.HorizontalLegendBox).setTitle('通道').setAutoDispose({
            type: 'max-width',
            maxWidth: 0.3
        })
        legend.setAutoDispose(false)
    }
    legend.add(lineSerie)
    lineSeries[key] = lineSerie
    return lineSerie
}

// FPS 计算
let frameCount = 0
let lastTime = performance.now()
let fps = 0

// DPS 计算
let dps = 0
let totalDataPoints = 0 // 累计数据点数

import { formatNum } from '#utils'
function fpsAndDps() {
    const { layout } = param.config.uiConfig
    const now = performance.now()
    // 计算 FPS
    frameCount++
    if (now - lastTime >= 1000) {
        // 每秒更新一次
        fps = frameCount
        frameCount = 0
        lastTime = now

        // 计算 DPS
        dps = totalDataPoints
        totalDataPoints = 0

        // 更新标题
        chart.setTitle(`${layout.title.text} (FPS: ${fps}, DPS: ${formatNum(dps)})`)
    }
}

setInterval(() => {
    if (performance.now() - lastTime > 1300) {
        const { layout } = param.config.uiConfig
        chart.setTitle(`${layout.title.text} (FPS: 0, DPS: 0)`)
    }
}, 100)

function updateData(channelPoints) {
    if (resetLegend) {
        lineSeries = []
        legend.dispose()
        legend = null
        resetLegend = false
    }

    const { settings } = param.config.uiConfig
    const xAxisChannel = settings.xAxisChannel
    // 获取X轴数据（如果设置了X轴通道）
    const xAxisData = xAxisChannel !== undefined && xAxisChannel !== null 
        ? channelPoints[xAxisChannel] 
        : null

    // 用于记录实际的Y轴线条索引（跳过X轴通道）
    let lineIndex = 0
    
    for (const index in channelPoints) {
        const channelIndex = parseInt(index)
        
        // 如果当前通道是X轴数据源，跳过不显示为Y轴曲线
        if (xAxisChannel !== undefined && xAxisChannel !== null && channelIndex === xAxisChannel) {
            continue
        }

        let lineSerie = lineSeries[lineIndex]
        if (!lineSerie && settings.autoAppendLine) {
            // 获取通道名称，如果配置了line数组则使用配置的名称
            const lineName = settings.line && settings.line[channelIndex] 
                ? settings.line[channelIndex].name 
                : 'CH' + channelIndex
            lineSerie = addLine(lineIndex, lineName)
        }

        if (!lineSerie) {
            lineIndex++
            continue
        }

        if (!settings.isAppendData) {
            lineSerie.clear()
        }

        // 根据是否有X轴数据选择不同的添加方式
        if (xAxisData && xAxisData.length > 0) {
            // 使用指定通道作为X轴数据
            const yValues = channelPoints[index]
            const minLen = Math.min(xAxisData.length, yValues.length)
            // debugger
            // 构建 {x, y} 数据点数组
            const points = []
            for (let i = 0; i < minLen; i++) {
                points.push({ x: xAxisData[i], y: yValues[i] })
            }
            lineSerie.appendJSON(points)
            
            // 记录数据点数
            totalDataPoints += minLen
        } else {
            // 使用默认的步进方式（索引作为X轴）
            lineSerie.appendSamples({
                yValues: channelPoints[index],
                // Optional, defaults to 1
                step: 1
            })

            // 记录数据点数
            totalDataPoints += channelPoints[index].length
        }
        
        lineIndex++
    }
    fpsAndDps()
}

async function dispose() {
    for (const lineSerie of lineSeries) {
        lineSerie.clear()
        lineSerie.dispose()
    }
    lineSeries = []
    await chart?.dispose()
    lc?.dispose()
}

const value = defineModel()

import { MsArray } from '@/utils/MsArray'
watch(value, (value) => {
    const msArray = new MsArray({
        data: value?.data ?? [],
        shape: value.shape ?? [],
        strides: value?.strides ?? [],
        count: value?.count ?? 0,
        elementType: value?.elementType ?? ''
    })
    const rows = msArray.getFormattedData()
    updateData(rows)
})

// 重新创建LightningChart实例的通用函数
async function recreateChart() {
    await dispose()
    lc = lightningChart({
        license:
            '0002-n/X8/32N46BkrWb9wJ1FF88Jy0h8JwD0/P1plemwtDO/8iFxGtXV2ktvhOEkc+pqj5W5BnhPvaobCGEQZE8=-MEUCIQCZiQhNP4C0hfnk9HZm9RTgaA61CgPFcdER0kPjaCf+qQIgLeZ/5wp5H852W+OpVa8ZpOb+r4YZP9T1t4KQaZA509M=',
        licenseInformation: {
            appTitle: 'ETest',
            company: 'Beijing Kiyun Hengda Technology Co., Ltd.'
        },
        // LightningChartOptions
        overrideInteractionMouseButtons: {
            chartXYPanMouseButton: 0, // LMB
            chartXYRectangleZoomFitMouseButton: 2, // RMB
            axisXYPanMouseButton: 0, // LMB
            axisXYZoomMouseButton: 2 // RMB
        }
    })
    InitCanvas()
}

watch(param.config.uiConfig.layout, async () => {
    await recreateChart()
})

// 监听X轴通道设置变化，重新初始化图表
watch(
    () => param.config.uiConfig.settings.xAxisChannel,
    async () => {
        await recreateChart()
    }
)

let resetLegend = false
onMounted((_) => {
    InitCanvas()
    resetLegend = true
})

onBeforeUnmount(async (_) => {
    await dispose()
})
</script>
