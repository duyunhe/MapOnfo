// TwMapPropertyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MapOnfo.h"
#include "TwMapPropertyDlg.h"
#include "afxdialogex.h"
#include "MapOnfoView.h"
#include "map_struct.h"


// TwMapPropertyDlg 对话框

IMPLEMENT_DYNAMIC(TwMapPropertyDlg, CDialogEx)

TwMapPropertyDlg::TwMapPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(TwMapPropertyDlg::IDD, pParent)
	, m_name(_T(""))
	, m_chk1(FALSE)
	, m_chk2(FALSE)
	, m_chk3(FALSE)
	, m_chk4(FALSE)
	, m_chk5(FALSE)
	, m_chk6(FALSE)
	, m_chk7(FALSE)
	, m_chk8(FALSE)
	, m_chk9(FALSE)
	, m_chk10(FALSE)
	, m_chkOneway(FALSE)
	, m_chkDoubleWay(FALSE)
	, m_chkAll(FALSE)
{

}

TwMapPropertyDlg::~TwMapPropertyDlg()
{
}

void TwMapPropertyDlg::UpdateRoadData(Line *pRoad)
{
	if (pRoad == NULL)
	{
		m_nameCtl.SetWindowTextW(L"");
		m_ortCtl.SetWindowTextW(L"");
		m_typeCtl.SetWindowTextW(L"");
		return;
	}
	CString name, ort, type;
	name = pRoad->name;
	m_nameCtl.SetWindowTextW(name);
	type = pRoad->rank;
	if (pRoad->ort == ORT_DOUBLEWAY)
		ort = L"双向";
	else if (pRoad->ort == ORT_ONEWAY)
		ort = L"单向";
	else if (pRoad->ort == ORT_CONJ)
		ort = L"连接";
	m_ortCtl.SetWindowTextW(ort);
	m_typeCtl.SetWindowTextW(type);

	if (pRoad->par_lid != -1)
	{
		CString other;
		other.Format(L"%d,%.2f", pRoad->par_lid, pRoad->dist);
		m_otherCtl.SetWindowTextW(other);
	}
	else
	{
		m_otherCtl.SetWindowTextW(L"");
	}
}

void TwMapPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_nameCtl);
	DDX_Control(pDX, IDC_EDIT_TYPE, m_typeCtl);
	DDX_Control(pDX, IDC_EDIT_TYPE2, m_ortCtl);
	DDX_Control(pDX, IDC_BUTTON_PTN, m_pntCtl);
	DDX_Control(pDX, IDC_EDIT_OTHER, m_otherCtl);

	DDX_Check(pDX, IDC_CHECK1, m_chk1);
	DDX_Check(pDX, IDC_CHECK2, m_chk2);
	DDX_Check(pDX, IDC_CHECK3, m_chk3);
	DDX_Check(pDX, IDC_CHECK4, m_chk4);
	DDX_Check(pDX, IDC_CHECK5, m_chk5);
	DDX_Check(pDX, IDC_CHECK6, m_chk6);
	DDX_Check(pDX, IDC_CHECK7, m_chk7);
	DDX_Check(pDX, IDC_CHECK8, m_chk8);
	DDX_Check(pDX, IDC_CHECK9, m_chk9);
	DDX_Check(pDX, IDC_CHECK10, m_chk10);
	DDX_Check(pDX, IDC_CHECK11, m_chkOneway);
	DDX_Check(pDX, IDC_CHECK12, m_chkDoubleWay);
	DDX_Check(pDX, IDC_CHECK13, m_chkAll);
}


BEGIN_MESSAGE_MAP(TwMapPropertyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PTN, &TwMapPropertyDlg::OnBnClickedButtonPtn)
	ON_BN_CLICKED(IDC_BUTTON_ANL, &TwMapPropertyDlg::OnBnClickedButtonAnl)
	ON_BN_CLICKED(IDC_CHECK1, &TwMapPropertyDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &TwMapPropertyDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &TwMapPropertyDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &TwMapPropertyDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &TwMapPropertyDlg::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK6, &TwMapPropertyDlg::OnBnClickedCheck6)
	ON_BN_CLICKED(IDC_CHECK7, &TwMapPropertyDlg::OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_CHECK8, &TwMapPropertyDlg::OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_CHECK9, &TwMapPropertyDlg::OnBnClickedCheck9)
	ON_BN_CLICKED(IDC_CHECK10, &TwMapPropertyDlg::OnBnClickedCheck10)
	ON_BN_CLICKED(IDC_CHECK11, &TwMapPropertyDlg::OnBnClickedCheck11)
	ON_BN_CLICKED(IDC_CHECK12, &TwMapPropertyDlg::OnBnClickedCheck12)
	ON_BN_CLICKED(IDC_CHECK13, &TwMapPropertyDlg::OnBnClickedCheck13)
	ON_BN_CLICKED(IDC_BUTTON_LID, &TwMapPropertyDlg::OnBnClickedButtonLid)
