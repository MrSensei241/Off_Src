#include "stdafx.h"
#include "Revision.h"
#include "Updater.h"
#include "Config.h"

#define	UPDATER_REVISION_FILE_NAME "updater.revision.txt"

namespace Revision {
	DWORD Get()
	{
		CString temp;
		CStdioFile file;
		try {
			FATAL_ERROR(TRUE==file.Open(g_fullPath + UPDATER_REVISION_FILE_NAME, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate), THIS_FILE, __LINE__, Config::String::updaterRevisionTxtNotExist);
			if (FALSE == file.ReadString(temp)) {
				temp = "0";
				file.WriteString("0");
			}
		} catch (CFileException* e) {
			file.Close();
			e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			e->Delete();
			FATAL_ERROR(FALSE, THIS_FILE, __LINE__, Config::String::updaterRevisionTxtNotExist);
		}
		file.Close();
		return atoi(temp);
	}

	void Set(DWORD revision)
	{
		CStdioFile file;
		try {
			FATAL_ERROR(TRUE==file.Open(g_fullPath + UPDATER_REVISION_FILE_NAME, CFile::modeWrite | CFile::modeCreate), THIS_FILE, __LINE__, Config::String::updaterRevisionTxtNotExist);
			CString temp;
			temp.Format("%d", revision);
			file.WriteString(temp);
		} catch (CFileException* e) {
			file.Close();
			e->ReportError(MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			e->Delete();
			FATAL_ERROR(FALSE, THIS_FILE, __LINE__, Config::String::updaterRevisionTxtNotExist);
		}
		file.Close();
	}
}
