using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.IO;
using System.Windows.Threading;
using System.Diagnostics;

namespace G2PatchMaker
{
	/// <summary>
	/// Interaction logic for client.xaml
	/// </summary>
	public partial class client : UserControl
	{
		public client()
		{
			this.InitializeComponent();
            this.InitializeUI();
		}

        public delegate void ChangeListBoxString(String name);
        
        public ChangeListBoxString addClientData = null;
        public ChangeListBoxString addClientRelease = null;

        public ChangeListBoxString removeClientData = null;
        public ChangeListBoxString removeClientRelease = null;

        public void _removeClientData(String name)
        {
            int index = dataListBox.Items.IndexOf(name);
            if (index != -1)
            {
                dataListBox.Items.RemoveAt(index);

                if (dataListBox.Items.Count != 0)
                {
                    if (dataListBox.Items.Count == index)
                        dataListBox.SelectedIndex = index - 1;
                    else
                        dataListBox.SelectedIndex = index;
                }
            }
        }

        public void _addClientData(String name)
        {
            dataListBox.Items.Add(name);
        }

        public void _removeClientRelease(String name)
        {
            int index = ReleaseListBox.Items.IndexOf(name);
            if (index != -1)
            {
                ReleaseListBox.Items.RemoveAt(index);

                if (ReleaseListBox.Items.Count != 0)
                {
                    if (ReleaseListBox.Items.Count == index)
                        ReleaseListBox.SelectedIndex = index - 1;
                    else
                        ReleaseListBox.SelectedIndex = index;
                }
            }
        }

        public void _addClientRelease(String name)
        {
            ReleaseListBox.Items.Add(name);
        }

        #region 

        void InitializeUI()
        {
            dataListBox.AllowDrop = true;
            ReleaseListBox.AllowDrop = true;

            if (SaveData.read)
            {
                foreach (String name in SaveData.data)
                {
                    dataListBox.Items.Add(name);
                }

                foreach (String name in SaveData.release)
                {
                    ReleaseListBox.Items.Add(name);
                }
            }
            else
            {
                String[] names = Directory.GetDirectories(SaveData.datafolder);

                foreach (String name in names)
                {
                    if (name.IndexOf("svn") == -1)
                    {
                        dataListBox.Items.Add(name.Replace(SaveData.datafolder, ""));
                    }
                }
            }
        }

        void DataPartialBt_Click(object sender, RoutedEventArgs e)
        {
            List<String> datalist = new List<String>();
            foreach (String name in dataListBox.Items)
                datalist.Add(name);

            Window1.myWin.MakeIpfPartPatch("data", datalist);

            List<String> releaselist = new List<String>();
            foreach (String name in ReleaseListBox.Items)
                releaselist.Add(name);

            Window1.myWin.MakePakPatch("release", releaselist);
        }

        void DataFullBt_Click(object sender, RoutedEventArgs e)
        {
            List<String> datalist = new List<String>();
            foreach (String name in dataListBox.Items)
                datalist.Add(name);

            Window1.myWin.MakeIpfFullPatch("data", datalist);

            List<String> releaselist = new List<String>();
            foreach (String name in ReleaseListBox.Items)
                releaselist.Add(name);

            Window1.myWin.MakePakPatch("release", releaselist);
        }

