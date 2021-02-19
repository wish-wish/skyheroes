#include "SuperMapCloudMapStrategy.h"

std::string SuperMapCloudMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://t0.supermapcloud.com/FileService/image?map=quanguo&type=web&x=%d&y=%d&z=%d";
	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, x, y, 17 - z);
	return std::string(strUrl);
}

std::string SuperMapCloudMapStrategy::getDescription()
{
	return "SuperMap_Cloud";
}

