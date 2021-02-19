#ifndef __COCOS2D_MapControl_H__
#define __COCOS2D_MapControl_H__

#include "cocos2d.h"
#include "PhysicMap.h"
#include "RawTile.h"

USING_NS_CC;

typedef std::function<void(int nzoom)> MapCtlFunc;

class MapControl : public Layer
{
public:
	MapControl();
	MapControl(const RawTile& tile);
	~MapControl();

	//virtual void update(float delta);

	void onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent);
	void onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent);
	void onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent);
	void onTouchesCancelled(const std::vector<Touch*>& touches, Event *aevent);
	
	void setCenter(double lat, double lon, int zoom);
	Vec2 getCenter(double lat, double lon, int zoom);
	void goTo(int x, int y, int z, int offsetX, int offsetY);

	void updateScreen();

	void zoomOut();
	void zoomIn();

	PhysicMap* getMap();

	int getZoomLevel();
    
    void attachControlLayer(void* pControlLayer);
    
	Vec2 getGpsOffset();

	void setMapType(int nType);
	void dispatchZoom(int zoom);

public:
	enum CELL_Type
	{
		CELL_BACKGROUND = 0
	};
	
private:
	PhysicMap m_Map;
	Size m_vSize;
	Vec2 m_GPSOffset;
	bool m_bIsNew;
	Vec2 m_tBeginPos;
	
	Vec2 m_CenterPos;
	Vec2 m_CenterLatLon;
	Vec2 m_CenterTile;

	cc_timeval m_now; 
	cc_timeval m_firstEnd;
    Sprite* m_pSprite;
    void* m_pControlLayer;
    
    bool m_bMutiTouch;
    float m_fMutiDistance;
	Vec2 m_MutiPoint;
    float m_offsetX;
    float m_offsetY;
    int m_nTouches;
    double m_dScale;
    cc_timeval m_lastTouchTime;
	std::vector<Vec2> m_spritePnt;
};

#endif