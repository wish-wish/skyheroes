#pragma once

#include "cocos2d.h"
#include "RawTile.h"
#include "TileLoader.h"

USING_NS_CC;

class BitmapCache
{
public:

	~BitmapCache();

	static int CACHE_SIZE;

	Image* getTile(RawTile* pTile);
	void clear(bool bForce = false);

	bool putToCache(RawTile* pTile, Image* pData);

private:
	std::map<std::string,Image*> m_DataCache;
    std::vector<std::string> m_Index;
};
