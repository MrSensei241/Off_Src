#include "StdAfx.h"
#include "PaneIESView.h"


CPaneIESView::CPaneIESView ()
{

}

CPaneIESView::~CPaneIESView ()
{

}

CWnd* CPaneIESView::OnCreateView()
{
	return &m_treeComponent;
};

