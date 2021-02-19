#include "GoogleVectorMapStrategy.h"

std::string GoogleVectorMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://mts.google.com/vt/lyrs=m@216000000&hl=zh-CN&src=app&x=%d&y=%d&z=%d&s=";
	
	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, x, y, 17 - z);
	return std::string(strUrl);
}

std::string GoogleVectorMapStrategy::getDescription()
{
	return "G_Vector";
}

