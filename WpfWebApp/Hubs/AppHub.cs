using Microsoft.AspNetCore.SignalR;
using System.Diagnostics;

namespace WpfWebApp.Hubs
{
    public class AppHub : Hub
    {
        private static readonly Stopwatch _stopwatch = Stopwatch.StartNew();

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
    }
}
