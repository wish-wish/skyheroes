#ifndef __COCOS2D_MapStrategy_H__
#define __COCOS2D_MapStrategy_H__

#include "cocos2d.h"

USING_NS_CC;

class MapStrategy
{
public:
	MapStrategy();
	virtual ~MapStrategy();

	virtual std::string getUrl(int x, int y, int z) = 0;
	virtual std::string getDescription() = 0;
	virtual std::string getImageType() = 0;
};

#endif