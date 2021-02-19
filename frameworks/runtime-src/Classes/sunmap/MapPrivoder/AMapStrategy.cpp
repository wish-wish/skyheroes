#include "AMapStrategy.h"

std::string AMapStrategy::getUrl( int x, int y, int z )
{
	const char* pUrl = "http://webrd02.is.autonavi.com/appmaptile?x=%d&y=%d&z=%d&lang=zh_cn&size=1&scale=1&style=7";

	char strUrl[512] = { 0 };
	sprintf(strUrl,pUrl, x, y, 17 - z);
	return std::string(strUrl);
}

std::string AMapStrategy::getDescription()
{
	return "AMap";
}

