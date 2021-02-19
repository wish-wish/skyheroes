#include "OpenStreetMapStrategy.h"

std::string OpenStreetMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://b.tile.openstreetmap.org/%d/%d/%d.png";
	
	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, 17 - z, x, y);
	return std::string(strUrl);
}

std::string OpenStreetMapStrategy::getDescription()
{
	return "OSM";
}

std::string StamenStreetMapStategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://tile.stamen.com/toner/%d/%d/%d.png";

	char strUrl[512] = { 0 };
	sprintf(strUrl, pUrl, 17 - z, x, y);
	return std::string(strUrl);
}

std::string StamenStreetMapStategy::getDescription()
{
	return "Stamen";
}
