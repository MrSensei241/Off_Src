using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.ComponentModel;
using System.Windows.Threading;
using imc;

namespace G2PatchMaker
{
    /// <summary>
    /// Window1.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class Window1 : Window
    {
        public static bool m_checkFullFile = false;

        public Window1()
        {
            new SaveData();

            this.InitializeComponent();
            this.InitializeUI();
        }

        #region event

        void _removeClientData(String name)
        {
            makeall._removeClientData(name);
            makeclient._removeClientData(name);
        }

        void _addClientData(String name)
        {
            makeall._addClientData(name);
            makeclient._addClientData(name);
        }

        void _removeClientRelease(String name)
        {
            makeall._removeClientRelease(name);
            makeclient._removeClientRelease(name);
        }

        void _addClientRelease(String name)
        {
            makeall._addClientRelease(name);
            makeclient._addClientRelease(name);
        }

        void _removeServerData(String name)
        {
            makeall._removeServerData(name);
            makeserver._removeServerData(name);
        }

        void _addServerData(String name)
        {
            makeall._addServerData(name);
            makeserver._addServerData(name);
        }

        void _removeServerRelease(String name)
        {
            makeall._removeServerRelease(name);
            makeserver._removeServerRelease(name);
        }

        void _addServerRelease(String name)
        {
            makeall._addServerRelease(name);
            makeserver._addServerRelease(name);
        }

        #endregion

        #region init

        public static Window1 myWin;

        void InitializeUI()
        {
            myWin = this;
            progressGrid.Visibility = Visibility.Hidden;

            svnEnd.Text = SaveData.startversion;
            svnStart.Text = SaveData.endversion;

            imc.IPF.Init();

            this.PreviewKeyDown += new KeyEventHandler(Window1_PreviewKeyDown);
            this.Closed += new EventHandler(Window1_Closed);

            menu_showipf.Click += new RoutedEventHandler(menu_showipf_Click);
            menu_manual.Click += new RoutedEventHandler(menu_manual_Click);
            menu_info.Click += new RoutedEventHandler(menu_info_Click);
            menu_sendmail.Click += new RoutedEventHandler(menu_sendmail_Click);

            makeall.addClientData += _addClientData;
            makeclient.addClientData += _addClientData;
            makeall.addClientRelease += _addClientRelease;
            makeclient.addClientRelease += _addClientRelease;
            makeall.removeClientData += _removeClientData;
            makeclient.removeClientData += _removeClientData;
            makeall.removeClientRelease += _removeClientRelease;
            makeclient.removeClientRelease += _removeClientRelease;
            makeall.addServerData += _addServerData;
            makeserver.addServerData += _addServerData;
            makeall.addServerRelease += _addServerRelease;
            makeserver.addServerRelease += _addServerRelease;
            makeall.removeServerData += _removeServerData;
            makeserver.removeServerData += _removeServerData;
            makeall.removeServerRelease += _removeServerRelease;
            makeserver.removeServerRelease += _removeServerRelease;

            #region removelist

            RemoveList = new Dictionary<String, List<String>>();

            if (File.Exists(SaveData.patchfolder + "\\data.txt"))
            {
                List<String> removes = new List<String>();
                using (StreamReader sr = new StreamReader(SaveData.patchfolder + "\\data.txt", Encoding.GetEncoding("KS_C_5601-1987")))
                {
                    String line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        String key = line.Substring(0, 1);
                        Int32 count = line.IndexOf(":") - 1;
                        line = line.Substring(count, line.Length - count);

                        if (key.Equals("D"))
                            removes.Add(line);
                    }

                    sr.Close();
                }

                RemoveList.Add("data", removes);
            }

            if (File.Exists(SaveData.patchfolder + "\\release.txt"))
            {
                List<String> removes = new List<String>();
                using (StreamReader sr = new StreamReader(SaveData.patchfolder + "\\release.txt", Encoding.GetEncoding("KS_C_5601-1987")))
                {
                    String line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        String key = line.Substring(0, 1);
                        Int32 count = line.IndexOf(":") - 1;
                        line = line.Substring(count, line.Length - count);

                        if (key.Equals("D"))
                            removes.Add(line);
                    }

                    sr.Close();
                }

                RemoveList.Add("release", removes);
            }

