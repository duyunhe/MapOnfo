#pragma once
#include "MapOnfo.h"
#include "TwMapPropertyDlg.h"

// TwMapPropertyPane

class TwMapPropertyPane : public CDockablePane
{
	DECLARE_DYNAMIC(TwMapPropertyPane)
	TwMapPropertyDlg m_dlg;
	
public:
	TwMapPropertyPane();
	virtual ~TwMapPropertyPane();
	TwMapPropertyDlg* GetPropertyDlg();
	CView *m_pView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};


