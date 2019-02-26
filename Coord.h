#ifndef _COORD_H_
#define _COORD_H_

#ifdef DLL_IMPLEMENT
#define FUNC_EXT extern "C" __declspec(dllexport)
#else
#define FUNC_EXT extern "C" __declspec(dllimport)
#endif

#define PI			3.1415926535898
#define LIMIT		tan(0.0001*PI/(3600*180))//迭代允许的误差0.0001秒 

typedef struct _BLH
{
	double b;
	double l;
	double h;
} BLH;

typedef struct _XYZ
{
	double x;
	double y;
	double z;
} XYZ;

typedef struct _xyH
{
	double x;
	double y;
	double h;
} xyH;

typedef struct _Ellipsoid
{
	char name[16];
	double a;
	double r;
} Ellipsoid;

typedef struct _Param7
{
	double dx;
	double dy;
	double dz;
	double rx;
	double ry;
	double rz;
	double k;
} Param7;

typedef struct _Param4
{
	double dx;
	double dy;
	double r;
	double k;
} Param4;

extern Ellipsoid BJ54,XA80,WGS84;


FUNC_EXT int __stdcall WGS84_BLH_2_HZ_xyH(BLH src, xyH *dest);	//WGS84->杭州54

FUNC_EXT int __stdcall HZ_xyH_2_WGS84_BLH(xyH src, BLH *dest);	//杭州54->WGS84


#endif