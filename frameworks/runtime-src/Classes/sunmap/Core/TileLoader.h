#pragma once

#include "cocos2d.h"
#include "RawTile.h"
#include "MapStrategy.h"

USING_NS_CC;

class RawTileObject
{
public:
	RawTileObject();
	~RawTileObject();

	RawTile * m_pTile;
};

class TileLoader
{
public:
	TileLoader();
	TileLoader(void* pTileResolver);
	~TileLoader();

	void addToQueue(RawTile* pTile);

	void load(RawTile* pTile);

	RawTile* getFromQueue();

	void run();

	void clearQueue();
	void setMapStrategy(MapStrategy* pMapStrategy);
	MapStrategy* getStrategy();
	void setSavePath(std::string path){ savepath = path; }
private:	
	std::vector<RawTile*> m_Queue;
	bool m_bLoading;
	void* m_pTileResolver;

	static int Max_Thread;

	MapStrategy* m_pMapStrategy;
public:
	int m_nCount;
	std::string  savepath;
};

