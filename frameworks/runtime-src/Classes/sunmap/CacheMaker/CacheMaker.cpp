#include "CacheMaker/CacheMaker.h"
#include "GeoUtils.h"
#include "MapStrategyFactory.h"

#include "curl/curl.h"
#include "TileResolver.h"
#include <thread>

#include "SunFile.h"
#include "Core/CacheInfo.pb.h"
#include "FileOperation.h"


std::mutex  cachemutex;

#include "tiffio.h"
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

//template<> CacheMaker* Singleton<CacheMaker>::msSingleton = NULL;

void save_tiff_rgb(string output_filename,int nWidth,int nHeight,const char* pImageData) {
	TIFF *output_image;

	// Open the TIFF file
	if((output_image = TIFFOpen(output_filename.c_str(), "w")) == NULL){
		cerr << "Unable to write tif file: " << output_filename << endl;
	}

	// We need to set some values for basic tags before we can add any data
	TIFFSetField(output_image, TIFFTAG_IMAGEWIDTH, nWidth);
	TIFFSetField(output_image, TIFFTAG_IMAGELENGTH, nHeight);
	TIFFSetField(output_image, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(output_image, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(output_image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(output_image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(output_image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	// Write the information to the file
	TIFFWriteEncodedStrip(output_image, 0, (void* )pImageData, nWidth*nHeight* 3);

	// Close the file
	TIFFClose(output_image);
}
struct tileData
{
	RawTile* pRawTile;
	char nTryTime;
	CacheMaker* pMaker;

};

static size_t OnWriteImageData(void* buffer, size_t size, size_t nmemb, void* lpVoid)  
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

void* downloadToSave(void* ppCurl)
{
	tileData* pData = (tileData*)ppCurl;

	pData->nTryTime--;
	pData->pRawTile;

	std::string strUrl = pData->pMaker->getMapStrategy()->getUrl(pData->pRawTile->x,pData->pRawTile->y,pData->pRawTile->z);

	std::string strResponse;

	CURLcode res;
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteImageData);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	res = curl_easy_perform(curl);

	long retcode = 0;  
	res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
	if(retcode == 0)
	{
		//TODO:print the curl error	
	}
	double length;
	res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length); 

	if (res==CURLE_OK && retcode == 200 && ((int)length)>0 && strResponse.size()>0)
	{
		Image* pImage = new Image;
		if(pImage->initWithImageData((const unsigned char*)strResponse.c_str(),length) == false)
		{
			delete pImage;
			pImage = NULL;

			curl_easy_cleanup(curl);

			if(pData->nTryTime>0)
			{
				pData->nTryTime--;
				thread pic(downloadToSave, (void*)pData);
				pic.detach();
			}
			else
			{
				cachemutex.lock();
				pData->pMaker->update(pData->pRawTile);
				cachemutex.unlock();
				delete pData;
				pData = NULL;
			}
		}
		else
		{
			cachemutex.lock();
			pData->pMaker->update(NULL);
			cachemutex.unlock();

			curl_easy_cleanup(curl);

			//////////////////////////////////////////////////////////////////////////
			
			std::string strPath;
			std::string strRootPath = pData->pMaker->getSavePath();
			char fp[1024] = { 0 };
			sprintf(fp, "%s//%s//%d//%d//", strRootPath.c_str(), pData->pMaker->getMapStrategy()->getDescription().c_str()
				, pData->pRawTile->z, pData->pRawTile->y);
			strPath = std::string(fp);
	
			char ff[1024] = { 0 };
			sprintf(ff, "%s%d.%s", strPath.c_str(), pData->pRawTile->x, pData->pMaker->getImageType());
				std::string strFile = std::string(ff);

			if (!SunFile::IsExist(strPath.c_str(),true))
			{
				SunFile::MkDirEx(strPath.c_str());
			}
			
			if (!SunFile::IsExist(strFile.c_str(),false))
			{
				std::string strName = pData->pMaker->getImageType();
				if ( strName.compare("tiff")== 0)
				{
					unsigned char* m_pData = pImage->getData();
					int m_nWidth,m_nHeight;
					m_nWidth = m_nHeight = 256;
					unsigned char *pTempData = new unsigned char[m_nWidth * m_nHeight * 3];

					for (int i = 0; i < m_nHeight; ++i)
					{
						for (int j = 0; j < m_nWidth; ++j)
						{
							pTempData[(i * m_nWidth + j) * 3] = m_pData[(i * m_nWidth + j) * 4];
							pTempData[(i * m_nWidth + j) * 3 + 1] = m_pData[(i * m_nWidth + j) * 4 + 1];
							pTempData[(i * m_nWidth + j) * 3 + 2] = m_pData[(i * m_nWidth + j) * 4 + 2];
						}
					}

					save_tiff_rgb(strFile.c_str(),256,256,(const char*)pTempData);
					delete pTempData;
				}
				else
				{
					pImage->saveToFile(strFile.c_str());
				}
			}

			delete pData;
			pData = NULL;
		}

		delete pImage;
		pImage = NULL;
	}
	else
	{
		curl_easy_cleanup(curl);

		if(pData->nTryTime>0)
		{
			pData->nTryTime--;
			thread pic(downloadToSave, (void*)pData);
			pic.detach();
		}
		else
		{
			cachemutex.lock();
			pData->pMaker->update(pData->pRawTile);
			cachemutex.unlock();
			delete pData;
			pData = NULL;
		}
	}
	return NULL;
}

CacheMaker::CacheMaker(std::string strName, Vec2 pntLeftTop, Vec2 pntRightBottom, std::vector<int>& nZoomArray, int nMapStrategy)
{
	m_strName = strName;
	m_nMapStrategy = nMapStrategy;
	m_nTotalCount = 0;
	m_nMaxThread = 2;
	m_nThreadCount = 0;

	m_pntLeftTop = pntLeftTop;
	m_pntRightBottom = pntRightBottom;
	m_nZoomArray = nZoomArray;

	m_nTime = 0;

	m_pMapStrategy = NULL;
}

CacheMaker::CacheMaker()
{
	m_pMapStrategy = NULL;

	m_nMapStrategy = -1;
	m_nTotalCount = 0;
	m_nMaxThread = 10; 
	m_nThreadCount = 0;
	m_nTime = 0;
}

CacheMaker::~CacheMaker()
{
	clear();
}

bool CacheMaker::build(Vec2 pntLeftTop, Vec2 pntRightBottom, std::vector<int>& nZoomArray, int nMapStrategy)
{
	clear();

	m_pMapStrategy = MapStrategyFactory::getStrategy(nMapStrategy);

	int nSize = nZoomArray.size();

	double lon1 = pntLeftTop.x;
	double lat1 = pntLeftTop.y;
	double lon2 = pntRightBottom.x;
	double lat2 = pntRightBottom.y;
	m_nTotalCount = 0;
	for (int i = 0;i<nSize;i++)
	{
		int nZoom = nZoomArray[i];
		m_ZoomArray.push_back(nZoom);
		Vec2 pnt1 = GeoUtils::toTileXY(lat1,lon1,nZoom);
		Vec2 pnt2 = GeoUtils::toTileXY(lat2, lon2, nZoom);

		int x1 = pnt1.x;
		int y1 = pnt1.y;
		int x2 = pnt2.x+1;
		int y2 = pnt2.y+1;

		int nRow = fabs((float)(x2 - x1));
		int nCol = fabs((float)(y2 - y1));
		int nCount = 0;
		
		for (int m = x1;m<x2;m++)
		{
			for (int n = y1;n<y2;n++)
			{
				std::string strRootPath = this->getSavePath();
				
				std::string strPath;
				char fp[1024] = { 0 };
				sprintf(fp, "%s//%s//%d//%d//", strRootPath.c_str(), m_pMapStrategy->getDescription().c_str()
					, nZoom, n);
				strPath = std::string(fp);

				char ff[1024] = { 0 };
				sprintf(ff, "%s%d.%s", strPath.c_str(), m, this->getImageType());
				std::string strFile = std::string(ff);

				if (!SunFile::IsExist(strFile.c_str(), false))
				{
					RawTile* pTile = new RawTile(m, n, nZoom, nMapStrategy);
					m_TileArray.push_back(pTile);
					nCount++;
				}
				else
				{
					//already exists
					//CCLOG("EXISTS:%s_%d//%d//%d//%d", m_pMapStrategy->getDescription().getCString(), nMapStrategy, nZoom, n, m);
				}
			}
		}
		m_nTotalCount += nCount;
		m_NumberPerZoom.push_back(nCount);
	}

	return true;
}

void CacheMaker::clear()
{
	int nCount = m_TileArray.size();

	for (int i=0;i<nCount;i++)
	{
		RawTile* pTile = m_TileArray[i];
		delete pTile;
		pTile = NULL;
	}

	m_ZoomArray.clear();
	m_NumberPerZoom.clear();
	m_TileArray.clear();
}

void CacheMaker::generate()
{
	build(m_pntLeftTop, m_pntRightBottom, m_nZoomArray, m_nMapStrategy);

	m_nNeedTodown = m_nTotalCount;

	clock_t begin = clock();
	while (m_nNeedTodown>0)
	{
		if (m_nThreadCount<m_nMaxThread && m_TileArray.size()>0)
		{
			RawTile* pTile = m_TileArray[m_TileArray.size()-1];
			m_TileArray.pop_back();
			tileData* pData = new tileData;
			pData->pRawTile = pTile;
			pData->nTryTime = 5;
			pData->pMaker = this;

			cachemutex.lock();
			m_nThreadCount++;
			cachemutex.unlock();

			thread pic(downloadToSave,(void*)pData);
			pic.detach();
		}
	}
	clock_t end = clock();

	m_nTime = (end-begin)/1000;

	saveConfig();
}

void CacheMaker::setMaxThread( int nThread )
{
	m_nMaxThread = nThread;
}

int CacheMaker::getMaxThread()
{
	return m_nMaxThread;
}

int CacheMaker::getMapStrategyId()
{
	return m_nMapStrategy;
}

MapStrategy* CacheMaker::getMapStrategy()
{
	return m_pMapStrategy;
}

void CacheMaker::setSavePath( const char* strPath )
{
	m_strPath = strPath;
}

const char* CacheMaker::getSavePath()
{
	return m_strPath.c_str();
}

void CacheMaker::setImageType(std::string nType )
{
	m_strImageType = nType;
}

const char* CacheMaker::getImageType()
{
	return m_strImageType.c_str();
}

void CacheMaker::update( RawTile* pTile )
{
	m_nNeedTodown--;
	m_nThreadCount--;
	if(pTile != NULL)
	{
		RawTile tile(pTile->x,pTile->y,pTile->z,pTile->s);
		m_failTile.push_back(tile);
	}
}

void CacheMaker::saveConfig()
{
	sunMap::CacheInfo* pCache = new sunMap::CacheInfo;
	pCache->set_name(m_strName.c_str());

	sunMap::Bounds* pBounds = new sunMap::Bounds;
	pBounds->set_left(m_pntLeftTop.x);
	pBounds->set_top(m_pntLeftTop.y);
	pBounds->set_right(m_pntRightBottom.x);
	pBounds->set_bottom(m_pntRightBottom.y);

	pCache->set_allocated_bound(pBounds);

	std::string strMapId;
	char strategy[10] = { 0 };
	sprintf(strategy, "%d", m_nMapStrategy);
	pCache->set_mapid(strategy);

	pCache->set_imagetype(m_strImageType.c_str());

	pCache->set_time(m_nTime);

	sunMap::LodInfos* pInfos = new sunMap::LodInfos;
	pInfos->set_numlevel(m_ZoomArray.size());
	pInfos->set_numtile2need(m_nTotalCount);
	pInfos->set_numtileindisk(m_nTotalCount-m_failTile.size());

	int nSize = m_ZoomArray.size();
	for (int i = 0;i<nSize;i++)
	{
		sunMap::LodInfo* pInfo = pInfos->add_lodinfos();;
		pInfo->set_level(m_ZoomArray[i]);
		pInfo->set_numtile2need(m_NumberPerZoom[i]);
	}

	nSize = m_failTile.size();
	for (int i=0;i<nSize;i++ )
	{
		RawTile tile = m_failTile[i];
		int j = 0;
		int count = m_ZoomArray.size();
		for (j=0;j<count;j++)
		{
			if (m_ZoomArray[j] == tile.z)
				break;
		}
		sunMap::LodInfo* pInfo = pInfos->mutable_lodinfos(j);
		sunMap::CacheTile* pTile = pInfo->add_tilelost();
		pTile->set_x(tile.x);
		pTile->set_y(tile.y);
		pTile->set_z(tile.z);
	}

	pCache->set_allocated_lods(pInfos);
	
	int length = pCache->ByteSize();
	char* buf = new char[length];
	pCache->SerializeToArray(buf,length);

	FileOperation::saveFile(buf,length,m_strPath+"/"+m_strName + ".config");
	delete pCache;
	pCache = NULL;

// 	delete pBounds;
// 	pBounds = NULL;
// 
// 	delete pInfos;
// 	pInfos = NULL;

	delete []buf;
	buf = NULL;
}
