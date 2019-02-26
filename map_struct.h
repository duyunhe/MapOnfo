#pragma once
#include <vector>
#include <string>
#include <map>
#include "bilist.h"
using namespace std;

struct Line;
typedef struct Line* LinePtr;

#define RAIL_PT(pLine) (PointPtr)(pLine->point_list->head->forward->item)
#define TAIL_PT(pLine) (PointPtr)(pLine->point_list->head->backward->item)
#define HEAD_PL(pLine) (pLine->point_list->head)

#define ORT_ONEWAY		1
#define ORT_DOUBLEWAY	0
#define ORT_CONJ		4

struct Point
{
	double x;
	double y;
	int pid;
	bool cross;
	// PBiList line_list;
	vector<pair<int, int>> line_list;		// line id, seq of segment
	bool operator <(const struct Point&s) const;
	Point(double x, double y);
	Point();
	void addAdjectLine(int lid);
};

typedef struct Point* PointPtr;

struct PointLineInfo
{
	int lid;
	int seq;
	bool left;  
	bool forward;
};

struct ModPoint
{
	double x, y;
	vector<PointLineInfo> pli_list;
	int pid;
};

struct ModLine
{
	vector<int> point_list;
	int org_lid;
	int lid;
	bool left;
};

struct Segment
{
	Point begin_pt;
	Point end_pt;
};

struct Line
{
	//PBiList point_list;			// store pid
	vector<int> point_list;
	char *rank;
	int ort;		// ��˫��
	int lid;
	int rid;		// Road
	int begin_pid;
	int end_pid;
	char *name;		// Road Name
	int fact_dir;	// �ڵ���·��ʵ�ʵķ���
	int deg;		// �ڵ�·�еĶ��� ����Ϊ2  1Ϊ��·�����յ㣬3Ϊ�ֲ�
	int par_lid;	
	double dist;
	void addPoint(int pid);
	Line();
};

typedef struct LineSeq *LinePtrSeq;

struct LineInfo
{
	int lid;
	int seq;
	double theta;
	bool swap;
};	
bool cmp_lineinfo(LineInfo a, LineInfo b);

struct Road
{
	char *name;
	std::vector<LinePtr> line_list;
	std::map<int, int> near_list;			
	// nearest Line   near_list[line_id0] = line_id1
	// means the nearest line of l0 is l1
	std::vector<LinePtrSeq> lines;
};


struct Grid
{
	std::vector<int> line_list;		// lid
};