#pragma once 

#include "cocos2d.h"
#include "Core/Singleton.h"
#include "Core/CacheInfo.pb.h"
#include "RawTile.h"

USING_NS_CC;

class LocalSotrageManager :  public Singleton<LocalSotrageManager>
{
public:
	LocalSotrageManager();
	~LocalSotrageManager();

	static LocalSotrageManager* GetSingletonPtr();
	static LocalSotrageManager& GetSingleton();

	void AddLocalConfig(std::string  strPath, std::string  strName);
	Image* getTile(RawTile* pTile);

private:
	std::vector<std::string> m_CachePathArray;
	std::map<int,std::string> m_LocalMap;
	
};