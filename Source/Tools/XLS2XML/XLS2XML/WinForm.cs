using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace XLS2XML
{
    public partial class WinForm : Form
    {
        private Int32 m_sheetcount;
        
        static WaitHandle[] waitHandles = new WaitHandle[] 
        {
            new AutoResetEvent(false),
            new AutoResetEvent(false)
        };

        public WinForm()
        {
            InitializeComponent();
        }

        private void OpenFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                Program.g_open = openFileDialog.FileName;
                OpenText.Text = Program.g_open;
                textBox1.Text = null;
                progressBar.Visible = false;

                try
                {
                    Program.g_converter.m_excelreader.ReadExcel(Program.g_open);

                    m_sheetcount = Program.g_converter.m_excelreader.GetSheetCount();

                    for (Int32 i = 0; i < m_sheetcount; ++i)
                    {
                        String sheetname = Program.g_converter.m_excelreader.GetSheetName(i + 1);
                        SheetList.Items.Add(sheetname);
                    }
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }

                Program.g_converter.m_excelreader.CloseWorkbooks();
            }
        }

        private void SaveFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog FolderBrowserDlg = new FolderBrowserDialog();

            if (FolderBrowserDlg.ShowDialog() == DialogResult.OK)
            {
                Program.g_target = FolderBrowserDlg.SelectedPath;
                SaveText.Text = Program.g_target;
                textBox1.Text = null;
                progressBar.Visible = false;
            }
        }

        private void Convert_Click(object sender, EventArgs e)
        {
            if (Program.g_open.Length == 0)
                return;

            try
            {
                Program.g_converter.SetDirectoryInfo(Program.g_open, Program.g_target);
                Program.g_converter.m_excelreader.ReadExcel(Program.g_open);

                progressBar.Visible = true;
                progressBar.Maximum = m_sheetcount;
                progressBar.Minimum = 1;
                progressBar.Value = 1;
                progressBar.Step = 1;

                for (Int32 i = 0; i < m_sheetcount; ++i)
                {
                    String sheetname = Program.g_converter.m_excelreader.GetSheetName(i + 1);
                    textBox1.Text = sheetname;
                    this.Update();
                    progressBar.PerformStep();

                    Thread th = new Thread(new ParameterizedThreadStart(ThreadConvert));
                    th.SetApartmentState(ApartmentState.STA);
                    th.Start(sheetname);

                    WaitHandle.WaitAny(waitHandles);
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

            Program.g_converter.m_excelreader.CloseWorkbooks();
            
            MessageBox.Show("       완료");

            Program.g_open = "";
            OpenText.Text = Program.g_open;
            textBox1.Text = null;
            progressBar.Visible = false;
            SheetList.Items.Clear();
        }

        private void ThreadConvert(object data)
        {
            try
            {
                String sheetname = (String)data;

                if (sheetname.StartsWith("_")) { }	        // 참고용 sheet
                else if (sheetname.StartsWith("sheet")) { }	// 참고용 sheet
                else if (sheetname.StartsWith("Sheet")) { }	// 참고용 sheet
                else	                                    // 실제 데이터가 담긴 sheet
                {
                    Program.g_converter.SheetConvert();
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
            }

            ((AutoResetEvent)waitHandles[0]).Set();
        }
    }
}
