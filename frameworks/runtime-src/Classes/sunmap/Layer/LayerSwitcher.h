#ifndef __COCOS2D_LayerSwitcher_H__
#define __COCOS2D_LayerSwitcher_H__

#include "cocos2d.h"
#include "MapControlLayer.h"

USING_NS_CC;

class LayerSwitcher : public Layer
{
public:
	LayerSwitcher(Scene* pScene, MapControl* pControl);
	~LayerSwitcher();

	void menuCallback(Ref * pSender);

	void onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
	void onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
private:
	Scene* m_pScene;
	MapControl* m_pControl;
	Vec2 m_tBeginPos;
	Menu* m_pItemMenu;
};

#endif