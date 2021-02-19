#include "Layer/LocalTileManager.h"
#include "FileOperation.h"
#include "platform/CCFileUtils.h"
#include "SunFile.h"
#include "MapStrategyFactory.h"

//template<> LocalSotrageManager* Singleton<LocalSotrageManager>::msSingleton = NULL;
LocalSotrageManager* LocalSotrageManager::GetSingletonPtr()
{
	if(msSingleton == NULL)
	{
		msSingleton = new LocalSotrageManager();
	} 
	return msSingleton;
}
LocalSotrageManager& LocalSotrageManager::GetSingleton()
{  
	if(msSingleton == NULL)
	{
		msSingleton = new LocalSotrageManager();
	} 
	return ( *msSingleton );  
}

LocalSotrageManager::LocalSotrageManager()
{

}

LocalSotrageManager::~LocalSotrageManager()
{

}

void LocalSotrageManager::AddLocalConfig( std::string strPath,std::string strName )
{
	char strFile[2048] = {0};
	sprintf(strFile, "%s/%s", strPath.c_str(), strName.c_str());
	//读取的字节数，读取失败则为0
	//ssize_t len = 0;
	//读取的内容	
	//unsigned char *data = FileUtils::getInstance()->getFileData(strFile, "rb", &len);
	Data data=FileUtils::getInstance()->getDataFromFile(strFile);

	sunMap::CacheInfo* pCache = new sunMap::CacheInfo;

	pCache->ParseFromArray(data.getBytes(), data.getSize());
	std::string strMapId = pCache->mapid();
	int nID = atoi(strMapId.c_str());

	std::map<int ,std::string >::iterator l_it;
	l_it=m_LocalMap.find(nID);
	if(l_it==m_LocalMap.end())
	{
		m_LocalMap.insert(std::make_pair(nID,strPath.c_str()));
	}
}

Image* LocalSotrageManager::getTile( RawTile* pTile )
{
	int nID = (pTile->s);
	std::map<int ,std::string >::iterator l_it;
	l_it=m_LocalMap.find(nID);
	if(l_it==m_LocalMap.end())
	{
		return NULL;
	}
	else
	{
		std::string strPath = l_it->second;
		MapStrategy* strategy=MapStrategyFactory::getStrategy(nID);
		char strFilePath[1024] = {0};
		sprintf(strFilePath, "%s%s/%d/%d/%d.%s", strPath.c_str()
			, strategy->getDescription().c_str(),
			pTile->z,pTile->y,pTile->x,"png");

		if (!SunFile::IsExist(strFilePath,true))
		{
			return NULL;
		}

		Image* pImage = new Image;
		pImage->initWithImageFile(strFilePath);
		return pImage;
	}
}
