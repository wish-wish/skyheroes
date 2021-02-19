#ifndef __COCOS2D_BingMapStrategy_H__
#define BingMapStrategy_H__

#include "cocos2d.h"
#include "MapStrategy.h"

USING_NS_CC;

class BingMapStrategy : public MapStrategy
{
public:
	std::string getUrl(int x, int y, int z);
	std::string getDescription();
	std::string getImageType(){ return "png"; };
};

#endif