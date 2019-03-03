#pragma once
#include "afxwin.h"
#include "map_struct.h"


// TwMapPropertyDlg 对话框

class TwMapPropertyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TwMapPropertyDlg)

public:
	TwMapPropertyDlg(CWnd* pParent = NULL);   // 标准构造函数
	CView *m_pView;
	virtual ~TwMapPropertyDlg();
	void UpdateRoadData(Line* pRoad);

// 对话框数据
	enum { IDD = IDD_TWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_name;
	virtual BOOL OnInitDialog();
	CEdit m_nameCtl;
	CEdit m_typeCtl;
	CEdit m_ortCtl;
	afx_msg void OnBnClickedButtonPtn();
	CButton m_pntCtl;
	afx_msg void OnBnClickedButtonAnl();
	CEdit m_otherCtl;
	BOOL m_chk1;
	BOOL m_chk2;
	BOOL m_chk3;
	BOOL m_chk4;
	BOOL m_chk5;
	BOOL m_chk6;
	BOOL m_chk7;
	BOOL m_chk8;
	BOOL m_chk9;
	BOOL m_chk10;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedCheck8();
	afx_msg void OnBnClickedCheck9();
	afx_msg void OnBnClickedCheck10();
	BOOL m_chkOneway;
	BOOL m_chkDoubleWay;
	afx_msg void OnBnClickedCheck11();
	afx_msg void OnBnClickedCheck12();
	BOOL m_chkAll;
	afx_msg void OnBnClickedCheck13();
	afx_msg void OnBnClickedButtonLid();
	afx_msg void OnBnClickedButtonSelmode();
	CString m_line0;
	CString m_line1;
	afx_msg void OnBnClickedButtonOk0();
	afx_msg void OnBnClickedButtonCancel0();
	afx_msg void OnBnClickedButtonOk1();
	afx_msg void OnBnClickedButtonCancel1();
	afx_msg void OnBnClickedButtonCenter();

	void EnableCenterButtons(BOOL);
	CButton m_btnOK0;
	CButton m_btnOK1;
	CButton m_btnCancel0;
	// 各个按钮
	CButton m_btnCancel1;
	CButton m_btnCenter;
	afx_msg void OnBnClickedButtonExp();
	afx_msg void OnBnClickedButtonEdit();
};
