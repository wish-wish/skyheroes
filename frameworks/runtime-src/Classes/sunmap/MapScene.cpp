#include "MapScene.h"
#include "MapControlLayer.h"
#include "MapStrategyFactory.h"
#include "ControlLayer.h"
#include "LayerSwitcher.h"
#include "MarkerLayer.h"
#include "VectorLayer.h"

#include <geos/geom/GeometryFactory.h>
#include "geos/geom/CoordinateArraySequenceFactory.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "Feature.h"
#include <geos/geom.h>

#include "CacheMaker/CacheMaker.h"
#include "Layer/LocalTileManager.h"
#include "GeoUtils.h"
#include "RadMath.h"

using namespace cocos2d;
using namespace geos;
using namespace geos::geom;

Scene* MapScene::scene(PublicMsgHandler* pHandle)
{
	Scene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = Scene::create();
		CC_BREAK_IF(! scene);

		MapStrategyFactory::initMapStrategy();
		
		std::string path = FileUtils::getInstance()->getWritablePath();
		int mapsourcid = 6;
		std::string savepath = path + "tiles/";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		//savepath = "E:/oss/tiles/";
#endif
		////////////////////////////////////////////
		if (true)
		{
			Vec2 pnt1, pnt2;
// 			pnt1.x = -180;
// 			pnt1.y = 90;
// 			pnt2.x = 180;
// 			pnt2.y = -90;
			pnt1.x = 126.54;
			pnt1.y = 49;
			pnt2.x = 136;
			pnt2.y = 39;

			//default:10,11,12,13,14,15,16
			std::vector<int> nZoomArray;
			//for (int i = -3; i < 17; i++)
			//for (int i = 10; i < 15; i++)
			for (int i = 10; i < 17; i++)
			{
				nZoomArray.push_back(i);
			}
			CacheMaker make("tiles", pnt1, pnt2, nZoomArray, mapsourcid);
			make.setSavePath(savepath.c_str());
			make.setImageType("png");
			make.generate();
		}
		
		LocalSotrageManager::GetSingletonPtr()->AddLocalConfig(savepath.c_str(), "tiles.config");
		////////////////////////////////////////////

		// 'layer' is an autorelease object
		int currZoom = 12;
		RawTile tile(106, 54, currZoom, -1);//cell/tile coords
		MapControl* mapLayer = new MapControl(tile);
		mapLayer->setMapType(0);

		pHandle->attach((void*)mapLayer);
		mapLayer->getMap()->m_pHandler = pHandle;
		mapLayer->getMap()->getTileResolver()->setMapSourceId(mapsourcid);
		mapLayer->getMap()->getTileResolver()->m_pTileLoader->setSavePath(savepath);
		CC_BREAK_IF(!mapLayer);

		// add layer as a child to scene
		scene->addChild(mapLayer);
		mapLayer->setCenter(32.5, 114.0, currZoom); //latlng coords
		
		Vec2 pot = GeoUtils::getPixelOffsetInTile(32.5, 114.0, currZoom);//pixels coords
		Vec2 txy = GeoUtils::toTileXY(32.5, 114.0, currZoom);//tile coords
		//pLayer->getMap()->setGlobalOffset(txy);
		CCLOG("getPixelOffsetInTile:%f", pot.x, pot.y);
		CCLOG("toTileXY:%f,%f", txy.x, txy.y);

		LayerSwitcher* pLayerSwitcher = new LayerSwitcher(scene, mapLayer);
		scene->addChild(pLayerSwitcher);
		pLayerSwitcher->setLocalZOrder(-100);

		MarkerLayer* pMarkerLayer = new MarkerLayer(mapLayer);
		scene->addChild(pMarkerLayer);
		pMarkerLayer->setCurrZoom(currZoom);

		VectorLayer* pV = new VectorLayer(mapLayer);
		scene->addChild(pV);

		ControlLayer* pControlLayer = new ControlLayer(pLayerSwitcher, mapLayer);
		scene->addChild(pControlLayer);
		mapLayer->attachControlLayer(pControlLayer);

		/*

		//rect
		CoordinateArraySequence * cs = new CoordinateArraySequence();
		cs->add(Coordinate(114.23,26.54,0));
		cs->add(Coordinate(114.23,29.54,0));
		cs->add(Coordinate(116.23,29.54,0));
		cs->add(Coordinate(116.23,26.54,0));
		cs->add(Coordinate(114.23,26.54,0));
		
		//tringle
		//cs->add(Coordinate(117.23, 26.54, 0));
		//cs->add(Coordinate(118.23, 29.54, 0));
		//cs->add(Coordinate(117.23, 29.54, 0));
		//cs->add(Coordinate(117.23, 26.54, 0));

		GeometryFactory factory;
		LinearRing* viewBounds = factory.createLinearRing(cs);
		geos::geom::Polygon* p = GeometryFactory::getDefaultInstance()->createPolygon(viewBounds,NULL);

		int nCount = p->getNumPoints();
		nCount = ((Geometry*)viewBounds)->clone()->getNumPoints();
		CoordinateSequence* pCs = ((Geometry*)viewBounds)->getCoordinates();

		pV->AddFeatureLayer(new Feature(p));

		const GeometryFactory* pF = geos::geom::GeometryFactory::getDefaultInstance();
		geos::geom::Coordinate cp(110, 500);
		const geos::geom::Point* p1 = pF->createPoint(cp);
		pF->createLineString();        
		p1->getX();
		
		*/
	} while (0);

	return scene;
}



