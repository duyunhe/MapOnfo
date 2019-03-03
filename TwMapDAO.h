#pragma once

#include "sqlite3.h"
#include <vector>
#include <map>
#include <string>
#include "bilist.h"
#include "map_struct.h"

#define MAXMAPX 130000
#define MINMAPX 30000
#define MAXMAPY	130000
#define MINMAPY	30000
#define GRIDMAP	2000
#define GRID_ROW (MAXMAPY - MINMAPY) / GRIDMAP + 1
#define GRID_COL (MAXMAPX - MINMAPX) / GRIDMAP + 1

class TwMapDAO
{
public:
	sqlite3 *m_sql;
	Line m_line_list[10000];						// store lines, global
	map<int, int> line_index;						// line id -> index
	Grid m_grid[GRID_ROW][GRID_COL];
	
	std::vector<Road> m_road_list;
	Point m_point_list[50000];						// store points, global
	int m_point_cnt;
	
	int m_sel;
	std::map<std::string, int> name_map;
	std::map<Point, int> m_pid;
	
	Point center_point;
	double scale;
	double px, py;			// screen x & y

	Line m_map_line_list[20000];					// on map line
	Point m_map_point_list[100000];					// on map point
	vector<int> m_gene_list;

	void CrossPoint();
	void ProcessRoad();
	Road &GetCurRoad();
public:
	int m_line_cnt;
	TwMapDAO();
	~TwMapDAO();
	Point *GetPoint(int pid);
	int InsertPointID(Point);
	int GetRoadSelection();
	void SetRoadSelection(int sel);
	Line* GetNearLine(int lid);

	void ReadData();
	// ReadData  读取地图，设定地图中心点xy
	void ReadDataFromTxt();
	void GCRoad();
	void AnalysisRoad();
	// AnalysisRoad 分析道路
	void GetParallelLine();

	// GetSeq
	int GetDirection(Road &rd);
	void GetLineSeqs();
	void SetScreenXY(double x, double y);
	const Point& GetCenter();
	void SetCenter(double x, double y);
	Line *GetData();
	int FindNearestSegment(Point pt);
	int FindNearestPoint(Point pt);
	
	void MovePoint(int pid0, int pid1);
	// 将pid0的点移动到pid1
	Line* GetSelLine();
	Road* GetSelRoad(char* name);
	void DivideGrid();
	int GetRowCol(double x, double y, int &ir, int &ic);
};

