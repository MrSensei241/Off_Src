#include "stdafx.h"
#include "Option.h"
#include "XML.h"
#include "Updater.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Option {
	bool windowMode;
	bool directlyRun;

	void Init()
	{
		windowMode = false;
		directlyRun = false;

		CStdioFile file;
		if (FALSE == file.Open(g_gameRootPath + "release/user.xml", CFile::modeRead)) {
			return;
		}

		CString xml, temp;
		while (TRUE) {
			if (FALSE == file.ReadString(temp))
				break;
			xml += temp;
		}

		file.Close();

		XML::PARSEINFO info;
		XML::Node root;
		root.Load(xml, &info);
		
		if (info.erorr_occur) {
			root.Close();
			return;
		}

		XML::Attr* windowModeAttr = root.GetChildAttr("Mode", "WindowMode");
		
		if (NULL == windowModeAttr) {
			root.Close();
			return;
		}

		if (-1 != windowModeAttr->value.Find("1"))
			windowMode = true;

		root.Close();
	}
}
