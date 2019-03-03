
// MapOnfoView.cpp : CMapOnfoView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MapOnfo.h"
#endif

#define MOUSE_DOWN 0
#define MOUSE_UP   1

#include "MapOnfoDoc.h"
#include "MapOnfoView.h"
#include "MainFrm.h"
#include "TwMapPropertyDlg.h"
#include "TwGeo.h"
#include "common.h"
#include <set>
#include <algorithm>
using namespace std;

#define MOUSEINFOX 2000

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapOnfoView

IMPLEMENT_DYNCREATE(CMapOnfoView, CView)

BEGIN_MESSAGE_MAP(CMapOnfoView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SEL_MODE, &CMapOnfoView::OnSelMode)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_MENU_GRID, &CMapOnfoView::OnMenuGrid)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CT_MODE, &CMapOnfoView::OnCtMode)
END_MESSAGE_MAP()

// CMapOnfoView 构造/析构

CMapOnfoView::CMapOnfoView()
{
	// TODO:  在此处添加构造代码
	m_visPoint = 0;
	m_analysis = 0;
	m_showGridLine = 0;
	m_bPointSel = false;
}

CMapOnfoView::~CMapOnfoView()
{
}

BOOL CMapOnfoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMapOnfoView 绘制

void CMapOnfoView::OnDraw(CDC* pDC)
{
	CMapOnfoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	// 双缓冲
	CRect rcOldClip;
	pDC->GetClipBox(rcOldClip);

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int nWidth = ClientRect.Width();
	int nHeight = ClientRect.Height();
	CDC memDC;
	CBitmap bmp;
	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	CBitmap *pOldBit = memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(255, 255, 255));

	//DrawSegments(&memDC);
	//DrawGenePoints(&memDC);
	DrawGeneLines(&memDC);
	DrawGrid(&memDC);
	DrawMouseInfo(&memDC);
	DrawTestLine(&memDC, m_dao);
	DrawExpressLine(&memDC);
	DrawCross(&memDC);
	DrawSelGrid(&memDC);
	DrawSelPoint(&memDC);
	DrawDelaunay(&memDC);
	
	pDC->BitBlt(0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void CMapOnfoView::DrawSelPoint(CDC *pDC)
{
	if (!m_bPointSel)
		return;
	Point *pt_list = m_dao.m_point_list;
	int pid = m_selPointID;
	double x = pt_list[pid].x, y = pt_list[pid].y;
	int ix, iy;
	Point2Screen(x, y, ix, iy);
	DrawSmallCross(pDC, ix, iy);
	CString idx;
	idx.Format(L"%d", pid);
	pDC->TextOutW(ix + 5, iy + 5, idx);
}

void CMapOnfoView::DrawSelGrid(CDC *pDC)
{
	if (!m_showGridLine)
		return;
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 2, RGB(0x68, 0x22, 0x8b));
	pDC->SelectObject(&cPen);
	vector<int> &selid = m_selGrid->line_list;
	for (int i = 0; i < selid.size(); ++i)
	{
		int lid = selid[i];
		Line *lptr = &m_dao.m_line_list[lid];
		// 过滤掉单向，匝道，次要支路，步行，连杆道路
		int type = GetRoadTypeCode(lptr->rank);
		/*if (type == -1 || type == 3 || type == 4 || lptr->ort == ORT_ONEWAY)
			continue;*/
		int pid = lptr->point_list[0];
		Point *pptr = &m_dao.m_point_list[pid];
		double x = pptr->x, y = pptr->y;
		int ix, iy;
		Point2Screen(x, y, ix, iy);
		pDC->MoveTo(ix, iy);
		if (m_gridLinePoint && m_dao.GetRoadSelection() != -1 && m_selLineID == lid)
		{
			CString idx;
			idx.Format(L"0");
			pDC->TextOutW(ix + 5, iy + 5, idx);
		}
		
		for (int j = 1; j < lptr->point_list.size(); ++j)
		{
			pid = lptr->point_list[j];
			pptr = &m_dao.m_point_list[pid];
			x = pptr->x, y = pptr->y;
			Point2Screen(x, y, ix, iy);
			pDC->LineTo(ix, iy);

			if (m_gridLinePoint && m_dao.GetRoadSelection() != -1 && m_selLineID == lid)
			{
				CString idx;
				idx.Format(L"%d", j);
				pDC->TextOutW(ix + 5, iy + 5, idx);
			}
		}

		if (m_gridLinePoint)
		{
			//PBiNode cur = line->point_list->head->forward;
			Point *p0 = m_dao.GetPoint(lptr->point_list[0]);
			//PointPtr p0 = (PointPtr)cur->item;
			PointPtr p1 = m_dao.GetPoint(lptr->point_list[lptr->point_list.size() - 1]);
			double x = p0->x, y = p0->y;
			int ix0, iy0;
			Point2Screen(x, y, ix0, iy0);
			x = p1->x, y = p1->y;
			int ix1, iy1;
			Point2Screen(x, y, ix1, iy1);
			CString idx;
			idx.Format(L"%d", lptr->lid);
			pDC->TextOutW((ix0 + ix1) / 2, (iy0 + iy1) / 2, idx);
		}
	}
}

void CMapOnfoView::DrawDelaunay(CDC *pDC)
{
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 255));
	pDC->SelectObject(&cPen);
	for (int i = 0; i < m_drawModPoint[0].size(); ++i)
	{
		const ModPoint &pt = m_drawModPoint[0][i];
		int ix, iy;
		Point2Screen(pt.x, pt.y, ix, iy);
		if (i == 0)
		{
			pDC->MoveTo(ix, iy);
		}
		else
		{
			pDC->LineTo(ix, iy);
		}
	}
	for (int i = 0; i < m_drawModPoint[0].size(); ++i)
	{
		const ModPoint &pt = m_drawModPoint[0][i];
		int ix, iy;
		Point2Screen(pt.x, pt.y, ix, iy);
		DrawSmallCross(pDC, ix, iy);
	}
	for (int i = 0; i < m_drawModPoint[1].size(); ++i)
	{
		const ModPoint &pt = m_drawModPoint[1][i];
		int ix, iy;
		Point2Screen(pt.x, pt.y, ix, iy);
		if (i == 0)
		{
			pDC->MoveTo(ix, iy);
		}
		else
		{
			pDC->LineTo(ix, iy);
		}
	}
	for (int i = 0; i < m_drawModPoint[1].size(); ++i)
	{
		const ModPoint &pt = m_drawModPoint[1][i];
		int ix, iy;
		Point2Screen(pt.x, pt.y, ix, iy);
		DrawSmallCross(pDC, ix, iy);
	}
	DeleteObject(cPen);
}

