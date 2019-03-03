
// MapOnfoView.h : CMapOnfoView 类的接口
//

#pragma once
#include "map_struct.h"
#include "MapOnfoDoc.h"
#include "TwMapDAO.h"
#include <vector>
#include <set>
using namespace std;

#define MAP_SEL_MODE  1
#define MAP_EDIT_MODE 2

class CMapOnfoView : public CView
{
	TwMapDAO m_dao;
	Point m_center_point;
	Grid *m_selGrid;
	double m_scale;
	int m_mouseState;
	bool m_showGrid;
	int m_screenPx;
	int m_screenPy;
	CPoint m_last_point;
	Point m_lastFactPoint;		// 记录鼠标按下时选取的实际点
	CPoint m_mousePoint;
	Point m_factPoint;
	int m_mapMode;				// 0 view mode, 1 sel mode, 2 edit mode
	bool m_bCenterMode;
	bool m_bPointSel;			// 点已经被选中，此时高亮十字
	int m_CenterStep;
	bool m_visPoint;			// 0 print point, 1 not 
	bool m_analysis;			// 0 no analysis, 1 yes
	bool m_showGridLine;
	bool m_gridLinePoint;		// 0 no point, line id ...  1 yes
	int m_selLineID;
	int m_selPointID;			// edit mode下选中点
	vector<Segment> m_geneSegment;
	vector<ModPoint> m_drawModPoint[2];

	BOOL m_showType[10];		// 
	BOOL m_showOneWay;
	BOOL m_showDoubleWay;

	vector<ModPoint> m_GenePoints;
	vector<ModLine> m_GeneLines;

	set<int> m_ExpressLine0;
	set<int> m_ExpressLine1;

protected: // 仅从序列化创建
	CMapOnfoView();
	DECLARE_DYNCREATE(CMapOnfoView)

	int GetRoadTypeCode(char* type);
	void InitShowType();
	void ChangeShowType(int i);
	void ChangeAllShow();
	void ChangeAllNotShow();

	void DrawSegments(CDC *pDC);
	void DrawGenePoints(CDC *pDC);
	void DrawModLine(CDC *pDC, ModLine *ln);
	void DrawGeneLines(CDC *pDC);
	void DrawTestLine(CDC* pDC, TwMapDAO &dao);
	void DrawDelaunay(CDC* pDC);
	void DrawExpressLine(CDC *pDC);
	void DrawCross(CDC* pDC);
	void DrawGrid(CDC *pDC);
	void DrawSegment(Line &sgmt);
	void DrawSelGrid(CDC *pDC);
	void DrawSelPoint(CDC *pDC);
	void MoveScreen(int offsetX, int offsetY);
	void Point2Screen(double srcX, double srcY, int &pixelX, int &pixelY);
	void Screen2Point(int pixelX, int pixelY, double &sx, double &sy);
	void DrawMouseInfo(CDC *pDC);
	void ChangeVisPoint();
	void DrawSmallCross(CDC *pDC, int srcX, int srcY);
	void DrawLine(CDC *pDC, Line *line, bool, bool);
	void DrawLineCross(CDC *pDC, Line *line);
	void AnalysisSelLine();
	void Analysis();
	void AnalysisExpress();
	ModLine GeneCenterExpress(const vector<vector<int>> &exp_list);
	void ShowExpress();
	void ChangeOneway();
	void ChangeDoubleWay();
	void GeneDoubleWayGrid();			// 双向路生成两条对向边	Grid Test 
	void GeneDoubleWay();				// normal
	void GeneExpressPass();				// 快速路
	vector<int> FindNearDoubleLine(int lid);
	vector<ModPoint> GenePoints(const set<int> &lnSet, const Point &srcPt, Point *pt_list, Line* ln_list, int);
	void GeneExpressPoint(Point* pt, Segment* sgmt, ModPoint &mp);
	// 转角生成点
	void ChangeGridLinePoint();
	vector<ModPoint> DogLast(const vector<ModPoint> &src);
	vector<int> _DogLast(const vector<ModPoint> &src, int bid, int eid);

// 特性
public:
	CMapOnfoDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CMapOnfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSelMode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMenuGrid();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCtMode();
};

#ifndef _DEBUG  // MapOnfoView.cpp 中的调试版本
inline CMapOnfoDoc* CMapOnfoView::GetDocument() const
   { return reinterpret_cast<CMapOnfoDoc*>(m_pDocument); }
#endif

