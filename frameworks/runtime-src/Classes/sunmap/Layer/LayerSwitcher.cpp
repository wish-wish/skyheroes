#include "LayerSwitcher.h"
#include "MapStrategyFactory.h"
#include "VisibleRect.h"

#define LINE_SPACE          40

static Vec2 s_tCurPos = Vec2::ZERO;

LayerSwitcher::LayerSwitcher(Scene* pScene, MapControl* pControl) : m_tBeginPos(Vec2::ZERO)
{
	m_pScene = pScene;
	m_pControl = pControl;

	Size size = Director::getInstance()->getVisibleSize();
	Sprite* pSprite = Sprite::create("geo/combox.png");

	float nHeight = (MapStrategyFactory::m_mapStrategy.size() + 2) * LINE_SPACE;
	pSprite->setPosition(Vec2(size.width/2,size.height-nHeight/2));

	float spx = pSprite->getTextureRect().getMaxX();
	float spy = pSprite->getTextureRect().getMaxY();

	pSprite->setScaleX(size.width/3/spx*2);
	pSprite->setScaleY(nHeight/spy);
	pSprite->setOpacity(180);

	addChild(pSprite);

	// add menu items for tests
	m_pItemMenu = Menu::create();
	for (int i = 0; i < MapStrategyFactory::m_mapStrategy.size()+1; ++i)
	{
		std::string strLabel = "MapSrvType:";
		if (i != 0)
		{
			strLabel = MapStrategyFactory::m_mapStrategy[i-1]->getDescription();
		}
		
		LabelTTF* label = LabelTTF::create(strLabel.c_str(), "Arial", 24);
		// #endif        
		MenuItemLabel* pMenuItem = MenuItemLabel::create(label, this, menu_selector(LayerSwitcher::menuCallback));

		m_pItemMenu->addChild(pMenuItem, i + 10000);
		pMenuItem->setPosition( Vec2( VisibleRect::center().x, (VisibleRect::top().y - (i + 1) * LINE_SPACE) ));
	}

	m_pItemMenu->setContentSize(Size(VisibleRect::getVisibleRect().size.width+200, (MapStrategyFactory::m_mapStrategy.size() + 1) * (LINE_SPACE)));
	m_pItemMenu->setPosition(s_tCurPos);
	addChild(m_pItemMenu);

	setVisible(false);
}

LayerSwitcher::~LayerSwitcher()
{

}

void LayerSwitcher::menuCallback(Ref * pSender)
{
	MenuItem* pMenuItem = (MenuItem *)(pSender);
	int nIdx = pMenuItem->getLocalZOrder() - 10000;
	if (nIdx != 0)
	{
		m_pControl->getMap()->getTileResolver()->setMapSourceId(nIdx-1);
		MapStrategy* ms=MapStrategyFactory::getStrategy(nIdx - 1);
		std::string url=ms->getUrl(856, 416, 7);
		CCLOG("%s:%s", ms->getDescription().c_str(), url.c_str());
		setLocalZOrder(-100);
        setVisible(false);
	}
}


void LayerSwitcher::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	//CCLOG("LayerSwitcher::onTouchesBegan");
	m_tBeginPos = touches[0]->getLocation();
}

void LayerSwitcher::onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
	
	Touch* touch = (Touch*)(touches[0]);

	Vec2 touchLocation = touch->getLocation();
	float nMoveY = touchLocation.y - m_tBeginPos.y;

	Vec2 curPos = m_pItemMenu->getPosition();
	Vec2 nextPos = Vec2(curPos.x, curPos.y + nMoveY);

	if (nextPos.y < 0.0f)
	{
		m_pItemMenu->setPosition(Vec2::ZERO);
		return;
	}

	if (nextPos.y > ((MapStrategyFactory::m_mapStrategy.size() + 1)* LINE_SPACE - VisibleRect::getVisibleRect().size.height))
	{
		m_pItemMenu->setPosition(Vec2(0, ((MapStrategyFactory::m_mapStrategy.size() + 1)* LINE_SPACE - VisibleRect::getVisibleRect().size.height)));
		return;
	}

	m_pItemMenu->setPosition(nextPos);
	m_tBeginPos = touchLocation;
	s_tCurPos = nextPos;
}