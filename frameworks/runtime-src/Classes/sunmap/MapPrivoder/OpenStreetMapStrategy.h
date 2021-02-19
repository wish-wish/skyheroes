#ifndef __COCOS2D_OpenStreetMapStrategy_H__
#define __COCOS2D_OpenStreetMapStrategy_H__

#include "cocos2d.h"
#include "MapStrategy.h"

USING_NS_CC;

class OpenStreetMapStrategy : public MapStrategy
{
public:
	std::string getUrl(int x, int y, int z);
	std::string getDescription();
	std::string getImageType(){ return "png"; };
};

class StamenStreetMapStategy :public MapStrategy
{
	std::string getUrl(int x, int y, int z);
	std::string getDescription();
	std::string getImageType(){ return "png"; };
};

#endif