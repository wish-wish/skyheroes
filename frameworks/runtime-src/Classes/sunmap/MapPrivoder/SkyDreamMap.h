#ifndef __COCOS2D_SkyDream_H__
#define __COCOS2D_SkyDream_H__

#include "cocos2d.h"
#include "MapStrategy.h"

USING_NS_CC;

class SkyDreamMap : public MapStrategy
{
public:
	std::string getUrl(int x, int y, int z);
	std::string getDescription();
	std::string getImageType(){ return "png"; };
};

#endif