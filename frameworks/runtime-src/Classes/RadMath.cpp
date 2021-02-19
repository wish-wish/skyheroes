
#include "RadMath.h"

RadCircle::RadCircle(Vec3 pos, float radius)
{
	m_pos = pos;
	m_radius = radius;
}

RadCircle::~RadCircle()
{

}

Vec3 RadCircle::GetArcLength(Rect* arect, Size size, int max)
{
	VList list;
	
	int secid = this->GetScale9Sec(arect);
	
	float width = size.width*1.314520;
	int rounds = floor(m_radius / width);
	if (rounds < 1){ rounds = 1; }

	std::vector<float> radius;
	for (int i = 0; i < rounds; i++)
	{
		if (i < max||max<1)
		{
			radius.push_back(m_radius - width*i);
		}
		else
		{
			break;
		}
	}
	
	float count=0;
	m_rcounts.clear();
	m_secids.clear();
	float _radius = m_radius;
	Vec2 vrad;
	for (int i = radius.size()-1; i>=0; i--)
	{
		list.clear();
		m_radius = radius[i];
		this->GetRectIntersects(arect, &list);
		vrad = this->GetOffsetRad(arect, list, secid);
	
		float factor = vrad.x / (rad_pi * 2);

		int onenum = floor(factor*rad_pi * 2 * radius[i] / width);
		m_rcounts.push_back(Vec4(onenum,radius[i],vrad.x,vrad.y));
		m_secids.push_back(Vec2(secid, 0));
		count = count + onenum;
	}
	m_radius = _radius;
	return Vec3(count, vrad.y, 0);
}

float RadCircle::GetArcRad(Vec3 pos1, Vec3 pos2)
{
	Vec2 p1(pos1.x, pos1.y), p2(pos2.x, pos2.y),p(m_pos.x,m_pos.y);
	float angle1 = p.getAngle(p1);
	float angle2 = p.getAngle(p2);
	return abs(angle2-angle1);
}

float RadCircle::GetLengthRad(Vec3 pos1, Vec3 pos2)
{
	float length = this->GetLength();
	float rad = this->GetArcRad(pos1, pos2);
	return length*rad / 2 * rad_pi;
}

Rect RadCircle::GetRect(Rect* arect)
{
	Size size = Director::getInstance()->getVisibleSize();
	Rect rect(m_radius * 2, m_radius * 2, size.width - m_radius * 4, size.height - m_radius * 4);
	if (arect != nullptr)
	{
		rect = Rect(arect->getMinX() + m_radius * 2, arect->getMinY() + m_radius * 2, arect->size.width - m_radius * 4, arect->size.height - m_radius * 4);
	}
	return rect;
}