            if (File.Exists(SaveData.patchfolder + "\\server.txt"))
            {
                List<String> removes = new List<String>();
                using (StreamReader sr = new StreamReader(SaveData.patchfolder + "\\server.txt", Encoding.GetEncoding("KS_C_5601-1987")))
                {
                    String line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        String key = line.Substring(0, 1);
                        Int32 count = line.IndexOf(":") - 1;
                        line = line.Substring(count, line.Length - count);

                        if (key.Equals("D"))
                            removes.Add(line);
                    }

                    sr.Close();
                }

                RemoveList.Add("server_release", removes);
            }

            #endregion
        }

        void Window1_Closed(object sender, EventArgs e)
        {
            String filename = Directory.GetCurrentDirectory() + "\\savefile.txt";

            FileStream fs = new FileStream(filename, FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);
            makeall.WriteData(sw);
            sw.WriteLine(svnStart.Text);
            sw.WriteLine(svnEnd.Text);
            sw.Close();
            fs.Close();
        }

        void Window1_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.F1)
            {
                DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
                String name = Directory.GetParent(info.FullName).FullName;
                name += "\\manual\\패치 메이커.chm";

                Process.Start(name);
            }
        }
        #endregion

        #region svn

        delegate void ExportEvent(String[] input);
        delegate void DiffEvent(String[] input);

        public void StartExport(String[] input)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new ExportEvent(Export), input);
        }

        public void StartDiff(String[] input)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new DiffEvent(Diff), input);
        }

        void Export(String[] inputMsg)
        {
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.FileName = "CMD.exe";
            startInfo.WorkingDirectory = @"C:/";
            startInfo.WindowStyle = ProcessWindowStyle.Normal;
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;

            foreach (String msg in inputMsg)
            {
                process.EnableRaisingEvents = false;
                process.StartInfo = startInfo;
                process.Start();

                process.StandardInput.Write(msg);
                process.StandardInput.Close();

                string _result = process.StandardOutput.ReadToEnd();
                string _error = process.StandardError.ReadToEnd();
            }

            progressGrid.Visibility = Visibility.Hidden;
            tabcontrol.Visibility = Visibility.Visible;
        }

        Dictionary<String, List<String>> RemoveList = null;
        public void Diff(String[] input)
        {
            RemoveList = new Dictionary<String, List<String>>();
            List<String> difflist = new List<String>(input);

            for (int i = 0; i < difflist.Count / 6; ++i)
            {
                System.Diagnostics.Process process = new System.Diagnostics.Process();
                System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
                startInfo.FileName = "CMD.exe";
                startInfo.WorkingDirectory = @"C:/";
                startInfo.WindowStyle = ProcessWindowStyle.Hidden;
                startInfo.CreateNoWindow = true;
                startInfo.UseShellExecute = false;
                startInfo.RedirectStandardInput = true;
                startInfo.RedirectStandardOutput = true;
                startInfo.RedirectStandardError = true;

                process.EnableRaisingEvents = false;
                process.StartInfo = startInfo;
                process.Start();

                process.StandardInput.Write(input[i * 6]);
                process.StandardInput.Close();

                string _result = process.StandardOutput.ReadToEnd();
                string _error = process.StandardError.ReadToEnd();

                process.EnableRaisingEvents = false;
                process.StartInfo = startInfo;
                process.Start();
                process.StandardInput.Write(input[i * 6 + 1]);
                process.StandardInput.Close();

                _result = process.StandardOutput.ReadToEnd();
                _error = process.StandardError.ReadToEnd();

                List<String> removes = new List<String>();
                using (StreamReader sr = new StreamReader(input[i * 6 + 2], Encoding.GetEncoding("KS_C_5601-1987")))
                {
                    String line;
                    while ((line = sr.ReadLine()) != null)
                    {
                        String key = line.Substring(0, 1);
                        Int32 count = line.IndexOf(":") - 1;
                        line = line.Substring(count, line.Length - count);

                        if (key.Equals("D"))
                            removes.Add(line);
                        else
                            Window1.myWin.CopyFile(line, input[i * 6 + 3], input[i * 6 + 4]);
                    }

                    sr.Close();
                }

                RemoveList.Add(input[i * 6 + 5], removes);
            }

            progressGrid.Visibility = Visibility.Hidden;
            tabcontrol.Visibility = Visibility.Visible;
        }

        #endregion

        public void CreateFolder(String name, bool _create)
        {
            if (!Directory.Exists(Directory.GetCurrentDirectory() + name))
            {
                Directory.CreateDirectory(Directory.GetCurrentDirectory() + name);
            }
            else if (_create)
            {
                Directory.Delete(Directory.GetCurrentDirectory() + name);
                Directory.CreateDirectory(Directory.GetCurrentDirectory() + name);
            }
        }

        public void CopyFile(String line, String originalpath, String savepath)
        {
            if (line.IndexOf(originalpath) == -1)
                return;

            String name = System.IO.Path.GetFileName(line);
            String path = line;
            path = path.Replace(originalpath, "");

            if (path.Length == 0)
                return;

            if (path.IndexOf("\\") == 0)
                path = path.Substring(1);

            if (path.Equals(name))
            {
                if (Directory.Exists(line)) Directory.CreateDirectory(savepath + "\\" + name);
                else File.Copy(line, savepath + "\\" + name);
            }
            else
            {
                String foldername = path.Substring(0, path.IndexOf("\\"));
                Directory.CreateDirectory(savepath + "\\" + foldername);

                CopyFile(line, originalpath + foldername + "\\", savepath + "\\" + foldername);
            }
        }

        #region menu

        void menu_sendmail_Click(object sender, RoutedEventArgs e)
        {
            SendMail mail = new SendMail();
            mail.Show();
        }

        void menu_info_Click(object sender, RoutedEventArgs e)
        {

        }

        void menu_manual_Click(object sender, RoutedEventArgs e)
        {
            DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
            String name = Directory.GetParent(info.FullName).FullName;
            name += "\\manual\\패치 메이커.chm";

            Process.Start(name);
        }

        void menu_showipf_Click(object sender, RoutedEventArgs e)
        {
            ShowIpfDialog dlg = new ShowIpfDialog();
            dlg.Show();
        }

        public String SVN_StartVersion
        {
            get { return svnStart.Text; }
        }

        public String SVN_EndVersion
        {
            get { return svnEnd.Text; }
        }

        private void HelpBt_Click(object sender, RoutedEventArgs e)
        {
            DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
            String name = Directory.GetParent(info.FullName).FullName;
            name += "\\manual\\패치 메이커.chm";

            Process.Start(name);
        }

        #endregion

        #region make

        delegate void MakeIpfFull(String input, List<String> names);
        delegate void MakeServer(List<String> datas, List<String> releases);

        int _ipf_count = 0;
        void _makeipf_full(String input, List<String> names)
        {
            imc.IPF ipf = new imc.IPF();

            String folderName = Directory.GetCurrentDirectory() + "\\patch\\" + Window1.myWin.SVN_EndVersion;
            Directory.CreateDirectory(folderName);

            String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\" + input);
            int _fileSize = 0;

            try { _fileSize = Convert.ToInt32(maxSize.Text); }
            catch (System.Exception ) { }

            foreach (String directoryname in directories)
            {
                String ipfNmae = System.IO.Path.GetFileNameWithoutExtension(directoryname);
                if (names.IndexOf(ipfNmae) != -1)
                {
                    _ipf_count = 0;

                    String fullname = folderName + "\\" + ipfNmae;
                    ipf.CreatePartialLoadIpf(fullname + ".ipf", 0, Convert.ToInt32(Window1.myWin.SVN_EndVersion));

                    if (_fileSize > 0)
                        AddIpfFileData(fullname, 0, Convert.ToInt32(Window1.myWin.SVN_EndVersion), Convert.ToInt32(maxSize.Text) * 1024 * 1024, ipf, directoryname.Replace("\\", "/"), "");
                    else
                        AddIpfFileData(ipf, directoryname.Replace("\\", "/"), "");

                    ipf.CloseIpf();
                }
            }

            tabcontrol.Visibility = Visibility.Visible;
            progressGrid.Visibility = Visibility.Hidden;
        }

        void _makeipf_part(String input, List<String> names)
        {
            imc.IPF ipf = new imc.IPF();
            ipf.CreatePartialLoadIpf(Directory.GetCurrentDirectory() + "\\patch\\" + Window1.myWin.SVN_EndVersion + "_001001.ipf"
                , Convert.ToInt32(Window1.myWin.SVN_StartVersion), Convert.ToInt32(Window1.myWin.SVN_EndVersion));

            AddRemoveList(ipf, input);

            String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\" + input);

            foreach (String directoryname in directories)
            {
                String ipfNmae = System.IO.Path.GetFileNameWithoutExtension(directoryname);
                if (names.IndexOf(ipfNmae) != -1)
                {
                    AddIpfFileData(ipf, directoryname.Replace("\\", "/"), "");
                }
            }

            ipf.CloseIpf();

            tabcontrol.Visibility = Visibility.Visible;
            progressGrid.Visibility = Visibility.Hidden;
        }

        void _makepak(String input, List<String> names)
        {
            imc.IPF ipf = new imc.IPF();

            ipf.CreateReleaseIpf(Directory.GetCurrentDirectory() + "\\patch\\" + Window1.myWin.SVN_EndVersion + "_001001.pak", false);

            String directoryname = SaveData.patchfolder + "\\" + input;
            String[] sub_directories = Directory.GetDirectories(directoryname);
            String[] sub_files = Directory.GetFiles(directoryname);

            foreach (String _directoryname in sub_directories)
            {
                if (names.IndexOf(System.IO.Path.GetFileNameWithoutExtension(_directoryname)) != -1)
                {
                    String _name = _directoryname.Replace("\\", "/");
                    _name = _name.Replace(directoryname.Replace("\\", "/"), "");
                    AddIpfFileData(ipf, directoryname.Replace("\\", "/"), _name + "/");
                }
            }

            foreach (String filename in sub_files)
            {
                if (names.IndexOf(System.IO.Path.GetFileName(filename)) != -1)
                {
                    ipf.AddFileFromFile("", filename.Replace("\\", "/"));
            }
            }

            ipf.CloseIpf();

            tabcontrol.Visibility = Visibility.Visible;
            progressGrid.Visibility = Visibility.Hidden;
        }

        string ReadHashCode()
        {
            string path = SaveData.serverfolder + "hash.txt";
            if (File.Exists(path) == false)
            {
                MessageBox.Show("cannot find hash file");
                tabcontrol.Visibility = Visibility.Visible;
                progressGrid.Visibility = Visibility.Hidden;
                ServerPack_MakeHashKey();
                System.Console.WriteLine(path);
                return "None";
            }

            return File.ReadAllText(path);
        }

        public void RemoveHashKey()
        {
            string path = SaveData.serverfolder + "hash.txt";
            System.IO.File.Delete(path);

            if (System.IO.File.Exists(path))
            {
                MessageBox.Show("hash.txt remove fail!!!!!!!. you must delete this file!!!!!");
                return;
            }

        }

        void _makeserver_full(List<String> datas, List<String> releases)
        {
            string key = ReadHashCode();
            if ("None" == key)
                return;

            RemoveHashKey();

            imc.IPF ipf = new imc.IPF();
            ipf.CreatePartialLoadIpf(Directory.GetCurrentDirectory() + "\\patch\\" + Window1.myWin.SVN_EndVersion + "_" + key + ".server", 0, 0);

            if (Directory.Exists(SaveData.patchfolder + "\\data"))
            {
                String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\data");

                foreach (String directoryname in directories)
                {
                    if (datas.IndexOf(System.IO.Path.GetFileNameWithoutExtension(directoryname)) != -1)
                    {
                        AddIpfFileData(ipf, SaveData.patchfolder.Replace("\\", "/") + "/data"
                            , "/" + System.IO.Path.GetFileNameWithoutExtension(directoryname) + "/");
                    }
                }
            }

            if (Directory.Exists(SaveData.patchfolder + "\\server_release"))
            {
                String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\server_release");
                String[] files = Directory.GetFiles(SaveData.patchfolder + "\\server_release");

                foreach (String directoryname in directories)
                {
                    if (releases.IndexOf(System.IO.Path.GetFileNameWithoutExtension(directoryname)) != -1)
                    {
                        AddIpfFileData(ipf, SaveData.patchfolder.Replace("\\", "/") + "/server_release"
                            , "/" + System.IO.Path.GetFileNameWithoutExtension(directoryname) + "/");
                    }
                }

                foreach (String filename in files)
                {
                    if (releases.IndexOf(System.IO.Path.GetFileName(filename)) != -1)
                    {
                        ipf.AddFileFromFile("server_release/", filename.Replace("\\", "/"));
                }
            }
            }

            ipf.CloseIpf();
            tabcontrol.Visibility = Visibility.Visible;
            progressGrid.Visibility = Visibility.Hidden;
        }

        void _makeserver_part(List<String> datas, List<String> releases)
        {
            string key = ReadHashCode();
            if ("None" == key)
                return;

            RemoveHashKey();

            imc.IPF ipf = new imc.IPF();
            ipf.CreatePartialLoadIpf(Directory.GetCurrentDirectory() + "\\patch\\" + Window1.myWin.SVN_EndVersion + "_" + key + ".server", 0, 0);

            AddRemoveList(ipf, "data");
            AddRemoveList(ipf, "server_release");

            if (Directory.Exists(SaveData.patchfolder + "\\data"))
            {
                String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\data");

                foreach (String directoryname in directories)
                {
                    if (datas.IndexOf(System.IO.Path.GetFileNameWithoutExtension(directoryname)) != -1)
                    {
                        AddIpfFileData(ipf, SaveData.patchfolder.Replace("\\", "/") + "/data"
                            , "/" + System.IO.Path.GetFileNameWithoutExtension(directoryname) + "/");
                    }
                }
            }

            if (Directory.Exists(SaveData.patchfolder + "\\server_release"))
            {
                String[] directories = Directory.GetDirectories(SaveData.patchfolder + "\\server_release");
                String[] files = Directory.GetFiles(SaveData.patchfolder + "\\server_release");

                foreach (String directoryname in directories)
                {
                    if (releases.IndexOf(System.IO.Path.GetFileNameWithoutExtension(directoryname)) != -1)
                    {
                        AddIpfFileData(ipf, SaveData.patchfolder.Replace("\\", "/") + "/server_release"
                            , "/" + System.IO.Path.GetFileNameWithoutExtension(directoryname) + "/");
                    }
                }

                foreach (String filename in files)
                {
                    if (releases.IndexOf(System.IO.Path.GetFileName(filename)) != -1)
                    {
                        ipf.AddFileFromFile("", filename.Replace("\\", "/"));
                }
            }
            }

            ipf.CloseIpf();    
            tabcontrol.Visibility = Visibility.Visible;
            progressGrid.Visibility = Visibility.Hidden;
        }

        public void MakeIpfFullPatch(String input, List<String> names)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new MakeIpfFull(_makeipf_full), input, names);
        }

        public void MakeIpfPartPatch(String input, List<String> names)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new MakeIpfFull(_makeipf_part), input, names);
        }

        public void MakePakPatch(String input, List<String> names)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;
            try
            {
                tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new MakeIpfFull(_makepak), input, names);
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Data.ToString());
            }

        }

        public void MakeServerFullPatch(List<String> datas, List<String> releases)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new MakeServer(_makeserver_full), datas, releases);
        }

        public void MakeServerPartPatch(List<String> datas, List<String> releases)
        {
            tabcontrol.Visibility = Visibility.Hidden;
            progressGrid.Visibility = Visibility.Visible;

            tabcontrol.Dispatcher.Invoke(DispatcherPriority.ContextIdle, new MakeServer(_makeserver_part), datas, releases);
        }

        // AddIpfFileData(1)
        private void AddIpfFileData(imc.IPF ipf, String name, String substring)
        {
            String[] sub_directories = Directory.GetDirectories(name + substring);
            String[] sub_files = Directory.GetFiles(name + substring);

            String temp = substring;
            if (temp.StartsWith("/")) temp = temp.Substring(1);

            foreach (String directoryname in sub_directories)
            {
                String _name = directoryname.Replace("\\", "/");
                _name = _name.Replace(name, "");
                AddIpfFileData(ipf, name, _name + "/");
            }

            String xmlName = name.Substring(name.LastIndexOf("SVN_DATA/data/") + 14);

            bool chkIes = false;
            String chkPath = "ies";

            if (xmlName == "xml")
            {
                chkIes = true;
                chkPath = "ies";
            }
            else if (xmlName == "xml_client")
            {
                chkIes = true;
                chkPath = "ies_client";
            }
            String iesPath = SaveData.datafolder + chkPath + substring;
            if (Directory.Exists(iesPath) == false)
            {
                chkIes = false;
            }

            if (chkIes)
            {
                foreach (String filename in sub_files)
                {                    
                    String[] sub_iesfiles = Directory.GetFiles(iesPath);

                    bool find = false;
                    foreach (String filenameies in sub_iesfiles)
                    {
                        String iesFileName = filenameies.Substring(filenameies.LastIndexOf(iesPath) + iesPath.Length + 1); // +1 == \\
                        iesFileName = iesFileName.Substring(0, iesFileName.Length - 4); // -4 == .ies

                        String xmlFileName = filename.Substring(filename.LastIndexOf(name + substring) + (name + substring).Length + 1);  // +1 == \\
                        xmlFileName = xmlFileName.Substring(0, xmlFileName.Length - 4); // -4 == .ies
                        if (iesFileName == xmlFileName)
                        {
                            find = true;
                            break;
                        }
                    }
                    if (find == false)
                    {
                        ipf.AddFileFromFile(temp, filename.Replace("\\", "/"));
                        Console.WriteLine("AddIpfFileData(1): ipfName(" + temp + ") file(" + filename.Replace("\\", "/") + ")");
                    }
                }
            }
            else
            {
                foreach (String filename in sub_files)
                {
                    ipf.AddFileFromFile(temp, filename.Replace("\\", "/"));
                    Console.WriteLine("AddIpfFileData(1): ipfName(" + temp + ") file(" + filename.Replace("\\", "/") + ")");
                }
            }
        }

        // AddIpfFileData(2)
        private void AddIpfFileData(String ipfname, int start, int end, int size, imc.IPF ipf, String name, String substring)
        {
            String[] sub_directories = Directory.GetDirectories(name + substring);
            String[] sub_files = Directory.GetFiles(name + substring);

            String temp = substring;
            if (temp.StartsWith("/")) temp = temp.Substring(1);

            foreach (String directoryname in sub_directories)
            {
                String _name = directoryname.Replace("\\", "/");
                _name = _name.Replace(name, "");
                AddIpfFileData(ipfname, start, end, size, ipf, name, _name + "/");
            }

            String xmlName = name.Substring(name.LastIndexOf("SVN_DATA/data/") + 14);

            bool chkIes = false;
            String chkPath = "ies";

            if (xmlName == "xml")
            {
                chkIes = true;
                chkPath = "ies";
            }
            else if (xmlName == "xml_client")
            {
                chkIes = true;
                chkPath = "ies_client";
            }
            String iesPath = SaveData.datafolder + chkPath + substring;
            if (Directory.Exists(iesPath) == false)
            {
                chkIes = false;
            }

            if (chkIes)
            {
                foreach (String filename in sub_files)
                {
                    String[] sub_iesfiles = Directory.GetFiles(iesPath);

                    bool find = false;
                    foreach (String filenameies in sub_iesfiles)
                    {
                        String iesFileName = filenameies.Substring(filenameies.LastIndexOf(iesPath) + iesPath.Length + 1); // +1 == \\
                        iesFileName = iesFileName.Substring(0, iesFileName.Length - 4); // -4 == .ies

                        String xmlFileName = filename.Substring(filename.LastIndexOf(name + substring) + (name + substring).Length + 1);  // +1 == \\
                        xmlFileName = xmlFileName.Substring(0, xmlFileName.Length - 4); // -4 == .ies
                        if (iesFileName == xmlFileName)
                        {
                            find = true;
                            break;
                        }
                    }
                    if (find == false)
                    {
                        ipf.AddFileFromFile(temp, filename.Replace("\\", "/"));
                        Console.WriteLine("AddIpfFileData(2): ipfName(" + temp + ") file(" + filename.Replace("\\", "/") + ")");
                    }
                }
            }
            else
            {
                foreach (String filename in sub_files)
                {
                    Console.WriteLine("AddIpfFileData(2): ipfName(" + temp + ") file(" + filename.Replace("\\", "/") + ")");
                    if (!ipf.AddFileFromFile(temp, filename.Replace("\\", "/"), size))
                    {
                        ipf.CloseIpf();

                        ++_ipf_count;
                        ipf.CreatePartialLoadIpf(ipfname + _ipf_count + ".ipf", start, end);
                    }
                }
            }
        }

        private void AddRemoveList(imc.IPF ipf, String name)
        {
            if (RemoveList.ContainsKey(name))
            {
                foreach (String filename in RemoveList[name])
                {
                    String _remove = filename;
                    _remove = _remove.Replace(SaveData.datafolder, "");

                    ipf.AddRemoveFileName(name, _remove.Replace("\\", "/"));
                    Console.WriteLine("AddRemoveList: ipfName(" + name + ") file(" + _remove.Replace("\\", "/") + ")");
                }
            }
        }

        private void ServerFullCheck_Click(object sender, RoutedEventArgs e)
        {
            if (Process.GetProcessesByName("ldManagerHashCode").Length > 0)
            {
                MessageBox.Show("Doing make hashCode. have to shut down program");
                return;
            }

            string keyFile = SaveData.serverfolder + "ldManagerHashCode.exe";
            if (File.Exists(keyFile) == false)
            {
                MessageBox.Show("[ldManagerHashCode.exe] cannot find in server_relase folder. plz check file ");
                return;
            }

            RemoveHashKey();
            DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
            string arg = "-ROOT " + Directory.GetParent(info.FullName).FullName + " -CHECKPATH ROOT" + " -CUR " + Directory.GetCurrentDirectory();
            System.Diagnostics.Process.Start(keyFile, arg);
        }

        private void ServerPack_MakeHashKey()
        {
            if (Process.GetProcessesByName("ldManagerHashCode").Length > 0)
            {
                MessageBox.Show("Doing make hashCode. have to shut down program");
                return;
            }

            string keyFile = SaveData.serverfolder + "ldManagerHashCode.exe";
            if (File.Exists(keyFile) == false)
            {
                MessageBox.Show("[ldManagerHashCode.exe] cannot find in server_relase folder. plz check file ");
                return;
            }

            RemoveHashKey();
            DirectoryInfo info = Directory.GetParent(Directory.GetCurrentDirectory());
            string arg = "-ROOT " + Directory.GetParent(info.FullName).FullName + " -CHECKPATH " + SaveData.patchfolder + " -CUR " + Directory.GetCurrentDirectory();
            System.Diagnostics.Process.Start(keyFile, arg);
        }
        #endregion
    }
}
