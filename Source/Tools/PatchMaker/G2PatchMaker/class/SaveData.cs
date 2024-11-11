using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace G2PatchMaker
{
    public class SaveData
    {
        public static List<String> data = new List<String>();
        public static List<String> release = new List<String>();
        public static List<String> server_data = new List<String>();
        public static List<String> server_release = new List<String>();
        public static String startversion = "0";
        public static String endversion = "0";
        public static bool read = false;
        public static String patchfolder = "";
        public static String datafolder = "";
        public static String releasefolder = "";
        public static String serverfolder = "";

        public SaveData()
        {
            patchfolder = Directory.GetCurrentDirectory() + "\\patch\\SVN_DATA";

            DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
            datafolder = Directory.GetParent(info.FullName).FullName;
            releasefolder = Directory.GetParent(info.FullName).FullName;
            serverfolder = Directory.GetParent(info.FullName).FullName;
            datafolder += "\\data\\";
            releasefolder += "\\release\\";
            serverfolder += "\\server_release\\";

            String filename = Directory.GetCurrentDirectory() + "\\savefile.txt";

            if (File.Exists(filename))
            {
                StreamReader sr = new StreamReader(filename, Encoding.GetEncoding("KS_C_5601-1987"));
                SetStringList(sr.ReadLine(), data);
                SetStringList(sr.ReadLine(), release);
                SetStringList(sr.ReadLine(), server_data);
                SetStringList(sr.ReadLine(), server_release);
                startversion = sr.ReadLine();
                endversion = sr.ReadLine();
                sr.Close();

                read = true;
            }
        }

        void SetStringList(String _value, List<String> _list)
        {
            while (!_value.Equals(""))
            {
                String name = _value.Substring(0, _value.IndexOf(" "));
                _value = _value.Substring(_value.IndexOf(" ") + 1);
                _list.Add(name);
            }
        }
    }
}
