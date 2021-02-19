
#include "VectorLayer.h"
#include "VisibleRect.h"
#include "GeoUtils.h"
#include <geos/geom/GeometryFactory.h>
#include "geos/geom/CoordinateSequence.h"
#include <geos/geom/CoordinateArraySequenceFactory.h>
#include "Feature.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/MultiPolygon.h"

using namespace geos;
using namespace geos::geom;

VectorLayer::VectorLayer(MapControl* pControl)
{
    m_pControl = pControl;
    m_points = NULL;
    m_nPntNum = 0;
	
	m_draw = DrawNode::create(2);
	this->addChild(m_draw);

	setTouchEnabled(true);
}

VectorLayer::~VectorLayer()
{
    for (std::vector<Feature*>::iterator iter = m_Features.begin(); iter != m_Features.end(); ++iter)
    {
        Feature* pFeature = *iter;
        delete  pFeature;
        pFeature = NULL;
    }
    
    m_Features.clear();
    
    if (m_points) {
        delete []m_points;
        m_points = NULL;
    }
}

void VectorLayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    
}


void VectorLayer::update(float delta)
{

}

void VectorLayer::drawFeatures()
{
	m_draw->clear();

	if (m_Features.size() == 0) {
		return;
	}

	RawTile dTile = m_pControl->getMap()->m_defTile;
	Vec2 pntOffset = m_pControl->getMap()->getGlobalOffset();

	Vec2 pntTopLeft, pntRightBottom, pntMarker;
	pntTopLeft.x = dTile.x * 256 - pntOffset.x;
	pntTopLeft.y = dTile.y * 256 - pntOffset.y;
	pntRightBottom.x = pntTopLeft.x + m_pControl->getMap()->getWidth();
	pntRightBottom.y = pntTopLeft.y + m_pControl->getMap()->getHeight();

	int zoom = 17 - m_pControl->getZoomLevel();

	Vec2 pntMapTL, pntMapRB;
	pntMapTL = GeoUtils::getLatLong(pntTopLeft.x, pntTopLeft.y, zoom);
	pntMapRB = GeoUtils::getLatLong(pntRightBottom.x, pntRightBottom.y, zoom);

	std::vector<Coordinate> pntVector;
	pntVector.push_back(Coordinate(pntMapTL.x, pntMapTL.y, 0));
	pntVector.push_back(Coordinate(pntMapTL.x, pntMapRB.y, 0));
	pntVector.push_back(Coordinate(pntMapRB.x, pntMapRB.y, 0));
	pntVector.push_back(Coordinate(pntMapRB.x, pntMapTL.y, 0));
	CoordinateSequence * cs = CoordinateArraySequenceFactory::instance()->create(&pntVector);
	LinearRing* viewBounds = geos::geom::GeometryFactory::getDefaultInstance()->createLinearRing(cs);
	//int zoom = 17 - m_pControl->getZoomLevel();

	double scale = (1 << (17 - zoom)) * GeoUtils::TILE_SIZE;

	//cocos2d::Vec2 pnt((int) (normalised.x * scale), (int) (normalised.y * scale));
	//Vec2 offsetGps = m_pControl->getGpsOffset();

	//double latFix = lat + offsetGps.y*pow(10.0, -5);
	//double lonFix = lon + offsetGps.x*pow(10.0, -5);

	for (std::vector<Feature*>::iterator iter = m_Features.begin(); iter != m_Features.end(); ++iter)
	{
		Feature* pFeature = *iter;
		Geometry* pGeo = pFeature->GetGeometry();
		//if(!viewBounds->contains(pGeo))
		//    continue;
		int nCount = pGeo->getNumPoints();
		CoordinateSequence* pCs = pGeo->getCoordinates();
		if (pGeo->getGeometryTypeId() != GEOS_MULTIPOLYGON &&
			pGeo->getGeometryTypeId() != GEOS_MULTILINESTRING)
		{
			if (m_points == NULL)
			{
				m_points = new Vec2[nCount];
			}
			else if (nCount > m_nPntNum)
			{
				delete[]m_points;
				m_points = new Vec2[nCount];
				m_nPntNum = nCount;
			}
			for (int i = 0; i < nCount; i++) {
				m_pnt = GeoUtils::toNormalisedPixelCoords(pCs->getAt(i).y, pCs->getAt(i).x);
				m_points[i] = Vec2(m_pnt.x*scale - pntTopLeft.x, m_pControl->getMap()->getHeight() - (m_pnt.y*scale - pntTopLeft.y));
			}
		}
		Style style = pFeature->GetStyle();
		int gtid = pGeo->getGeometryTypeId();
		switch (gtid) {
		case GEOS_POINT:
			m_draw->drawPoint(m_points[0], style.lineWidth, style.linecolor);
			break;
		case GEOS_POLYGON:
			m_draw->setLineWidth(style.lineWidth);
			m_draw->drawSolidPoly(m_points, nCount, style.fillcolor);
			break;
		case GEOS_LINESTRING:
			m_draw->setLineWidth(style.lineWidth);
			m_draw->drawPoly(m_points, nCount, false, style.linecolor);
			break;
		case GEOS_LINEARRING:
			m_draw->setLineWidth(style.lineWidth);
			m_draw->drawPoly(m_points, nCount, true, style.linecolor);
			break;
		case GEOS_MULTIPOINT:
			m_draw->setLineWidth(style.lineWidth);
			m_draw->drawPoints(m_points, nCount, style.linecolor);
			break;
		case GEOS_MULTIPOLYGON:
		case GEOS_MULTILINESTRING:
		{
			int nSubGeo = pGeo->getNumGeometries();
			for (int j = 0; j < nSubGeo; j++) {
				const Geometry* pSubGeo = pGeo->getGeometryN(j);
				nCount = pSubGeo->getNumPoints();
				pCs = pSubGeo->getCoordinates();
				if (m_points == NULL)
				{
					m_points = new Vec2[nCount];
				}
				else if (nCount > m_nPntNum)
				{
					delete[]m_points;
					m_points = new Vec2[nCount];
					m_nPntNum = nCount;
				}
				for (int i = 0; i < nCount; i++) {
					m_pnt = GeoUtils::toNormalisedPixelCoords(pCs->getAt(i).y, pCs->getAt(i).x);
					m_points[i] = Vec2(m_pnt.x*scale - pntTopLeft.x, m_pControl->getMap()->getHeight() - (m_pnt.y*scale - pntTopLeft.y));
				}
				if (pSubGeo->getGeometryTypeId() == GEOS_POLYGON){
					m_draw->setLineWidth(style.lineWidth);
					m_draw->drawSolidPoly(m_points, nCount, style.fillcolor);
				}
				else if (pSubGeo->getGeometryTypeId() == GEOS_LINEARRING){
					m_draw->setLineWidth(style.lineWidth);
					m_draw->drawPoly(m_points, nCount, true, style.linecolor);
				}
				else{
					m_draw->setLineWidth(style.lineWidth);
					m_draw->drawPoly(m_points, nCount, false, style.linecolor);
				}
			}
		}
		break;
		default:
			break;
		}
	}
}

void VectorLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent)
{
	//CCLOG("VectorLayer::onTouchesBegan");
	m_tBeginPos = touches[0]->getLocation();
}

void VectorLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent)
{

}

void VectorLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent)
{

}

void VectorLayer::AddFeatureLayer(Feature* pFeature)
{
    m_Features.push_back(pFeature);
	this->drawFeatures();
}

void VectorLayer::AddFeatureLayers(Feature* pFeature,int length)
{
    for (int i=0; i<length; i++) {
        m_Features.push_back(pFeature++);
    }
	this->drawFeatures();
}

