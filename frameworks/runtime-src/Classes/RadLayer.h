#pragma once

#include "cocos2d.h"

USING_NS_CC;

class RadLayer :public LayerColor
{
public:
	RadLayer();
	virtual ~RadLayer();

	static RadLayer * create(const Color4B& color, GLfloat width, GLfloat height);
	
	virtual bool onTouchBegan(Touch *touch, Event *aevent);
	virtual void onTouchMoved(Touch *touch, Event *aevent);
	virtual void onTouchEnded(Touch *touch, Event *aevent);
	virtual void onTouchCancelled(Touch *touch, Event *aevent);
	virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent);
	virtual void onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent);
	virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent);
	virtual void onTouchesCancelled(const std::vector<Touch*>&touches, Event *aevent);
};