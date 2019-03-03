#include "stdafx.h"
#include "map_struct.h"
#include "TwGeo.h"

void Line::addPoint(int pid)
{
	point_list.push_back(pid);
}

Line::Line()
{
	par_lid = -1;
}

bool Point::operator <(const struct Point&s) const
{
	return uid < s.uid;
}

Point::Point(double x, double y)
{
	this->x = x, this->y = y;
	cross = 0;
}

Point::Point()
{
	cross = 0;
}

void Point::addAdjectLine(int lid)
{
	//line_list.push_back(lid);
}

bool cmp_lineinfo(LineInfo a, LineInfo b)
{
	return a.theta < b.theta;
}