namespace LC8172.WpfHost.Models;

/// <summary>
/// 波形数据
/// ⚠️ 修改此类时，同步更新前端：src/LC8172.WebUI/src/types/models.ts
/// </summary>
public class WaveformData
{
    /// <summary>数据点列表 [[x1, y1], [x2, y2], ...]</summary>
    public List<double[]> Points { get; set; } = [];

    /// <summary>采样率（单位：MHz）</summary>
    public double SampleRate { get; set; }

    /// <summary>采集通道号</summary>
    public int Channel { get; set; }

    /// <summary>时间戳</summary>
    public DateTime Timestamp { get; set; } = DateTime.Now;
}

