using System.Windows;
using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.FileProviders;
using LC8172.WpfHost.Hubs;
using System.IO;

namespace LC8172.WpfHost;

internal class Program
{
    private static WebApplication? _webApp;

    [STAThread]
    static void Main(string[] args)
    {
        // 启动 SignalR 服务器（后台运行）
        var serverTask = Task.Run(async () => await StartSignalRServer());

        // 等待服务器启动
        Thread.Sleep(1000);

        // 启动 WPF 应用
        var app = new Application();
        app.Startup += OnStartup;
        app.Exit += OnExit;
        app.Run(new MainWindow());
    }

    private static void OnStartup(object sender, StartupEventArgs e)
    {
        Console.WriteLine("✅ LC-8172 应用启动");
    }

    private static async void OnExit(object sender, ExitEventArgs e)
    {
        Console.WriteLine("⏹️ LC-8172 应用退出");
        if (_webApp != null)
        {
            await _webApp.StopAsync();
            await _webApp.DisposeAsync();
        }
    }

    private static async Task StartSignalRServer()
    {
        var builder = WebApplication.CreateBuilder();

        // 添加 SignalR 服务（使用 MessagePack 二进制协议）
        builder.Services.AddSignalR().AddMessagePackProtocol();

        // 跨域配置
        builder.Services.AddCors(options =>
        {
            options.AddDefaultPolicy(builder =>
            {
                builder.SetIsOriginAllowed(x => true)
                       .AllowAnyHeader()
                       .AllowAnyMethod()
                       .AllowCredentials();
            });
        });

        _webApp = builder.Build();
        _webApp.UseCors();

        // 检查 wwwroot 目录是否存在
        var wwwrootPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "wwwroot");
        Console.WriteLine($"📁 wwwroot 路径: {wwwrootPath}");
        Console.WriteLine($"📁 wwwroot 存在: {Directory.Exists(wwwrootPath)}");

        // 提供静态文件服务（从 wwwroot 目录）
        if (Directory.Exists(wwwrootPath))
        {
            _webApp.UseStaticFiles(new StaticFileOptions
            {
                FileProvider = new PhysicalFileProvider(wwwrootPath),
                RequestPath = ""
            });
            Console.WriteLine("✅ 静态文件服务已配置");

            // 默认路由到 index.html
            _webApp.MapFallbackToFile("index.html", new StaticFileOptions
            {
                FileProvider = new PhysicalFileProvider(wwwrootPath)
            });
            Console.WriteLine("✅ FallbackToFile 已配置");
        }
        else
        {
            Console.WriteLine("⚠️ wwwroot 目录不存在，跳过静态文件服务");
        }

        // 映射 SignalR Hub
        _webApp.MapHub<AcquisitionHub>("/acquisitionHub");

        Console.WriteLine("🚀 SignalR 服务器启动在 http://localhost:5000");

        // 在后台启动服务器
        await _webApp.RunAsync("http://localhost:5000");
    }
}

