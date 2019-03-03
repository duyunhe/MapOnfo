#pragma once
#include "map_struct.h"
#define TW_GEO_PI 3.1415926

double Point2Segment(Point px, Point p0, Point p1, int &state);
double Point2Point(Point p0, Point p1);
double Segment2Segment(Point p0, Point p1, Point q0, Point q1, int &state);
int Point2Line(LinePtr l, PointPtr p, double &dist, Point *pt_list, Segment &seg);
int Line2Line(LinePtr l0, LinePtr l1, double &dist, Point* pt_list);
inline bool Zero(double x);
bool Equal(double x, double y);
bool CrossProduct(Point pt, Point p0, Point p1);
double GetDist(Point px, Point py);
double GetDist(ModPoint px, ModPoint py);
double DotProduct(Point pt, Point p0, Point p1);
double GetCos(Point pt, Point p0, Point p1);
void GetParallel(Point p0, Point p1, Point &dstp0, Point &dstp1, Point &dstp2, Point &dstp3);
void GetLineEquation(Point p0, Point p1, double &a, double &b, double &c);
void GetPointOffset(Point pt, Segment sgmt, double offset, ModPoint &dstPt);
// 计算垂直于线段的偏移点
void GetPointOffsetOnSegment(Segment sgmt, double offset, ModPoint &dstPt);
// 计算从线段起点开始在线段上的偏移点
void GetCrossPoint(Point s0p0, Point s0p1, Point s1p0, Point s1p1, ModPoint &crsPt);
double GetVectorSin(Point p0, Point p1);
double GetVectorAngle(Point p0, Point p1);
double ProjectDist(LinePtr l0, LinePtr l1, Point *pt_list);
double PointProjectSegment(Point pt, Point p0, Point p1);
vector<ModPoint> GetCenterPoints(const vector<ModPoint> &pts0, const vector<ModPoint> &pts1);
vector<ModPoint> InsertPoints(const vector<ModPoint> &pts);