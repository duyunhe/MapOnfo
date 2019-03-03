#include "stdafx.h"
#include "TwMapDAO.h"
#include "Coord.h"
#include "TwGeo.h"
#include <ctime>
#include <set>
#include <string>
#include "common.h"
using namespace std;


TwMapDAO::TwMapDAO()
{
	int res = sqlite3_open("hz1.db", &m_sql);
	m_sel = -1;
	m_line_cnt = 0;
	m_point_cnt = 0;
}

TwMapDAO::~TwMapDAO()
{
	sqlite3_close(m_sql);
	m_sql = NULL;

	for (int i = 0; i < m_road_list.size(); ++i)
	{
		free(m_road_list[i].name);
		m_road_list[i].name = NULL;
	}

	for (int i = 0; i < m_line_cnt; ++i)
	{
		free(m_line_list[i].name);
		free(m_line_list[i].rank);
		m_line_list[i].name = NULL;
		m_line_list[i].rank = NULL;
	}
}

void TwMapDAO::GCRoad()
{

}

Point *TwMapDAO::GetPoint(int pid)
{
	return &m_point_list[pid];
}

int TwMapDAO::InsertPointID(Point pt)
{
	std::map<Point, int>::iterator it = m_pid.find(pt);
	int pid;
	if (it != m_pid.end())
	{
		pid = m_pid[pt];
	}
	else
	{
		pid = int(m_pid.size());
		pt.pid = pid;
		m_point_list[pid] = pt;
		m_point_cnt++;
		m_pid[pt] = pid;
	}
	return pid;
}

Line* TwMapDAO::GetNearLine(int lid)
{
	Line* ln = &m_line_list[lid];
	int rid = ln->rid;
	Road &rd = m_road_list[rid];
	std::map<int, int>::iterator it = rd.near_list.find(lid);
	if (it == rd.near_list.end())
		return NULL;
	int near_lid = it->second;
	return &m_line_list[near_lid];
}

void TwMapDAO::GetParallelLine()
{
	if (m_sel == -1)
		return;
	Line *ln = &m_line_list[m_sel];
	std::string s = ln->name;
	int rid = name_map[s];
	Road &rd = m_road_list[rid];
	if (rd.near_list.size() > 0)			// already analysis
		return;
	int line_size = int(rd.line_list.size());
	for (int i = 0; i < line_size; ++i)
	{
		Line *ln = rd.line_list[i];
		if (ln->ort == 2)  // 单向
		{
			double min_dist = 1e10;
			int sel_id = -1;		// select line id
			for (int j = 0; j < line_size; ++j)
			{
				if (i == j)
					continue;
				Line *ln1 = rd.line_list[j];
				if (ln1->ort == 2)
				{
					double dist;
					int ret = Line2Line(ln, ln1, dist, m_point_list);
					if (min_dist > dist)
					{
						min_dist = dist;
						sel_id = ln1->lid;
					}
				}
			}
			ln->dist = min_dist;
			rd.near_list[ln->lid] = sel_id;
			ln->par_lid = sel_id;
		}
	}
}

int TwMapDAO::GetDirection(Road &rd)
{

	// first find dx and dy
	double minx = 1e10, miny = 1e10, maxx = -1e10, maxy = -1e10;
	int easti = -1, southi = -1;
	for (int i = 0; i < rd.line_list.size(); ++i)
	{
		Line* ln = rd.line_list[i];
		for (int i = 0; i < ln->point_list.size(); ++ i)
		//for (PBiNode cur = ln->point_list->head->forward; cur != ln->point_list->head; cur = cur->forward)
		{
			Point *p = &m_point_list[ln->point_list[i]];
			if (p->x < minx)
			{
				minx = p->x;
				easti = i;
			}
			if (p->y < miny)
			{
				miny = p->y;
				southi = i;
			}
			if (p->x > maxx)
			{
				maxx = p->x;
			}
			if (p->y > maxy)
			{
				maxy = p->y;
			}
		}
	}
	double dx = maxx - minx, dy = maxy - miny;
	int ort;
	if (dy > dx)
		ort = 0;			// 南北向
	else
		ort = 1;			// 东西向
	return ort;
}

