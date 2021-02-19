#include "TileLoader.h"
#include "curl/curl.h"
#include "TileResolver.h"
#include <thread>

#include "PhysicMap.h"
#include "SunFile.h"
#include "CacheMaker.h"

int TileLoader::Max_Thread =20;

std::mutex tilemutex;

struct curl_data
{
	TileResolver* pTileResolver;
	RawTile* pRawTile;
	MapStrategy* strategy;
	char nTryTime;
};

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)  
{  
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);  
	if( NULL == str || NULL == buffer )  
	{  
		return -1;  
	}  

	char* pData = (char*)buffer;  
	str->append(pData, size * nmemb);  
	return nmemb;  
}  

void* download(void* pCurl)
{
	
	RawTile* pTile = ((curl_data*)pCurl)->pRawTile;
	TileResolver* pTileResolver = ((curl_data*)pCurl)->pTileResolver;
	 ((curl_data*)pCurl)->nTryTime--;
	//try
	{	
		std::string strUrl = pTileResolver->m_pTileLoader->getStrategy()->getUrl(pTile->x,pTile->y,pTile->z);
		
		std::string strResponse;

		CURLcode res;
		CURL* curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

		res = curl_easy_perform(curl);

		long retcode = 0;  
		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
		if (retcode == 0)
		{
			//TODO:print the curl error	
		}
		double length;
		res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length); 


		if (res==CURLE_OK && retcode == 200 && ((int)length)>0 && strResponse.size()>0)
		{
			Image* pImage = new Image;
			if (pImage->initWithImageData((unsigned char*)strResponse.c_str(), (int)length) == false)
            {
                delete pImage;
                pImage = NULL;
                
                curl_easy_cleanup(curl);
                
                if(((curl_data*)pCurl)->nTryTime>0)
                {
					std::thread pid(download, (void*)pCurl);
					pid.detach();
                }
                else
                {
                    ((TileResolver*)(pTileResolver))->m_pTileLoader->m_nCount--;
                    delete (curl_data*)pCurl;
                    pCurl = NULL;
                }
            }
			else
			{
				char strPath[2048] = { 0 };
				std::string strRootPath = ((TileResolver*)(pTileResolver))->m_pTileLoader->savepath;
				sprintf(strPath, "%s//%s//%d//%d//", strRootPath.c_str()
					, ((curl_data*)pCurl)->strategy->getDescription().c_str()
					,pTile->z, pTile->y);
				if (!SunFile::IsExist(strPath,true))
				{
					SunFile::MkDirEx(strPath);
				}

				char strFile[2048] = {0};
				sprintf(strFile,"%s%d.%s", strPath, pTile->x
					, ((curl_data*)pCurl)->strategy->getImageType().c_str());
				pImage->saveToFile(strFile,false);
				Data* data = new Data();
				data->copy((unsigned char*)strResponse.c_str(), (int)length);
				if(((TileResolver*)(pTileResolver))->put2Cache(pTile,pImage))
				{
					((PhysicMap*)(((TileResolver*)(pTileResolver))->m_pPhysicMap))->m_pHandler->addEvent(pTile, data);
				}
				((TileResolver*)(pTileResolver))->m_pTileLoader->m_nCount--;
				curl_easy_cleanup(curl);

				delete (curl_data*)pCurl;
				pCurl = NULL;
			}
		}
		else
		{
			curl_easy_cleanup(curl);

			if(((curl_data*)pCurl)->nTryTime>0)
			{
				std::thread pid(download,(void*)pCurl);
				pid.detach();
			}
			else
			{
				((TileResolver*)(pTileResolver))->m_pTileLoader->m_nCount--;
				delete (curl_data*)pCurl;
				pCurl = NULL;
			}
			
			//pthread_mutex_unlock(&mutex);
		}
	}				

	return NULL;
}

TileLoader::TileLoader()
{
	m_bLoading = false;
	m_pTileResolver = NULL;
	m_nCount = 0;
}

TileLoader::TileLoader( void* pTileResolver )
{
	m_bLoading = false;
	m_pTileResolver = pTileResolver;
	m_nCount = 0;
}

TileLoader::~TileLoader()
{
	m_nCount = 0;
}

void TileLoader::addToQueue( RawTile* pTile )
{
	tilemutex.lock();
	m_Queue.push_back(pTile);
	tilemutex.unlock();
}

void TileLoader::load( RawTile* pTile )
{
	tilemutex.lock();
	m_Queue.push_back(pTile);
	tilemutex.unlock();
}

RawTile* TileLoader::getFromQueue()
{
	tilemutex.lock();
	int nCount = m_Queue.size();
	if (nCount>0)
	{
		RawTile* pTile = m_Queue[nCount-1];
		m_Queue.pop_back();
		tilemutex.unlock();
		return pTile;
	}
	tilemutex.unlock();
	return NULL;
}

void TileLoader::clearQueue()
{
	tilemutex.lock();
	m_Queue.clear();
	tilemutex.unlock();
}

void TileLoader::run()
{
	while (true) {
		if(m_Queue.size()>0 && m_nCount< TileLoader::Max_Thread)
		{
			//try
			{
				RawTile* pTile = getFromQueue();
				if (pTile)
				{

					std::string strPath;
					char fp[1024] = { 0 };
					sprintf(fp, "%s//%s//%d//%d//", this->savepath.c_str(), m_pMapStrategy->getDescription().c_str()
						, pTile->z, pTile->y);
					strPath = std::string(fp);

					char ff[1024] = { 0 };
					sprintf(ff, "%s%d.%s", strPath.c_str(), pTile->x, m_pMapStrategy->getImageType().c_str());
					std::string strFile = std::string(ff);

					if (!SunFile::IsExist(strFile.c_str(), false))
					{
						m_nCount++;
						
						curl_data* pcData = new curl_data;
						pcData->pRawTile = pTile;
						pcData->pTileResolver = (TileResolver*)m_pTileResolver;
						pcData->strategy = m_pMapStrategy;
						pcData->nTryTime = 5;
						std::thread pid(download, (void*)pcData);
						pid.detach();
					}
					else
					{
						Image* pImage = new Image;
						if (pImage->initWithImageFile(strFile.c_str()) == false)
						{
							m_nCount++;
							curl_data* pcData = new curl_data;
							pcData->pRawTile = pTile;
							pcData->pTileResolver = (TileResolver*)m_pTileResolver;
							pcData->strategy = m_pMapStrategy;
							pcData->nTryTime = 5;
							std::thread pid(download, (void*)pcData);
							pid.detach();
						}
						else
						{
							if (((TileResolver*)(m_pTileResolver))->put2Cache(pTile, pImage))
							{
								Data data = FileUtils::getInstance()->getDataFromFile(strFile);
								Data* idat = new Data();
								idat->copy(data.getBytes(), data.getSize());
								((PhysicMap*)(((TileResolver*)(m_pTileResolver))->m_pPhysicMap))->m_pHandler->addEvent(pTile, idat);
							}
						}
					}
				}
			}			
		}
	}
}

void TileLoader::setMapStrategy( MapStrategy* pMapStrategy )
{
	m_pMapStrategy = pMapStrategy;
}

MapStrategy* TileLoader::getStrategy()
{
	return m_pMapStrategy;
}

RawTileObject::RawTileObject()
{
	m_pTile = NULL;
}

RawTileObject::~RawTileObject()
{
	m_pTile = NULL;
}