void CMapOnfoView::DrawModLine(CDC *pDC, ModLine *ln)
{
	/*if (ln->org_lid != 2349)
		return;*/
	for (int i = 0; i < int(ln->point_list.size()); ++i)
	{
		int pid = ln->point_list[i];
		const ModPoint &pt = m_GenePoints[pid];
		int ix, iy;
		Point2Screen(pt.x, pt.y, ix, iy);
		if (i == 0)
		{
			pDC->MoveTo(ix, iy);
		}
		else
		{
			pDC->LineTo(ix, iy);
		}
	}
}

void CMapOnfoView::DrawGeneLines(CDC *pDC)
{
	CPen cPen;
	cPen.CreatePen(PS_DASHDOT, 2, RGB(255, 0, 255));
	pDC->SelectObject(&cPen);
	for (int i = 0; i < int(m_GeneLines.size()); ++i)
	{
		DrawModLine(pDC, &m_GeneLines[i]);
	}
	DeleteObject(cPen);
}

void CMapOnfoView::DrawGrid(CDC *pDC)
{
	if (!m_showGrid || m_scale < 0.05)
		return;
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 1, RGB(147, 147, 147));
	pDC->SelectObject(&cPen);
	double minX = MINMAPX, maxX = MAXMAPX;
	double minY = MINMAPY, maxY = MAXMAPY;
	for (double x = minX; x <= maxX; x += GRIDMAP)
	{
		double sx = x, sy = minY;
		int ix, iy;
		Point2Screen(sx, sy, ix, iy);
		pDC->MoveTo(ix, iy);
		sy = maxY;
		Point2Screen(sx, sy, ix, iy);
		pDC->LineTo(ix, iy);
	}
	for (double y = minY; y <= maxY; y += GRIDMAP)
	{
		double sx = minX, sy = y;
		int ix, iy;
		Point2Screen(sx, sy, ix, iy);
		pDC->MoveTo(ix, iy);
		sx = maxX;
		Point2Screen(sx, sy, ix, iy);
		pDC->LineTo(ix, iy);
	}
	DeleteObject(cPen);
}

void CMapOnfoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMapOnfoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMapOnfoView 诊断

#ifdef _DEBUG
void CMapOnfoView::AssertValid() const
{
	CView::AssertValid();
}

void CMapOnfoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CMapOnfoView::Point2Screen(double srcX, double srcY, int &pixelX, int &pixelY)
{
	pixelX = int((srcX - m_center_point.x) * m_scale) + m_screenPx / 2;
	pixelY = int(m_screenPy / 2 - (srcY - m_center_point.y) * m_scale);
}

void CMapOnfoView::Screen2Point(int ix, int iy, double &sx, double &sy)
{
	sx = (ix - m_screenPx / 2) / m_scale + m_center_point.x;
	sy = (m_screenPy / 2 - iy) / m_scale + m_center_point.y;
}

void CMapOnfoView::DrawMouseInfo(CDC *pDC)
{
	CString str_text;
	str_text.Format(L"%d, %d, %.2f, %.2f", m_mousePoint.x, m_mousePoint.y, m_factPoint.x, m_factPoint.y);
	pDC->TextOutW(MOUSEINFOX, 10, str_text);
	CString str_center_msg;
	str_center_msg.Format(L"center_point %.2f, %.2f", m_center_point.x, m_center_point.y);
	pDC->TextOutW(MOUSEINFOX, 30, str_center_msg);
}

void CMapOnfoView::DrawSmallCross(CDC *pDC, int srcX, int srcY)
{
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	pDC->SelectObject(&cPen);
	int OFFSET = 8;
	pDC->MoveTo(srcX - OFFSET, srcY);
	pDC->LineTo(srcX + OFFSET, srcY);
	pDC->MoveTo(srcX, srcY - OFFSET);
	pDC->LineTo(srcX, srcY + OFFSET);
	DeleteObject(cPen);
}

void CMapOnfoView::DrawSegments(CDC *pDC)
{
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	pDC->SelectObject(&cPen);
	for (int i = 0; i < m_geneSegment.size(); ++i)
	{
		Point begin_pt = m_geneSegment[i].begin_pt, end_pt = m_geneSegment[i].end_pt;
		double x = begin_pt.x, y = begin_pt.y;
		int ix, iy;
		Point2Screen(x, y, ix, iy);
		pDC->MoveTo(ix, iy);
		x = end_pt.x, y = end_pt.y;
		//int ix, iy;
		Point2Screen(x, y, ix, iy);
		pDC->LineTo(ix, iy);
	}
	DeleteObject(cPen);
}

void CMapOnfoView::DrawCross(CDC *pDC)
{
	CPen cPen;
	cPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	pDC->SelectObject(&cPen);
	pDC->MoveTo(m_screenPx / 2 - 10, m_screenPy / 2);
	pDC->LineTo(m_screenPx / 2 + 10, m_screenPy / 2);
	pDC->MoveTo(m_screenPx / 2, m_screenPy / 2 + 10);
	pDC->LineTo(m_screenPx / 2, m_screenPy / 2 - 10);
	DeleteObject(cPen);
}

void CMapOnfoView::ChangeDoubleWay()
{
	m_showDoubleWay = 1 - m_showDoubleWay;
	Invalidate();
}

void CMapOnfoView::ChangeOneway()
{
	m_showOneWay = 1 - m_showOneWay;
	Invalidate();
}

void CMapOnfoView::DrawGenePoints(CDC *pDC)
{
	for (int i = 0; i < m_GenePoints.size(); ++i)
	{
		const ModPoint &t = m_GenePoints[i];
		double x = t.x, y = t.y;
		int ix, iy;
		CString info;
		info.Format(L"%d:", i);
		for (int j = 0; j < t.pli_list.size(); ++j)
		{
			CString idx;
			idx.Format(L"%d,%d,%d,%d,", t.pli_list[j].lid, t.pli_list[j].seq, t.pli_list[j].left, t.pli_list[j].forward);
			info += idx;
		}
		
		Point2Screen(x, y, ix, iy);
		pDC->TextOutW(ix + 5, iy + 5, info);
		DrawSmallCross(pDC, ix, iy);
	}
}