VList* RadCircle::GetWidgetsPos(Rect* arect, VList* list, Size asize, int count, int show_type)
{
	Vec3 vec = this->GetArcLength(arect,asize,0);
	while (count > vec.x)
	{
		m_radius = m_radius*1.1;
		vec = this->GetArcLength(arect,asize,0);
	}
	int letcount=count;
	if (show_type == 0)//outer first
	{
		for (int j = m_rcounts.size()-1; j >= 0; j--)
		{
			if (letcount > 0)
			{
				float onecount = m_rcounts[j].x > letcount ? letcount : m_rcounts[j].x;
				float factor = m_rcounts[j].z / onecount, offset = factor / 2;
				//offset = 0;
				for (int i = 0; i < m_rcounts[j].x; i++)
				{
					Vec3 ret;
					//if (m_secids[j].x == 2 || m_secids[j].x == 4 || m_secids[j].x == 6 || m_secids[j].x == 8)
					{
						ret.x = m_pos.x + m_rcounts[j].y*cos(m_rcounts[j].w + offset + factor*(i));
						ret.y = m_pos.y + m_rcounts[j].y*sin(m_rcounts[j].w + offset + factor*(i));
					}
					//else
					//{
					//	ret.x = m_pos.x + m_rcounts[j].y*cos(m_rcounts[j].w + offset + factor*(i + 0.5));
					//	ret.y = m_pos.y + m_rcounts[j].y*sin(m_rcounts[j].w + offset + factor*(i + 0.5));
					//}
					ret.z = 1.0;
					list->push_back(ret);
				}
			}
			letcount = letcount - m_rcounts[j].x;
		}
	}
	else //inner first
	{
		for (int j = 0; j < m_rcounts.size(); j++)
		{
			if (letcount > 0)
			{
				float onecount = m_rcounts[j].x > letcount ? letcount : m_rcounts[j].x;
				float factor = m_rcounts[j].z / onecount, offset = factor / 2;
				//offset = 0;
				for (int i = 0; i < m_rcounts[j].x; i++)
				{
					Vec3 ret;
					ret.x = m_pos.x + m_rcounts[j].y*cos(m_rcounts[j].w + offset + factor*(i + 0.5));
					ret.y = m_pos.y + m_rcounts[j].y*sin(m_rcounts[j].w + offset + factor*(i + 0.5));
					ret.z = 1.0;
					list->push_back(ret);
				}
			}
			letcount = letcount - m_rcounts[j].x;
		}
	}
	return list;
}

VList* RadCircle::GetWidgetsPos(float offset, float arc, VList* list, Size asize, int count)
{
	float r = asize.width*1.1314520;
	/*
	float arclen = arc*r;
	while (floor(arclen / r)<count)
	{
		m_radius = m_radius*1.1;
		arclen = arc*m_radius;
	}
	*/
	float arc_spare = arc / count;
	for (int i = 0; i < count; i++)
	{
		float x = m_pos.x + m_radius*cos(offset + arc_spare*(i + 0.5));
		float y = m_pos.y + m_radius*sin(offset + arc_spare*(i + 0.5));
		Vec3 p(x, y, 1);
		list->push_back(p);
	}
	return list;
}

VList* RadCircle::GetCircleIntersects(Vec3 c2, float radius2, VList* list)
{
	//http://blog.csdn.net/qq_24378389/article/details/53736852
	//TODO:
	return list;
}

VList* RadCircle::GetLineIntersects(Vec3 l1, Vec3 l2, VList* list)
{
	//TODO:fix it
	//https://thecodeway.com/blog/?p=932
	float a = (l2.x - l1.x)*(l2.x - l1.x) + (l2.y - l1.y)*(l2.y - l1.y);
	float b = 2 * (l2.x - l1.x)*(l1.x - m_pos.x) + (l2.y - l1.y)*(l1.y - m_pos.y);
	float c = m_pos.x*m_pos.x + m_pos.y*m_pos.y + l1.x*l1.x + l1.y*l1.y - 2 * (m_pos.x*l1.x + m_pos.y*l1.y) - m_radius*m_radius;
	float b4ac =b*b - 4 * a*c;
	float sb4ac = sqrt(b4ac);
	float u1 = (-b + sb4ac) / (2 * a);
	float u2 = (-b - sb4ac) / (2 * a);
	CCLOG("b4ac %f,u1 %f, u2 %f", b4ac, u1, u2);
	if (b4ac-0 < 0.000001)
	{
		//one point
		if (u1 >= 0 && u1 <= 1)
		{
			//got point
			Vec3 p = l1.lerp(l2, u1);
			if (!this->IsInList(p, list))
			{
				list->push_back(p);
			}
		}
		else if (u2>=0 && u1<=1)
		{
			//got point
			Vec3 p=l1.lerp(l2, u2);
			if (!this->IsInList(p, list))
			{
				list->push_back(p);
			}
		}
	}
	else if (b4ac > 0)
	{
		//two point
		Vec3 p1 = l1.lerp(l2, u1);
		Vec3 p2 = l1.lerp(l2, u2);
		if (!this->IsInList(p1, list))
		{
			list->push_back(p1);
		}
		if (!this->IsInList(p2, list))
		{
			list->push_back(p2);
		}
	}
	return list;
}

