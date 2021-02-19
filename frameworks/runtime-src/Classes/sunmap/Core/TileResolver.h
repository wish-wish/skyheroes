#pragma once

#include "cocos2d.h"
#include "RawTile.h"
#include "TileLoader.h"
#include "BitmapCache.h"
#include "SqliteLocalStorage.h"

USING_NS_CC;

class TileResolver
{
public:
	enum Map_Type
	{
		Map_Hyber = 0,
		Map_Service = 1,
		Map_Cache = 2,
	};

public:
	TileResolver();
	TileResolver(void* pPhysicMap);
	~TileResolver();

	void resetLoaded();
	void incLoaded();
	int getLoaded();

	void getTile(RawTile* pTile);

	void setMapSourceId(int sourceId);
	int getMapSourceId();

	 void updateMap(RawTile* pTile, Image* pTexture);

	 bool put2Cache(RawTile* pTile, Image* pTexture);

	 void clearQueue();
	 void clearCache(bool bForce = false);

	 void load(RawTile* pTile);
	 void setMapType(int nType);
private:	
	
	void cthread();
private:
	
	int m_nStrategyId;
	int m_MapType;
	

public:
	void* m_pPhysicMap;
	TileLoader* m_pTileLoader;

	SqliteLocalStorage m_SqliteProvider;
	BitmapCache m_CacheProvider;

	int m_nLoaded;
};