void CMapOnfoView::DrawLine(CDC *pDC, Line *line, bool selPoint, bool selLine)
{
	int rankCode = GetRoadTypeCode(line->rank);
	if (rankCode == -1 || m_showType[rankCode] != TRUE)
		return;
	if (line->ort == ORT_ONEWAY && !m_showOneWay)
		return;
	if (line->ort == ORT_DOUBLEWAY && !m_showDoubleWay)
		return;
	int pid = line->point_list[0];
	Point *p = m_dao.GetPoint(pid);
	double x = p->x, y = p->y;
	int ix, iy;
	Point2Screen(x, y, ix, iy);
	if (selPoint && m_visPoint)
		pDC->TextOutW(ix + 5, iy + 5, L"0");
	//point2screen(x, y, m_center_point, m_scale, m_screenPx, m_screenPy, ix, iy);
	pDC->MoveTo(ix, iy);
	for (int j = 1; j < line->point_list.size(); ++ j)
	{
		p = m_dao.GetPoint(line->point_list[j]);
		double x = p->x, y = p->y;
		Point2Screen(x, y, ix, iy);
		//point2screen(x, y, m_center_point, m_scale, m_screenPx, m_screenPy, ix, iy);
		pDC->LineTo(ix, iy);

		if (selPoint && m_visPoint)
		{
			CString idx;
			idx.Format(L"%d", j);
			pDC->TextOutW(ix + 5, iy + 5, idx);
		}
	}
	if (selLine)
	{
		//PBiNode cur = line->point_list->head->forward;
		Point *p0 = m_dao.GetPoint(line->point_list[0]);
		//PointPtr p0 = (PointPtr)cur->item;
		PointPtr p1 = m_dao.GetPoint(line->point_list[line->point_list.size() - 1]);
		double x = p0->x, y = p0->y;
		int ix0, iy0;
		Point2Screen(x, y, ix0, iy0);
		x = p1->x, y = p1->y;
		int ix1, iy1;
		Point2Screen(x, y, ix1, iy1);
		CString idx;
		idx.Format(L"%d", line->lid);
		pDC->TextOutW(ix0 + 20, iy0 + 20, idx);
	}
}

void CMapOnfoView::InitShowType()
{
	for (int i = 0; i < 10; ++i)
	{
		m_showType[i] = TRUE;
	}
	m_showType[4] = FALSE;
	m_showOneWay = TRUE;
	m_showDoubleWay = TRUE;
}

void CMapOnfoView::DrawLineCross(CDC *pDC, Line *line)
{
	for (int i = 0; i < line->point_list.size(); ++ i)
	//for (PBiNode cur = line->point_list->head->forward; cur != line->point_list->head; cur = cur->forward)
	{
		PointPtr p = m_dao.GetPoint(line->point_list[i]);
		if (!p->cross)
			continue;
		int ix, iy;
		double x = p->x, y = p->y;
		Point2Screen(x, y, ix, iy);
		DrawSmallCross(pDC, ix, iy);
	}
}

int CMapOnfoView::GetRoadTypeCode(char* type)
{
	if (type == NULL)
		return -1;
	char* def[10] = { "主干路", "次要支路", "高速公路", "匝道", "步行街", "主要支路", "次干路", "二级公路", "一级公路", "快速路" };
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(type, def[i]) == 0)
			return i;
	}
	return -1;
}


