#include "MapControlLayer.h"
#include "GeoUtils.h"
#include "ControlLayer.h"
#include "AppData.h"

MapControl::MapControl() : m_tBeginPos(Vec2::ZERO)
{
	setTouchEnabled(true);
	//setSwallowsTouches(true);
	Size size = Director::getInstance()->getVisibleSize();
	RawTile rawTile;
    m_pSprite = NULL;
	m_Map.initPhysicMap(size.width,size.height,rawTile,(void*)this);

	m_bIsNew = true;	
	m_firstEnd.tv_sec = m_firstEnd.tv_usec = 0;
	autorelease();
    
    Image* pImage = m_Map.getBgImage();
	Texture2D* pTexture = new Texture2D();
    pTexture->initWithImage(pImage);
    
    
	m_pSprite = Sprite::createWithTexture(pTexture);
    m_pSprite->setPosition(Vec2(size.width/2, size.height/2));
    float winw = size.width; //获取屏幕宽度
    float winh = size.height;//获取屏幕高度
    
    float spx = m_pSprite->getTextureRect().getMaxX();
    float spy = m_pSprite->getTextureRect().getMaxY();
    
    m_pSprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
    m_pSprite->setScaleY(winh/spy);
    m_pSprite->retain();
    m_pControlLayer = NULL;
    m_bMutiTouch = false;
    m_fMutiDistance = 0.0;
    m_offsetX = m_offsetY = 0.0;
    m_nTouches = 0;
    m_dScale = 1.0;
}


MapControl::MapControl( const RawTile& tile ): m_tBeginPos(Vec2::ZERO)
{
	setTouchEnabled(true);
	//setSwallowsTouches(false);
	m_bIsNew = true;
	Size size = Director::getInstance()->getVisibleSize();
	m_vSize = size;
    m_pSprite = NULL;
	m_Map.initPhysicMap(size.width,size.height,tile,(void*)this);
	m_firstEnd.tv_sec = m_firstEnd.tv_usec = 0;
    
    Image* pImage = m_Map.getBgImage();
	Texture2D* pTexture = new Texture2D();
    pTexture->initWithImage(pImage);
    
    
	m_pSprite = Sprite::createWithTexture(pTexture);
    m_pSprite->setPosition(Vec2(size.width/2, size.height/2));
    float winw = size.width; //获取屏幕宽度
    float winh = size.height;//获取屏幕高度
    
    float spx = m_pSprite->getTextureRect().getMaxX();
    float spy = m_pSprite->getTextureRect().getMaxY();
    
    m_pSprite->setScaleX(winw/spx); //设置精灵宽度缩放比例
    m_pSprite->setScaleY(winh/spy);
    m_pSprite->retain();
    
    m_pControlLayer = NULL;
    m_bMutiTouch = false;
    m_fMutiDistance = 0.0;
    m_offsetX = m_offsetY = 0.0;
    m_nTouches = 0;
    m_dScale = 1.0;
}

MapControl::~MapControl()
{

}

void MapControl::attachControlLayer(void* pControlLayer)
{
    m_pControlLayer = pControlLayer;
}

void MapControl::onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent)
{
	if (this->isEatMsg(aevent))
	{
		m_tBeginPos = touches[0]->getLocation();
		return;
	}
	//CCLOG("MapControl::onTouchesBegan");
	m_nTouches += touches.size();
	if (touches.size() > 1)
	{
		return;
	}
	m_Map.m_bInMove = false;
	if (m_nTouches == 1)
	{
		Touch* touch = touches[0];

		m_tBeginPos = touch->getLocation();
		
		CCTime::gettimeofdayCocos2d(&m_now, NULL);
		if (m_firstEnd.tv_sec != 0)//double click
		{
			long nTime = (m_now.tv_sec - m_firstEnd.tv_sec) * 1000 + (m_now.tv_usec - m_firstEnd.tv_usec) / 1000;
			if (nTime > 100 && nTime < 250)
			{
				
				Vec2 touchLocation = touches[0]->getLocation();
				Vec2 center = Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
				Vec2 offset = m_Map.getGlobalOffset();
				
				float nMoveX = offset.x + (center.x - touchLocation.x);
				float nMoveY = offset.y - (center.y - touchLocation.y);

				m_Map.setGlobalOffset(Vec2(nMoveX, nMoveY));
				m_Map.quickHack(true);

				zoomOut();
				if (m_pControlLayer)
				{
					((ControlLayer*)m_pControlLayer)->updateScale();
				}
			}
		}

		m_firstEnd.tv_sec = m_now.tv_sec;
		m_firstEnd.tv_usec = m_now.tv_usec;
	}
	m_tBeginPos = touches[0]->getLocation();
}