VList* RadCircle::GetAxisLineIntersects(Rect* rect, int line, VList* list)
{
	float factorx = 1, factory = 1;
	if (line == 0)
	{
		factorx = (m_pos.x-rect->getMinX()) / m_radius;
		if (factorx <= 1)
		{
			float arc = acos(factorx);
			float height = m_radius*sin(arc);
			Vec3 p1 = Vec3(rect->getMinX(), m_pos.y + height, rad_pi-arc);
			Vec3 p2 = Vec3(rect->getMinX(), m_pos.y - height, rad_pi+arc);
			if (p1.y >= rect->getMinY() && p1.y <= rect->getMaxY() && !this->IsInList(p1, list))
			{
				list->push_back(p1);
			}
			if (p2.y >= rect->getMinY() && p2.y <= rect->getMaxY() && !this->IsInList(p2, list))
			{
				list->push_back(p2);
			}
		}
	}
	else if (line == 1)
	{
		factory = (rect->getMaxY()-m_pos.y) / m_radius;
		if (factory <= 1)
		{
			float arc = acos(factory);
			float width = m_radius*sin(arc);
			Vec3 p1 = Vec3(m_pos.x - width, rect->getMaxY(),rad_pi/2+arc);
			Vec3 p2 = Vec3(m_pos.x + width, rect->getMaxY(),rad_pi/2-arc);
			if (p1.x >= rect->getMinX() && p1.x <= rect->getMaxX() && !this->IsInList(p1, list))
			{
				list->push_back(p1);
			}
			if (p2.x >= rect->getMinX() && p2.x <= rect->getMaxX() && !this->IsInList(p2, list))
			{
				list->push_back(p2);
			}
		}
	}
	else if (line == 2)
	{
		factorx = (rect->getMaxX() - m_pos.x) / m_radius;
		if (factorx <= 1)
		{
			float arc = acos(factorx);
			float height = m_radius*sin(arc);
			Vec3 p1 = Vec3(rect->getMaxX(), m_pos.y + height,arc);
			Vec3 p2 = Vec3(rect->getMaxX(), m_pos.y - height,2*rad_pi-arc);
			if (p1.y >= rect->getMinY() && p1.y <= rect->getMaxY() && !this->IsInList(p1, list))
			{
				list->push_back(p1);
			}
			if (p2.y >= rect->getMinY() && p2.y <= rect->getMaxY() && !this->IsInList(p2, list))
			{
				list->push_back(p2);
			}
		}
	}
	else if (line == 3)
	{
		factory = (m_pos.y-rect->getMinX()) / m_radius;
		if (factory <= 1)
		{
			float arc = acos(factory);
			float width = m_radius*sin(arc);
			Vec3 p1 = Vec3(m_pos.x + width, rect->getMinY(),3*rad_pi/2+arc);
			Vec3 p2 = Vec3(m_pos.x - width, rect->getMinY(), 3 * rad_pi / 2 - arc);
			if (p1.x >= rect->getMinX() && p1.x <= rect->getMaxX() && !this->IsInList(p1, list))
			{
				list->push_back(p1);
			}
			if (p2.x >= rect->getMinX() && p2.x <= rect->getMaxX() && !this->IsInList(p2, list))
			{
				list->push_back(p2);
			}
		}
	}
	return list;
}

float RadCircle::GetArea()
{
	return rad_pi*m_radius*m_radius;
}

float RadCircle::GetLength()
{
	return 2 * rad_pi*m_radius;
}

bool RadCircle::IsInList(Vec3 p, VList* list)
{
	for (int i = 0; i < list->size(); i++)
	{
		Vec2 p2(p.x, p.y), l((*list)[i].x, (*list)[i].y);
		if (p2.fuzzyEquals(l,0.00001))
		{
			return true;
		}
	}
	return false;
}

