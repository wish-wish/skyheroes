
#include "RadLayer.h"
#include "SpriteWithHue.h"

RadLayer::RadLayer()
{
	setTouchEnabled(true);
}

RadLayer::~RadLayer()
{

}

RadLayer * RadLayer::create(const Color4B& color, GLfloat width, GLfloat height)
{
	RadLayer * layer = new (std::nothrow) RadLayer();
	if (layer && layer->initWithColor(color, width, height))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}


bool RadLayer::onTouchBegan(Touch *touch, Event *aevent)
{
	//CCLOG("RadLayer::onTouchBegan");
	bool ret = false;
	if (this->isSwallowsTouches())
	{
		std::string msg = "cancel_inherite";
		aevent->setEventData((void*)msg.c_str(), false, msg.length());
	}
	Vector<Node*> nodes = this->getChildren();
	bool ishit = false;
	for(int i=0;i<nodes.size();i++)
	{
	    Node* node=nodes.at(i);
		SpriteWithHue* sp = (SpriteWithHue*)node;
		SpriteWithHue* title = (SpriteWithHue*)sp->getChildByName("title");
		auto camera = Camera::getVisitingCamera();
		if (sp->hitTest(touch->getLocation(), camera, nullptr))
		{
			ishit = true;
		}
		if (title!=nullptr&&title->hitTest(touch->getLocation(), camera, nullptr))
		{
			ishit = true;
		}
	}
	if (!ishit){ 
		//this->getEventDispatcher()->dispatchCustomEvent("closeOper");
		return false;
	}
	if (!this->isEatMsg(aevent))
	{
		ret = LayerColor::onTouchBegan(touch, aevent);
	}
	return ret;
}

void RadLayer::onTouchMoved(Touch *touch, Event *aevent)
{
	LayerColor::onTouchMoved(touch, aevent);
}

void RadLayer::onTouchEnded(Touch *touch, Event *aevent)
{
	LayerColor::onTouchEnded(touch, aevent);
}

void RadLayer::onTouchCancelled(Touch *touch, Event *aevent)
{
	LayerColor::onTouchCancelled(touch, aevent);
}

void RadLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event *aevent)
{
	LayerColor::onTouchesBegan(touches, aevent);
}

void RadLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event *aevent)
{
	LayerColor::onTouchesMoved(touches, aevent);
}

void RadLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event *aevent)
{
	LayerColor::onTouchesEnded(touches, aevent);
}

void RadLayer::onTouchesCancelled(const std::vector<Touch*>&touches, Event *aevent)
{
	LayerColor::onTouchesCancelled(touches, aevent);
}