void TwMapDAO::GetLineSeqs()
{
	/* 
	if (m_sel == -1)
		return;
	
	Road &rd = GetCurRoad();
	int direction = GetDirection(rd);
	// 1 west - east
	// 0 south - north

	char *road_name = rd.name;
	std::vector<std::pair<int, int>> deg_record;
	// 此处，需要判断各个Line的degree
	int line_size = int(rd.line_list.size());
	int **line_deg = (int**)malloc(sizeof(int*) * line_size);		// line_deg[n][2]
	for (int i = 0; i < line_size; ++i)
	{
		line_deg[i] = (int*)malloc(sizeof(int) * 2);
	}
	for (int i = 0; i < rd.line_list.size(); ++i)
	{
		LinePtr pLn = rd.line_list[i];	
		PBiNode head = pLn->point_list->head;
		PBiNode rail = head->forward;
		PBiNode tail = head->backward;
		if (rail == head || rail == tail)			// 一般不会发生
		{
			TRACE(L"not impossible", pLn->lid);
			continue;
		}
		int deg = 0;
		PointPtr p = (PointPtr)rail->item;
		PBiNode headL = p->line_list->head;
		for (PBiNode nodeL = headL->forward; nodeL != headL; nodeL = nodeL->forward)
		{
			LinePtr pl = (LinePtr)nodeL->item;
			if (strcmp(road_name, pl->name) == 0)
			{
				deg++;
			}
		}
		line_deg[i][0] = deg;
		if (deg == 1)
		{
			deg_record.push_back(std::pair<int, int>(pLn->lid, 0));
		}                                                          

		deg = 0; 
		p = (PointPtr)tail->item;
		headL = p->line_list->head;
		for (PBiNode nodeL = headL->forward; nodeL != headL; nodeL = nodeL->forward)
		{
			LinePtr pl = (LinePtr)nodeL->item;
			if (strcmp(road_name, pl->name) == 0)
			{
				deg++;
			}
		}
		line_deg[i][1] = deg;
		if (deg == 1)
		{
			deg_record.push_back(std::pair<int, int>(pLn->lid, 1));
		}
	}
	for (int i = 0; i < line_size; ++i)
	{
		free(line_deg[i]);
		line_deg[i] = 0;
	}
	free(line_deg);
	line_deg = 0;

	// 计算方向与起始点（两条）
	double sumx = 0, sumy = 0;
	int cntx = 0, cnty = 0;
	for (int i = 0; i < deg_record.size(); ++i)
	{
		int lid = deg_record[i].first;
		int mark = deg_record[i].second;
		LinePtr pl = &m_line_list[lid];
		if (direction == 1)		// 东西
		{
			if (mark == 0)
			{
				sumx += (RAIL_PT(pl))->x;
				cntx++;
			}
			else
			{
				sumx += (TAIL_PT(pl))->x;
				cntx++;
			}
		}
		else
		{
			if (mark == 0)
			{
				sumy += (RAIL_PT(pl))->y;
				cnty++;
			}
			else
			{
				sumy += (TAIL_PT(pl))->y;
				cnty++;
			}
		}
	}
	std::vector<std::pair<int, int>> startid;
	std::vector<std::vector<int>> seqs;
	if (direction == 1)
	{
		double aveX = sumx / cntx;
		for (int i = 0; i < deg_record.size(); ++i)
		{
			int lid = deg_record[i].first;
			int mark = deg_record[i].second;
			LinePtr pl = &m_line_list[lid];
			if (mark == 0)
			{
				double px = (RAIL_PT(pl))->x;
				if (px < aveX)		// 东西向中靠西
				{
					startid.push_back(std::pair<int, int>((RAIL_PT(pl))->pid, lid));
				}
			}
			else
			{
				double px = (TAIL_PT(pl))->x;
				if (px < aveX)
				{
					startid.push_back(std::pair<int, int>((TAIL_PT(pl))->pid, lid));
				}
			}
		}
		// 得到起点
		std::vector<int> part;
		bool vis[1000] = {};		// line visited
		while (true)
		{
			std::vector<int> lidsels = ParseEdgeRoad(direction, vis, startid);			// one partition
			if (lidsels.empty())
				break;
			for (int i = 0; i < lidsels.size(); ++i)
			{
				int sel = lidsels[i];
				int lid = startid[sel].second;
				part.push_back(lid);
				vis[lid] = 1;
			}
		}
		
	}
	*/
}