vector<ModPoint> CMapOnfoView::GenePoints(const set<int> &lnSet, const Point &srcPt, Point *pt_list, Line* ln_list, int seq)
{
	int deg = 0;
	vector<pair<int, int>> info;
	for (int i = 0; i < srcPt.line_list.size(); ++i)
	{
		int lid = srcPt.line_list[i].first, seq = srcPt.line_list[i].second;
		if (lnSet.count(lid) != 0)
		{
			deg++;
			info.push_back(pair<int, int>(lid, seq));
		}
	}
	if (deg == 1)
	{
		int lid = info[0].first, seq = info[0].second;
		int pid0 = ln_list[lid].point_list[seq], pid1 = ln_list[lid].point_list[seq + 1];
		bool sw = false;
		bool begin_pt = false;
		int len = int(ln_list[lid].point_list.size());
		if (seq == 0 && srcPt.pid == pid0)
		{
			begin_pt = true;
		}
		else
		{
			begin_pt = false;
		}
		if (srcPt.pid == pid1)
		{
			swap(pid0, pid1);
			sw = true;
		}
		// 让pid0成为当前点
		Point p0, p1;
		p0.x = pt_list[pid0].x, p0.y = pt_list[pid0].y;
		p1.x = pt_list[pid1].x, p1.y = pt_list[pid1].y;

		Point dstp0, dstp1, _dstp0, _dstp1;
		GetParallel(p0, p1, dstp0, dstp1, _dstp0, _dstp1);		// right and left

		vector<ModPoint> temp;
		ModPoint endPt0;
		endPt0.x = dstp0.x;
		endPt0.y = dstp0.y;
		PointLineInfo pli;
		pli.left = false;
		if (sw)
			pli.left = !pli.left;
		if (!pli.left)
			pli.forward = begin_pt;
		else
			pli.forward = !begin_pt;
		pli.lid = lid;
		pli.seq = seq;
		endPt0.pli_list.push_back(pli);
		temp.push_back(endPt0);
		ModPoint endPt1;
		endPt1.x = _dstp0.x;
		endPt1.y = _dstp0.y;
		pli.left = true;
		if (sw)
			pli.left = !pli.left;
		if (!pli.left)
			pli.forward = begin_pt;
		else
			pli.forward = !begin_pt;
		endPt1.pli_list.push_back(pli);
		temp.push_back(endPt1);
		return temp;
	}
	else if (deg >= 2)
	{
		/*LineInfo *tempInfo;
		tempInfo = (LineInfo*)malloc(sizeof(LineInfo) * deg);*/
		LineInfo buf[6];
		for (int i = 0; i < deg; ++i)
		{
			int lid = info[i].first, seq = info[i].second;
			buf[i].lid = lid, buf[i].seq = seq;
			int pid0 = ln_list[lid].point_list[seq], pid1 = ln_list[lid].point_list[seq + 1];
			if (srcPt.pid == pid1)
			{
				swap(pid0, pid1);
				buf[i].swap = true;
			}
			else
			{
				buf[i].swap = false;
			}
			buf[i].theta = GetVectorAngle(pt_list[pid0], pt_list[pid1]);
		}
		sort(buf, buf + deg, cmp_lineinfo);
		vector<ModPoint> temp;
		for (int i = 0; i < deg; ++i)
		{
			int lid0 = buf[i].lid, seq0 = buf[i].seq;
			int j = i + 1 == deg ? 0 : i + 1;
			int lid1 = buf[j].lid, seq1 = buf[j].seq;
			bool sw0 = buf[i].swap, sw1 = buf[j].swap; 

			Point s0p0, s0p1, s1p0, s1p1;
			int s0pid0 = ln_list[lid0].point_list[seq0], s0pid1 = ln_list[lid0].point_list[seq0 + 1];
			int s1pid0 = ln_list[lid1].point_list[seq1], s1pid1 = ln_list[lid1].point_list[seq1 + 1];
			if (sw0)
			{
				swap(s0pid0, s0pid1);
			}
			if (sw1)
			{
				swap(s1pid0, s1pid1);
			}

			s0p0.x = pt_list[s0pid0].x, s0p0.y = pt_list[s0pid0].y;
			s0p1.x = pt_list[s0pid1].x, s0p1.y = pt_list[s0pid1].y;
			s1p0.x = pt_list[s1pid0].x, s1p0.y = pt_list[s1pid0].y;
			s1p1.x = pt_list[s1pid1].x, s1p1.y = pt_list[s1pid1].y;

			Point dstp0, dstp1, _dstp0, _dstp1;
			Point dstp2, dstp3, _dstp2, _dstp3;
			GetParallel(s0p0, s0p1, dstp0, dstp1, _dstp0, _dstp1);		// right and left
			GetParallel(s1p0, s1p1, dstp2, dstp3, _dstp2, _dstp3);

			ModPoint crPt;
			GetCrossPoint(_dstp0, _dstp1, dstp2, dstp3, crPt);
			PointLineInfo pli;
			pli.left = true;
			if (sw0)
				pli.left = !pli.left;
			pli.forward = !sw0;
			pli.lid = lid0;
			pli.seq = seq0;
			crPt.pli_list.push_back(pli);
			pli.left = false;
			if (sw1)
				pli.left = !pli.left;
			pli.forward = !sw1;
			pli.lid = lid1;
			pli.seq = seq1;
			crPt.pli_list.push_back(pli);
			crPt.pid = int(temp.size());
			temp.push_back(crPt);
		}
		return temp;
		/*free(tempInfo);*/
	}
	vector<ModPoint> temp;
	return temp;
}

vector<int> CMapOnfoView::FindNearDoubleLine(int lid)
{
	vector<int> line_list;
	Point *pt_list = m_dao.m_point_list;
	Line *ln_list = m_dao.m_line_list;
	set<pair<int, int>> candidate;

	// lid 和index 是一样的
	for (int i = 0; i < ln_list[lid].point_list.size(); ++i)
	{
		int pid = ln_list[lid].point_list[i];
		int r, c;
		m_dao.GetRowCol(pt_list[pid].x, pt_list[pid].y, r, c);
		candidate.insert(pair<int, int>(r, c));
	}
	set<int> line_set;
	for (set<pair<int, int>>::iterator it = candidate.begin(); it != candidate.end(); ++it)
	{
		int r = it->first, c = it->second;
		vector<int> &ids = m_dao.m_grid[r][c].line_list;
		for (int i = 0; i < ids.size(); ++i)
		{
			int lid = ids[i];
			if (ln_list[lid].ort == ORT_DOUBLEWAY && (strcmp(ln_list[lid].rank, "主干路") == 0 
				|| strcmp(ln_list[lid].rank, "次干路") == 0))
				line_set.insert(ids[i]);
		}
	}
	for (set<int>::iterator it = line_set.begin(); it != line_set.end(); ++it)
	{
		line_list.push_back(*it);
	}
	return line_list;
}

void CMapOnfoView::GeneExpressPoint(Point* pt, Segment *sgmt, ModPoint &mp)
{
	GetPointOffset(*pt, *sgmt, 20, mp);
}

void CMapOnfoView::GeneExpressPass()
{
	if (m_dao.GetRoadSelection() == -1)
		return;
	int sel = m_dao.GetRoadSelection();
	Point *pt_list = m_dao.m_point_list;
	Line *ln_list = m_dao.m_line_list;
	m_GeneLines.clear();
	m_GenePoints.clear();
	for (int i = 0; i < m_dao.m_line_cnt; ++i)
	{
		int lid = ln_list[i].lid;
		if (lid == sel && ln_list[i].ort == ORT_ONEWAY && strcmp(ln_list[i].rank, "快速路") == 0)
		{
			vector<int> &ids = FindNearDoubleLine(i);
			ModLine line;
			line.org_lid = lid;
			line.left = false;
			line.lid = int(m_GeneLines.size());
			for (int j = 0; j < ln_list[i].point_list.size(); ++j)
			{
				int pid = ln_list[i].point_list[j];
				int sel_lid = 0;
				double minDist = 1e10;
				Segment selSgmt;
				for (int k = 0; k < ids.size(); ++k)
				{
					int tar_lid = ids[k];
					double dist;
					Segment sg;
					Point2Line(&ln_list[tar_lid], &pt_list[pid], dist, pt_list, sg);
					if (dist < minDist)
					{
						minDist = dist;
						sel_lid = tar_lid; 
						selSgmt = sg;
					}
				}
				TRACE(L"%d,%.2f\n", sel_lid, minDist);
				ModPoint mp;
				mp.pid = int(m_GenePoints.size());
				GeneExpressPoint(&pt_list[pid], &selSgmt, mp);
				m_GenePoints.push_back(mp);
				line.point_list.push_back(mp.pid);
			}
			m_GeneLines.push_back(line);
		}
	}
	Invalidate();
}

