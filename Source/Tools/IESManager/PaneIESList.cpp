#include "StdAfx.h"
#include "PaneIESList.h"
#include "uiutil.h"
#include "toolnet.h"
#include "geIESModify.h"
#include "GeSession.h"


CPaneIESList::CPaneIESList() : CDialog(CPaneIESList::IDD)
{
	m_class = NULL;
	m_useSort = FALSE;
	m_isAcsend = TRUE;
}

BOOL CPaneIESList::OnInitDialog()
{
	CDialog::OnInitDialog();

	imc::CVector<imc::CStringID> idSpaces;
	imcIES::IClassList* list = imcIES::GetClassList("IESManager");
	for (int i = 0 ; i < list->Count() ; i++)
	{
		imcIES::IClass* iclass = list->GetByIndex(i);
		imc::CStringID  idspc;
		iclass->Get("IDSpace", idspc);
		if (idSpaces.Find(idspc) == idSpaces.InvalidIndex()){
			idSpaces.AddToTail(idspc);
		}
	}

	for (int i = 0; i < idSpaces.Count();  i++)
	{
		uiutil::BuildIDSpaceTree(m_tree, idSpaces.Element(i).c_str());
	}

	InitListHead();
	InitCheckBoxes();

	return TRUE;
}

void CPaneIESList::InitCheckBoxes()
{
	m_db.SetCheck(TRUE);
	m_rollback.SetCheck(TRUE);
	m_noapply.SetCheck(FALSE);
}

void CPaneIESList::InitListHead()
{
	m_allHistory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_allHistory.InsertColumn(0, "리비전", LVCFMT_LEFT, 80);
	m_allHistory.InsertColumn(1, "이름", LVCFMT_LEFT, 120);
	m_allHistory.InsertColumn(2, "속성", LVCFMT_LEFT, 150);
	m_allHistory.InsertColumn(3, "변경값", LVCFMT_LEFT, 400);

	m_curCls.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_curCls.InsertColumn(0, "속성", LVCFMT_LEFT, 0);
	m_curCls.InsertColumn(1, "별칭", LVCFMT_LEFT, 120);
	m_curCls.InsertColumn(2, "값", LVCFMT_LEFT, 200);

	m_curLog.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_curLog.InsertColumn(0, "리비전", LVCFMT_LEFT, 30);
	m_curLog.InsertColumn(1, "속성", LVCFMT_LEFT, 120);
	m_curLog.InsertColumn(2, "로그", LVCFMT_LEFT, 300);
}


void CPaneIESList::UpdateAll()
{
	UpdateAllHistory();
	UpdateCategory();
	UpdateProperty();
	UpdateCurrentLog();
}

void CPaneIESList::UpdateAllHistory()
{
	m_allHistory.DeleteAllItems();

	IES_HISTORY& history = Session().M_IES_HISTORY;
	char buf[512];
	int idx = 0;
	for (int i = history.FirstInorder() ; i != history.InvalidIndex() ; i = history.NextInorder(i))
	{
		IES_IDSPACE_HISTORY* his = history.Element(i);
		imc::CStringID idSpace = history.Key(i);
		m_allHistory.InsertItem(idx, "[IDSPACE]");
		sprintf(buf, "[%s]", idSpace.c_str());
		m_allHistory.SetItemText(idx, 1, buf);
		idx++;

		for (int j = his->Head() ; j != his->InvalidIndex() ; j = his->Next(j))
		{
			IES_OBJ_HISTORY* objHis = his->Element(j);
			int classID = his->Key(j);
			for (int k = objHis->FirstInorder() ; k != objHis->InvalidIndex() ; k = objHis->NextInorder(k))
			{
				IES_VALUE_HISTORY* valList = objHis->Element(k);
				imc::CStringID propName = objHis->Key(k);
				for (int l = 0 ; l < valList->Count() ; l++)
				{
					const IES_MANAGER_EDIT_INFO& info = valList->Element(l);
					sprintf(buf, "%d", info.revision);
					m_allHistory.InsertItem(idx, buf);
					if (l == 0){
						imcIES::IClass* pClass = imcIES::GetClass(idSpace, classID);
						m_allHistory.SetItemText(idx, 1, GetClassReadName(pClass).c_str());
						m_allHistory.SetItemText(idx, 2, propName.c_str());
					} else {
						m_allHistory.SetItemText(idx, 1, "");
						m_allHistory.SetItemText(idx, 2, "");
					}
					info.GetLogText(buf);
					m_allHistory.SetItemText(idx, 3, buf);
					idx++;
				}
			}
		}
	}
}