void TwMapDAO::AnalysisRoad()
{
	GetParallelLine();
	//GetLineSeqs();
}

Line* TwMapDAO::GetData()
{
	return m_line_list;
}

Line* TwMapDAO::GetSelLine()
{
	if (m_sel == -1)
		return NULL;
	return &m_line_list[m_sel];
}


void TwMapDAO::SetScreenXY(double x, double y)
{
	px = x, py = y;
}


const Point& TwMapDAO::GetCenter()
{
	return center_point;
}


void TwMapDAO::SetCenter(double x, double y)
{
	center_point.x = x, center_point.y = y;
}

int TwMapDAO::GetRoadSelection()
{
	return m_sel;
}

void TwMapDAO::SetRoadSelection(int sel)
{
	m_sel = sel;
}

void TwMapDAO::MovePoint(int pid0, int pid1)
{
	double x = m_point_list[pid0].x, y = m_point_list[pid0].y;
	int r, c;
	GetRowCol(x, y, r, c);
	set<int> lid_set;
	for (int i = 0; i < m_grid[r][c].line_list.size(); ++i)
	{
		int lid = m_grid[r][c].line_list[i];
		for (int j = 0; j < m_line_list[lid].point_list.size(); ++j)
		{
			if (m_line_list[lid].point_list[j] == pid0)
				m_line_list[lid].point_list[j] = pid1;
		}
	}
}

int TwMapDAO::FindNearestPoint(Point pt)
{
	double min_dist = 1e20;
	double x = pt.x, y = pt.y;
	int ir, ic;
	GetRowCol(x, y, ir, ic);
	int sel_pid = -1;
	for (int i = 0; i < m_grid[ir][ic].point_list.size(); ++i)
	{
		Point p0 = m_point_list[m_grid[ir][ic].point_list[i]];
		double dist = GetDist(p0, pt);
		if (dist < min_dist)
		{
			min_dist = dist;
			sel_pid = m_grid[ir][ic].point_list[i];
		}
	}
	if (min_dist > 10)
	{
		sel_pid = -1;
	}
	return sel_pid;
}


int TwMapDAO::FindNearestSegment(Point pt)
{
	double min_dist = 1e20;
	for (int i = 0; i < m_line_cnt; ++i)
	{
		vector<int> &pid_list = m_line_list[i].point_list;
		//PBiNode cur = list->head->forward;
		for (int j = 0; j < pid_list.size() - 1; ++ j)
		//for (; cur->forward != list->head; cur = cur->forward)		// Segment
		{
			int st;
			PointPtr p0 = &m_point_list[pid_list[j]], p1 = &m_point_list[pid_list[j + 1]];
			double dist = Point2Segment(pt, *p0, *p1, st);
			if (dist < min_dist)
			{
				min_dist = dist;
				m_sel = i;
			}
		}
	}
	if (min_dist > 10)
	{
		m_sel = -1;
	}
	return m_line_list[m_sel].lid;
}


