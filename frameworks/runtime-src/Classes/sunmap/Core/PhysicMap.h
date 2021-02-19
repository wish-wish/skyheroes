#ifndef __COCOS2D_PhysicMap_H__
#define __COCOS2D_PhysicMap_H__

#include "cocos2d.h"
#include "RawTile.h"
#include "TileResolver.h"
#include "PublicMsgHandler.h"

USING_NS_CC;

class PhysicMap
{
public:
	PhysicMap();
	~PhysicMap();

	void initPhysicMap(int width, int height, RawTile defTile,void* pMapControl);

	int getWidth();
	int getHeight();
	void setWidth(int width);
	 void setHeight(int height) ;

	void goTo(int x, int y, int z, int offsetX, int offsetY);
	void resetCell(int width, int height);

	Image* getCell(int x, int y);

	void update(Image* pTexture, RawTile* pTile);

	Vec2 getGlobalOffset();
	void setGlobalOffset(Vec2 pnt);

	void updateMap();

	 bool quickHack(bool bForce = false);
	void move(int dx, int dy);
	void setBitmap(Image* cell, int x, int y);
	int zoomS(double dz);

	void zoom(int x, int y, int z);

	TileResolver* getTileResolver();

	void reloadTiles();
    Image* getBgImage();

	int getZoomLevel();
	int getZoom();

	void setMapType(int nType);
private:
	void reload(int x, int y, int z);
	void loadCells(RawTile tile);

	int getCellSize(int length);
	
	void setBitmap(int nType, int x, int y);

	void releaseCells();
	void readBgData();
private:
	static int m_nZoom;

	int m_nWidth;
	int m_nHeight;

	TileResolver* m_pTileResolver;

	int m_nInZoom;

	Vec2 m_pntGlobalOffset;
	int m_nCorrectionX;
	int m_nCorrectionY;

	void* m_pMapControl;

	Image* m_pBg;
public:
	RawTile m_defTile;
	float m_fScaleFactor;
	bool m_bInMove;

	std::vector<Image*> m_Cells;
	int m_nTotalCells;
	int m_nTotalCellsX;
	int m_nTotalCellsY;

	bool m_update;

	PublicMsgHandler* m_pHandler;
};

#endif