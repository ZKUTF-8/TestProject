namespace LC8172.WpfHost.Models;

/// <summary>
/// 闸门计算结果
/// ⚠️ 修改此类时，同步更新前端：src/LC8172.WebUI/src/types/models.ts
/// </summary>
public class GateResult
{
    /// <summary>闸门ID</summary>
    public string GateId { get; set; } = string.Empty;

    /// <summary>闸门名称</summary>
    public string GateName { get; set; } = string.Empty;

    /// <summary>上升沿是否找到</summary>
    public bool RisingEdgeFound { get; set; }

    /// <summary>上升沿电压值</summary>
    public double RisingEdgeValue { get; set; }

    /// <summary>上升沿时间（TOF）</summary>
    public double RisingEdgeTime { get; set; }

    /// <summary>下降沿是否找到</summary>
    public bool FallingEdgeFound { get; set; }

    /// <summary>下降沿电压值</summary>
    public double FallingEdgeValue { get; set; }

    /// <summary>下降沿时间（TOF）</summary>
    public double FallingEdgeTime { get; set; }

    /// <summary>正峰值是否找到（PPA - Positive Peak Amplitude）</summary>
    public bool PPAFound { get; set; }

    /// <summary>正峰值电压</summary>
    public double PPAValue { get; set; }

    /// <summary>正峰值时间（TOF）</summary>
    public double PPATime { get; set; }

    /// <summary>负峰值是否找到（NPA - Negative Peak Amplitude）</summary>
    public bool NPAFound { get; set; }

    /// <summary>负峰值电压</summary>
    public double NPAValue { get; set; }

    /// <summary>负峰值时间（TOF）</summary>
    public double NPATime { get; set; }

    /// <summary>子闸门结果列表</summary>
    public List<GateResult> ChildResults { get; set; } = [];
}