void TwMapDAO::ProcessRoad()
{
	// find Road
	for (int i = 0; i < m_line_cnt; ++i)
	{
		LinePtr pLine = &m_line_list[i];
		char* sname = pLine->name;
		std::string str_name = sname;
		if (str_name == "")
			continue;
		std::map<std::string, int>::iterator it;
		if ((it = name_map.find(sname)) != name_map.end())
		{
			int road_id = name_map[sname];
			m_road_list[road_id].line_list.push_back(pLine);
			pLine->rid = road_id;
		}
		else
		{
			int road_id = (int)m_road_list.size();
			Road rd;
			int name_len = int(strlen(sname));
			rd.name = (char*)malloc(name_len + 1);
			strcpy(rd.name, sname);
			rd.line_list.push_back(pLine);
			m_road_list.push_back(rd);
			name_map[sname] = road_id;
			pLine->rid = road_id;
		}
		
	}
}

Road& TwMapDAO::GetCurRoad()
{
	Line* ln = &m_line_list[m_sel];
	std::string s = ln->name;
	int rid = name_map[s];
	return m_road_list[rid];
}

Road* TwMapDAO::GetSelRoad(char *name)
{
	Road* pRoad = NULL;
	std::string str_name = name;
	std::map<std::string, int>::iterator it;
	if ((it = name_map.find(name)) == name_map.end())
	{
		return pRoad;
	}
	int rid = it->second;
	return &m_road_list[rid];
}


void TwMapDAO::ReadDataFromTxt()
{
}


int TwMapDAO::GetRowCol(double x, double y, int &ir, int &ic)
{
	double maxX = MAXMAPX, maxY = MAXMAPY, minX = MINMAPX, minY = MINMAPY;
	ic = int((x - minX) / GRIDMAP), ir = int((y - minY) / GRIDMAP);
	return 0;
}


void TwMapDAO::ReadData()
{
	clock_t bt = clock();
	sqlite3_stmt *stmt = NULL;
	
	const char *sql = "select * from TB_SEG_POINT order by s_id, seq;";
	int res = sqlite3_prepare_v2(m_sql, sql, -1, &stmt, NULL);
	// for center point
	double cl = 120.160055, cb = 30.258832;
	BLH blh;
	xyH xyh;
	blh.b = cb, blh.l = cl, blh.h = 0;
	WGS84_BLH_2_HZ_xyH(blh, &xyh);
	center_point.x = xyh.y, center_point.y = xyh.x;
	int last_sid = -1;
	int last_pid = -1;

	// 道路id从0到6704
	if (res == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int sid = sqlite3_column_int(stmt, 1);
			int seq = sqlite3_column_int(stmt, 2);
			double longi = sqlite3_column_double(stmt, 3);
			double lati = sqlite3_column_double(stmt, 4);
			string sLongi = double2String(longi);
			string sLati = double2String(lati);
			blh.b = lati, blh.l = longi, blh.h = 0;
			WGS84_BLH_2_HZ_xyH(blh, &xyh);
			// point id
			Point pt(xyh.y, xyh.x);
			pt.uid = sLongi + ";" + sLati;
			int pid = InsertPointID(pt);
			if (m_line_list[sid].point_list.size() == 0)			// new line
			{
				Line line;
				line.name = NULL;
				line.rank = NULL;
				line.lid = sid;
				line.addPoint(pid);
				m_line_list[sid] = line;
				m_line_cnt++;
				//m_point_list[pid].addAdjectLine(sid);
			}
			else
			{
				if (last_pid != pid)
				{
					//m_point_list[pid].addAdjectLine(sid);
					m_line_list[sid].addPoint(pid);
				}
			}
			last_pid = pid;
		}
	}

	for (int i = 0; i < m_line_cnt; ++i)                          
	{
		LinePtr ln = &m_line_list[i];
		PointPtr p0 = &m_point_list[ln->point_list[0]];
		PointPtr p1 = &m_point_list[ln->point_list[ln->point_list.size() - 1]];
		// PointPtr p0 = RAIL_PT(ln);
		// PointPtr p1 = TAIL_PT(ln);
		m_line_list[i].begin_pid = p0->pid;
		m_line_list[i].end_pid = p1->pid;
	}

	const char *sql2 = "select * from tb_segment";
	res = sqlite3_prepare_v2(m_sql, sql2, -1, &stmt, NULL);
	if (res == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int sid = sqlite3_column_int(stmt, 0);
			char *name = (char*)sqlite3_column_text(stmt, 1);
			int ort = sqlite3_column_int(stmt, 2);
			char *srank = (char*)sqlite3_column_text(stmt, 3);
			char *tempRank = (char*)malloc(16);
			char *szName = (char*)malloc(32);
			CodeConvert(CP_ACP, szName, CP_UTF8, name);
			CodeConvert(CP_ACP, tempRank, CP_UTF8, srank);

			int name_len = int(strlen(szName));
			m_line_list[sid].name = (char*)malloc(name_len + 1);
			strcpy(m_line_list[sid].name, szName);
			free(szName);
			
			m_line_list[sid].ort = ort;
			m_line_list[sid].rank = (char*)malloc(16);
			strcpy(m_line_list[sid].rank, tempRank);
			free(tempRank);
		}
	}

	for (int i = 0; i < m_line_cnt; ++i)
	{
		int lid = m_line_list[i].lid;
		int ort = m_line_list[i].ort;
		for (int j = 0; j < m_line_list[i].point_list.size(); ++j)
		{
			int pid = m_line_list[i].point_list[j];
			if (ort == ORT_ONEWAY)
			{
				if (j != m_line_list[i].point_list.size() - 1)
					m_point_list[pid].line_list.push_back(pair<int, int>(lid, j));
			}
			else
			{
				if (j - 1 >= 0)
					m_point_list[pid].line_list.push_back(pair<int, int>(lid, j - 1));
				if (j != m_line_list[i].point_list.size() - 1)
					m_point_list[pid].line_list.push_back(pair<int, int>(lid, j));
			}
		}
	}

	sqlite3_finalize(stmt);
	clock_t et = clock();
	TRACE(L"read data, %d ms\n", et - bt);

	ProcessRoad();
	//CrossPoint();
	DivideGrid();
}