        void DataDiffBt_Click(object sender, RoutedEventArgs e)
        {
            Window1.myWin.CreateFolder("\\patch", false);

            if (Directory.Exists(SaveData.patchfolder))
                Directory.Delete(SaveData.patchfolder, true);

            Directory.CreateDirectory(SaveData.patchfolder);
            Directory.CreateDirectory(SaveData.patchfolder + "\\data\\");
            Directory.CreateDirectory(SaveData.patchfolder + "\\release\\");

            List<String> output = new List<String>();
            String textname = SaveData.patchfolder + "\\data.txt";

            int version = Convert.ToInt32(Window1.myWin.SVN_StartVersion) - 1;
            output.Add("svn diff -r " + version + ":" + Window1.myWin.SVN_EndVersion
                + " " + SaveData.datafolder + " --summarize >" + textname + Environment.NewLine);
            output.Add("svn update -r " + Window1.myWin.SVN_EndVersion + " "
                + SaveData.datafolder + " " + SaveData.datafolder + Environment.NewLine);
            output.Add(textname);
            output.Add(SaveData.datafolder);
            output.Add(SaveData.patchfolder + "\\data\\");
            output.Add("data");

            textname = SaveData.patchfolder + "\\release.txt";
            output.Add("svn diff -r " + version + ":" + Window1.myWin.SVN_EndVersion
                + " " + SaveData.releasefolder + " --summarize >" + textname + Environment.NewLine);
            output.Add("svn update -r " + Window1.myWin.SVN_EndVersion + " "
                + SaveData.releasefolder + " " + SaveData.releasefolder + Environment.NewLine);
            output.Add(textname);
            output.Add(SaveData.releasefolder);
            output.Add(SaveData.patchfolder + "\\release\\");
            output.Add("release");

            Window1.myWin.StartDiff(output.ToArray());
        }

        void DataExprotBt_Click(object sender, RoutedEventArgs e)
        {
            Window1.myWin.CreateFolder("\\patch", false);

            if (Directory.Exists(SaveData.patchfolder))
                Directory.Delete(SaveData.patchfolder, true);

            Directory.CreateDirectory(SaveData.patchfolder);
            

            List<String> msgList = new List<string>();
            if (Window1.myWin.SVN_EndVersion.Equals("0"))
                msgList.Add("svn export " + SaveData.datafolder + " " +
                    SaveData.patchfolder + "\\data\\" + Environment.NewLine);
            else
                msgList.Add("svn export -r " + Window1.myWin.SVN_EndVersion + " " +
                    SaveData.datafolder + " " + SaveData.patchfolder + "\\data\\" + Environment.NewLine);

            if (Window1.myWin.SVN_EndVersion.Equals("0"))
                msgList.Add("svn export " + SaveData.releasefolder + " " +
                    SaveData.patchfolder + "\\release\\" + Environment.NewLine);
            else
                msgList.Add("svn export -r " + Window1.myWin.SVN_EndVersion + " " +
                    SaveData.releasefolder + " " +  SaveData.patchfolder + "\\release\\" + Environment.NewLine);

            Window1.myWin.StartExport(msgList.ToArray());
        }

        void dataListBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                if (dataListBox.SelectedIndex != -1)
                {
                    if (removeClientData != null)
                        removeClientData(dataListBox.SelectedItem as String);
                }
            }
        }

        void dataListBox_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("FileName"))
            {
                String[] names = e.Data.GetData(DataFormats.FileDrop, true) as String[];
                foreach (String name in names)
                {
                    if (name.IndexOf(SaveData.datafolder, 0) != -1)
                    {
                        if (dataListBox.Items.IndexOf(name.Replace(SaveData.datafolder, "")) == -1)
                        {
                            if (addClientData != null)
                                addClientData(name.Replace(SaveData.datafolder, ""));
                        }
                    }
                }
            }
        }

        private void ReleaseListBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                if (ReleaseListBox.SelectedIndex != -1)
                {
                    if (removeClientRelease != null)
                        removeClientRelease(ReleaseListBox.SelectedItem as String);
                }
            }
        }

        private void ReleaseListBox_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("FileName"))
            {
                String[] names = e.Data.GetData(DataFormats.FileDrop, true) as String[];
                foreach (String name in names)
                {
                    if (name.IndexOf(SaveData.releasefolder, 0) != -1)
                    {
                        if (ReleaseListBox.Items.IndexOf(name.Replace(SaveData.releasefolder, "")) == -1)
                        {
                            if (addClientRelease != null)
                                addClientRelease(name.Replace(SaveData.releasefolder, ""));
                        }
                    }
                }
            }
        }

        #endregion
    }
}