void CPaneIESList::UpdateCategory()
{
	imcIES::IClassList* list = imcIES::GetClassList("IESManager");
	if (list == NULL)
		return;

	imc::CVector<imc::CStringID> cateList;
	for (int i = 0 ; i < list->Count() ; i++)
	{
		imcIES::IClass* iclass = list->GetByIndex(i);
		imc::CStringID  idspc;
		imc::CStringID  Cate;
		iclass->Get("IDSpace", idspc);
		iclass->Get("Cate", Cate);
		if (idspc ==  "All" || idspc == m_idSpace)
		{
			if (cateList.InvalidIndex() == cateList.Find(Cate))
				cateList.AddToTail(Cate);
		}
	}

	m_category.ResetContent();
	m_category.AddString("ALL");
	for (int i = 0 ; i < cateList.Count() ; i ++)
	{
		m_category.AddString(cateList.Element(i).c_str());
	}

	m_category.AddString("ALLPROPERTY");
	m_category.SetCurSel(0);
		
}

void CPaneIESList::UpdateProperty()
{
	if (m_class == NULL)
		return;

	CString cate;
	m_category.GetWindowText(cate);
	imc::CStringID curCategory = LPCTSTR(cate);
	if (curCategory == "ALLPROPERTY")
		UpdateAllProperty();
	else
		UpdateCategoryProperty(curCategory);

}


void CPaneIESList::UpdateAllProperty()
{
	m_curCls.DeleteAllItems();
	int cnt = m_class->GetPropertyCount();
	int idx = 0;
	for (int i = 0 ; i < cnt ; i ++)
	{
		const imcIES::PROPERTY_ENTRY* pEntry = m_class->GetPropertyEntryByIndex(i);
		char buf[256];
		sprintf(buf, imc::CStringID(pEntry->key).c_str());

		m_curCls.InsertItem(idx, buf);
		m_curCls.SetItemText(idx, 1, buf);
		pEntry->GetStringValue(buf);
		m_curCls.SetItemText(idx, 2, buf);
		idx++;
	}
}

void CPaneIESList::UpdateCategoryProperty(imc::CStringID curCategory)
{
	m_curCls.DeleteAllItems();
	int idx = 0;
	
	imcIES::IClassList* list = imcIES::GetClassList("IESManager");
	if (list == NULL)
		return;

	char buf[1024];
	for (int i = 0 ; i < list->Count() ; i++)
	{
		imcIES::IClass* iclass = list->GetByIndex(i);
		imc::CStringID idspace;
		iclass->Get("IDSpace", idspace);
		if (idspace != "All" && idspace != m_idSpace)
			continue;

		imc::CStringID Cate;
		iclass->Get("Cate", Cate);
		if (curCategory != "ALL" && curCategory != Cate)
			continue;

		imc::CStringID  ViewName;
		iclass->Get("ViewName", ViewName);

		imc::CStringID  PropName;
		iclass->Get("PropName", PropName);

		const imcIES::PROPERTY_ENTRY* pEntry = m_class->GetPropertyEntry(PropName);
		if (pEntry == NULL)
			continue;

		m_curCls.InsertItem(idx, PropName.c_str());
		m_curCls.SetItemText(idx, 1, ViewName.c_str());
		pEntry->GetStringValue(buf);
		m_curCls.SetItemText(idx, 2, buf);
		idx++;
	}
}

void CPaneIESList::UpdateSelectedCls(imc::CStringID idSpace, int classID)
{
	m_idSpace = idSpace;
	m_class = imcIES::GetClass(idSpace, classID);
	if (m_class== NULL)
		return;

	imc::CStringID readName = GetClassReadName(m_class);
	m_readName.SetWindowText(readName.c_str());

	UpdateCategory();
	UpdateProperty();
	UpdateCurrentLog();
}