void MapControl::onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent)
{
	//TODO:边界处理
	bool isLock = AppSetting::getSingleton().getLock();
	int sel = AppSetting::getSingleton().getSelMode();
	if (sel == 2 || sel == 3 || sel == 4)
	{
		isLock = true;
	}
	if (this->isEatMsg(aevent)&&isLock)
	{
		return;
	}
	if (touches.size() == 1 && m_bMutiTouch == false)
	{
		cc_timeval m_now2;
		CCTime::gettimeofdayCocos2d(&m_now2, NULL);

		m_Map.m_bInMove = true;
		Touch* touch = touches[0];
		Vec2 touchLocation = touch->getLocation();

		float nMoveX = m_Map.getGlobalOffset().x + (touchLocation.x - m_tBeginPos.x);
		float nMoveY = m_Map.getGlobalOffset().y - (touchLocation.y - m_tBeginPos.y);
		m_Map.setGlobalOffset(Vec2(nMoveX, nMoveY));
		
		if (m_tBeginPos.equals(touchLocation) == false)
		{
			Ref* child;

			for each (child in getChildren())
			{
				Sprite* pNode = (Sprite*)child;
				if (pNode && pNode->getTag() != 0)
				{
					ActionInterval* actionBy = MoveBy::create(0.1, Vec2(touchLocation.x - m_tBeginPos.x, touchLocation.y - m_tBeginPos.y));
					pNode->runAction(actionBy);
				}
			}
			m_tBeginPos = touchLocation;
		}

		if ((m_now2.tv_sec > m_now.tv_sec))//move 
		{
			m_Map.m_bInMove = false;
			Touch* touch = touches[0];

			Vec2 touchLocation = touch->getLocation();
			
			float nMoveX = m_Map.getGlobalOffset().x + (touchLocation.x - m_tBeginPos.x);
			float nMoveY = m_Map.getGlobalOffset().y - (touchLocation.y - m_tBeginPos.y);

			m_Map.setGlobalOffset(CCPoint(nMoveX, nMoveY));
			m_Map.quickHack();
			m_tBeginPos = touchLocation;

			m_now.tv_usec = m_now2.tv_usec;
		}
	}
	else if (touches.size() == 2)
	{

		Touch *pCurTouch1;
		Touch *pCurTouch2;
		int i = 0;
		for each (Touch* iterTouch in touches)
		{
			if (i == 0)
			{
				pCurTouch1 = iterTouch;
			}
			else if (i == 1)
			{
				pCurTouch2 = iterTouch;
				break;
			}
			i++;
		}

		float lastDistance = pCurTouch1->getLocation().distance(pCurTouch2->getLocation());
		if (fabs(m_fMutiDistance - 0) < 0.1)
		{
			m_fMutiDistance = lastDistance;
		}

		if (m_bMutiTouch == false)
		{
			m_fMutiDistance = lastDistance;
			m_bMutiTouch = true;

			Ref* child;

			for each (child in getChildren())
			{
				Sprite* pNode = (Sprite*)child;
				if (pNode && pNode->getTag() != 0)
				{
					m_spritePnt.push_back(pNode->getPosition());

				}
			}
			CCTime::gettimeofdayCocos2d(&m_lastTouchTime, NULL);
		}
		else
		{
			cc_timeval m_nowTouchTime;
			CCTime::gettimeofdayCocos2d(&m_nowTouchTime, NULL);

			if (m_nowTouchTime.tv_sec == m_lastTouchTime.tv_sec && m_nowTouchTime.tv_usec - m_lastTouchTime.tv_usec < 20000)
				return;

			m_lastTouchTime = m_nowTouchTime;
			float offsetX, offsetY;
			if (lastDistance - m_fMutiDistance > 0)
			{
				m_dScale += 0.1;// (m_dScale * sqrt(lastDistance/m_fMutiDistance) );
				if (m_dScale > 2)
					m_dScale = 2;
			}
			else
			{

				m_dScale -= 0.1;// (m_dScale * sqrt(lastDistance/m_fMutiDistance) );
				if (m_dScale < 0.5)
					m_dScale = 0.5;
			}
			//m_dScale = (m_dScale * lastDistance/m_fMutiDistance );
			Vec2 pntOrg(m_Map.getWidth() / 2, m_Map.getHeight() / 2);
			int nIndex = 0;
			if (fabs(m_dScale - 1) > 0.1)
			{

				Ref* child;
				for each (child in getChildren())
				{
					Sprite* pNode = (Sprite*)child;
					if (pNode && pNode->getTag() != 0)
					{
						Vec2 pntNow = pNode->getPosition();
						Vec2 pnt = m_spritePnt[nIndex++];
						offsetX = pntOrg.x + (pnt.x - pntOrg.x)*m_dScale;
						offsetY = pntOrg.y + (pnt.y - pntOrg.y)*m_dScale;

						ActionInterval* actionBy = MoveBy::create(0.00001, CCPoint(offsetX - pntNow.x, offsetY - pntNow.y));
						ActionInterval* ScaleTo = ScaleBy::create(0.00001, m_dScale / pNode->getScale());

						pNode->runAction(CCSpawn::create(ScaleTo, actionBy, NULL));
					}
				}
			}
		}
	}
}

