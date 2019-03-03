#include "stdafx.h"
#include "TwGeo.h"
#include <cmath>

#define EPS 1e-6


inline bool Zero(double x)
{
	return x < EPS && x > -EPS;
}

bool Equal(double x, double y)
{
	return Zero(x - y);
}

double Segment2Segment(Point p0, Point p1, Point q0, Point q1, int &state)
{
	return 0;
}

int Point2Line(LinePtr l, PointPtr p, double &dist, Point *pt_list, Segment &seg)
{
	double min_dist = 1e20;
	int sel = -1;
	for (int i = 0; i < l->point_list.size() - 1; ++i)
	{
		Point *p0 = &pt_list[l->point_list[i]], *p1 = &pt_list[l->point_list[i + 1]];
		int s;
		double d = Point2Segment(*p, *p0, *p1, s);
		if (d < min_dist)
		{
			min_dist = d;
			sel = i;
		}
	}
	dist = min_dist;
	seg.begin_pt.x = pt_list[l->point_list[sel]].x;
	seg.begin_pt.y = pt_list[l->point_list[sel]].y;

	seg.end_pt.x = pt_list[l->point_list[sel + 1]].x;
	seg.end_pt.y = pt_list[l->point_list[sel + 1]].y;
	return 0;
}

int Line2Line(LinePtr l0, LinePtr l1, double &dist, Point *pt_list)			// like Hausdorff Distance
{
	double h_dist = 0;
	for (int i = 0; i < l0->point_list.size(); ++ i)
	//for (PBiNode pi = l0->point_list->head->forward; pi != l0->point_list->head; pi = pi->forward)
	{
		Point *pt = &pt_list[l0->point_list[i]];
		//Point *pt = (Point*)pi->item;
		double min_dist = 1e20;
		for (int j = 0; j < l1->point_list.size(); ++ j)
		//for (PBiNode pj = l1->point_list->head->forward; pj->forward != l1->point_list->head; pj = pj->forward)
		{
			PointPtr p0 = &pt_list[l1->point_list[0]], p1 = &pt_list[l1->point_list[l1->point_list.size() - 1]];
			int s;
			double dist = Point2Segment(*pt, *p0, *p1, s);
			min_dist = min(min_dist, dist);
		}
		h_dist = max(h_dist, min_dist);
	}
	dist = h_dist;
	return 0;
}

double PointProjectSegment(Point pt, Point p0, Point p1)
{
	double x = pt.x, y = pt.y;
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	double d2 = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
	if (Zero(d2))
	{
		return 0;
	}
	double cr = (x1 - x0) * (x - x0) + (y1 - y0) * (y - y0);
	double px = x0 + (x1 - x0) / d2 * cr, py = y0 + (y1 - y0) / d2 * cr;
	return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}

double Point2Segment(Point pt, Point p0, Point p1, int &state)
{
	double x = pt.x, y = pt.y;
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	double d2 = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
	if (Zero(d2))
	{
		state = -2;
		return 0;
	}
	double cr = (x1 - x0) * (x - x0) + (y1 - y0) * (y - y0);
	if (cr < 0)
	{
		state = -1;
		return sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
	}
	
	if (cr > d2)
	{
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
	}
	double px = x0 + (x1 - x0) / d2 * cr, py = y0 + (y1 - y0) / d2 * cr;
	state = 0;
	return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}


bool CrossProduct(Point pt, Point p0, Point p1)
{
	double x = pt.x, y = pt.y;
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	return (y - y0) * (x1 - x0) - (y1 - y0) * (x - x0) > 0;
}


double GetDist(Point px, Point py)
{
	return sqrt((px.x - py.x) * (px.x - py.x) + (px.y - py.y) * (px.y - py.y));
}

double GetDist(ModPoint px, ModPoint py)
{
	return sqrt((px.x - py.x) * (px.x - py.x) + (px.y - py.y) * (px.y - py.y));
}

double DotProduct(Point pt, Point p0, Point p1)
// dot(ab, ac)  a: pt   b: p0,  c: p1
{
	double x = pt.x, y = pt.y;
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	return (x0 - x) * (x1 - x) + (y0 - y) * (y1 - y);
}

double SelfProduct(Point p0, Point p1)
{
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}


double GetCos(Point pt, Point p0, Point p1)
{
	double dp = DotProduct(pt, p0, p1);
	return dp / (SelfProduct(pt, p0) * SelfProduct(pt, p1));
}


void GetPointOffset(Point pt, Segment sgmt, double offset, ModPoint &dstPt)
{
	double x0 = sgmt.begin_pt.x, y0 = sgmt.begin_pt.y;
	double x1 = sgmt.end_pt.x, y1 = sgmt.end_pt.y;
	double x = pt.x, y = pt.y;
	Point pa = sgmt.begin_pt;
	Point pb = sgmt.end_pt;
	double r = DotProduct(pa, pt, pb) / DotProduct(pa, pb, pb);
	double xd = (x1 - x0) * r, yd = (y1 - y0) * r;
	double xc = x0 + xd, yc = y0 + yd;
	
	double unit = sqrt((x - xc) * (x - xc) + (y - yc) * (y - yc));
	double ux = (x - xc) / unit, uy = (y - yc) / unit;
	dstPt.x = xc + offset * ux, dstPt.y = yc + offset * uy;
}


