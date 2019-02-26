// TwMapPropertyPane.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MapOnfo.h"
#include "TwMapPropertyPane.h"


// TwMapPropertyPane

IMPLEMENT_DYNAMIC(TwMapPropertyPane, CDockablePane)

TwMapPropertyPane::TwMapPropertyPane()
{

}

TwMapPropertyPane::~TwMapPropertyPane()
{
}


BEGIN_MESSAGE_MAP(TwMapPropertyPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// TwMapPropertyPane ��Ϣ�������




int TwMapPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_dlg.Create(IDD_TWDLG, this);
	m_dlg.ShowWindow(SW_SHOW);
	m_dlg.m_pView = m_pView;

	return 0;
}


void TwMapPropertyPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	if (m_dlg.GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_dlg.MoveWindow(rect);
	}

}

TwMapPropertyDlg* TwMapPropertyPane::GetPropertyDlg()
{
	return &m_dlg;
}


void TwMapPropertyPane::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
	m_dlg.DestroyWindow();
}
