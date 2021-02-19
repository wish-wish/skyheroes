#include "GoogleTerrainMapStrategy.h"

std::string GoogleTerrainMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://mts.google.com/vt/lyrs=t@130,r@207000000&hl=zh-CN&src=app&x=%d&y=%d&z=%d&s=";
	
	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, x, y, 17 - z);
	return std::string(strUrl);
}

std::string GoogleTerrainMapStrategy::getDescription()
{
	return "G_Terrain";
}

