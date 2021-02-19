#ifndef __COCOS2D_ControlLayer_H__
#define __COCOS2D_ControlLayer_H__

#include "cocos2d.h"
#include "MapControlLayer.h"
#include "LayerSwitcher.h"

USING_NS_CC;

class ControlLayer : public Layer
{
public:
	ControlLayer(LayerSwitcher* pLayerSwitcher,MapControl* pControl);
	virtual ~ControlLayer();

	// a selector callback
	void zoomIn(Ref* pSender);
	void zoomOut(Ref* pSender);
	void layerswitch(Ref* pSender);
    void updateScale();

	bool onOperSprite(Sprite* ahue, int atype, Touch * atouch, Event * aevent);
	void setDrawNode(DrawNode* draw);
private:
	LayerSwitcher* m_pLayerSwitcher;
	MapControl* m_pControl;
	std::vector<Sprite*> m_ScaleList;
	int m_max;
	int m_select_mode;//5种选择模式，1，单选，2，单点多选，3，框选，4，1+3，5，2+3
	bool m_lock;
	DrawNode* m_draw;
};

#endif