float RadCircle::CalcRadius(float minArclength)
{
	//TODO:TODO:
	return minArclength;
}

int RadCircle::GetScale9Sec(Rect* arect)
{
	Rect rect = this->GetRect(arect);
	int secindex = 0;
	if (m_pos.x < rect.getMinX())
	{
		if (m_pos.y < rect.getMinY())
		{
			secindex = 1;
		}
		else if (m_pos.y > rect.getMaxY())
		{
			secindex = 3;
		}
		else
		{
			secindex = 2;
		}
	}
	else if (m_pos.x > rect.getMaxX())
	{
		if (m_pos.y < rect.getMinY())
		{
			secindex = 7;
		}
		else if (m_pos.y > rect.getMaxY())
		{
			secindex = 5;
		}
		else
		{
			secindex = 6;
		}
	}
	else
	{
		if (m_pos.y < rect.getMinY())
		{
			secindex = 8;
		}
		else if (m_pos.y > rect.getMaxY())
		{
			secindex = 4;
		}
		else
		{
			secindex = 9;
		}
	}
	return secindex;
}

cocos2d::Vec2 RadCircle::GetOffsetRad(Rect* arect, VList& list, int secid)
{
	float rad = 0;
	float offset = 0;
	if (list.size() > 1)
	{
		if (secid == 1)
		{
			if (list.size() == 2)
			{
				rad = list[0].z + 2 * rad_pi - list[1].z;
				offset = list[1].z;
			}
			else if (list.size() == 4)
			{
				rad = list[1].z + 2 * rad_pi - list[2].z;
				offset = list[2].z;
			}
		}
		else if (secid == 3)
		{
			if (list.size() == 2)
			{
				rad = 2 * rad_pi - list[0].z + list[1].z;
			}
			else if (list.size() == 4)
			{
				rad = 2 * rad_pi - list[0].z + list[3].z;
			}
			offset = list[0].z;
		}
		else if (secid == 5)
		{
			if (list.size() == 2)
			{
				rad = list[1].z - list[0].z;
				offset = list[0].z;
			}
			else if (list.size() == 4)
			{
				rad = list[3].z - list[0].z;
			}
			offset = list[0].z;
		}
		else if (secid == 7)
		{
			if (list.size() == 2)
			{
				rad = list[1].z - list[0].z;
			}
			else if (list.size() == 4)
			{
				rad = list[3].z - list[0].z;
			}
			offset = list[0].z;
		}
		else if (secid == 2)
		{
			rad = 2 * rad_pi - list[1].z + list[0].z;
			offset =  list[1].z;
		}
		else if (secid == 4)
		{
			rad = 2 * rad_pi - list[0].z + list[1].z;
			offset =  list[0].z;
		}
		else if (secid == 6)
		{
			rad = list[1].z - list[0].z;
			offset =  list[0].z;
		}
		else if (secid == 8)
		{
			rad = list[1].z + 2 * rad_pi - list[0].z;
			offset = list[0].z;
		}
		else if (secid == 9)
		{
			rad = 2 * rad_pi;
			offset = 0;
		}
	}
	else
	{
		rad = 2 * rad_pi;
		offset = 0;
	}
	return Vec2(rad, offset);
}

VList* RadCircle::GetRectIntersects(Rect* arect, VList*list)
{
	Size size = Director::getInstance()->getVisibleSize();
	Rect rect(Vec2(10,10),Size(size.width-20,size.height-20));
	this->GetAxisLineIntersects(&rect,0, list);
	this->GetAxisLineIntersects(&rect,1, list);
	this->GetAxisLineIntersects(&rect,2, list);
	this->GetAxisLineIntersects(&rect,3, list);
	return list;
}

RadTringle::RadTringle(Vec3 p1, Vec3 p2, Vec3 p3)
	:m_p1(p1),
	m_p2(p2),
	m_p3(p3)
{
	
}