END_MESSAGE_MAP()


// TwMapPropertyDlg 消息处理程序


BOOL TwMapPropertyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(TRUE);
	m_chk1 = TRUE;
	m_chk2 = TRUE;
	m_chk3 = TRUE;
	m_chk4 = TRUE;
	m_chk5 = FALSE;
	m_chk6 = TRUE;
	m_chk7 = TRUE;
	m_chk8 = TRUE;
	m_chk9 = TRUE;
	m_chk10 = TRUE;
	m_chkOneway = TRUE;
	m_chkDoubleWay = TRUE;
	m_chkAll = TRUE;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void TwMapPropertyDlg::OnBnClickedButtonPtn()
{
	// TODO:  在此添加控件通知处理程序代码
	((CMapOnfoView*)m_pView)->ChangeVisPoint();
}


void TwMapPropertyDlg::OnBnClickedButtonAnl()
{
	// TODO:  在此添加控件通知处理程序代码
	//((CMapOnfoView*)m_pView)->AnalysisSelLine();
	((CMapOnfoView*)m_pView)->Analysis();
}


void TwMapPropertyDlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk1 = 1 - m_chk1;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(0);
}


void TwMapPropertyDlg::OnBnClickedCheck2()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk2 = 1 - m_chk2;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(1);
}


void TwMapPropertyDlg::OnBnClickedCheck3()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk3 = 1 - m_chk3;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(2);
}


void TwMapPropertyDlg::OnBnClickedCheck4()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk4 = 1 - m_chk4;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(3);
}


void TwMapPropertyDlg::OnBnClickedCheck5()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk5 = 1 - m_chk5;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(4);
}


void TwMapPropertyDlg::OnBnClickedCheck6()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk6 = 1 - m_chk6;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(5);
}


void TwMapPropertyDlg::OnBnClickedCheck7()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk7 = 1 - m_chk7;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(6);
}


void TwMapPropertyDlg::OnBnClickedCheck8()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk8 = 1 - m_chk8;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(7);
}


void TwMapPropertyDlg::OnBnClickedCheck9()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk9 = 1 - m_chk9;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(8);
}


void TwMapPropertyDlg::OnBnClickedCheck10()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chk10 = 1 - m_chk10;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeShowType(9);
}


void TwMapPropertyDlg::OnBnClickedCheck11()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chkOneway = 1 - m_chkOneway;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeOneway();
}


void TwMapPropertyDlg::OnBnClickedCheck12()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chkDoubleWay = 1 - m_chkDoubleWay;
	UpdateData(FALSE);
	((CMapOnfoView*)m_pView)->ChangeDoubleWay();
}


void TwMapPropertyDlg::OnBnClickedCheck13()
{
	// TODO:  在此添加控件通知处理程序代码
	m_chkAll = 1 - m_chkAll;
	if (m_chkAll == TRUE)
	{
		m_chk1 = TRUE;
		m_chk2 = TRUE;
		m_chk3 = TRUE;
		m_chk4 = TRUE;
		m_chk5 = FALSE;
		m_chk6 = TRUE;
		m_chk7 = TRUE;
		m_chk8 = TRUE;
		m_chk9 = TRUE;
		m_chk10 = TRUE;
		((CMapOnfoView*)m_pView)->ChangeAllShow();
	}
	else
	{
		m_chk1 = FALSE;
		m_chk2 = FALSE;
		m_chk3 = FALSE;
		m_chk4 = FALSE;
		m_chk5 = FALSE;
		m_chk6 = FALSE;
		m_chk7 = FALSE;
		m_chk8 = FALSE;
		m_chk9 = FALSE;
		m_chk10 = FALSE;
		((CMapOnfoView*)m_pView)->ChangeAllNotShow();
	}
	UpdateData(FALSE);
}


void TwMapPropertyDlg::OnBnClickedButtonLid()
{
	// TODO:  在此添加控件通知处理程序代码
	((CMapOnfoView*)m_pView)->ChangeGridLinePoint();
}
