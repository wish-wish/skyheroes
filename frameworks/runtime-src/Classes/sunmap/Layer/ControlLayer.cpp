#include "ControlLayer.h"
#include "SpriteWithHue.h"
#include "AppData.h"


ControlLayer::ControlLayer(LayerSwitcher* pLayerSwitcher, MapControl* pControl) 
	: m_max(22)
	, m_select_mode(0)
	, m_lock(true)
{
	m_pLayerSwitcher = pLayerSwitcher;
	m_pControl = pControl;

	do 
	{

		CC_BREAK_IF(! Layer::init());

		m_draw = DrawNode::create(3);
		this->addChild(m_draw);

		m_ScaleList.reserve(m_max);
		for (int i = 0; i<m_max; ++i)
		{
			char str[128] = {0};
			sprintf(str,"geo/scale%d.png", i + 1);

			SpriteWithHue* swh=SpriteWithHue::create(str);
			swh->setEnable(false);
			swh->retain();
			swh->setPosition(Vec2(34, 20));
			m_ScaleList.push_back(swh);
		}

		int zoom = pControl->getZoomLevel();

		addChild(m_ScaleList[zoom-1],4,4);
		Size size = Director::getInstance()->getVisibleSize();
		SpriteWithHue* zoomin = SpriteWithHue::create("geo/zoomin.png");
		zoomin->setName("in");
		zoomin->setPosition(Vec2(34,60));
		zoomin->setEnable(true);
		zoomin->setSelect(false);
		zoomin->setDefineInherite(true);
		zoomin->setCallBack(std::bind(&ControlLayer::onOperSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		SpriteWithHue* zoomout = SpriteWithHue::create("geo/zoomout.png");
		zoomout->setName("out");
		zoomout->setPosition(Vec2(34,115));
		zoomout->setEnable(true);
		zoomout->setSelect(false);
		zoomout->setDefineInherite(true);
		zoomout->setCallBack(std::bind(&ControlLayer::onOperSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		SpriteWithHue* ding = SpriteWithHue::create("geo/ding.png");
		ding->setName("lock");
		ding->setPosition(Vec2(34, size.height - 30));
		ding->setEnable(true);
		ding->setSelect(false);
		ding->setDefineInherite(true);
		ding->setCallBack(std::bind(&ControlLayer::onOperSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		SpriteWithHue* sel_mode = SpriteWithHue::create("geo/sel_mode.png");
		sel_mode->setName("sel_mode");
		sel_mode->setPosition(Vec2(34, size.height-85));
		sel_mode->setEnable(true);
		sel_mode->setSelect(false);
		sel_mode->setDefineInherite(true);
		sel_mode->setCallBack(std::bind(&ControlLayer::onOperSprite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		this->m_select_mode = 0;
		sel_mode->setHue(0);
		sel_mode->setTag(123);
		addChild(zoomin);
		addChild(zoomout);
		addChild(sel_mode);
		addChild(ding);
	} while (0);
}


ControlLayer::~ControlLayer()
{
	for (int i = 0; i < m_max; i++)
	{
		m_ScaleList[i]->release();
	}
}

void ControlLayer::zoomIn(Ref* pSender)
{
	if (!AppSetting::getSingleton().getLock())
	{
		m_pControl->zoomIn();
		updateScale();
	}
}

void ControlLayer::zoomOut(Ref* pSender)
{
	if (!AppSetting::getSingleton().getLock())
	{
		m_pControl->zoomOut();
		updateScale();
	}
}

void ControlLayer::layerswitch(Ref* pSender)
{
	m_pLayerSwitcher->setLocalZOrder(100);
    m_pLayerSwitcher->setVisible(true);
}

void ControlLayer::updateScale()
{
    removeChildByTag(4);
    
	int zoom = m_pControl->getZoomLevel();
	addChild(m_ScaleList[zoom-1],4,4);
}

bool ControlLayer::onOperSprite(Sprite* ahue, int atype, Touch * atouch, Event * aevent)
{
	EventDispatcher* dispatch = Director::getInstance()->getEventDispatcher();
	std::string name = ahue->getName();
	if (atype == 0)
	{
		if (name.compare("sel_mode")==0)
		{
			std::string data = "cancel_inherite";
			aevent->setEventData((void*)data.c_str(), false, data.length());
		}
	}
	else if (atype == 2)
	{
		if (name.compare("in") == 0)
		{
			this->zoomIn(nullptr);
		}
		else if (name.compare("out")==0)
		{
			this->zoomOut(nullptr);
		}
		else if (name.compare("lock") == 0)
		{
			this->m_lock = !this->m_lock;
			SpriteWithHue* hue = (SpriteWithHue*)ahue;
			if (this->m_lock)
			{
				hue->setHue(0);
			}
			else if (!this->m_lock)
			{
				hue->setHue(1.5 * M_PI);
			}
			AppSetting::getSingleton().setLock(this->m_lock);
			if (this->m_lock)
			{
				this->m_select_mode = 0;
				SpriteWithHue* shue = (SpriteWithHue*)this->getChildByTag(123);
				if (shue != nullptr)
				{
					shue->setHue(0);
				}
				char nchar[10] = { 0 };
				sprintf(nchar, "%d", this->m_select_mode);
				dispatch->dispatchCustomEvent("selMode", (void*)nchar);
				AppSetting::getSingleton().setSelMode(this->m_select_mode);
			}
		}
		else if (name.compare("sel_mode") == 0)
		{
			this->m_select_mode = (this->m_select_mode + 1) % 5;
			SpriteWithHue* hue = (SpriteWithHue*)ahue;
			if (this->m_select_mode == 0)//point select one one time default , can move  can scale
			{
				hue->setHue(0);
			}
			else if (this->m_select_mode == 1)//point select more one time , can move  can scale
			{
				hue->setHue(0.8 * M_PI);
			}
			else if (this->m_select_mode == 2)//rect select  , can't move  can scale, anti inner rect
			{
				hue->setHue(1.1 * M_PI);
			}
			else if (this->m_select_mode == 3)//rect select & one one time, can't move  can scale, anti inner rect
			{
				hue->setHue(1.4 * M_PI);
			}
			else if (this->m_select_mode == 4)//rect select & more one time, can't move  can scale, anti inner rect
			{
				hue->setHue(1.7 * M_PI);
			}
			char nchar[10] = { 0 };
			sprintf(nchar, "%d", this->m_select_mode);
			dispatch->dispatchCustomEvent("selMode", (void*)nchar);
			AppSetting::getSingleton().setSelMode(this->m_select_mode);
		}
	}
	return true;
}