void MapControl::onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent)
{
	if (this->isEatMsg(aevent))
	{
		m_Map.m_bInMove = false;
		m_tBeginPos = touches[0]->getLocation();
		return;
	}
	m_nTouches -= touches.size();
	if (m_nTouches < 0){ m_nTouches = 0; }
	if (m_bMutiTouch == true) {
		if (m_nTouches == 0)
		{
			m_Map.m_bInMove = false;

			m_bMutiTouch = false;
			
			if (m_dScale > 1.4)
			{
				zoomOut();
				if (m_pControlLayer)
				{
					((ControlLayer*)m_pControlLayer)->updateScale();
				}

			}
			else if (m_dScale < 0.6)
			{
				zoomIn();
				if (m_pControlLayer)
				{
					((ControlLayer*)m_pControlLayer)->updateScale();
				}

			}
			else
			{
				Ref* child;
				int nIndex = 0;
				for each (child in getChildren())
				{
					Sprite* pNode = (Sprite*)child;
					if (pNode && pNode->getTag() != 0)
					{
						ActionInterval* actionTo = MoveTo::create(0.1, m_spritePnt[nIndex++]);
						ActionInterval* ScaleTo = ScaleTo::create(0.1, 1);
						pNode->runAction(CCSpawn::create(actionTo, ScaleTo, NULL));
					}
				}
			}
			m_offsetX = m_offsetY = 0.0;
			m_dScale = 1.0;
			m_spritePnt.clear();
		}
		return;
	}

	m_Map.m_bInMove = false;
	Touch* touch = touches[touches.size()-1];
	Vec2 touchLocation = touch->getLocation();
	//Vec2 offset = m_Map.getGlobalOffset();
	//float nMoveX = offset.x + (touchLocation.x - m_tBeginPos.x);
	//float nMoveY = offset.y - (touchLocation.y - m_tBeginPos.y);
	//m_Map.setGlobalOffset(Vec2(nMoveX, nMoveY));
	
	//Vec2 center = Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	//Vec2 move=center-touchLocation;

	//Vec2 p = GeoUtils::getLatLong(touchLocation.x, touchLocation.y, m_Map.getZoom());
	//CCLOG("lat=%f,lon=%f,zoom=%d,movex=%f,movey=%f", p.x, p.y,m_Map.getZoom(),nMoveX,nMoveY);
	//this->setCenter(p.x, p.y, m_Map.getZoomLevel());
	//TODO:边界处理
	m_tBeginPos = touchLocation;
	m_Map.quickHack(true);
	updateScreen();
}


void MapControl::onTouchesCancelled(const std::vector<Touch*>& touches, Event *aevent)
{
	this->onTouchesEnded(touches, aevent);
}

Vec2 MapControl::getGpsOffset()
{
    return  m_GPSOffset;
}

void MapControl::goTo( int x, int y, int z, int offsetX, int offsetY )
{
	m_Map.goTo(x, y, z, offsetX, offsetY);

	updateScreen();
}


