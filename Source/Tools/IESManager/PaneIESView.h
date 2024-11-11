#pragma once

#include "PaneView.h"

class CPaneIESView : public CPaneView
{
public:
	CPaneIESView ();
	virtual ~CPaneIESView ();

	CWnd*			OnCreateView() override;

protected:
	CXTTreeCtrl				m_treeComponent;
};

