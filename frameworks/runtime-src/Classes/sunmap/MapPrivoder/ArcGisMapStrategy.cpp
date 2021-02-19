#include "ArcGisMapStrategy.h"

std::string ArcGisMapStrategy::getUrl(int x, int y, int z)
{
	const char* pUrl = "http://www.arcgisonline.cn/ArcGIS/rest/services/ChinaOnlineCommunity/MapServer/tile/%d/%d/%d";
	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, 17 - z, y, x);
	return std::string(strUrl);
}

std::string ArcGisMapStrategy::getDescription()
{
	return "ArcGIS";
}