Vec2 MapControl::getCenter(double lat, double lon, int zoom)
{
	double latFix = lat + m_GPSOffset.y*pow(10.0, -5);
	double lonFix = lon + m_GPSOffset.x*pow(10.0, -5);
	Vec2 p = GeoUtils::toTileXY(latFix, lonFix, zoom);
	Vec2 off = GeoUtils::getPixelOffsetInTile(latFix, lonFix, zoom);
	Vec2 cpixel = Vec2(p.x * 256 + off.x, p.y * 256 + off.y);
	return cpixel;
}

void MapControl::setCenter(double lat, double lon, int zoom)
{
	m_CenterPos = this->getCenter(lat, lon, zoom);
	m_CenterLatLon = Vec2(lat, lon);

	m_CenterTile = GeoUtils::toTileXY(lat, lon, zoom);
	m_GPSOffset = m_CenterTile;
	double factor = pow(10.0, -5);
	double latFix = lat + m_GPSOffset.y*factor;
	double lonFix = lon + m_GPSOffset.x*factor;
	Vec2 off = GeoUtils::getPixelOffsetInTile(latFix, lonFix, zoom);

	goTo((int)m_CenterTile.x, (int)m_CenterTile.y, zoom, (int)off.x, (int)off.y);
}

void MapControl::updateScreen()
{
	this->removeAllChildrenWithCleanup(true);
    if(m_pSprite !=  NULL)
    {
        addChild(m_pSprite,0,0);
    }
	for (int i = 2; i < m_Map.m_nTotalCellsX+2; i++) {
		for (int j = 2; j < m_Map.m_nTotalCellsY+2; j++) {
			if ((i > 1 && i < m_Map.m_nTotalCellsX+2) && ((j > 1 && j < m_Map.m_nTotalCellsY+2))) {
				Image* pData =  m_Map.getCell(i - 2, j - 2);
				if (pData != NULL && pData->getData() != NULL) {
					m_bIsNew = false;

					Texture2D* pTexture = new Texture2D();
					pTexture->initWithImage(pData);

					Sprite* pSprite = new Sprite();
					pSprite->init();
					pSprite->initWithTexture(pTexture);
					Vec2 point((i - 2) * GeoUtils::TILE_SIZE + m_Map.getGlobalOffset().x+ GeoUtils::TILE_SIZE/2
						,m_Map.getHeight()-((j - 2) * GeoUtils::TILE_SIZE + m_Map.getGlobalOffset().y+ GeoUtils::TILE_SIZE/2));
					pSprite->setPosition(point);
					//pSprite->autorelease();
					addChild(pSprite,1,i+j);

					pTexture->release();
					pSprite->release();

					//m_Map.setBitmap((Image*)NULL,i - 2, j - 2);
				}
			}
// 			else {
// 				if (pmap.scaleFactor == 1) {
// 					cs.drawBitmap(CELL_BACKGROUND, 
// 						(i - 2) * TILE_SIZE + pmap.getGlobalOffset().x, 
// 						(j - 2) * TILE_SIZE + pmap.getGlobalOffset().y, paint);
// 				} else {
// 					cs.drawBitmap(EMPTY_BACKGROUND, 
// 						(i - 2) * TILE_SIZE + pmap.getGlobalOffset().x, 
// 						(j - 2) * TILE_SIZE + pmap.getGlobalOffset().y, paint);
// 				}
// 			}
		}
	}
}

void MapControl::zoomOut()
{
	//CCLOG("TODO:zoomOut");
	if (m_Map.getZoomLevel() < 23)
	{
		float zoom = 1;
		zoom = m_Map.zoomS(2);
		this->dispatchZoom(zoom);
	}
}

void MapControl::zoomIn()
{
	//CCLOG("TODO:zoomIn");
	if (m_Map.getZoomLevel() > 0)
	{
		float zoom = 1;
		zoom = m_Map.zoomS(0.5);
		this->dispatchZoom(zoom);
	}
}

PhysicMap* MapControl::getMap()
{
	return &m_Map;
}

int MapControl::getZoomLevel()
{
	return m_Map.getZoomLevel();
}

void MapControl::setMapType( int nType )
{
	m_Map.setMapType(nType);
}

void MapControl::dispatchZoom(int zoom)
{
	EventDispatcher* dispatch = Director::getInstance()->getEventDispatcher();
	char nchar[10] = { 0 };
	sprintf(nchar, "%d", zoom);
	dispatch->dispatchCustomEvent("mapZoom", (void*)nchar);
}