void GetParallel(Point p0, Point p1, Point &dstp0, Point &dstp1, Point &dstp2, Point &dstp3)
{
	double d = 10;
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	double dx = x1 - x0, dy = y1 - y0;
	double norm = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
	double unitx = dx / norm, unity = dy / norm;
	// 右手边
	double xh0 = x0 + unity * d, yh0 = y0 - unitx * d;
	double xh1 = x1 + unity * d, yh1 = y1 - unitx * d;
	dstp0.x = xh0, dstp0.y = yh0;
	dstp1.x = xh1, dstp1.y = yh1;

	// 左手边
	double xh2 = x0 - unity * d, yh2 = y0 + unitx * d;
	double xh3 = x1 - unity * d, yh3 = y1 + unitx * d;
	dstp2.x = xh2, dstp2.y = yh2;
	dstp3.x = xh3, dstp3.y = yh3;
}


void GetLineEquation(Point p0, Point p1, double &a, double &b, double &c)
{
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	a = y1 - y0, b = x0 - x1, c = x1 * y0 - y1 * x0;
	double d = sqrt(a * a + b * b);
	a = a / d, b = b / d, c = c / d;
}


void GetCrossPoint(Point s0p0, Point s0p1, Point s1p0, Point s1p1, ModPoint &crsPt)
{
	double a0, b0, c0;
	GetLineEquation(s0p0, s0p1, a0, b0, c0);

	double a1, b1, c1;
	GetLineEquation(s1p0, s1p1, a1, b1, c1);

	double d = a0 * b1 - a1 * b0;
	if (fabs(d) < 1e-10)
		return;
	else
	{
		double px = (b0 * c1 - b1 * c0) / d;
		double py = (c0 * a1 - c1 * a0) / d;
		crsPt.x = px, crsPt.y = py;
	}
}


double GetVectorSin(Point p0, Point p1)
{
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	double x = x1 - x0, y = y1 - y0;
	return x / sqrt(x * x + y * y);
}


double GetVectorAngle(Point p0, Point p1)
{
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	double x = x1 - x0, y = y1 - y0;
	double angle = atan2(y, x) * 180.0 / TW_GEO_PI;
	return angle;
}


double Point2Point(Point p0, Point p1)
{
	double x0 = p0.x, y0 = p0.y;
	double x1 = p1.x, y1 = p1.y;
	return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

double ProjectDist(LinePtr src, LinePtr target, Point *pt_list)
{
	/*
	src : 待投影的道路，需要计算该路应投影至哪条道路
	target : 候选道路
	*/
	for (int i = 0; i < target->point_list.size() - 1; ++i)
	{
		int pid0 = target->point_list[i], pid1 = target->point_list[i + 1];
		Point p0 = pt_list[pid0], p1 = pt_list[pid1];
		// 找到最大值
		double max_dist = 0;
		for (int j = 0; j < src->point_list.size(); ++j)
		{
			int pid = src->point_list[j];
			Point pt = pt_list[pid];
			double project_dist = PointProjectSegment(pt, p0, p1);
			max_dist = max(max_dist, project_dist);
		}
	}
	return 0;
}


vector<ModPoint> GetCenterPoints(const vector<ModPoint> &pts0, const vector<ModPoint> &pts1)
{
	vector<ModPoint> ret;
	int i = 0, j = 0;			// two pointer
	while (i < pts0.size() - 1 && j < pts1.size() - 1)
	{
		// add Point
		ModPoint pt;
		pt.x = (pts0[i].x + pts1[j].x) / 2, pt.y = (pts0[i].y + pts1[j].y) / 2;
		ret.push_back(pt);
		// move on
		int ni = i + 1, nj = j + 1;
		double disti, distj;
		disti = GetDist(pts0[ni], pts1[j]);
		distj = GetDist(pts0[i], pts1[nj]);
		if (disti <= distj)
		{
			i = ni;
		}
		else
		{
			j = nj;
		}
	}
	if (i == pts0.size() - 1)		// i到终点
	{
		while (j < pts1.size())
		{
			ModPoint pt;
			pt.x = (pts0[i].x + pts1[j].x) / 2, pt.y = (pts0[i].y + pts1[j].y) / 2;
			ret.push_back(pt);
			++j;
		}
	}
	else
	{
		while (i < pts0.size())
		{
			ModPoint pt;
			pt.x = (pts0[i].x + pts1[j].x) / 2, pt.y = (pts0[i].y + pts1[j].y) / 2;
			ret.push_back(pt);
			++i;
		}
	}
	return ret;
}


void GetPointOffsetOnSegment(Segment sgmt, double offset, ModPoint &dstPt)
{
	double x0 = sgmt.begin_pt.x, y0 = sgmt.begin_pt.y;
	double x1 = sgmt.end_pt.x, y1 = sgmt.end_pt.y;
	double dx = x1 - x0, dy = y1 - y0;
	double len = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
	double x = x0 + offset / len * dx, y = y0 + offset / len * dy;
	dstPt.x = x, dstPt.y = y;
}


vector<ModPoint> InsertPoints(const vector<ModPoint> &pts)
{
	vector<ModPoint> ret;
	for (int i = 0; i < pts.size() - 1; ++i)
	{
		const double OFF = 20;
		ModPoint p0, p1;
		p0 = pts[i], p1 = pts[i + 1];
		double dist = GetDist(p0, p1);
		double offset = OFF;
		ret.push_back(pts[i]);
		if (dist > OFF)
		{
			while (offset < dist)
			{
				Segment sg;
				sg.begin_pt.x = p0.x, sg.begin_pt.y = p0.y;
				sg.end_pt.x = p1.x, sg.end_pt.y = p1.y;
				ModPoint pt;
				GetPointOffsetOnSegment(sg, offset, pt);
				ret.push_back(pt);
				offset += OFF;
			}
		}
	}
	ret.push_back(pts[pts.size() - 1]);
	return ret;
}


