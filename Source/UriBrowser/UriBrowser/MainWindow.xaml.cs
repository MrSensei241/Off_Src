using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Pipes;
using System.IO;

namespace UriBrowser
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        NamedPipeClientStream m_pipeClient;
        string m_uri;
        public MainWindow()
        {
            string[] args = Environment.GetCommandLineArgs();
            m_uri = ParseCommandLine(args);
            if (m_uri != null && m_uri.Length > 0)
            {
                InitializeComponent();
                WebBrowser.Navigating += WebBrowser_Navigating;
                this.Loaded += MainWindow_Loaded;
                this.Closed += MainWindow_Closed;
            }
            else
            {
                Application.Current.Shutdown();
            }
        }

        private string ParseCommandLine(string[] args)
        {
            for (int i = 0; i < args.Length - 1; i++)
            {
                if (args[i].Equals("-uri"))
                {
                    string url = args[i + 1];
                    return url;
                }
            }
            return null;
        }

        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            m_pipeClient = new NamedPipeClientStream(".", "imc_tos_browser_pipe", PipeDirection.Out, PipeOptions.None);
            if (m_pipeClient == null)
            {
                Application.Current.Shutdown();
            }

            try
            {
                m_pipeClient.Connect(1000);
            }
            catch (TimeoutException ex)
            {
                m_pipeClient.Close();
                Application.Current.Shutdown();
            }

            if (m_uri == null)
            {
                m_pipeClient.Close();
                Application.Current.Shutdown();
            }

            WebBrowser.Navigate(new Uri(m_uri));
        }

        private void MainWindow_Closed(object sender, EventArgs e)
        {
            if (m_pipeClient != null)
            {
                m_pipeClient.Close();
                m_pipeClient.Dispose();
            }
        }

        void WriteToPipe(string absoluteUri)
        {
            try
            {
                if (m_pipeClient != null)
                {
                    absoluteUri += "\0";
                    byte[] bytes = Encoding.UTF8.GetBytes(absoluteUri);
                    m_pipeClient.Write(bytes, 0, bytes.Length);
                    m_pipeClient.Flush();
                }
            }
            catch(Exception e)
            {
                
            }
        }

        private void WebBrowser_Navigating(object sender, NavigatingCancelEventArgs e)
        {
            WriteToPipe(e.Uri.AbsoluteUri);
        }
    }
}