void CMapOnfoView::GeneDoubleWay()
{
	set<int> vpt, vln;
	for (int i = 0; i < m_dao.m_gene_list.size(); ++i)
	{
		vln.insert(m_dao.m_gene_list[i]);
	}
	Point *pt_list = m_dao.m_point_list;
	Line *ln_list = m_dao.m_line_list;
	m_GenePoints.clear();
	m_GeneLines.clear();

	for (int i = 0; i < m_dao.m_gene_list.size(); ++i)
	{
		int lid = m_dao.m_gene_list[i];
		for (int j = 0; j < ln_list[lid].point_list.size(); ++j)
		{
			int pid = ln_list[lid].point_list[j];
			if (vpt.count(pid) == 0)
			{
				/*if (lid == 2349)*/
				{
					vector<ModPoint> pts = GenePoints(vln, pt_list[pid], pt_list, ln_list, j);
					for (int k = 0; k < pts.size(); ++k)
						m_GenePoints.push_back(pts[k]);
				}
				vpt.insert(pid);
			}
		}
	}
	// 生成新的Line
	// 占位
	map<int, pair<int, int>> line_index;
	// 索引
	for (int i = 0; i < m_dao.m_gene_list.size(); ++i)
	{
		ModLine ml;
		int lid = m_dao.m_gene_list[i];
		ml.left = 0;
		ml.org_lid = lid;
		ml.lid = int(m_GeneLines.size());
		ml.point_list.resize(ln_list[lid].point_list.size(), 0);
		m_GeneLines.push_back(ml);

		ml.left = 1;
		ml.org_lid = lid;
		ml.lid = int(m_GeneLines.size());
		ml.point_list.resize(ln_list[lid].point_list.size(), 0);
		m_GeneLines.push_back(ml);

		line_index[lid] = pair<int, int>(int(m_GeneLines.size()) - 2, int(m_GeneLines.size()) - 1);
	}
	// Gene Lines中的点
	for (int i = 0; i < m_GenePoints.size(); ++i)
	{
		const ModPoint &t = m_GenePoints[i];
		if (t.pli_list.size() == 2)	// 非端点
		{
			int lid0 = t.pli_list[0].lid, seq0 = t.pli_list[0].seq;
			bool left0 = t.pli_list[0].left, f0 = t.pli_list[0].forward;
			int lid1 = t.pli_list[1].lid, seq1 = t.pli_list[1].seq;
			bool left1 = t.pli_list[1].left, f1 = t.pli_list[1].forward;
			if (lid0 == lid1)
			{
				int maxSeq = max(seq0, seq1);
				if (left0 == 0)		// 右手边，顺序
				{
					int lid = line_index[lid0].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[maxSeq] = i;
				}
				else			// 左手边，逆序
				{
					int lid = line_index[lid0].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - maxSeq] = i;
				}
			}
			else
			{
				if (left0 == 0)
				{
					int seq = seq0;
					if (!f0)
						seq++;
					int lid = line_index[lid0].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[seq] = i;
				}
				else
				{
					int seq = seq0;
					if (!f0)
						seq++;
					int lid = line_index[lid0].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - seq] = i;
				}
				if (left1 == 0)
				{
					int seq = seq1;
					if (!f1)
						seq++;
					int lid = line_index[lid1].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[seq] = i;
				}
				else
				{
					int seq = seq1;
					if (!f1)
						seq++;
					int lid = line_index[lid1].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - seq] = i;
				}
			}
		}
		else    // 端点
		{
			int lid0 = t.pli_list[0].lid, seq0 = t.pli_list[0].seq;
			bool left0 = t.pli_list[0].left, f0 = t.pli_list[0].forward;
			if (left0 == 0)		// 右
			{
				int seq = seq0;
				int lid = line_index[lid0].first;
				ModLine &l = m_GeneLines[lid];
				if (f0)
					l.point_list[0] = i;
				else
					l.point_list[l.point_list.size() - 1] = i;
			}
			else
			{
				int seq = seq0;
				int lid = line_index[lid0].second;
				ModLine &l = m_GeneLines[lid];
				if (f0)
					l.point_list[0] = i;
				else
					l.point_list[l.point_list.size() - 1] = i;
			}
		}
	}
	Invalidate();
}

void CMapOnfoView::GeneDoubleWayGrid()
{
	if (m_selGrid == NULL)
		return;
	vector<int> &lid_list = m_selGrid->line_list;
	set<int> vpt, vln;	
	for (int i = 0; i < lid_list.size(); ++i)
	{
		vln.insert(lid_list[i]);
	}
	Point *pt_list = m_dao.m_point_list;
	Line *ln_list = m_dao.m_line_list;
	m_GenePoints.clear();
	m_geneSegment.clear();
	m_GeneLines.clear();
	for (int i = 0; i < lid_list.size(); ++i)
	{
		int lid = lid_list[i];
		for (int j = 0; j < ln_list[lid].point_list.size(); ++j)
		{
			int pid = ln_list[lid].point_list[j];
			if (vpt.count(pid) == 0)
			{
				/*if (lid == 2349)*/
				{
					vector<ModPoint> pts = GenePoints(vln, pt_list[pid], pt_list, ln_list, j);
					for (int k = 0; k < pts.size(); ++k)
						m_GenePoints.push_back(pts[k]);
				}
				vpt.insert(pid);
			}
		}
	}
	// 生成新的Line
	// 占位
	map<int, pair<int, int>> line_index;
	// 索引
	for (int i = 0; i < int(lid_list.size()); ++i)
	{
		ModLine ml;
		int lid = lid_list[i];
		ml.left = 0;
		ml.org_lid = lid;
		ml.lid = int(m_GeneLines.size());
		ml.point_list.resize(ln_list[lid].point_list.size(), 0);
		m_GeneLines.push_back(ml);

		ml.left = 1;
		ml.org_lid = lid;
		ml.lid = int(m_GeneLines.size());
		ml.point_list.resize(ln_list[lid].point_list.size(), 0);
		m_GeneLines.push_back(ml);

		line_index[lid] = pair<int, int>(int(m_GeneLines.size()) - 2, int(m_GeneLines.size()) - 1);
	}
	// Gene Lines中的点
	for (int i = 0; i < m_GenePoints.size(); ++i)
	{
		const ModPoint &t = m_GenePoints[i];
		if (t.pli_list.size() == 2)	// 非端点
		{
			int lid0 = t.pli_list[0].lid, seq0 = t.pli_list[0].seq;
			bool left0 = t.pli_list[0].left, f0 = t.pli_list[0].forward;
			int lid1 = t.pli_list[1].lid, seq1 = t.pli_list[1].seq;
			bool left1 = t.pli_list[1].left, f1 = t.pli_list[1].forward;
			if (lid0 == lid1)
			{
				int maxSeq = max(seq0, seq1);
				if (left0 == 0)		// 右手边，顺序
				{
					int lid = line_index[lid0].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[maxSeq] = i;
				}
				else			// 左手边，逆序
				{
					int lid = line_index[lid0].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - maxSeq] = i;
				}
			}
			else
			{
				if (left0 == 0)
				{
					int seq = seq0;
					if (!f0)
						seq++;
					int lid = line_index[lid0].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[seq] = i;
				}
				else
				{
					int seq = seq0;
					if (!f0)
						seq++;
					int lid = line_index[lid0].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - seq] = i;
				}
				if (left1 == 0)
				{
					int seq = seq1;
					if (!f1)
						seq++;
					int lid = line_index[lid1].first;
					ModLine &l = m_GeneLines[lid];
					l.point_list[seq] = i;
				}
				else
				{
					int seq = seq1;
					if (!f1)
						seq++;
					int lid = line_index[lid1].second;
					ModLine &l = m_GeneLines[lid];
					l.point_list[l.point_list.size() - 1 - seq] = i;
				}
			}
		}
		else    // 端点
		{
			int lid0 = t.pli_list[0].lid, seq0 = t.pli_list[0].seq;
			bool left0 = t.pli_list[0].left, f0 = t.pli_list[0].forward;
			if (left0 == 0)		// 右
			{
				int seq = seq0; 
				int lid = line_index[lid0].first;
				ModLine &l = m_GeneLines[lid];
				if (f0)
					l.point_list[0] = i;
				else
					l.point_list[l.point_list.size() - 1] = i;
			}
			else
			{
				int seq = seq0;
				int lid = line_index[lid0].second;
				ModLine &l = m_GeneLines[lid];
				if (f0)
					l.point_list[0] = i;
				else
					l.point_list[l.point_list.size() - 1] = i;
			}
		}
	}
	Invalidate();
}

