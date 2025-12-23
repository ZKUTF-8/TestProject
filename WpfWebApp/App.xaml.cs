using System.Windows;
using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using WpfWebApp.Hubs;

namespace WpfWebApp
{
    /// <summary>
    /// WPF 应用程序主类
    /// </summary>
    public partial class App : Application
    {
        private IHost? _host;

        public App()
        {
            // 初始化应用程序
        }

        protected override async void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // 启动 SignalR 服务
            _ = StartSignalRServer();
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            if (_host != null)
            {
                await _host.StopAsync();
                _host.Dispose();
            }
            base.OnExit(e);
        }

        private async Task StartSignalRServer()
        {
            var builder = WebApplication.CreateBuilder();

            // 添加 SignalR 服务（使用 MessagePack 二进制协议）
            builder.Services.AddSignalR().AddMessagePackProtocol();

            // 跨域配置
            builder.Services.AddCors(options =>
            {
                options.AddDefaultPolicy(builder =>
                {
                    builder.SetIsOriginAllowed(x => true).AllowAnyHeader().AllowAnyMethod().AllowCredentials();
                });
            });

            var app = builder.Build();

            app.UseCors();

#if !DEBUG
            // 生产环境：提供静态文件服务
            app.UseStaticFiles(new Microsoft.AspNetCore.Builder.StaticFileOptions
            {
                FileProvider = new Microsoft.Extensions.FileProviders.PhysicalFileProvider(
                    System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "wwwroot")),
                RequestPath = ""
            });

            // 默认路由到 index.html
            app.MapFallbackToFile("index.html", new Microsoft.AspNetCore.Builder.StaticFileOptions
            {
                FileProvider = new Microsoft.Extensions.FileProviders.PhysicalFileProvider(
                    System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "wwwroot"))
            });
#endif

            // 映射 SignalR Hub
            app.MapHub<AppHub>("/hub");

            _host = app;

            // 在后台启动服务器
            await app.RunAsync("http://[::]:5000");
        }
    }
}
