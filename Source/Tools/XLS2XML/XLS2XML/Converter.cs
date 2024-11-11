using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace XLS2XML
{
    class Converter
    {
        public ExcelReader m_excelreader = new ExcelReader();

        private String m_currentdirectory;
        private String m_opendirectory;
        private String m_savedirectory;
        private String m_subdirectory;
        private String m_openfile;
        private String m_savefile;

        private int m_startcolumn;
        private String m_oldcategory;
        private String m_newcategory;

        private List<String> m_menulist = new List<String>();

        public void StartConvert(String open, String target)
        {
            SetDirectoryInfo(open, target);
            Console.WriteLine("*********** " + m_openfile + " ***********");

            try
            {
                m_excelreader.ReadExcel(m_opendirectory);

                Int32 count = m_excelreader.GetSheetCount();

                for (Int32 i = 0; i < count; ++i)
                {
                    String sheetname = m_excelreader.GetSheetName(i + 1);

                    if (sheetname.StartsWith("_")) { }	        // 참고용 sheet
                    else if (sheetname.StartsWith("sheet")) { }	// 참고용 sheet
                    else if (sheetname.StartsWith("Sheet")) { }	// 참고용 sheet
                    else	                                    // 실제 데이터가 담긴 sheet
                    {
                        SheetConvert();
                    }
                }
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            Console.WriteLine("*********** Complete ***********");
            m_excelreader.CloseWorkbooks();
        }

        public void SheetConvert()
        {
            String savename = m_excelreader.GetSubName();
            if (savename.Length == 0)
                return;

            SetSubDirectory(savename);
            if (CheckTime())
                StartXLS2XML();
        }

        public void SetDirectoryInfo(String open, String target)
        {
            m_currentdirectory = Directory.GetCurrentDirectory();

            m_opendirectory = SearchDirectory(open);
            m_openfile = Path.GetFileName(m_opendirectory);

            m_savedirectory = SearchDirectory(target);
        }

        private String SearchDirectory(String _folder)
        {
            String path = _folder;
            DirectoryInfo dinfo;

            if (path.StartsWith("../") || path.StartsWith("..\\"))
            {
                dinfo = Directory.GetParent(m_currentdirectory);
                path = path.Substring(3);

                while (true)
                {
                    if (path.StartsWith("../") || path.StartsWith("..\\"))
                    {
                        dinfo = Directory.GetParent(dinfo.FullName);
                        path = path.Substring(3);
                    }
                    else
                    {
                        return dinfo.FullName + "\\" + path;
                    }
                }
            }
            else if (path.IndexOf(":") != -1)
            {
                return path;
            }
            else
            {
                return m_currentdirectory + path;
            }
        }

        private void SetSubDirectory(String sheetname)
        {
            String path = "";

            if (sheetname.StartsWith("\\"))
            {
                sheetname = sheetname.Substring(1);
                Int32 find = sheetname.IndexOf("\\");
                while (find != -1)
                {
                    path += "\\";
                    path += sheetname.Substring(0, find);
                    sheetname = sheetname.Substring(find + 1);

                    find = sheetname.IndexOf("\\");
                }
            }

            if (sheetname.IndexOf(".") != -1) m_savefile = sheetname;
            else m_savefile = sheetname.ToLower() + ".xml";

            m_subdirectory = m_savedirectory + path;
            if (m_subdirectory.Length > 0)
                m_subdirectory = m_subdirectory + "\\" + m_savefile;
        }

        public bool CheckTime()
        {
            DateTime exceldate = Directory.GetLastWriteTime(m_opendirectory);
            DateTime xmldate = Directory.GetLastWriteTime(m_subdirectory);

            if (exceldate > xmldate)
                return true;

            return false;
        }

        public void StartXLS2XML()
        {
            m_startcolumn = 2;
            XMLWriter xmlwriter = null;
            bool bcategory = false;

            try
            {
                Console.Write(m_savefile);
                xmlwriter = new XMLWriter(m_subdirectory);
                xmlwriter.WriteStartDocument();
                xmlwriter.WriteStartIdspace(m_excelreader.GetCellData(2, 1));

                bcategory = SetCategory(xmlwriter);
                SetMenu(m_startcolumn);

                int rows = m_excelreader.GetRowsCount() + 1;
                for (int rowcount = 2; rowcount < rows; ++rowcount)
                {
                    if (bcategory) CheckCategory(xmlwriter, rowcount);
                    WriteRow(xmlwriter, rowcount);
                }

                xmlwriter.WriteEndCategory();
                xmlwriter.WriteEndIdspace();
                xmlwriter.WriteEndDocument();

                m_menulist.Clear();
                xmlwriter.Close();
                Console.WriteLine("Complete");
            }
            catch (Exception ex)
            {
                m_menulist.Clear();

                if (xmlwriter != null)
                {
                    xmlwriter.Close();
                    File.Delete(m_subdirectory);
                }

                throw ex;
            }
        }

        private bool SetCategory(XMLWriter xmlwriter)
        {
            if (m_excelreader.GetCellData(1, m_startcolumn).StartsWith("Category"))
            {
                ++m_startcolumn;
                m_excelreader.SortExcel();
                xmlwriter.WriteStartCategory(m_oldcategory = m_excelreader.GetCellData(2, 2));
                return true;
            }
            else
            {
                xmlwriter.WriteStartCategory();
                return false;
            }
        }

        private void SetMenu(int startcolumn)
        {
            m_menulist.Clear();

            int columns = m_excelreader.GetColumnCount() + 1;

            for (int count = startcolumn; count < columns; ++count)
            {
                m_menulist.Add(m_excelreader.GetCellData(1, count));
            }
        }

        private void WriteRow(XMLWriter xmlwriter, Int32 Row)
        {
            Int32 _Column = m_startcolumn;
            Console.Write(".");
            xmlwriter.WriteStartElement();

            foreach (String menu in m_menulist)
            {
                String value;
                value = m_excelreader.GetCellData(Row, _Column);

                if (0 == value.Length)
                    continue;

                xmlwriter.WriteAddEliemetData(menu, m_excelreader.GetCellData(Row, _Column));
                ++_Column;
            }

            xmlwriter.WriteEndElement();
        }

        private void CheckCategory(XMLWriter xmlwriter, Int32 Row)
        {
            m_newcategory = m_excelreader.GetCellData(Row, 2);

            if (m_newcategory != m_oldcategory)
            {
                m_oldcategory = m_newcategory;
                xmlwriter.WriteEndCategory();
                xmlwriter.WriteStartCategory(m_oldcategory);
            }
        }
    }
}
