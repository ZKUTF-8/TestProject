using System.Windows;

namespace WpfWebApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            InitializeWebView();
        }

        private async void InitializeWebView()
        {
            await webView.EnsureCoreWebView2Async();

#if DEBUG
            // 开发环境：加载 Vite 开发服务器（默认端口 5173）
            webView.CoreWebView2.Navigate("http://localhost:5173");
#else
            // 生产环境：加载本地HTTP服务器（避免file://协议的CORS问题）
            webView.CoreWebView2.Navigate("http://localhost:5000");

            // 生产环境：禁用开发者工具和浏览器快捷键
            webView.CoreWebView2.Settings.AreDevToolsEnabled = false;
            webView.CoreWebView2.Settings.AreBrowserAcceleratorKeysEnabled = false;

            // 在 WPF 层面拦截键盘事件
            webView.PreviewKeyDown += (sender, e) =>
            {
                var key = e.Key;
                var ctrl = (e.KeyboardDevice.Modifiers & System.Windows.Input.ModifierKeys.Control) != 0;

                // 屏蔽的快捷键列表
                if (key == System.Windows.Input.Key.F12 || // F12 - 开发者工具
                    (ctrl && key == System.Windows.Input.Key.R) || // Ctrl+R - 刷新
                    key == System.Windows.Input.Key.F5 || // F5 - 刷新
                    (ctrl && key == System.Windows.Input.Key.F) || // Ctrl+F - 查找
                    (ctrl && key == System.Windows.Input.Key.P) || // Ctrl+P - 打印
                    (ctrl && key == System.Windows.Input.Key.S) || // Ctrl+S - 保存页面
                    (ctrl && key == System.Windows.Input.Key.U) || // Ctrl+U - 查看源代码
                    (ctrl && (key == System.Windows.Input.Key.OemPlus || key == System.Windows.Input.Key.OemMinus || key == System.Windows.Input.Key.Add || key == System.Windows.Input.Key.Subtract))) // Ctrl+Plus/Minus - 缩放
                {
                    e.Handled = true;
                }
            };
#endif
        }
    }
}