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

            // 开发环境：加载 Vite 开发服务器（默认端口 5173）
            webView.CoreWebView2.Navigate("http://localhost:5173");
        }
    }
}