using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace XLS2XML
{
    class Program
    {
        static public String g_open = "";
        static public String g_target = "";
        static public Converter g_converter = new Converter();

        [DllImport("user32.dll")]
        public static extern IntPtr
        FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [STAThread]
        static void Main(string[] args)
        {
            ExcelReader.Initialize();
            
            commandline(args);

            if (args.Length == 0)
            {
                IntPtr hWnd = FindWindow(null, Console.Title);
                ShowWindow(hWnd, 0);
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new WinForm());
                ShowWindow(hWnd, 1);
            }

            ExcelReader.Terminate();
        }

        static void commandline(string[] args)
        {
            int index = 0;

            while (args.Length > index)
            {
                if (args[index].StartsWith("-open"))
                {
                    ++index;
                    g_open = args[index];
                    g_converter.StartConvert(g_open, g_target);
                }
                else if (args[index].StartsWith("-target"))
                {
                    ++index;
                    g_target = args[index];
                }

                ++index;
            }
        }
    }
}
