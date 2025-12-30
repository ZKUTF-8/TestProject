namespace LC8172.WpfHost.Models;

/// <summary>
/// 闸门类型
/// </summary>
public enum GateType
{
    /// <summary>根闸门（整个采集窗口）</summary>
    Root = 0,

    /// <summary>上升沿检测</summary>
    Rising = 1,

    /// <summary>下降沿检测</summary>
    Falling = 2,

    /// <summary>数据门（寻峰）</summary>
    Data = 3
}

/// <summary>
/// 闸门配置
/// ⚠️ 修改此类时，同步更新前端：src/LC8172.WebUI/src/types/models.ts
/// </summary>
public class GateConfig
{
    /// <summary>唯一标识</summary>
    public string Id { get; set; } = Guid.NewGuid().ToString();

    /// <summary>闸门名称</summary>
    public string Name { get; set; } = string.Empty;

    /// <summary>起始时间（相对父闸门，单位：微秒）</summary>
    public double Start { get; set; }

    /// <summary>闸门长度（单位：微秒）</summary>
    public double Length { get; set; }

    /// <summary>阈值电压（单位：伏特，用于边沿检测）</summary>
    public double Threshold { get; set; }

    /// <summary>闸门类型</summary>
    public GateType Type { get; set; }

    /// <summary>显示颜色（HEX格式，如 #FFFF00）</summary>
    public string Color { get; set; } = "#FFFF00";

    /// <summary>应用的采集通道号</summary>
    public int Channel { get; set; }

    /// <summary>父闸门ID（可选）</summary>
    public string? ParentId { get; set; }

    /// <summary>子闸门列表</summary>
    public List<GateConfig> Children { get; set; } = [];
}

