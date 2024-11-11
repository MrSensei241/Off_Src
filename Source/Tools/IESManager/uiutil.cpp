#include "stdafx.h"

#include "PaneManager.h"
#include "uiutil.h"

namespace uiutil
{
	void BuildIDSpaceTree(CTreeCtrl& tree, imc::CStringID idSpace)
	{
		const char * pgroupkeyname = idSpace.c_str();
		HTREEITEM sounditem = tree.InsertItem(pgroupkeyname);
		tree.SetItemData(sounditem, 0);

		imcIES::IClassList* list = imcIES::GetClassList(idSpace);
		if (list == NULL)
			return;

		int cnt = list->Count();
		for (int i = 0 ; i < cnt ; i ++)
		{
			imcIES::IClass* cls = list->GetByIndex(i);
			const char * name = GetClassReadName(cls).c_str();
			HTREEITEM clasName = tree.InsertItem(name, sounditem);
			tree.SetItemData(clasName, cls->GetID());

		}
	}

	imc::CStringID GetEditText(CEdit& edit)
	{
		CString cstr;
		edit.GetWindowText(cstr);
		return imc::CStringID(cstr);
	}
};