void CMapOnfoView::DrawExpressLine(CDC *pDC)
{
	CPen cPen0, cPen1;
	cPen0.CreatePen(PS_SOLID, 2, RGB(46, 139, 87));			// seagreen
	cPen1.CreatePen(PS_SOLID, 2, RGB(218, 165, 32));		// goldenrod

	set<int>::iterator it;
	pDC->SelectObject(&cPen0);
	for (it = m_ExpressLine0.begin(); it != m_ExpressLine0.end(); ++it)
	{
		int lid = *it;
		Line *res = m_dao.GetData();
		DrawLine(pDC, &res[lid], true, true);
	}

	pDC->SelectObject(&cPen1);
	for (it = m_ExpressLine1.begin(); it != m_ExpressLine1.end(); ++it)
	{
		int lid = *it;
		Line *res = m_dao.GetData();
		DrawLine(pDC, &res[lid], true, true);
	}

	DeleteObject(cPen0);
	DeleteObject(cPen1);
}

void CMapOnfoView::DrawTestLine(CDC *pDC, TwMapDAO &dao)
{
	CPen cPen, cSelPen, cMulPen, cSelPen2;
	cPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	cSelPen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	cSelPen2.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	cMulPen.CreatePen(PS_SOLID, 2, RGB(225, 158, 53));
	pDC->SelectObject(&cPen);
	Line *res = dao.GetData();
	int sel = dao.GetRoadSelection();
	for (int i = 0; i < dao.m_line_cnt; ++i)
	{
		// Segment
		if (i == sel)
			continue;
		DrawLine(pDC, &res[i], false, false);
	}
	
	pDC->SelectObject(&cSelPen);
	if (sel != -1 && (m_mapMode == MAP_SEL_MODE || m_mapMode == MAP_EDIT_MODE))
	{
		DrawLine(pDC, &res[sel], true, true);
	}

	if (m_analysis)
	{
		char *name = res[sel].name;
		Road *p = dao.GetSelRoad(name);
		int parall_sid = -1;			// 对面车道
		if (res[sel].ort == 0)
		{
			Line *parLine = dao.GetNearLine(sel);
			if (parLine != NULL)
			{
				parall_sid = parLine->lid;
				pDC->SelectObject(&cSelPen2);
				DrawLine(pDC, parLine, false, true);
			}
		}
		for (int i = 0; i < p->line_list.size(); ++i)
		{
			int sid = p->line_list[i]->lid;
			if (sid == sel || sid == parall_sid)
				continue;
			pDC->SelectObject(&cMulPen);
			DrawLine(pDC, &res[sid], false, true);
			DrawLineCross(pDC, &res[sid]);
		}
	}

	DeleteObject(cMulPen);
	DeleteObject(cPen);
	DeleteObject(cSelPen);
	DeleteObject(cSelPen2);
}

CMapOnfoDoc* CMapOnfoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapOnfoDoc)));
	return (CMapOnfoDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapOnfoView 消息处理程序

void Test()
{
	std::map<Point, int> a;
	Point p0(64534.123123, 64534.123123);
	Point p1(64534.123123, 64534.123123);
	Point p2(64534.123124, 64534.123124);
	a[p0] = 0;
	a[p1] = 1;
	a[p2] = 2;
	int flag = 0;
}


void CMapOnfoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	InitShowType();
	m_screenPx = GetSystemMetrics(SM_CXFULLSCREEN);
	m_screenPy = GetSystemMetrics(SM_CYFULLSCREEN);
	m_dao.ReadData();
	m_center_point = m_dao.GetCenter();
	m_scale = 1.5;			// init scale
	m_mouseState = -1;		// no select
	m_mapMode = 0;			// viewMode
	m_showGrid = 0;
	m_showGridLine = 0;
	m_gridLinePoint = 0;
	m_selGrid = NULL;
	m_bCenterMode = 0;
	

	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_pane.GetPropertyDlg()->m_pView = this;
}


void CMapOnfoView::ChangeGridLinePoint()
{
	m_gridLinePoint = !m_gridLinePoint;
	Invalidate();
}


void CMapOnfoView::ChangeVisPoint()
{
	m_visPoint = !m_visPoint;
	Invalidate();
}


void CMapOnfoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_mapMode == 1 && m_last_point.x == point.x && m_last_point.y == point.y)
	{
		Point factPoint;
		Screen2Point(point.x, point.y, factPoint.x, factPoint.y);
		m_selLineID = m_dao.FindNearestSegment(factPoint);
		if (m_dao.GetRoadSelection() == -1)
			m_analysis = false;
		Invalidate(TRUE);

		CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
		TwMapPropertyDlg *pDlg = pMain->GetPane()->GetPropertyDlg();
		pDlg->UpdateRoadData(m_dao.GetSelLine());
		//m_analysis = false;
	}
	else if (!m_bPointSel && m_mouseState == MOUSE_DOWN)
	{
		int ix = point.x - m_last_point.x, iy = point.y - m_last_point.y;
		MoveScreen(ix, iy);
	}
	// 选中点 Edit Mode
	if (m_mapMode == MAP_EDIT_MODE && m_last_point.x == point.x && m_last_point.y == point.y)
	{
		Point factPoint;
		Screen2Point(point.x, point.y, factPoint.x, factPoint.y);
		m_selPointID = m_dao.FindNearestPoint(factPoint);
		m_bPointSel = false;
		if (m_selPointID == -1)
			return;
		m_bPointSel = true;
		Invalidate();
	}
	else if (m_bPointSel && m_mouseState == MOUSE_DOWN)
	{
		Point factPoint;
		Screen2Point(point.x, point.y, factPoint.x, factPoint.y);
		int near_id = m_dao.FindNearestPoint(factPoint);
		if (near_id != -1)
		{
			m_dao.MovePoint(m_selPointID, near_id);
		}
		Invalidate();
	}
	m_mouseState = MOUSE_UP;
	CView::OnLButtonUp(nFlags, point);
}


void CMapOnfoView::MoveScreen(int offsetX, int offsetY)
{
	double dx = double(offsetX) / m_scale;
	double dy = double(offsetY) / m_scale;

	m_center_point.x -= dx, m_center_point.y += dy;
	Invalidate(TRUE);
}


void CMapOnfoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//TRACE("down %d, %d\n", point.x, point.y);
	m_last_point = point;
	m_mouseState = MOUSE_DOWN;
	CView::OnLButtonDown(nFlags, point);
}


void CMapOnfoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_mousePoint = point;
	Screen2Point(point.x, point.y, m_factPoint.x, m_factPoint.y);
	//screen2point(point.x, point.y, m_center_point, m_scale, screenPx, screenPy, m_factPoint.x, m_factPoint.y);
	RECT rect; 
	LPRECT lpRect = &rect;
	lpRect->left = MOUSEINFOX;
	lpRect->right = MOUSEINFOX + 250;
	lpRect->top = 10;
	lpRect->bottom = 50;
	InvalidateRect(lpRect);
	CView::OnMouseMove(nFlags, point);
}


BOOL CMapOnfoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
	{
		m_scale *= 1.2;
	}
	else
	{
		m_scale /= 1.2;
	}
	Invalidate(TRUE);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMapOnfoView::OnSelMode()
{
	// TODO:  在此添加命令处理程序代码  
	if (m_mapMode == MAP_SEL_MODE)
		m_mapMode = 0;
	else
		m_mapMode = MAP_SEL_MODE;
	//m_mapMode = 1 - m_mapMode;
	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	CString modes[] = { L"View Mode", L"Sel Mode" };
	pMain->SetMode(modes[m_mapMode]);
	if (m_mapMode == 0)
	{
		m_dao.SetRoadSelection(-1);
	}
}

void CMapOnfoView::AnalysisSelLine()
{
	//GeneDoubleWay();
}


void CMapOnfoView::ShowExpress()
{
	int sel = m_dao.GetRoadSelection();
	if (sel == -1)
		return;
	m_ExpressLine0.clear();
	Line *ln_list = m_dao.m_line_list;
	for (int i = 0; i < m_dao.m_line_cnt; ++i)
	{
		if (ln_list[i].ort == ORT_ONEWAY && strcmp(ln_list[i].rank, "快速路") == 0 && 
			strcmp(ln_list[sel].name, ln_list[i].name) == 0)
		{
			m_ExpressLine0.insert(i);
		}
	}
	Invalidate();
}

ModLine CMapOnfoView::GeneCenterExpress(const vector<vector<int>> &exp_list)
// exp_list 存放 lid
// 两个vector 放对向的两条道路，按顺序存放lid
{
	Point *pt_list = m_dao.m_point_list;
	Line* ln_list = m_dao.m_line_list;

	// 生成两条ModPoint为顺序的道路
	vector<ModPoint> mp0;
	for (int i = 0; i < exp_list[0].size(); ++i)
	{
		int lid = exp_list[0][i];
		if (i == 0)
		{
			for (int j = 0; j < ln_list[lid].point_list.size(); ++j)
			{
				int pid = ln_list[lid].point_list[j];
				ModPoint pt;
				pt.x = pt_list[pid].x, pt.y = pt_list[pid].y;
				mp0.push_back(pt);
			}
		}
		else
		{
			for (int j = 1; j < ln_list[lid].point_list.size(); ++j)
			{
				int pid = ln_list[lid].point_list[j];
				ModPoint pt;
				pt.x = pt_list[pid].x, pt.y = pt_list[pid].y;
				mp0.push_back(pt);
			}
		}
	}
	vector<ModPoint> mp1;
	for (int i = 0; i < exp_list[1].size(); ++i)
	{
		int lid = exp_list[1][i];
		if (i == 0)
		{
			for (int j = 0; j < ln_list[lid].point_list.size(); ++j)
			{
				int pid = ln_list[lid].point_list[j];
				ModPoint pt;
				pt.x = pt_list[pid].x, pt.y = pt_list[pid].y;
				mp1.push_back(pt);
			}
		}
		else
		{
			for (int j = 1; j < ln_list[lid].point_list.size(); ++j)
			{
				int pid = ln_list[lid].point_list[j];
				ModPoint pt;
				pt.x = pt_list[pid].x, pt.y = pt_list[pid].y;
				mp1.push_back(pt);
			}
		}
	}
	// 两条道路同序
	reverse(mp1.begin(), mp1.end());
	// 插值
	vector<ModPoint> new_mp0 = InsertPoints(mp0);
	vector<ModPoint> new_mp1 = InsertPoints(mp1);
	
	vector<ModPoint> center_list = GetCenterPoints(new_mp0, new_mp1);
	m_drawModPoint[0] = new_mp0;
	m_drawModPoint[1] = new_mp1;
	ModLine retLine;
	for (int i = 0; i < center_list.size(); ++i)
	{
		center_list[i].pid = int(m_GenePoints.size());
		m_GenePoints.push_back(center_list[i]);
		retLine.point_list.push_back(center_list[i].pid);
	}
	m_GeneLines.push_back(retLine);
	return retLine;
}

