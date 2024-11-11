using System;
using System.IO;
using System.Windows;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace G2PatchMaker
{
    public static class Logger
    {
        static string logFileName;

        public static void Init()
        {
            Window1.myWin.CreateFolder("\\log", false);
            logFileName = "PatchMakerLog_" + DateTime.Now.ToString("MMddHHmmss") + ".txt";
        }

        public static void Log(String msg, bool showMessageBox = false, bool enableConsole = true)
        {
            msg = "[" + DateTime.Now.ToString("MM/dd HH:mm:ss") + "] " + msg;

            if (enableConsole)
                Console.WriteLine(msg);

            FileStream fs = new FileStream("log\\" + logFileName, FileMode.Append, FileAccess.Write);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine(msg);
            sw.Close();
            fs.Close();

            if (showMessageBox)
            {
                MessageBox.Show(msg);
            }
        }
    }
}
