#pragma once

#include "TileResolver.h"
#include "cocos2d.h"
#include "Singleton.h"

USING_NS_CC;

//Éú³É»º´æÍßÆ¬Àà
class CacheMaker:public Singleton<CacheMaker>
{
public:
	CacheMaker();
	CacheMaker(std::string strName, Vec2 pntLeftTop, Vec2 pntRightBottom, std::vector<int>& nZoomArray, int nMapStrategy);
	~CacheMaker();

	void setMaxThread(int nThread);
	int getMaxThread();

	int getMapStrategyId();
	MapStrategy* getMapStrategy();

	void setSavePath(const char* strPath);
	const char* getSavePath();

	void setImageType(std::string nType);
	const char* getImageType();

	bool build(Vec2 pntLeftTop, Vec2 pntRightBottom, std::vector<int>& nZoomArray, int nMapStrategy);

	void clear();

	void generate();

	void saveConfig();

	void update(RawTile* pTile);

private:

	std::vector<int> m_ZoomArray;
	std::vector<int> m_NumberPerZoom;
	std::vector<RawTile*> m_TileArray;
	int m_nTotalCount;
	int m_nNeedTodown;
	int m_nMapStrategy;

	MapStrategy* m_pMapStrategy;

	int m_nMaxThread;

	std::string m_strPath;
	std::string m_strImageType;

	int m_nThreadCount;
	std::vector<RawTile> m_failTile;
	std::vector<int> m_nZoomArray;
	std::string m_strName;

	Vec2 m_pntLeftTop;
	Vec2 m_pntRightBottom;
	int m_nTime;
};
