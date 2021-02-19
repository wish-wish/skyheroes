#include "BingMapStrategy.h"

std::string BingMapStrategy::getUrl( int x, int y, int z )
{
	int zi = 17-z;
	std::string strUrl = "";

	int i = 'z';
	for (i = zi; i>0; --i)
	{
		char digit = '0';
		int mask = 1 << (i - 1);
		if ((x & mask) != 0) {
			digit++;
		}
		if ((y & mask) != 0) {
			digit++;
			digit++;
		}
		strUrl += digit;
	}
	const char* pUrl = "http://r0.tiles.ditu.live.com/tiles/r%s.jpeg?g=91&mkt=zh-Hans&shading=hill";

	char retUrl[512] = { 0 };
	sprintf(retUrl, pUrl, strUrl.c_str());
	return std::string(retUrl);
}

std::string BingMapStrategy::getDescription()
{
	return "Bing";
}