void CPaneIESList::UpdateCurrentLog()
{
	if (m_class == NULL)
		return;

	m_curLog.DeleteAllItems();

	IES_HISTORY& history = Session().M_IES_HISTORY;
	IES_OBJ_HISTORY* his = iesman::GetObjectHistory(m_idSpace, m_class->GetID(), history);
	if (his == NULL)
		return;

	char buf[512];
	int idx = 0;
	for (int i = his->FirstInorder() ; i != his->InvalidIndex() ; i = his->NextInorder(i))
	{
		imc::CStringID propName = his->Key(i);
		IES_VALUE_HISTORY * valList = his->Element(i);
		for (int j = 0 ; j < valList->Count(); j ++)
		{
			const IES_MANAGER_EDIT_INFO& info = valList->Element(j);
			sprintf(buf, "%d", info.revision);
			m_curLog.InsertItem(idx, buf);
			m_curLog.SetItemText(idx, 1, propName.c_str());
			info.GetLogText(buf);
			m_curLog.SetItemText(idx, 2, buf);
			idx++;
		}
	}

}

void CPaneIESList::DoDataExchange(CDataExchange* pDX)
{
	
	DDX_Control(pDX, IDC_IDSPACES, m_tree);
	DDX_Control(pDX, IDC_ALLHISTORY, m_allHistory);
	DDX_Control(pDX, IDC_PROPERTIES, m_curCls);
	DDX_Control(pDX, IDC_SELECTED_LOG, m_curLog);
	DDX_Control(pDX, IDC_PROPCATEGORY, m_category);
	DDX_Control(pDX, IDC_READNAME, m_readName);
	DDX_Control(pDX, IDC_EDITING_PROP, m_editingProp);
	DDX_Control(pDX, IDC_CHANGE_PROP, m_changeProp);
	DDX_Control(pDX, IDC_EDIT_LOG_INPUT, m_editLogInput);
	DDX_Control(pDX, IDC_APPLYDB, m_db);
	DDX_Control(pDX, IDC_ROLLBACK, m_rollback);
	DDX_Control(pDX, IDC_NOTAPPLY, m_noapply);

	
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPaneIESList, CDialog)


	ON_BN_CLICKED(IDOK, &CPaneIESList::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPaneIESList::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_COMMIT, &CPaneIESList::OnCommit)
	ON_BN_CLICKED(IDC_BUTTON1, &CPaneIESList::OnDelete)
	ON_BN_CLICKED(IDC_BUTTON3, &CPaneIESList::OnSaveXML)
	
	ON_NOTIFY(NM_CLICK, IDC_IDSPACES, &CPaneIESList::OnNMClickIdspaces)
	ON_NOTIFY(TVN_SELCHANGED, IDC_IDSPACES, &CPaneIESList::OnTvnSelchangedIdspaces)
	ON_CBN_SELCHANGE(IDC_PROPCATEGORY, &CPaneIESList::OnCbnSelchangePropcategory)
	ON_NOTIFY(HDN_ITEMCHANGING, 0, &CPaneIESList::OnHdnItemchangingProperties)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROPERTIES, &CPaneIESList::OnLvnItemchangedProperties)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_ALLHISTORY, &CPaneIESList::OnNMCustomdrawAllhistory)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ALLHISTORY, &CPaneIESList::OnLvnColumnclickAllhistory)
END_MESSAGE_MAP()


void CPaneIESList::OnBnClickedOk()
{

}

void CPaneIESList::OnBnClickedCancel()
{

}

void CPaneIESList::OnCommit()
{
	imc::CStringID changeValue = uiutil::GetEditText(m_changeProp);
	imc::CStringID log = uiutil::GetEditText(m_editLogInput);

	net::ChangeIESProp(m_idSpace.c_str(), m_class->GetID(), m_selectedPropName.c_str(), changeValue.c_str(), log.c_str());
}

void CPaneIESList::GetSelectedRevision(INT_LIST* list)
{
	POSITION pos = m_allHistory.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;

	while(pos)
	{
		int item = m_allHistory.GetNextSelectedItem(pos);
		CString str = m_allHistory.GetItemText(item, 0);
		int revision = atoi(str);
		list->AddToTail(revision);
	}
}

void CPaneIESList::OnDelete()
{
	bool db = m_db.GetCheck();
	bool delHis = !m_noapply.GetCheck();
	bool rollback = m_rollback.GetCheck();

	INT_LIST revisionList;
	GetSelectedRevision(&revisionList);
	net::DeleteIESHis(db, delHis, rollback, revisionList);
}

