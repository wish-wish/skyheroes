#include "GoogleSatelliteMapStrategy.h"

std::string GoogleSatelliteMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://khms.google.com/kh/v=129&src=app&x=%d&y=%d&z=%d&s=";

	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, x, y, 17 - z);
	return std::string(strUrl);
}

std::string GoogleSatelliteMapStrategy::getDescription()
{
	return "G_Satellite";
}

