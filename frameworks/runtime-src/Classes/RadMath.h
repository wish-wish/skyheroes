#pragma once

#include "cocos2d.h"
USING_NS_CC;

const double rad_pi = 3.14159265358979324;
const int TILE_SIZE = 256;

typedef std::vector<Vec3> VList;

class RadCircle
{
public:
	explicit RadCircle(Vec3 pos, float radius);
	virtual ~RadCircle();
	int GetScale9Sec(Rect* arect);
	Vec2 GetOffsetRad(Rect* arect,VList& list,int secid);
	Vec3 GetArcLength(Rect* arect,Size size,int max=-1);
	float GetArcRad(Vec3 pos1, Vec3 pos2);
	float GetLengthRad(Vec3 pos1, Vec3 pos2);
	Rect  GetRect(Rect* arect);
	VList* GetWidgetsPos(Rect* arect, VList* list,Size asize,int count,int show_type=0);
	VList* GetWidgetsPos(float offset,float arc, VList* list,Size asize, int count);
	VList* GetRectIntersects(Rect* arect, VList* list);
	VList* GetCircleIntersects(Vec3 c2, float radius2, VList* list);
	VList* GetLineIntersects(Vec3 l1, Vec3 l2, VList* list);
	VList*  GetAxisLineIntersects(Rect* rect,int line, VList* list);
	float GetArea();
	float GetLength();
	Vec3 GetCenter(){ return m_pos; };
	float GetRadius(){ return m_radius; };
	bool IsInList(Vec3 p, VList* list);

	float CalcRadius(float minArclength);
private:
	Vec3 m_pos;
	float m_radius;
	std::vector<Vec4> m_rcounts;
	std::vector<Vec2> m_secids;
};

class Rad
{

};

struct RadNode
{
	int m_level;
	float m_offset;
	float m_arc;
	int m_index;
	RadCircle* m_circle;
};

class RadTree
{
public:
	explicit RadTree(Vec3 center,Size size,int level,int count);
	virtual ~RadTree();
	std::vector<RadNode*>* GetCircle();
	VList* GetWidgetPos(int level, int index,VList* list);
	VList* GetAllWidgetPos(VList* list);
private:
	Vec3 m_center;
	Size m_wsize;
	int m_level;
	int m_count;
	std::vector<RadNode*> m_circles;
};


class RadTringle
{
public:
	explicit RadTringle(Vec3 p1, Vec3 p2, Vec3 p3);
	virtual ~RadTringle();

	float GetArea();
	float GetSidgeLength();
	VList* GetSorted(int type, VList* list);//type=0 anti-clockwise;=1 clockwise;
private:
	Vec3 m_p1;
	Vec3 m_p2;
	Vec3 m_p3;
};

class RadRectange
{
public:
	explicit RadRectange(Vec2 p1, Size size);
	virtual ~RadRectange();

	VList* GetSorted(int type, VList* list);//type=0 anti-clockwise;=1 clockwise;
	float GetArea();
public:
	Rect m_rect;
};

class Vec84
{
public:
	explicit Vec84(double lat,double lng);
	virtual ~Vec84();
	double GetDistance(Vec84* other);
	double GetLatitude(){ return m_latitude; };
	double GetLongtitude(){ return m_longitude; };
	static double EQUATOR;
private:
	double m_latitude;
	double m_longitude;
};

class GeoConvert
{
public:
	GeoConvert();
	virtual ~GeoConvert();
	
	static double Lat_Min;
	static double Lat_Max;
	static double Lng_Min;
	static double Lng_Max;

	Vec2 ToPixel(Vec2 coord, int zoom);
	Vec2 ToLatLng(Vec2 point, int zoom);

	double Radius(int zoom);
	int GetMaxPixels(int zoom);
	int FalseNorthing(int zoom);
	Vec2 ToPixel2(Vec2 coord, int zoom);
	Vec2 ToLatLng2(Vec2 point, int zoom);
};