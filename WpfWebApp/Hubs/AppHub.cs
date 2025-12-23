using Microsoft.AspNetCore.SignalR;
using System.Diagnostics;

namespace WpfWebApp.Hubs
{
    public class AppHub : Hub
    {
        private static readonly Stopwatch _stopwatch = Stopwatch.StartNew();
        private static readonly Dictionary<string, CancellationTokenSource> _streamingClients = new();
        private static readonly Random _random = new();

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
}
