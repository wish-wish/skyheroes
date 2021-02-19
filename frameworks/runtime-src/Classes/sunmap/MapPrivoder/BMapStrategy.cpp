#include "BMapStrategy.h"
#include "tools.h"

std::string BMapStrategy::getUrl(int x, int y, int z)
{
	/*
	http://api.map.baidu.com/
	http://api0.map.bdimg.com/
	http://its.map.baidu.com:8002/
	http://or.map.bdimg.com/
	http://online0.map.bdimg.com/
	http://online1.map.bdimg.com/
	http://online2.map.bdimg.com/
	http://online3.map.bdimg.com/
	http://online4.map.bdimg.com/
	*/
	const char* pUrl = "http://online1.map.bdimg.com/tile/?qt=tile&x=%s&y=%s&z=%d&styles=pl";
	z = 17 - z;
	z--;

	int offsetX = 2<<z;
	int offsetY = offsetX - 1;

	int numX = x - offsetX;
	int numY = -y + offsetY;

	z++;
	int num = (x + y)%8 + 1;

	char xi[128] = { 0 };
	sprintf(xi, "%d", numX);
	std::string xs = std::string((char*)xi);
	Tools::string_replace(xs, "-", "M");
	

	char yi[128] = { 0 };
	sprintf(yi, "%d", numY);
	std::string ys = std::string((char*)yi);
	Tools::string_replace(ys, "-", "M");



	//原来：http://q3.baidu.com/it/u=x=721;y=209;z=12;v=014;type=web&fm=44
	//更新：http://online1.map.bdimg.com/tile/?qt=tile&x=23144&y=6686&z=17&styles=pl

	char retUrl[512] = { 0 };
	sprintf(retUrl, pUrl, xs.c_str(), ys.c_str(), z);
	return std::string(retUrl);
}

std::string BMapStrategy::getDescription()
{
	return "BaiduMap";
}

