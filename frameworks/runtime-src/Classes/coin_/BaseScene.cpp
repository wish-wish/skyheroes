
#include "BaseScene.h"
#include "CellScene.h"

USING_NS_CC;


Scene* BaseScene::createTNScene(float t,Scene* scene)
{
    int randomint=CardRandom()%25;
    Scene* finalscene=scene;
    switch (randomint) {
        case 0:
            finalscene= TransitionCrossFade::create(t, scene);
            break;
        case 1:
            finalscene= TransitionRotoZoom::create(t, scene);
            break;
        case 2:
            finalscene= TransitionJumpZoom::create(t, scene);
            break;
        case 3:
            finalscene= TransitionMoveInL::create(t, scene);
            break;
        case 4:
            finalscene= TransitionMoveInR::create(t, scene);
            break;
        case 5:
            finalscene= TransitionMoveInT::create(t, scene);
            break;
        case 6:
            finalscene= TransitionMoveInB::create(t, scene);
            break;
        case 7:
            finalscene= TransitionSlideInL::create(t, scene);
            break;
        case 8:
            finalscene= TransitionSlideInR::create(t, scene);
            break;
        case 9:
            finalscene= TransitionSlideInB::create(t, scene);
            break;
        case 10:
            finalscene= TransitionSlideInT::create(t, scene);
            break;
        case 11:
            finalscene= TransitionShrinkGrow::create(t, scene);
            break;
        case 12:
            finalscene= TransitionFlipX::create(t, scene);
            break;
        case 13:
            finalscene= TransitionFlipY::create(t, scene);
            break;
        case 14:
            finalscene= TransitionFlipAngular::create(t, scene);
            break;
        case 15:
            finalscene= TransitionZoomFlipX::create(t, scene);
            break;
        case 16:
            finalscene= TransitionZoomFlipAngular::create(t, scene);
            break;
        case 17:
            finalscene= TransitionFade::create(t, scene);
            break;
        case 18:
            finalscene= TransitionCrossFade::create(t, scene);
            break;
        case 19:
            finalscene= TransitionTurnOffTiles::create(t, scene);
            break;
        case 20:
            finalscene= TransitionSplitCols::create(t, scene);
            break;
        case 21:
            finalscene= TransitionFadeTR::create(t, scene);
            break;
        case 22:
            finalscene= TransitionFadeBL::create(t, scene);
            break;
        case 23:
            finalscene= TransitionFadeUp::create(t, scene);
            break;
        case 24:
            finalscene= TransitionFadeDown::create(t, scene);
            break;
/*
        case 25:
            finalscene= TransitionSceneOriented::create(t, scene,TransitionScene::Orientation::LEFT_OVER);
            break;
        case 26:
            finalscene= TransitionSceneOriented::create(t, scene,TransitionScene::Orientation::RIGHT_OVER);
            break;
        case 27:
            finalscene= TransitionSceneOriented::create(t, scene,TransitionScene::Orientation::DOWN_OVER);
            break;
        case 28:
            finalscene= TransitionSceneOriented::create(t, scene,TransitionScene::Orientation::UP_OVER);
            break;
*/
        default:
            finalscene= TransitionCrossFade::create(t, scene);
            break;
    }
    return finalscene;
}

bool BaseScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
    
    FileUtils::getInstance()->addSearchPath("res",true);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	//add touch listener
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(BaseScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(BaseScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//add background
	auto layerColorBG = LayerColor::create(Color4B(180, 170, 160, 255));
	this->addChild(layerColorBG);
    
    m_RowCount = getRows();
    m_ColCount = getCols();
    
	createCardSprite(visibleSize,Vec2::ZERO);
	return true;
}

//create card with screen size
void BaseScene::createCardSprite(Size size,Vec2 offset)
{
    int maxwidth=size.width<size.height?size.width:size.height;
	int cardSize = (maxwidth-GridBrder*2) / m_RowCount;
    m_CardSize = cardSize;
    
	//绘制出nXn的单元格
    CCLOG("createCardSprite %d * %d",m_RowCount,m_RowCount);
	for (int i = 0; i < m_RowCount; i++)
	{
		for (int j = 0; j < m_RowCount; j++)
		{
			//need screen pixels match
            Size vs=Director::getInstance()->getVisibleSize();
            int x=vs.width/2-(m_CardSize*m_RowCount)/2+m_CardSize*i+offset.x;
            int y=vs.height/2-(m_CardSize*m_RowCount)/2+m_CardSize*j+offset.y;
			CardButton *card = CardButton::createCardSprite(j*m_RowCount+i+1, m_CardSize, m_CardSize, x,y,true,getWordsName());
            card->setGridPos(i, j);
			cardArr[i][j] = card;
			addChild(card);
            card->getCardBtn()->setUserData((void*)card);
            card->getCardBtn()->addTouchEventListener(CC_CALLBACK_2(BaseScene::onClick, this));
		}
	}
}



bool BaseScene::onTouchBegan(Touch* touch, Event* event)
{
	Vec2 beginTouch = touch->getLocation();  //获取OpenGL坐标，以左下角为原点
	beginX = beginTouch.x;
	beginY = beginTouch.y;
	return true;
}

void BaseScene::onTouchEnded(Touch* touch, Event* event)
{
	Vec2 endTouch = touch->getLocation();  //获取OpenGL坐标，以左下角为原点
}




