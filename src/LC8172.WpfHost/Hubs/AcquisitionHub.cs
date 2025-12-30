using Microsoft.AspNetCore.SignalR;
using System.Diagnostics;
using LC8172.WpfHost.Models;

namespace LC8172.WpfHost.Hubs;

/// <summary>
/// 采集卡通信Hub
/// 提供前端与采集卡之间的双向通信接口
/// </summary>
public class AcquisitionHub : Hub
{
    private static readonly Stopwatch _stopwatch = Stopwatch.StartNew();
    private static readonly Dictionary<string, CancellationTokenSource> _streamingClients = [];
    private static readonly Random _random = new();

    // ==================== 核心采集功能 ====================

    /// <summary>
    /// 配置闸门参数
    /// </summary>
    /// <param name="gates">闸门配置列表</param>
    public async Task SetGates(List<GateConfig> gates)
    {
        try
        {
            // TODO: 调用采集卡DLL，配置闸门参数
            // 示例：AcquisitionCard.SetGates(gates);

            Console.WriteLine($"收到闸门配置，共 {gates.Count} 个闸门");
            foreach (var gate in gates)
            {
                Console.WriteLine($"  - {gate.Name}: Type={gate.Type}, Start={gate.Start}μs, Length={gate.Length}μs");
            }

            // 通知所有客户端闸门已配置
            await Clients.All.SendAsync("GatesConfigured", gates);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"配置闸门失败: {ex.Message}");
            throw;
        }
    }

    /// <summary>
    /// 开始数据采集
    /// </summary>
    /// <param name="channel">采集通道号</param>
    /// <param name="sampleRate">采样率（MHz）</param>
    public async Task<WaveformData> StartAcquisition(int channel, double sampleRate)
    {
        try
        {
            Console.WriteLine($"开始采集: 通道={channel}, 采样率={sampleRate}MHz");

            // TODO: 调用采集卡DLL，启动采集
            // 示例：var data = AcquisitionCard.StartAcquisition(channel, sampleRate);

            // 模拟波形数据
            var waveformData = GenerateMockWaveform(channel, sampleRate);

            // 推送波形数据给调用者
            await Clients.Caller.SendAsync("WaveformData", waveformData);

            return waveformData;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"采集失败: {ex.Message}");
            throw;
        }
    }

    /// <summary>
    /// 停止数据采集
    /// </summary>
    public async Task StopAcquisition()
    {
        try
        {
            Console.WriteLine("停止采集");

            // TODO: 调用采集卡DLL，停止采集
            // 示例：AcquisitionCard.StopAcquisition();

            // 通知所有客户端采集已停止
            await Clients.All.SendAsync("AcquisitionStopped");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"停止采集失败: {ex.Message}");
            throw;
        }
    }

    /// <summary>
    /// 获取闸门计算结果
    /// </summary>
    public async Task<List<GateResult>> GetGateResults()
    {
        try
        {
            // TODO: 从采集卡获取闸门计算结果
            // 示例：var results = AcquisitionCard.GetGateResults();

            // 模拟结果数据
            var results = GenerateMockGateResults();

            return await Task.FromResult(results);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"获取闸门结果失败: {ex.Message}");
            throw;
        }
    }

    // ==================== 辅助方法 ====================

    /// <summary>
    /// 生成模拟波形数据（用于测试）
    /// </summary>
    private WaveformData GenerateMockWaveform(int channel, double sampleRate)
    {
        List<double[]> points = [];
        int pointCount = 10000;
        double frequency = 1.0; // 1MHz

        for (int i = 0; i < pointCount; i++)
        {
            double time = i / sampleRate; // 微秒
            double value = Math.Sin(2 * Math.PI * frequency * time) +
                          (_random.NextDouble() - 0.5) * 0.1; // 添加噪声
            points.Add(new[] { time, value });
        }

        return new WaveformData
        {
            Points = points,
            SampleRate = sampleRate,
            Channel = channel,
            Timestamp = DateTime.Now
        };
    }

    /// <summary>
    /// 生成模拟闸门结果（用于测试）
    /// </summary>
    private List<GateResult> GenerateMockGateResults()
    {
        return
        [
            new()
                {
                    GateId = "fsf",
                    GateName = "FSF",
                    RisingEdgeFound = true,
                    RisingEdgeValue = 0.366,
                    RisingEdgeTime = 2.500,
                    FallingEdgeFound = false
                },
                new()
                {
                    GateId = "data1",
                    GateName = "Data1",
                    PPAFound = true,
                    PPAValue = 0.496,
                    PPATime = 6.132,
                    NPAFound = true,
                    NPAValue = -0.496,
                    NPATime = 8.727
                }
        ];
    }

    // ==================== 测试用方法（保留用于性能测试） ====================

    /// <summary>
    /// 客户端连接时触发
    /// </summary>
    public override async Task OnConnectedAsync()
    {
        await base.OnConnectedAsync();
        // 连接成功后主动推送欢迎消息
        await Clients.Caller.SendAsync("ReceiveMessage", $"欢迎连接！连接ID: {Context.ConnectionId}");
    }

    /// <summary>
    /// 获取当前服务器时间（精确到微秒）
    /// </summary>
    public async Task<string> GetCurrentTime()
    {
        var now = DateTime.Now;
        var microseconds = _stopwatch.Elapsed.TotalMilliseconds % 1 * 1000;
        return await Task.FromResult($"{now:yyyy-MM-dd HH:mm:ss.fff}.{microseconds:000}");
    }

    /// <summary>
    /// 测试延迟：返回高精度时间戳（微秒级）
    /// </summary>
    public async Task<double> PingTest(double clientTimestamp)
    {
        // 返回服务器的高精度时间戳（单位：毫秒，带小数表示微秒）
        return await Task.FromResult(_stopwatch.Elapsed.TotalMilliseconds);
    }

    /// <summary>
    /// 服务端主动推送消息给所有客户端
    /// </summary>
    public async Task BroadcastMessage(string message)
    {
        await Clients.All.SendAsync("ReceiveMessage", message);
    }

    /// <summary>
    /// 吞吐量测试：简单的回显方法
    /// </summary>
    public async Task<bool> ThroughputTest()
    {
        return await Task.FromResult(true);
    }

    /// <summary>
    /// 带宽测试：返回指定大小的数据（单位：MB）
    /// </summary>
    public async Task<byte[]> BandwidthTest(int sizeInMB)
    {
        // 生成指定大小的数据
        int sizeInBytes = sizeInMB * 1024 * 1024;
        byte[] data = new byte[sizeInBytes];

        // 填充一些数据（避免压缩）
        Random random = new Random();
        random.NextBytes(data);

        return await Task.FromResult(data);
    }

    /// <summary>
    /// 启动实时数据流（2通道 x 30000点）
    /// </summary>
    public async Task StartStreaming()
    {
        var connectionId = Context.ConnectionId;

        // 如果已经在流式传输，先停止
        if (_streamingClients.ContainsKey(connectionId))
        {
            await StopStreaming();
        }

        var cts = new CancellationTokenSource();
        _streamingClients[connectionId] = cts;

        // 提前捕获客户端代理，避免在后台任务中访问已释放的 Hub 实例
        var client = Clients.Client(connectionId);

        // 在后台任务中持续发送数据
        _ = Task.Run(async () =>
        {
            try
            {
                while (!cts.Token.IsCancellationRequested)
                {
                    // 生成两个通道的数据（每通道300点）
                    var channel1 = GenerateWaveform(30000, 1.0, 1.0);
                    var channel2 = GenerateWaveform(30000, 2.0, 1.0);

                    // 使用捕获的客户端代理发送数据
                    await client.SendAsync("StreamData", new
                    {
                        channel1,
                        channel2
                    }, cts.Token);

                    // 让出 CPU 时间片，避免 100% CPU 占用，但不主动睡眠
                    await Task.Yield();
                }
            }
            catch (OperationCanceledException)
            {
                // 正常取消
            }
            catch (Exception ex)
            {
                Console.WriteLine($"流式传输错误: {ex.Message}");
            }
            finally
            {
                _streamingClients.Remove(connectionId);
            }
        }, cts.Token);

        await Task.CompletedTask;
    }

    /// <summary>
    /// 停止实时数据流
    /// </summary>
    public async Task StopStreaming()
    {
        var connectionId = Context.ConnectionId;

        if (_streamingClients.TryGetValue(connectionId, out var cts))
        {
            cts.Cancel();
            _streamingClients.Remove(connectionId);
        }

        await Task.CompletedTask;
    }

    /// <summary>
    /// 生成波形数据（模拟采集卡底噪：纯随机噪声）
    /// </summary>
    private static double[] GenerateWaveform(int points, double frequency, double amplitude)
    {
        var data = new double[points];
        for (int i = 0; i < points; i++)
        {
            // 纯随机白噪声（模拟采集卡底噪）
            // 使用高斯分布的随机数会更真实
            double noise1 = (_random.NextDouble() - 0.5) * 2.0;
            double noise2 = (_random.NextDouble() - 0.5) * 2.0;

            // Box-Muller 变换生成高斯分布噪声
            double gaussianNoise = Math.Sqrt(-2.0 * Math.Log(Math.Abs(noise1) + 0.0001)) *
                                   Math.Cos(2 * Math.PI * noise2);

            // 归一化到幅度范围
            data[i] = gaussianNoise * amplitude * 0.3;

            // 限幅（模拟ADC饱和）
            if (data[i] > amplitude) data[i] = amplitude;
            if (data[i] < -amplitude) data[i] = -amplitude;
        }
        return data;
    }

    /// <summary>
    /// 客户端断开连接时清理
    /// </summary>
    public override async Task OnDisconnectedAsync(Exception? exception)
    {
        var connectionId = Context.ConnectionId;
        if (_streamingClients.TryGetValue(connectionId, out var cts))
        {
            cts.Cancel();
            _streamingClients.Remove(connectionId);
        }
        await base.OnDisconnectedAsync(exception);
    }
}
