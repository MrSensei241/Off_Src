using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Office.Interop.Excel;

namespace XLS2XML
{
    class ExcelReader
    {
        private static Microsoft.Office.Interop.Excel.Application m_excel = null;
        private Workbook m_workbook = null;
        private Worksheet m_worksheet = null;
        private Range m_range = null;

        public static void Initialize()
        {
            try
            {
                m_excel = new Microsoft.Office.Interop.Excel.Application();
            }
            catch (System.Exception)
            {
                Console.WriteLine("Excel이 설치되어 있지 않습니다.");
                Environment.Exit(0);
            }
        }

        public static void Terminate()
        {
            if (null != m_excel)
            {
                m_excel.Quit();
                m_excel = null;
            }
        }

        public void CloseWorkbooks()
        {
            if (null != m_workbook)
            {
                m_workbook.Close(false, Missing.Value, Missing.Value);
                m_workbook = null;
                m_excel.Workbooks.Close();

                GC.Collect();
            }
        }

        public void ReadExcel(String filename)
        {
            try
            {
                m_workbook = m_excel.Workbooks.Open(filename,
                    Missing.Value, Missing.Value, Missing.Value,
                    Missing.Value, Missing.Value, Missing.Value,
                    XlSaveAsAccessMode.xlNoChange, Missing.Value,
                    Missing.Value, Missing.Value, Missing.Value,
                    Missing.Value, Missing.Value, Missing.Value);
            }
            catch (System.Exception)
            {
                SystemException ex = new SystemException(filename + " 파일을 열수 없습니다.");
                throw ex;
            }
        }

        public void SortExcel()
        {
            m_range.Sort(m_range.Columns[1, Missing.Value], XlSortOrder.xlAscending,
                        m_range.Columns[2, Missing.Value], Missing.Value, XlSortOrder.xlAscending,
                        Missing.Value, XlSortOrder.xlAscending, XlYesNoGuess.xlNo,
                        Missing.Value, Missing.Value, XlSortOrientation.xlSortColumns,
                        XlSortMethod.xlPinYin, XlSortDataOption.xlSortNormal,
                        XlSortDataOption.xlSortNormal, XlSortDataOption.xlSortNormal);
        }

        public Int32 GetSheetCount()
        {
            return m_workbook.Worksheets.Count;
        }

        public String GetSheetName(int sheetnum)
        {
            m_worksheet = m_workbook.Worksheets[sheetnum] as Worksheet;
            m_range = m_worksheet.get_Range("A1", Missing.Value).CurrentRegion;

            return m_worksheet.Name;
        }

        public String GetSubName()
        {
            if ( m_range.ListObject != null )
                return m_range.ListObject.Name;

            return "";
        }

        public Int32 GetRowsCount()
        {
            return m_range.Rows.Count;
        }

        public Int32 GetColumnCount()
        {
            return m_range.Columns.Count;
        }

        public String GetCellData(int row, int column)
        {
            Range cell = m_range.get_Item(row, column) as Range;

            if (cell.Validation.Value == false)
            {
                SystemException ex = new SystemException("유효하지 않은 데이터가 있습니다. : " + GetCell_ID(column) + row);
                throw ex;
            }

            return cell.Text as String;
        }

        private String GetCell_ID(int num)
        {
            String str = "";
            char c = 'A';

            if (num > 676)
            {
                c += (char)((num / 676) - 1);
                str += c;
            }

            if (num > 26)
            {
                c += (char)(((num % 676) / 26) - 1);
                str += c;
            }

            c += (char)((num % 26) - 1);
            str += c;

            return str;
        }
    }
}
