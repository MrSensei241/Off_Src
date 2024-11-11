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
using System.Windows.Shapes;
using System.IO;

namespace G2PatchMaker
{
	/// <summary>
	/// Interaction logic for ShowIpfDialog.xaml
	/// </summary>
	public partial class ShowIpfDialog : Window
	{
		public ShowIpfDialog()
		{
			this.InitializeComponent();
            this.InitializeUI();
		}

        imc.IPF m_ipf = null;

        void InitializeUI()
        {
            IpfTreeView.AllowDrop = true;

            AddFileBt.Click += new RoutedEventHandler(AddFileBt_Click);
            SaveBt.Click += new RoutedEventHandler(SaveBt_Click);
            ExtractBt.Click += new RoutedEventHandler(ExtractBt_Click);
        }

        void AddFileBt_Click(object sender, RoutedEventArgs e)
        {
        }

        private void SaveBt_Click(object sender, RoutedEventArgs e)
        {
            if (m_ipf != null)
            {
                try
                {
                    m_ipf.ChangeVersion(ipfFileName, Convert.ToInt32(StartVersion.Text), Convert.ToInt32(EndVersion.Text));
                }
                catch (System.Exception) {}
            }
        }

        private void ExtractBt_Click(object sender, RoutedEventArgs e)
        {
            if (m_ipf != null)
            {
                try
                {
                    Window1.myWin.CreateFolder("\\patch", false);

                    if (Directory.Exists(Directory.GetCurrentDirectory() + "\\patch\\Extract"))
                        Directory.Delete(Directory.GetCurrentDirectory() + "\\patch\\Extract", true);

                    Directory.CreateDirectory(Directory.GetCurrentDirectory() + "\\patch\\Extract");

                    m_ipf.ExtractAllToFile(Directory.GetCurrentDirectory() + "\\patch\\Extract");
                }
                catch (System.Exception) { }
            }
        }

        void AddFile(String filenname)
        {
            //m_ipf.AddFileFromFile(
            //    , System.IO.Path.GetFileNameWithoutExtension(filename));
        }

        void ClearIPF()
        {
            treeItems.Clear();
            IpfTreeView.Items.Clear();

            if (m_ipf != null)
            {
                m_ipf.CloseIpf();
                StartVersion.Text = "";
                EndVersion.Text = "";
            }
        }

        Dictionary<String, TreeViewItem> treeItems = new Dictionary<String, TreeViewItem>();
        String ipfFileName = "";
        private void IpfTreeView_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("FileName"))
            {
                String[] names = e.Data.GetData(DataFormats.FileDrop, true) as String[];
                foreach (String name in names)
                {
                    if ((name.IndexOf(".server") != -1) || (name.IndexOf(".ipf") != -1))
                    {
                        ClearIPF();

                        m_ipf = new imc.IPF();
                        if (m_ipf.LoadPartialIpf(name))
                        {
                            ipfFileName = name;
                            StartVersion.Visibility = Visibility.Visible;
                            EndVersion.Visibility = Visibility.Visible;
                            SaveBt.Visibility = Visibility.Visible;

                            StartVersion.Text = m_ipf.GetStartVersion().ToString();
                            EndVersion.Text = m_ipf.GetEndVersion().ToString();

                            List<String> _removelist = m_ipf.GetRemoveDataNames();
                            TreeViewItem removetvi = new TreeViewItem();
                            removetvi.Header = "_Remove_";
                            IpfTreeView.Items.Add(removetvi);

                            foreach (String filename in _removelist)
                            {
                                TreeViewItem _removefiletvi = new TreeViewItem();
                                _removefiletvi.Header = filename;
                                removetvi.Items.Add(_removefiletvi);
                            }

                            List<String> _list = m_ipf.GetDataNames();
                            foreach (String filename in _list)
                            {
                                TreeViewItem tvi = new TreeViewItem();
                                tvi.Header = System.IO.Path.GetFileNameWithoutExtension(filename);
                                IpfTreeView.Items.Add(tvi);
                                treeItems.Add(System.IO.Path.GetFileNameWithoutExtension(filename), tvi);

                                List<String> _sublist = m_ipf.GetDataNames(filename);
                                foreach (String subname in _sublist)
                                    SetTreeList(subname, tvi, "");
                            }
                        }
                    }
                    else if (name.IndexOf(".pak") != -1)
                    {
                        ClearIPF();

                        m_ipf = new imc.IPF();
                        if (m_ipf.LoadReleaseIpf(name))
                        {
                            ipfFileName = name;
                            StartVersion.Visibility = Visibility.Hidden;
                            EndVersion.Visibility = Visibility.Hidden;
                            SaveBt.Visibility = Visibility.Hidden;

                            String temp = System.IO.Path.GetFileNameWithoutExtension(name);
                            temp = temp.Substring(0, temp.IndexOf("_"));
                            temp = "pak_" + temp;
                            TreeViewItem tvi = new TreeViewItem();
                            tvi.Header = temp;
                            IpfTreeView.Items.Add(tvi);
                            treeItems.Add(temp, tvi);

                            List<String> _list = m_ipf.GetDataNames();
                            foreach (String filename in _list)
                                SetTreeList(filename, tvi, "");
                        }
                    }

                    return;
                }
            }
        }

        #region TreeSetting

        void SetTreeList(String name, TreeViewItem tvi, String temp)
        {
            if (name.IndexOf("/") != -1)
            {
                String directoryname = new String(name.ToCharArray(0, name.IndexOf("/")));

                if (treeItems.ContainsKey(temp + directoryname))
                {
                    name = name.Substring(name.IndexOf("/") + 1);
                    SetTreeList(name, treeItems[temp + directoryname], temp + directoryname);
                }
                else
                {
                    TreeViewItem subtvi = new TreeViewItem();
                    subtvi.Header = directoryname;

                    treeItems.Add(temp + directoryname, subtvi);
                    tvi.Items.Add(subtvi);

                    name = name.Substring(name.IndexOf("/") + 1);
                    SetTreeList(name, subtvi, temp + directoryname);
                }
            }
            else
            {
                TreeViewItem subtvi = new TreeViewItem();
                subtvi.Header = name;

                tvi.Items.Add(subtvi);
            }
        }

        void SetChildTree(TreeViewItem tvi, String path)
        {
            String[] directories = Directory.GetDirectories(path);
            String[] files = Directory.GetFiles(path);

            foreach (String childname in directories)
            {
                TreeViewItem newtvi = new TreeViewItem();
                newtvi.Header = childname.Replace(path + "\\", "");
                tvi.Items.Add(newtvi);

                SetChildTree(tvi, childname);
            }

            foreach (String childname in files)
            {
                TreeViewItem newtvi = new TreeViewItem();
                newtvi.Header = childname.Replace(path + "\\", "");
                tvi.Items.Add(newtvi);
            }
        }

        #endregion
	}
}