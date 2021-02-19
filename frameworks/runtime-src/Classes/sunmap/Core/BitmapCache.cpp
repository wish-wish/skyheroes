#include "BitmapCache.h"
#include <thread>




//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::mutex bitmapmutex;

int BitmapCache::CACHE_SIZE = 200;

Image* BitmapCache::getTile( RawTile* pTile )
{
	Image* pData = NULL;
	std::map<std::string,Image*>::iterator l_it;

	l_it = m_DataCache.find(pTile->toString().c_str());
	 if(l_it!=m_DataCache.end ())
	{
		pData = l_it->second;
	}

	return pData;
}

bool BitmapCache::putToCache( RawTile* pTile, Image* pData )
{
	bitmapmutex.lock();
    
	std::map<std::string,Image*>::iterator l_it;

	l_it=m_DataCache.find(pTile->toString().c_str());
	if(l_it!=m_DataCache.end ())
	{
		delete pData;
		pData = NULL;
		bitmapmutex.unlock();
		return false;
	}

	m_DataCache.insert(std::map<std::string, Image*>::value_type(pTile->toString().c_str(), pData));
	m_Index.push_back(pTile->toString().c_str());
    
	bitmapmutex.unlock();
	return true;
}

BitmapCache::~BitmapCache()
{
	clear(true);
}

void BitmapCache::clear(bool bForce)
{
	if(bForce == true || m_DataCache.size()>BitmapCache::CACHE_SIZE)
	{
		bitmapmutex.lock();
		std::map<std::string,Image*>::iterator it = m_DataCache.begin();   
		for(;it!=m_DataCache.end();++it)
		{
			Image* pData = it->second;
			delete pData;
		}

		m_DataCache.clear();
		m_Index.clear();
		bitmapmutex.unlock();
	}
}