RadTringle::~RadTringle()
{

}

float RadTringle::GetArea()
{
	//TODO:
	return 0;
}

float RadTringle::GetSidgeLength()
{
	Vec2 p1(m_p1.x, m_p1.y), p2(m_p2.x, m_p2.y), p3(m_p3.x, m_p3.y);
	return p1.getDistance(p2) + p2.getDistance(p2) + p3.getDistance(p1);
}

VList* RadTringle::GetSorted(int type, VList *list)
{
	//TODO:
	return list;
}

RadRectange::RadRectange(Vec2 p1, Size size)
	:m_rect(p1, size)
{
	
}

RadRectange::~RadRectange()
{

}

VList* RadRectange::GetSorted(int type, VList* list)
{
	return list;
}

float RadRectange::GetArea()
{
	return m_rect.size.width*m_rect.size.height;
}

RadTree::RadTree(Vec3 center, Size size, int level, int count)
:m_center(center)
,m_wsize(size)
,m_count(count)
,m_level(level)
{
	if (level > 2)
	{
		m_level = 2;
	}
	float radius = size.width*1.1314520;
	RadNode* node = new RadNode();
	node->m_level = 1;
	node->m_arc = 2 * rad_pi;
	node->m_offset = 0;
	node->m_index = 0;
	node->m_circle = new RadCircle(center,radius);
	m_circles.push_back(node);
	if (level > 1)
	{
		float arc = 2 * rad_pi / (count+2);
		float offset = arc / (count);
		for (int i = 0; i < count; i++)
		{
			RadNode* node = new RadNode();
			node->m_level = 2;
			node->m_arc = arc;
			node->m_offset = i*arc + offset*(2*i+1);
			node->m_index = i;
			node->m_circle = new RadCircle(center, radius*2.5);
			m_circles.push_back(node);
		}
	}
}

RadTree::~RadTree()
{
	for (int i = 0; i < m_circles.size(); i++)
	{
		delete m_circles[i]->m_circle;
		delete m_circles[i];
	}
	m_circles.clear();
}

std::vector<RadNode*>* RadTree::GetCircle()
{
	return &m_circles;
}

VList* RadTree::GetWidgetPos(int level, int index, VList* list)
{
	for (int i = 0; i < m_circles.size(); i++)
	{
		if (m_circles[i]->m_index == index&&m_circles[i]->m_level == level)
		{
			m_circles[i]->m_circle->GetWidgetsPos(m_circles[i]->m_offset, m_circles[i]->m_arc, list,m_wsize, m_count);
			return list;
		}
	}
	return list;
}

VList* RadTree::GetAllWidgetPos(VList* list)
{
	for (int i = 0; i < m_circles.size(); i++)
	{
		m_circles[i]->m_circle->GetWidgetsPos(m_circles[i]->m_offset, m_circles[i]->m_arc, list, m_wsize, m_count);
	}
	return list;
}

double Vec84::EQUATOR = 6378137.0;

Vec84::Vec84(double lat, double lng)
	:m_longitude(lng)
	,m_latitude(lat)
{
	
}

Vec84::~Vec84()
{

}

double Vec84::GetDistance(Vec84* other)
{
	
	double lon1 = this->m_longitude;
	double lon2 = other->m_longitude;
	double lat1 = this->m_latitude;
	double lat2 = other->m_latitude;
	
	double dLat = lat2 - lat1*rad_pi/180;
	double dLon = lon2 - lon1*rad_pi/180;
	double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1*rad_pi/180) * cos(lat2*rad_pi/180) * sin(dLon / 2) * sin(dLon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));

	return c * EQUATOR;
}

double GeoConvert::Lat_Min = -85.05112878;
double GeoConvert::Lat_Max = 85.05112878;
double GeoConvert::Lng_Min = -180.0;
double GeoConvert::Lng_Max = 180.0;

