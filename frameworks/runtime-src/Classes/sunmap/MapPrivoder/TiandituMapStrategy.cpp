#include "TiandituMapStrategy.h"

std::string TiandituMapStrategy::getUrl(int x, int y, int z)
{
	char* pUrl = "http://t0.tianditu.com/vec_w/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=vec&STYLE=default&TILEMATRIXSET=w&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&FORMAT=tiles";

	char strUrl[512] = { 0 };
	sprintf(strUrl,  pUrl, 17 - z, y, x);
	return std::string(strUrl);
}

std::string TiandituMapStrategy::getDescription()
{
	return "Tianditu";
}

