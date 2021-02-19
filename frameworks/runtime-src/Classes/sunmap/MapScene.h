#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "MapControlLayer.h"
#include "PublicMsgHandler.h"

class MapScene : public cocos2d::Layer
{
public:

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::Scene* scene(PublicMsgHandler* pHandle);

	// implement the "static node()" method manually
	CREATE_FUNC(MapScene);
};

#endif  // __HELLOWORLD_SCENE_H__