void TwMapDAO::DivideGrid()
{
	for (int i = 0; i < m_line_cnt; ++i)
	{
		std::set<std::pair<int, int>> grid;
		for (int j = 0; j < m_line_list[i].point_list.size(); ++j)
		{
			int ir, ic;
			int pid = m_line_list[i].point_list[j];
			double x = m_point_list[pid].x, y = m_point_list[pid].y;
			GetRowCol(x, y, ir, ic);
			grid.insert(pair<int, int>(ir, ic));
		}
		set<pair<int, int>>::iterator it;
		for (it = grid.begin(); it != grid.end(); ++it)
		{
			int ir = it->first, ic = it->second;
			m_grid[ir][ic].line_list.push_back(m_line_list[i].lid);
		}
		if (m_line_list[i].ort == ORT_ONEWAY || strcmp(m_line_list[i].rank, "步行街") == 0 ||
			strcmp(m_line_list[i].rank, "匝道") == 0 || strcmp(m_line_list[i].rank, "连杆道路") == 0 ||
			strcmp(m_line_list[i].rank, "次要支路") == 0)
			continue;
		m_gene_list.push_back(m_line_list[i].lid);
	}

	for (int i = 0; i < m_point_cnt; ++i)
	{
		double x = m_point_list[i].x, y = m_point_list[i].y;
		int ir, ic;
		GetRowCol(x, y, ir, ic);
		m_grid[ir][ic].point_list.push_back(m_point_list[i].pid);
	}
}


void TwMapDAO::CrossPoint()
{
	/*for (int i = 0; i < m_point_cnt; ++i)
	{
		Point *pt = &m_point_list[i];
		if (BiSize(pt->line_list) > 1)
		{
			LinePtr p0;
			PBiList list = pt->line_list;
			PBiNode cur = list->head->forward;
			p0 = (LinePtr)cur->item;
			cur = cur->forward;
			while (cur != list->head)
			{
				LinePtr p1 = (LinePtr)cur->item;
				if (strcmp(p0->name, p1->name) != 0)
				{
					pt->cross = 1;
					break;
				}
				cur = cur->forward;
			}
		}
	}*/
}