GeoConvert::GeoConvert()
{

}

GeoConvert::~GeoConvert()
{

}

Vec2 GeoConvert::ToPixel(Vec2 coord, int zoom)
{
	double lat = coord.y;
	double lng = coord.x;
	lat = std::min(std::max(lat, Lat_Min), Lat_Max);
	lng = std::min(std::max(lng, Lng_Min), Lng_Max);

	double lat_arc = lat*rad_pi / 180.0;
	double helpx = (lng + 180.0) / 360.0;
	double helpy = 0.5 - log((1.0 + sin(lat_arc)) / (1.0 - sin(lat_arc))) / (4.0*rad_pi);
	double size = (int)TILE_SIZE << zoom;

	//lat = 0.5 - ((log(tan((PI / 4) + ((0.5 * PI * lat) / 180))) / PI) / 2.0);
	//if (lng > 180) { lng -= 360;}
	//lng /= 360;
	//lng += 0.5;

	int x = (int)std::min(std::max(helpx*size + 0.5, 0.0), size - 1.0);
	int y = (int)std::min(std::max(helpy*size + 0.5, 0.0), size - 1.0);

	return Vec2(x,y);
}

Vec2 GeoConvert::ToLatLng(Vec2 point, int zoom)
{
	double x = point.x;
	double y = point.y;
	double size = (int)TILE_SIZE << zoom;
	
	double helpx = (std::min(std::max(x, 0.0), size - 1.0) / size) - 0.5;
	double helpy = 0.5 - (std::min(std::max(y, 0.0), size - 1.0) / size);
	//double lat = 90.0 - 360.0 * atan(exp(-helpy*2.0*rad_pi)) / rad_pi;

	double factor = -helpy*2.0*rad_pi;
	double lng = 90.0 - 360.0 * atan(exp(factor)) / rad_pi;
	double lat = 360.0*helpx;

	return Vec2(lat,lng);
}

double GeoConvert::Radius(int zoom)
{
	double r = TILE_SIZE*(1 << zoom) / (2.0*rad_pi);
	return r;
}

int GeoConvert::GetMaxPixels(int zoom)
{
	double p = TILE_SIZE*(1 << zoom);
	return p;
}

int GeoConvert::FalseNorthing(int zoom)
{
	double fn = -1.0*GetMaxPixels(zoom) / 2.0;
	return fn;
}

Vec2 GeoConvert::ToPixel2(Vec2 coord, int zoom)
{
	int x = (int)((TILE_SIZE*(1 << zoom)*(coord.y + 180.0)) / 360.0);
	x = std::min(x, TILE_SIZE*(1 << zoom) - 1);
	if (coord.x < Lat_Min)
		coord.x = Lat_Min;
	else if (coord.x > Lat_Max)
		coord.x = Lat_Max;
	double sinlat = sin(coord.x*rad_pi / 180);
	double loglat = log((1.0 - sinlat) / (1.0 - sinlat));
	int y = (int)(TILE_SIZE*(1 << zoom)*(0.5 - (loglat / (4.0*rad_pi))));
	y = std::min(y, TILE_SIZE*(1 << zoom) - 1);

	return Vec2(x, y);
}

Vec2 GeoConvert::ToLatLng2(Vec2 point, int zoom)
{
	double lat = point.y + FalseNorthing(zoom);
	lat = rad_pi / 2 - (2 * atan(exp(-1.0*lat / Radius(zoom))));
	lat = -1 * lat*rad_pi * 180;

	double lng = ((360.0*point.x) / TILE_SIZE*(1 << zoom)) - 180.0;

	if (lat > Lat_Max)
		lat = Lat_Max;
	else if (lat < Lat_Min)
		lat = Lat_Min;

	if (lng>Lng_Max)
		lng = Lng_Max;
	else if (lng < Lng_Min)
		lng = Lng_Min;

	return Vec2(lat, lng);
}