void CPaneIESList::OnSaveXML()
{
	INT_LIST revisionList;
	GetSelectedRevision(&revisionList);
	IES_HISTORY	& history = Session().M_IES_HISTORY;
	int count = iesman::WriteIESHistoryToXML(history, revisionList);
	char buf[256];
	sprintf(buf, "%d개 저장완료", count);
	MessageBox(buf, buf);
}

void CPaneIESList::OnNMClickIdspaces(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void CPaneIESList::OnTvnSelchangedIdspaces(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM item = pNMTreeView->itemNew.hItem;
	if (item == NULL)
		return;

	int iValue = m_tree.GetItemData(item);
	if (iValue == 0)
		return;

	HTREEITEM parent = m_tree.GetParentItem(item);
	imc::CStringID name = m_tree.GetItemText(item);
	imc::CStringID idSpace = m_tree.GetItemText(parent);

	UpdateSelectedCls(idSpace, iValue);
	*pResult = 0;
}

void CPaneIESList::OnCbnSelchangePropcategory()
{
	UpdateProperty();
	UpdateCurrentLog();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CPaneIESList::OnHdnItemchangingProperties(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CPaneIESList::OnLvnItemchangedProperties(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int curItem = pNMLV->iItem;
	int sub = pNMLV->iSubItem;

	imc::CStringID item = m_curCls.GetItemText(curItem, 0);
	imc::CStringID propValue = m_curCls.GetItemText(curItem, 2);
	m_selectedPropName = item;
	char buf[512];
	sprintf(buf, "%s 값변경 : %s ->", m_selectedPropName.c_str(), propValue.c_str());
	m_editingProp.SetWindowText(buf);
	m_changeProp.SetWindowText(propValue.c_str());

	*pResult = 0;
}

void CPaneIESList::OnNMCustomdrawAllhistory(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* v = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if (CDDS_PREPAINT == v->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	} else if (CDDS_ITEMPREPAINT == v->nmcd.dwDrawStage)
	{
		v->clrText = 0xFF00FFFF;
		*pResult = CDRF_DODEFAULT;
	}
	v->clrText = 0xFF00FFFF;
	*pResult = CDRF_DODEFAULT;

}

void CPaneIESList::OnLvnColumnclickAllhistory(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (FALSE == m_useSort)
	{
		m_isAcsend = FALSE;
		m_useSort = TRUE;
	} else 
	{
		m_isAcsend = !m_isAcsend;
		if (FALSE == m_isAcsend)
			m_useSort = FALSE;
	}

	if (m_useSort == FALSE){
		//UpdateAllHistory();
		//return;
	}

	int col = pNMLV->iSubItem;

	int nItem = m_allHistory.GetItemCount();
	CString ** arStr = new CString*[nItem];
	for (int i = 0 ; i < nItem ; i++)
	{
		arStr[i] =  new CString(m_allHistory.GetItemText(i, col));
		m_allHistory.SetItemData(i, (LPARAM) arStr[i]);
	}

	
	// 정렬 관련된 구조체 변수 생성 및 데이터 초기화
	SORT_PARAMS sort_params;
	sort_params.handle = m_allHistory.GetSafeHwnd();
	sort_params.nCol = col;
	sort_params.isAcend = m_isAcsend;

	// 정렬 함수 호출
	m_allHistory.SortItems(&SortFunc, (LPARAM)&sort_params );

	for (int i = 0 ; i < nItem ;i++)
	{
		delete arStr[i];
	}

	delete []arStr;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

int CALLBACK CPaneIESList::SortFunc(LPARAM lParam1 , LPARAM lParam2 , LPARAM lParamSort)
{
	CString& strItem1 = *(CString*) lParam1;
	CString& strItem2 = *(CString*) lParam2;

	SORT_PARAMS *pSortParams = (SORT_PARAMS *)lParamSort;
	int nCol = pSortParams->nCol;
	BOOL bAscend = pSortParams->isAcend;

	if (nCol == 0)
	{
		INT nVal1 = _tstoi(strItem1);
		INT nVal2 = _tstoi(strItem2);
		if (bAscend)   
			return nVal1 - nVal2;
		else
			return nVal2 - nVal1;

	}
	else
	{
		strItem1.MakeLower();
		strItem2.MakeLower();

		if(bAscend)
			return strItem1.Compare(strItem2);
		else
			return strItem2.Compare(strItem1);
	}
}