void CMapOnfoView::AnalysisExpress()
{
	m_ExpressLine0.clear();
	m_GeneLines.clear();
	m_GenePoints.clear();

	Line *ln_list = m_dao.m_line_list;
	/*int sel = m_dao.GetRoadSelection();
	if (sel == -1)
		return;*/
	// 先打表确定各个道路的起始lid
	map<unsigned int, pair<int, int>> express_hash;
	map<unsigned int, int> exp_index;
	express_hash[0] = pair<int, int>(5141, 132);
	express_hash[1] = pair<int, int>(258, 5044);
	express_hash[2] = pair<int, int>(1599, 1565);
	express_hash[3] = pair<int, int>(6065, 4381);
	express_hash[4] = pair<int, int>(5602, 462);
	express_hash[5] = pair<int, int>(4562, 794);
	express_hash[6] = pair<int, int>(6582, 1354);
	express_hash[7] = pair<int, int>(5280, 424);
	express_hash[8] = pair<int, int>(94, 5110);
	exp_index[ELFhash("中河高架路")] = 0;
	exp_index[ELFhash("钱江四桥（复兴大桥）")] = 1;
	exp_index[ELFhash("时代大道高架")] = 2;
	exp_index[ELFhash("钱江三桥（西兴大桥）")] = 3;
	exp_index[ELFhash("德胜高架路")] = 4;
	exp_index[ELFhash("石祥高架路")] = 5;
	exp_index[ELFhash("上塘高架路")] = 6;
	exp_index[ELFhash("石桥高架路")] = 7;
	exp_index[ELFhash("石大快速路")] = 8;
	int cnt = 9;
	set<int> temp;
	map<int, int> temp_hash;
	vector<set<int>> express_set_list;
	vector<map<int, int>> link_list;
	express_set_list.resize(cnt, temp);
	link_list.resize(cnt, temp_hash);
	for (int i = 0; i < m_dao.m_line_cnt; ++i)
	{
		if (ln_list[i].ort == ORT_ONEWAY && strcmp(ln_list[i].rank, "快速路") == 0)
		{
			char *name = ln_list[i].name;
			int idx = exp_index[ELFhash(name)];			// 0 - 8
			express_set_list[idx].insert(i);
			int p0 = ln_list[i].point_list[0];
			link_list[idx][p0] = i;
		}
	}
	vector<vector<vector<int>>> info_list;
	for (int i = 0; i < cnt; ++i)
	{
		const set<int> &eset = express_set_list[i];
		set<int> stop;
		stop.insert(express_hash[i].first);
		stop.insert(express_hash[i].second);
		map<int, int> &link_map = link_list[i];
		set<int>::iterator it;
		int lid = express_hash[i].first;
		vector<vector<int>> temp;
		vector<int> lid_list0;
		while (true)
		{
			lid_list0.push_back(lid);
			int pid = ln_list[lid].point_list[ln_list[lid].point_list.size() - 1];
			if (link_map.find(pid) == link_map.end())
				break;
			lid = link_map[pid];
			if (stop.find(lid) != stop.end())
				break;
		}
		temp.push_back(lid_list0);
		vector<int> lid_list1;
		lid = express_hash[i].second;
		while (true)
		{
			lid_list1.push_back(lid);
			int pid = ln_list[lid].point_list[ln_list[lid].point_list.size() - 1];
			if (link_map.find(pid) == link_map.end())
				break;
			lid = link_map[pid];
			if (stop.find(lid) != stop.end())
				break;
		}
		temp.push_back(lid_list1);
		info_list.push_back(temp);
	}
	for (int i = 0; i < cnt; ++i)
	{
		if (i == 8)
			GeneCenterExpress(info_list[i]);
	}
	Invalidate();
}


void CMapOnfoView::Analysis()
{
	//GeneDoubleWay();
	ShowExpress();
}


BOOL CMapOnfoView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}


void CMapOnfoView::ChangeShowType(int i)
{
	m_showType[i] = 1 - m_showType[i];
	Invalidate();
}

void CMapOnfoView::ChangeAllShow()
{
	for (int i = 0; i < 10; ++i)
	{
		m_showType[i] = 1;
	}
	m_showType[4] = 0;
	Invalidate();
}

void CMapOnfoView::ChangeAllNotShow()
{
	for (int i = 0; i < 10; ++i)
	{
		m_showType[i] = 0;
	}
	Invalidate();
}

void CMapOnfoView::OnMenuGrid()
{
	// TODO:  在此添加命令处理程序代码
	m_showGrid = !m_showGrid;
	Invalidate();
}


void CMapOnfoView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_showGrid)
		m_showGridLine = true;
	else
	{
		m_showGridLine = false;
		m_selGrid = NULL;
		return;
	}
	double x, y;
	Screen2Point(point.x, point.y, x, y);
	int ir, ic;
	m_dao.GetRowCol(x, y, ir, ic);
	m_selGrid = &m_dao.m_grid[ir][ic];
	Invalidate();
	CView::OnLButtonDblClk(nFlags, point);
}


void CMapOnfoView::OnCtMode()
{
	// TODO:  在此添加命令处理程序代码
	// 编辑模式
	if (m_mapMode == MAP_SEL_MODE)
		m_mapMode = MAP_EDIT_MODE;
	else if (m_mapMode == MAP_EDIT_MODE)
		m_mapMode = 0;
		
	if (m_mapMode != MAP_EDIT_MODE)
		m_bPointSel = false;
	Invalidate();

	CMainFrame *pMain = (CMainFrame*)AfxGetMainWnd();
	if (m_mapMode == MAP_EDIT_MODE)
		pMain->SetMode(L"Edit Mode");
	else
		pMain->SetMode(L"View Mode");
}
