
#include "CellScene.h"
#include "ActionModule.h"
#include "BaseScene.h"
//#include "ShareWrapper.h"

USING_NS_CC;

CellSceneBase::~CellSceneBase()
{
    _eventDispatcher->removeEventListener(touchListener);
}

Scene* CellSceneBase::createScene(string SceneName)
{
	auto scene = Scene::create();
    
    Scene*  object=nullptr;
    if (SceneName.compare("SceneMoney")==0) {
        object= SceneMoney::create();
    }
    else if (SceneName.compare("SceneMoneyInBa5")==0)
    {
        object=SceneMoneyInBa5::create();
    }
    if (scene!=nullptr) {
        scene->addChild(object);
    }
	return scene;
}

bool CellSceneBase::init()
{
	if ( !Scene::init() )
	{
		return false;
	}

    this->zeroArray();
    
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

    m_RCount = CardDispManager::getInstance()->GetDefRows();
    
	//add background
	auto layerColorBG = LayerColor::create(Color4B(180, 170, 160, 255));
	this->addChild(layerColorBG);
	
	//create score
    m_DispData = nullptr;
    m_isAnimating = false;
    
	cardNumberTTF = Label::create("", "Consolas", 50);
    cardNumberTTF->setPosition(Vec2(GridBrder, visibleSize.height - 50-GridBrder));
    cardNumberTTF->setAnchorPoint(Vec2(0,0.5));
    cardNumberTTF->setAlignment(TextHAlignment::LEFT);
	addChild(cardNumberTTF);
	cardNumberTTF->setVisible(false);
    
    maxNumber = Label::create("", "Consolas", 50);
    maxNumber->setPosition(Vec2(GridBrder, visibleSize.height - 100-GridBrder));
    maxNumber->setAnchorPoint(Vec2(0,0.5));
    maxNumber->setAlignment(TextHAlignment::LEFT);
    addChild(maxNumber);
	maxNumber->setVisible(false);

    maxRecordNumber = Label::create("", "Consolas", 50);
    maxRecordNumber->setAlignment(TextHAlignment::RIGHT);
    maxRecordNumber->setString("");
    maxRecordNumber->setAnchorPoint(Vec2(1,1));
    maxRecordNumber->setPosition(Vec2(visibleSize.width-GridBrder, visibleSize.height - 50-GridBrder));
    addChild(maxRecordNumber);
	maxRecordNumber->setVisible(false);

    aboutText = Label::create("", "Consolas", 35);
    aboutText->setAlignment(TextHAlignment::RIGHT);
    aboutText->setString("");
    aboutText->setAnchorPoint(Vec2(1,1));
    aboutText->setPosition(Vec2(visibleSize.width-GridBrder, 70));
    addChild(aboutText);
	aboutText->setVisible(false);

    timeNumber = Label::create("", "Consolas", 35);
    timeNumber->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 170-GridBrder));
    addChild(timeNumber);
	timeNumber->setVisible(false);

	Vec3 vs = this->calcCardSize(visibleSize);
	this->createCardSprite(visibleSize, Vec2(vs.x, vs.y));

    m_GameData = CardDispManager::getInstance()->getGameData();
    m_GameData->m_IsDefRowDirty = false;
    
    if (!CardDispManager::getInstance()->getIsReturn()) {
        CardDispManager::getInstance()->clearData();
        CardDispManager::getInstance()->setCurrentScene(GetSceneName());
        int num=getMinNumber();
        m_GameData->m_Score =0;
        m_GameData->m_CountSeconds=0;
        createCardNumber(num);
        createCardNumber();
    }else
    {
        m_DispData=CardDispManager::getInstance()->GetLeafStep();
        this->showCardsNumber(m_DispData);
        doCheck();
    }

    Button* btnShare=Button::create();
    btnShare->setPosition(Vec2(100, 100));
    btnShare->setTitleText("Share");
    btnShare->setTitleFontSize(40);
    btnShare->addTouchEventListener(CC_CALLBACK_2(CellSceneBase::onShare, this));
    addChild(btnShare);

    Button* btnReset=Button::create();
    btnReset->setPosition(Vec2(220, 100));
    btnReset->setTitleText("Exit");
    btnReset->setTitleFontSize(40);
    btnReset->addTouchEventListener(CC_CALLBACK_2(CellSceneBase::onReset, this));
    addChild(btnReset);
    
	/*
    Button* btnPrior=Button::create();
    btnPrior->setPosition(Vec2(visibleSize.width-220, m_RCount*m_CardSize + 100+GridBrder));
    btnPrior->setTitleText("Prior");
    btnPrior->setTitleFontSize(40);
    btnPrior->addTouchEventListener(CC_CALLBACK_2(CellSceneBase::onPrior, this));
    addChild(btnPrior);
    
    Button* btnNext=Button::create();
    btnNext->setPosition(Vec2(visibleSize.width-100, m_RCount*m_CardSize + 100+GridBrder));
    btnNext->setTitleText("Next");
    btnNext->setTitleFontSize(40);
    btnNext->addTouchEventListener(CC_CALLBACK_2(CellSceneBase::onNext, this));
    addChild(btnNext);
    */

    if(!isScheduled(schedule_selector(CellSceneBase::RenderFunc))) {
        schedule(schedule_selector(CellSceneBase::RenderFunc), 0.2);
    }
    
    //初始时生成两个2
    this->updateScore();
    
    int num=CardDispManager::getInstance()->getMaxNumber();
	int index = -1;
    string name=CardDispManager::getInstance()->getTransName(getNameList(), num,&index);
    string goal=CardDispManager::getInstance()->getGoalStr(getNameList(), num);
    if (goal.compare("")!=0) {
        maxRecordNumber->setString(__String::createWithFormat("%s/%s",name.c_str(),goal.c_str())->getCString());
    }else
    {
        maxRecordNumber->setString(__String::createWithFormat("%s",name.c_str())->getCString());
    }
    //aboutText->setString("dev by cclin 2015.2");
    
    
    //add touch listener
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(CellSceneBase::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(CellSceneBase::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
	return true;
}

void CellSceneBase::RenderFunc(float delta)
{
    m_GameData->m_CountSeconds+=1;
    int zzzcount=m_GameData->m_CountSeconds%10;
    
    long long dSeconds=m_GameData->m_CountSeconds/10;
    long long days=dSeconds / (60*60*24);
    
    int Seconds=dSeconds % (60*60*24);
    int hour=Seconds / (60*60);
    
    int minute=(Seconds % (60*60))/60;
    
    int second=Seconds % 60;
    
    if(days>0)
        timeNumber->setString(__String::createWithFormat("%03lld:%02i:%02i:%02i:%i",days,hour,minute,second,zzzcount)->getCString());
    else
        timeNumber->setString(__String::createWithFormat("%02i:%02i:%02i:%i",hour,minute,second,zzzcount)->getCString());
}

void CellSceneBase::onReset(Ref* ref,Widget::TouchEventType type)
{
    if(type==Widget::TouchEventType::ENDED)
    {
        //Director::getInstance()->replaceScene(BaseScene::createTNScene(0.5,SceneMoney::create()));
		Director::getInstance()->popScene();
    }
}

void CellSceneBase::onShare(Ref* ref,Widget::TouchEventType type)
{
    if(type==Widget::TouchEventType::ENDED)
    {
        string disp=m_DispData->toJson();
        map<string,string> params;
        //ShareWrapper::getInstance()->authMenu(params);
        
        CCLOG("Disp %s",disp.c_str());
    }
}

void CellSceneBase::onNext(Ref* ref,Widget::TouchEventType type)
{
    if(type==Widget::TouchEventType::ENDED)
    {
        CardDispData* data=m_DispData->getNextData();
        if (data!=nullptr) {
            m_DispData=data;
            this->showCardsNumber(data);
        }else
        {
            CCLOG("Already Last");
        }
        doCheck();
    }
}

void CellSceneBase::onPrior(Ref* ref,Widget::TouchEventType type)
{
    if(type==Widget::TouchEventType::ENDED)
    {
        CardDispData* data=m_DispData->getPriorData();
        if (data!=nullptr) {
            m_DispData=data;
            this->showCardsNumber(data);
        }else
        {
            CCLOG("Already First");
        }
        doCheck();
    }
}

void CellSceneBase::showCardsNumber(CardDispData* data)
{
    if (data==nullptr) {
        return;
    }
    NumberList* list=data->getNumberList();
    for (int i=0; i<list->size(); i++) {
        int x=i/m_RCount;
        int y=i%m_RCount;
        cardArr[x][y]->setNumber((*list)[i]);
        cardArr[x][y]->swapBuffer();
    };
    
    this->updateScore();
}

void CellSceneBase::saveStepData(CardDispData* data)
{
    for (int i=0; i<m_RCount; i++) {
        for (int j=0; j<m_RCount; j++) {
            data->setData(i, j, cardArr[i][j]->getNumber());
        }
    }
}

void CellSceneBase::writeHistoryData()
{
    CardDispManager::getInstance()->writeDataList();
}

void CellSceneBase::destoryCardSprite()
{
    for (int i = 0; i < maxRCount; i++)
    {
        for (int j = 0; j < maxRCount; j++)
        {
            if (cardArr[i][j]!=NULL) {
                cardArr[i][j]->removeFromParentAndCleanup(true);
                cardArr[i][j]=NULL;
            }
        }
    }
}

void CellSceneBase::zeroArray()
{
    for (int i = 0; i < maxRCount; i++)
    {
        for (int j = 0; j < maxRCount; j++)
        {
            cardArr[i][j]=nullptr;
        }
    }
}

//create card with screen size
void CellSceneBase::createCardSprite(Size size,Vec2 offset)
{
    
    
	//绘制出nXn的单元格
    CCLOG("createCardSprite %d * %d",m_RCount,m_RCount);
	for (int i = 0; i < m_RCount; i++)
	{
		for (int j = 0; j < m_RCount; j++)
		{
			//need screen pixels match lanscape
            int tcode=this->checkTerrainCode(udAny, i, j);
            int x=m_CardSize*i+GridBrder+offset.x;
            int y=m_CardSize*j+GridBrder+offset.y;
			CardSprite *card = CardSprite::createCardSprite(tcode, m_CardSize, m_CardSize, x, y,-1,isNameTrans(),getNameList());
            card->setGridPos(i, j);
			cardArr[i][j] = card;
			addChild(card);
            card->swapBuffer();
            if (tcode<0) {
                
                /*
                ArmatureModule::getInstance()->AddArmatureDataFile("effect/ui_effect.ExportJson");
                float x=cardArr[i][j]->getPixelsPosX();
                float y=cardArr[i][j]->getPixelsPosY();
                Rect rect(x,y,m_CardSize,m_CardSize);
                Armature* arm=ArmatureModule::getInstance()->PlayAround("ui_effect", "lianhuafxeffect",rect,nullptr,2.0f,rtClockWise);
                this->addChild(arm,EffectZorder);
                */
                
                /*
                ArmatureDataManager::getInstance()->addArmatureFileInfo("effect/ui_effect.ExportJson");
                
                Armature* armature = Armature::create("ui_effect");
                this->addChild(armature,1000);
                armature->setPosition(Vec2(x,y));
                Rect rect(x,y,m_CardSize,m_CardSize);
                Sequence* seq=ActionModule::getInstance()->NewAroundAction(rect,Vec2(x,y),nullptr,1);
                RepeatForever* rep=RepeatForever::create(seq);
                armature->runAction(rep);
                
                armature->getAnimation()->play("lianhuafxeffect");
                */
                
            }
		}
	}
}


Vec3 CellSceneBase::calcCardSize(Size size)
{
	int maxwidth = size.width < size.height ? size.width : size.height;
	int cardSize = (maxwidth - GridBrder * 2) / m_RCount;
	int diff = size.width - size.height;
	m_CardSize = cardSize;
	int width = cardSize*m_RCount;
	int offsetx = (size.width - width) / 2;
	int offsety = (size.height - width) / 2;
	return Vec3(offsetx-GridBrder, offsety-GridBrder, cardSize);
}

void CellSceneBase::changeRowCount(int count)
{
    this->destoryCardSprite();
    this->zeroArray();
    m_RCount = count;
    CardDispManager::getInstance()->SetDefRows(m_RCount);
    this->updateScore();
    Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec3 vs = this->calcCardSize(visibleSize);
    this->createCardSprite(visibleSize,Vec2(vs.x,vs.y));
}

bool CellSceneBase::onTouchBegan(Touch* touch, Event* event)
{
	Vec2 beginTouch = touch->getLocation();  //获取OpenGL坐标，以左下角为原点
	beginX = beginTouch.x;
	beginY = beginTouch.y;

	return true;
}

void CellSceneBase::onTouchEnded(Touch* touch, Event* event)
{
	Vec2 endTouch = touch->getLocation();  //获取OpenGL坐标，以左下角为原点
	//计算手指在X，Y移动的距离
	//CCLOG("x=%f y=%f",endTouch.x,endTouch.y);
	endX = beginX - endTouch.x;
	endY = beginY - endTouch.y;
    
    if (!m_isAnimating) {
        this->Logic();
    }
}


void CellSceneBase::clearUseds(UserDirection ud)
{
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = 0; y < m_RCount; y++)
        {
            int TerrainCode=this->checkTerrainCode(ud, x, y);
            if (TerrainCode!=0) {
                cardArr[x][y]->m_SrcUsed = true;
                cardArr[x][y]->m_DstUsed = true;
            }else
            {
                cardArr[x][y]->m_SrcUsed = false;
                cardArr[x][y]->m_DstUsed = false;
            }
            cardArr[x][y]->m_SrcPos.clear();
        }
    }
}

void CellSceneBase::doBackNumbers()
{
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            cardArr[x][y]->setOldNumber(cardArr[x][y]->getNumber());
            cardArr[x][y]->m_SrcUsed = false;
            cardArr[x][y]->m_DstUsed = false;
        }
    }
}

int64_t CellSceneBase::AddScore(int num)
{
    CardDispManager::getInstance()->getGameData()->m_Score+=num;
    return CardDispManager::getInstance()->getGameData()->m_Score;
};

bool CellSceneBase::MMLeft()
{
    this->clearUseds(udLeft);
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            if (cardArr[x][y]->getNumber()>0) {
                
                int destpos=-1;
                for (int i=x; i>=0; i--) {
                    if (checkTerrainCode(udLeft, i, y)!=0) break;
                    if (cardArr[i][y]->getNumber()==0&&!cardArr[i][y]->m_DstUsed) {
                        destpos = i;
                        continue;
                    }
                }
                if (destpos==-1) {
                    destpos=x;
                }
                if (!cardArr[x][y]->m_SrcUsed) {
                    if (m_DispData->isInMerge(&m_MergeList, x, y))
                    {
                        MergeObj* mergeob=m_DispData->getMergeObj(&m_MergeList, x, y);
                        if (mergeob->PosList[0]->x==x&&mergeob->PosList[0]->y==y) {
                            //do merge an move
                            int AllNum=0;
                            for (int i=0; i<mergeob->PosList.size(); i++) {
                                AllNum+=cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->getNumber();
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->m_SrcUsed = true;
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->setNumber(0);
                                if (mergeob->PosList[i]->x!=destpos) {
                                    cardArr[destpos][y]->addSrcPos(mergeob->PosList[i]->x, mergeob->PosList[i]->y);
                                }
                            }
                            AddScore(AllNum);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                SimpleAudioEngine::getInstance()->playEffect("r2048/effect_combine.mp3", false, 1.0, 1.0, 1.0);
                            }
                            mergeob->dstx = destpos;
                            mergeob->dsty = y;
                            cardArr[destpos][y]->setNumber(AllNum);
                            cardArr[destpos][y]->m_DstUsed = true;
                        }
                    }
                    else
                    {
                        //do one move
                        cardArr[destpos][y]->m_DstUsed = true;
                        if (destpos!=x) {
                            cardArr[destpos][y]->addSrcPos(x, y);
                            int num=cardArr[x][y]->getNumber();
                            cardArr[x][y]->setNumber(0);
                            cardArr[destpos][y]->setNumber(num);
                            if (CardDispManager::getInstance()->getIsSound())
                            {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/lianji_01.wav");
                            }
                        }
                        else
                        {
                            
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool CellSceneBase::MMRight()
{
    this->clearUseds(udRight);
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = m_RCount-1; x >= 0; x--)
        {
            if (cardArr[x][y]->getNumber()>0) {
                
                int destpos=-1;
                for (int i=x; i<m_RCount; i++) {
					if (checkTerrainCode(udRight, i, y) != 0) break;
                    if (cardArr[i][y]->getNumber()==0&&!cardArr[i][y]->m_DstUsed) {
                        destpos = i;
                        continue;
                    }
                }
                if (destpos==-1) {
                    destpos=x;
                }
                if (!cardArr[x][y]->m_SrcUsed) {
                    if (m_DispData->isInMerge(&m_MergeList, x, y))
                    {
                        MergeObj* mergeob=m_DispData->getMergeObj(&m_MergeList, x, y);
                        if (mergeob->PosList[0]->x==x&&mergeob->PosList[0]->y==y) {
                            //do merge an move
                            int AllNum=0;
                            for (int i=0; i<mergeob->PosList.size(); i++) {
                                AllNum+=cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->getNumber();
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->m_SrcUsed = true;
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->setNumber(0);
                                if (mergeob->PosList[i]->x!=destpos) {
                                    cardArr[destpos][y]->addSrcPos(mergeob->PosList[i]->x, mergeob->PosList[i]->y);
                                }
                            }
                            AddScore(AllNum);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                SimpleAudioEngine::getInstance()->playEffect("r2048/effect_combine.mp3", false, 1.0, 1.0, 1.0);
                            }
                            mergeob->dstx = destpos;
                            mergeob->dsty = y;
                            cardArr[destpos][y]->setNumber(AllNum);
                            cardArr[destpos][y]->m_DstUsed = true;
                        }
                    }
                    else
                    {
                        //do one move
                        cardArr[destpos][y]->m_DstUsed = true;
                        if (destpos!=x) {
                            cardArr[destpos][y]->addSrcPos(x, y);
                            int num=cardArr[x][y]->getNumber();
                            cardArr[x][y]->setNumber(0);
                            cardArr[destpos][y]->setNumber(num);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/lianji_01.wav");
                            }
                            
                        }
                        else
                        {
                            
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool CellSceneBase::MMUp()
{
    this->clearUseds(udUp);
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = m_RCount-1; y >= 0; y--)
        {
            if (cardArr[x][y]->getNumber()>0) {
                
                int destpos=-1;
                for (int i=y; i<m_RCount; i++) {
					if (checkTerrainCode(udUp, x, i) != 0) break;
                    if (cardArr[x][i]->getNumber()==0&&!cardArr[x][i]->m_DstUsed) {
                        destpos = i;
                        continue;
                    }
                }
                if (destpos==-1) {
                    destpos=y;
                }
                
                if (!cardArr[x][y]->m_SrcUsed) {
                    if (m_DispData->isInMerge(&m_MergeList, x, y))
                    {
                        MergeObj* mergeob=m_DispData->getMergeObj(&m_MergeList, x, y);
                        if (mergeob->PosList[0]->x==x&&mergeob->PosList[0]->y==y) {
                            //do merge an move
                            int AllNum=0;
                            for (int i=0; i<mergeob->PosList.size(); i++) {
                                AllNum+=cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->getNumber();
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->m_SrcUsed = true;
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->setNumber(0);

                                if (mergeob->PosList[i]->y!=destpos) {
                                    cardArr[x][destpos]->addSrcPos(mergeob->PosList[i]->x, mergeob->PosList[i]->y);
                                }
                            }
                            AddScore(AllNum);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/effect_combine.mp3", false, 1.0, 1.0, 1.0);
                            }
                            mergeob->dstx = x;
                            mergeob->dsty = destpos;
                            cardArr[x][destpos]->setNumber(AllNum);
                            cardArr[x][destpos]->m_DstUsed = true;
                        }
                    }
                    else
                    {
                        //do one move
                        cardArr[x][destpos]->m_DstUsed = true;
                        if (destpos!=y) {
                            cardArr[x][destpos]->addSrcPos(x, y);
                            int num=cardArr[x][y]->getNumber();
                            cardArr[x][y]->setNumber(0);
                            cardArr[x][destpos]->setNumber(num);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/lianji_01.wav");
                            }
                        }
                        else
                        {
                            
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool CellSceneBase::MMDown()
{
    this->clearUseds(udDown);
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = 0; y <m_RCount; y++)
        {
            if (cardArr[x][y]->getNumber()>0) {
                
                int destpos=-1;
                for (int i=y; i>=0; i--) {
					if (checkTerrainCode(udDown, x, i) != 0) break;
                    if (cardArr[x][i]->getNumber()==0&&!cardArr[x][i]->m_DstUsed) {
                        destpos = i;
                        continue;
                    }
                }
                if (destpos==-1) {
                    destpos=y;
                }
                if (!cardArr[x][y]->m_SrcUsed) {
                    if (m_DispData->isInMerge(&m_MergeList, x, y))
                    {
                        MergeObj* mergeob=m_DispData->getMergeObj(&m_MergeList, x, y);
                        if (mergeob->PosList[0]->x==x&&mergeob->PosList[0]->y==y) {
                            //do merge an move
                            int AllNum=0;
                            for (int i=0; i<mergeob->PosList.size(); i++) {
                                AllNum+=cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->getNumber();
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->m_SrcUsed = true;
                                cardArr[mergeob->PosList[i]->x][mergeob->PosList[i]->y]->setNumber(0);
                                
                                if (mergeob->PosList[i]->y!=destpos) {
                                    cardArr[x][destpos]->addSrcPos(mergeob->PosList[i]->x, mergeob->PosList[i]->y);
                                }
                            }
                            AddScore(AllNum);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/effect_combine.mp3", false, 1.0, 1.0, 1.0);
                            }
                            mergeob->dstx = x;
                            mergeob->dsty = destpos;
                            cardArr[x][destpos]->setNumber(AllNum);
                            cardArr[x][destpos]->m_DstUsed = true;
                        }
                    }
                    else
                    {
                        //do one move
                        cardArr[x][destpos]->m_DstUsed = true;
                        if (destpos!=y) {
                            cardArr[x][destpos]->addSrcPos(x, y);
                            int num=cardArr[x][y]->getNumber();
                            cardArr[x][y]->setNumber(0);
                            cardArr[x][destpos]->setNumber(num);
                            if (CardDispManager::getInstance()->getIsSound()) {
                                //SimpleAudioEngine::getInstance()->playEffect("r2048/lianji_01.wav");
                            }
                        }
                        else
                        {
                            
                        }
                    }
                }
            }
        }
    }
    return false;
}

void CellSceneBase::Logic()
{
    this->doBackNumbers();
    bool m_isMerge=false;
    if (abs(endX) > abs(endY))
    {
        //如果X轴移动的距离大于Y轴，则是左右移动
        if (endX + 5 > 0)
        {
            if (CanLeft())
            {
                m_isMerge=true;
            }
            MMLeft();
        }
        else
        {
            if (CanRight())
            {
                m_isMerge=true;
            }
            MMRight();
        }
    }
    else
    {
        if (endY + 5 > 0)
        {
            if (CanDown())
            {
                m_isMerge=true;
            }
            MMDown();
        }
        else
        {
            if (CanUp())
            {
                m_isMerge=true;
            }
            MMUp();
        }
    }
    doAnimations();
}


void CellSceneBase::doAnimations()
{
    m_isDirty = false;
    m_isAnimating = false;
    m_isChange = false;
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            for (int j=0;j<cardArr[x][y]->m_SrcPos.size();j++) {
                m_isDirty = true;
                m_isAnimating = true;
                int srcx=cardArr[x][y]->m_SrcPos[j].x;
                int srcy=cardArr[x][y]->m_SrcPos[j].y;
                if(srcx==x&&srcy==y)
                {
                    cardArr[x][y]->m_DstPos=Vec2(x,y);
                    int oldnum=cardArr[x][y]->getOldNumber();
                    
                    cardArr[x][y]->setDispNumber(oldnum);
                    Color3B color=CardSprite::GetBackColor(cardArr[x][y]->getNumber());
                    TintTo* tint=TintTo::create(MoveAniTime, color.r, color.g, color.b);
                    //RotateBy* rotate=RotateBy::create(AniTime, 360);
                    cardArr[x][y]->runAction(Sequence::create(DelayTime::create(MoveAniTime),
                                                              CallFunc::create(CC_CALLBACK_0(CellSceneBase::swapCard,this,cardArr[x][y])),NULL));
                    cardArr[x][y]->getBackground()->runAction(tint);
                    //CCLOG("delayshow old=%d new=%d x=%d y=%d",oldnum,cardArr[x][y]->getNumber(),x,y);
                }
                else
                {
                    
                    cardArr[srcx][srcy]->setDispNumber(0);
                    int sx=cardArr[srcx][srcy]->getPixelsPosX();
                    int sy=cardArr[srcx][srcy]->getPixelsPosY();
                    CardSprite *card = CardSprite::createCardSprite(cardArr[srcx][srcy]->getOldNumber(), m_CardSize, m_CardSize, sx, sy,-1,isNameTrans(),getNameList());
                    card->swapBuffer();
                    addChild(card,100);
                    
                    card->m_DstPos=Vec2(x, y);
                    
                    Color3B color=CardSprite::GetBackColor(cardArr[x][y]->getNumber());
                    TintTo* tint=TintTo::create(MoveAniTime, color.r, color.g, color.b);
                    MoveTo* move=MoveTo::create(MoveAniTime, cardArr[x][y]->GetCardPostion());
                    card->runAction(Sequence::create(move,
                                                     CallFunc::create(CC_CALLBACK_0(CellSceneBase::removeCard,this,card)),NULL)
                                    );
                    RotateBy* rotate=RotateBy::create(MoveAniTime, 360);
                    Spawn* spawn=Spawn::create(rotate,tint, NULL);
                    
                    card->getBackground()->runAction(spawn);
                    
                    if(cardArr[x][y]->getDispNumber()>0)
                    {
                        cardArr[x][y]->runAction(rotate->clone());
                    }
                    
                    //CCLOG("move old=%d new=%d x=%d y=%d dstx=%d y=%d",cardArr[x][y]->getOldNumber(),cardArr[x][y]->getNumber(),srcx,srcy,x,y);
                    m_isChange = true;
                }
            }
        }
    }
    
    if(m_isChange)
    {
        //
    }
}


void CellSceneBase::removeCard(CardSprite* b)
{
    if (b!=nullptr) {
        this->swapCard(b);
        b->removeFromParentAndCleanup(true);
    }
}

void CellSceneBase::swapCard(CardSprite* b)
{
    if (b!=nullptr) {
        int x=b->m_DstPos.x;
        int y=b->m_DstPos.y;
        if (cardArr[x][y]!=nullptr) {
            cardArr[x][y]->swapBuffer();
        }
    }
    if (m_isDirty) {
        m_isDirty = false;
        doCheck();
        updateScore();
    }
    if (m_isChange) {
        m_isChange=false;
        createCardNumber();
    }
    m_isAnimating = false;
}

void CellSceneBase::doDisplayNumbers()
{
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            cardArr[x][y]->swapBuffer();
        }
    }
}

void CellSceneBase::doCheck()
{
	bool isGameOver = true;
    
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            //cardArr[x][y]->getBackground()->stopAllActions();
            //cardArr[x][y]->getBackground()->setScale(1.0);
            cardArr[x][y]->StopHintAnim();
            cardArr[x][y]->getBackground()->setRotation(0);
        }
    }
    
    //TODO:Save MoveList
    
	//check up down left right is the same with me
    bool canleft=CanLeft();
    bool canright=CanRight();
    bool canup=CanUp();
    bool candown=CanDown();
    if (hasEmptyCard()||canleft||canright||canup||candown) {
        isGameOver = false;
    }
	if (isGameOver)
	{
		//restart
        CCLOG("GameOver");
		//Director::getInstance()->replaceScene(BaseScene::createTNScene(1.5, SceneMoney::create()));
		Director::getInstance()->popScene();
	}
}

void CellSceneBase::updateScore()
{
    int score=CardDispManager::getInstance()->getGameData()->m_Score;
    string scorestr=__String::createWithFormat("%i%s",score,CardDispManager::getInstance()->getTransUnit(getNameList()).c_str())->getCString();
    cardNumberTTF->setString(scorestr);
    
    int maxnum=getMaxCardNumber();
	int index = -1;
    string name=CardDispManager::getInstance()->getTransName(getNameList(), maxnum,&index);
    
    maxNumber->setString(__String::createWithFormat("%s",name.c_str())->getCString());
    if (score>CardDispManager::getInstance()->getMaxScore()) {
        CardDispManager::getInstance()->setMaxScore(score);
        //TODO:Effect & scound
        
    }
    string goal=CardDispManager::getInstance()->getGoalStr(getNameList(), maxnum);
    if (goal.compare(name)==0) {
        CCLOG("Game Complete");
		Director::getInstance()->replaceScene(BaseScene::createTNScene(1.5, SceneMoney::create()));
    }
    else
    {
        if (maxnum>CardDispManager::getInstance()->getMaxNumber()) {
            if (goal.compare("")!=0) {
                maxRecordNumber->setString(__String::createWithFormat("%s/%s",name.c_str(),goal.c_str())->getCString());
            }else
            {
                maxRecordNumber->setString(__String::createWithFormat("%s",name.c_str())->getCString());
            }
            CardDispManager::getInstance()->setMaxNumber(maxnum);
            CardDispManager::getInstance()->writeConfig();
            
            //TODO:Effect & scound
            //UIEffectManager::getInstance()->playEffect(this, "effect/ui_effect.ExportJson", "ui_effect", EFFECT_CALLBACK_3(CellSceneBase::onEffectDone,this), "lvtishengeffect",Vec2(0,0),1);
            
            if (CardDispManager::getInstance()->getIsSound()) {
                SimpleAudioEngine::getInstance()->playEffect("r2048/shengli.wav");
            }
        }
    }
}

void CellSceneBase::onEffectDone(Armature* armature,MovementEventType movementType,const std::string& movementID)
{
    CCLOG("Effect Done");
}

bool CellSceneBase::isMerge2048(CardSprite* a,CardSprite* b)
{
    if(a->getNumber()==b->getNumber())
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed=true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b)
{
    if (a->getNumber()==b->getNumber()
        &&(CardDispManager::getInstance()->isTransValueExists(getNameList(), a->getNumber()*2)
           )
        )
    {
        MergeObj* mergeobj=new struct MergeObj();
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
            
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}


bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;

        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber()+e->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = e->getGridPosX();
        posc->y = e->getGridPosY();
        mergeobj->PosList.push_back(posc);
        e->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        //e->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        e->ShowHintAnim();
        
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber()+e->getNumber()+f->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = e->getGridPosX();
        posc->y = e->getGridPosY();
        mergeobj->PosList.push_back(posc);
        e->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = f->getGridPosX();
        posc->y = f->getGridPosY();
        mergeobj->PosList.push_back(posc);
        f->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        //e->getBackground()->runAction(HingAnim());
        //f->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        e->ShowHintAnim();
        f->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber()+e->getNumber()+f->getNumber()+g->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = e->getGridPosX();
        posc->y = e->getGridPosY();
        mergeobj->PosList.push_back(posc);
        e->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = f->getGridPosX();
        posc->y = f->getGridPosY();
        mergeobj->PosList.push_back(posc);
        f->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = g->getGridPosX();
        posc->y = g->getGridPosY();
        mergeobj->PosList.push_back(posc);
        g->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        //e->getBackground()->runAction(HingAnim());
        //f->getBackground()->runAction(HingAnim());
        //g->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        e->ShowHintAnim();
        f->ShowHintAnim();
        g->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber()+e->getNumber()+f->getNumber()+g->getNumber()+h->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = e->getGridPosX();
        posc->y = e->getGridPosY();
        mergeobj->PosList.push_back(posc);
        e->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = f->getGridPosX();
        posc->y = f->getGridPosY();
        mergeobj->PosList.push_back(posc);
        f->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = g->getGridPosX();
        posc->y = g->getGridPosY();
        mergeobj->PosList.push_back(posc);
        g->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = h->getGridPosX();
        posc->y = h->getGridPosY();
        mergeobj->PosList.push_back(posc);
        h->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        //e->getBackground()->runAction(HingAnim());
        //f->getBackground()->runAction(HingAnim());
        //g->getBackground()->runAction(HingAnim());
        //h->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        e->ShowHintAnim();
        f->ShowHintAnim();
        g->ShowHintAnim();
        h->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

bool CellSceneBase::isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h,CardSprite* i)
{
    int number=a->getNumber()+b->getNumber()+c->getNumber()+d->getNumber()+e->getNumber()+f->getNumber()+g->getNumber()+h->getNumber()+i->getNumber();
    if(CardDispManager::getInstance()->isTransValueExists(getNameList(), number))
    {
        MergeObj* mergeobj=new struct MergeObj();
        
        MergePos* posa=new MergePos();
        posa->x = a->getGridPosX();
        posa->y = a->getGridPosY();
        mergeobj->PosList.push_back(posa);
        a->m_SrcUsed = true;
        
        MergePos* posb=new MergePos();
        posb->x = b->getGridPosX();
        posb->y = b->getGridPosY();
        mergeobj->PosList.push_back(posb);
        b->m_SrcUsed = true;
        
        MergePos* posc=new MergePos();
        posc->x = c->getGridPosX();
        posc->y = c->getGridPosY();
        mergeobj->PosList.push_back(posc);
        c->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = d->getGridPosX();
        posc->y = d->getGridPosY();
        mergeobj->PosList.push_back(posc);
        d->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = e->getGridPosX();
        posc->y = e->getGridPosY();
        mergeobj->PosList.push_back(posc);
        e->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = f->getGridPosX();
        posc->y = f->getGridPosY();
        mergeobj->PosList.push_back(posc);
        f->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = g->getGridPosX();
        posc->y = g->getGridPosY();
        mergeobj->PosList.push_back(posc);
        g->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = h->getGridPosX();
        posc->y = h->getGridPosY();
        mergeobj->PosList.push_back(posc);
        h->m_SrcUsed = true;
        
        posc=new MergePos();
        posc->x = i->getGridPosX();
        posc->y = i->getGridPosY();
        mergeobj->PosList.push_back(posc);
        i->m_SrcUsed = true;
        
        //a->getBackground()->runAction(HingAnim());
        //b->getBackground()->runAction(HingAnim());
        //c->getBackground()->runAction(HingAnim());
        //d->getBackground()->runAction(HingAnim());
        //e->getBackground()->runAction(HingAnim());
        //f->getBackground()->runAction(HingAnim());
        //g->getBackground()->runAction(HingAnim());
        //h->getBackground()->runAction(HingAnim());
        //i->getBackground()->runAction(HingAnim());
        a->ShowHintAnim();
        b->ShowHintAnim();
        c->ShowHintAnim();
        d->ShowHintAnim();
        e->ShowHintAnim();
        f->ShowHintAnim();
        g->ShowHintAnim();
        h->ShowHintAnim();
        i->ShowHintAnim();
        
        m_MergeList.push_back(mergeobj);
        return true;
    }
    return false;
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;

    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());

    //a->StopHintAnim();
    //b->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    if(c!=nullptr)
    {
        c->m_SrcUsed = false;
        //c->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    
    //a->StopHintAnim();
    //b->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    c->m_SrcUsed = false;
    if (d!=nullptr) {
        d->m_SrcUsed = false;
        //d->StopHintAnim();
    }
    
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());

    //a->StopHintAnim();
    //b->StopHintAnim();
    //c->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    c->m_SrcUsed = false;
    d->m_SrcUsed = false;
    if(e!=nullptr)
    {
        e->m_SrcUsed = false;
        //e->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());
    //e->getBackground()->runAction(HingAnim());
    
    //a->StopHintAnim();
    //b->StopHintAnim();
    //c->StopHintAnim();
    //d->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    c->m_SrcUsed = false;
    d->m_SrcUsed = false;
    e->m_SrcUsed = false;
    if(f!=nullptr)
    {
        f->m_SrcUsed = false;
        //f->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());
    //e->getBackground()->runAction(HingAnim());
    //f->getBackground()->runAction(HingAnim());

    //a->StopHintAnim();
    //b->StopHintAnim();
    //c->StopHintAnim();
    //d->StopHintAnim();
    //e->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g)
{
    if(a!=nullptr)
    {
        a->m_SrcUsed = false;
        //a->StopHintAnim();
    }
    if(b!=nullptr)
    {
        b->m_SrcUsed = false;
        //b->StopHintAnim();
    }
    if(c!=nullptr)
    {
        c->m_SrcUsed = false;
        //c->StopHintAnim();
    }
    if(d!=nullptr)
    {
        d->m_SrcUsed = false;
        //d->StopHintAnim();
    }
    if(e!=nullptr)
    {
        e->m_SrcUsed = false;
        //e->StopHintAnim();
    }
    if(f!=nullptr)
    {
        f->m_SrcUsed = false;
        //f->StopHintAnim();
    }
    if(g!=nullptr)
    {
        g->m_SrcUsed = false;
        //g->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());
    //e->getBackground()->runAction(HingAnim());
    //f->getBackground()->runAction(HingAnim());
    //g->getBackground()->runAction(HingAnim());
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    c->m_SrcUsed = false;
    d->m_SrcUsed = false;
    e->m_SrcUsed = false;
    f->m_SrcUsed = false;
    g->m_SrcUsed = false;
    if(h!=nullptr)
    {
        h->m_SrcUsed = false;
        //h->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());
    //e->getBackground()->runAction(HingAnim());
    //f->getBackground()->runAction(HingAnim());
    //g->getBackground()->runAction(HingAnim());
    //h->getBackground()->runAction(HingAnim());
    
    //a->StopHintAnim();
    //b->StopHintAnim();
    //c->StopHintAnim();
    //d->StopHintAnim();
    //e->StopHintAnim();
    //f->StopHintAnim();
    //g->StopHintAnim();
}

void CellSceneBase::clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h,CardSprite* i)
{
    a->m_SrcUsed = false;
    b->m_SrcUsed = false;
    c->m_SrcUsed = false;
    d->m_SrcUsed = false;
    e->m_SrcUsed = false;
    f->m_SrcUsed = false;
    g->m_SrcUsed = false;
    h->m_SrcUsed = false;
    if (i!=nullptr) {
        i->m_SrcUsed = false;
        //i->StopHintAnim();
    }
    //a->getBackground()->runAction(HingAnim());
    //b->getBackground()->runAction(HingAnim());
    //c->getBackground()->runAction(HingAnim());
    //d->getBackground()->runAction(HingAnim());
    //e->getBackground()->runAction(HingAnim());
    //f->getBackground()->runAction(HingAnim());
    //g->getBackground()->runAction(HingAnim());
    //h->getBackground()->runAction(HingAnim());
    //i->getBackground()->runAction(HingAnim());
    
    //a->StopHintAnim();
    //b->StopHintAnim();
    //c->StopHintAnim();
    //d->StopHintAnim();
    //e->StopHintAnim();
    //f->StopHintAnim();
    //g->StopHintAnim();
    //h->StopHintAnim();
}

RepeatForever* CellSceneBase::HingAnim()
{
    Sequence* seq=ActionModule::getInstance()->NewHintAnim(1.0,0.7);
    RepeatForever* rep=RepeatForever::create(seq);
    return rep;
}

bool CellSceneBase::CanLeft()
{
    this->clearUseds(udLeft);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            for (int j=x; j<m_RCount; j++) {//three
                if (checkTerrainCode(udLeft, j, y)!=0) continue;
                if(cardArr[j][y]->getNumber()>0&&!cardArr[j][y]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    for (int k=j+1; k<m_RCount; k++) {
                        if (checkTerrainCode(udLeft, k, y)!=0) break;
                        if(cardArr[k][y]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k+1; m<m_RCount; m++) {
                                if (checkTerrainCode(udLeft, m, y)!=0) break;
                                if(cardArr[m][y]->getNumber()>0)
                                {
                                    jjj=m;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                    }
                    else
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            if(isMerge)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                            }
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                    }
                    if (!isMerge) //one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}

bool CellSceneBase::CanRight()
{
    this->clearUseds(udRight);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = m_RCount-1; x >= 0; x--)
        {
            for (int j=x; j>=0; j--) {//three
                if (checkTerrainCode(udRight, j, y)!=0) continue;
                if(cardArr[j][y]->getNumber()>0&&!cardArr[j][y]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    for (int k=j-1; k>=0; k--) {
                        if (checkTerrainCode(udRight, k, y)!=0) break;
                        if(cardArr[k][y]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k-1; m>=0; m--) {
                                if (checkTerrainCode(udRight, m, y)!=0) break;
                                if(cardArr[m][y]->getNumber()>0)
                                {
                                    jjj=m;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge = false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                    }
                    else
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            if (isMerge) {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                            }
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                    }
                    if (!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
            
        }
    }
    return isMove;
}

bool CellSceneBase::CanUp()
{
    this->clearUseds(udUp);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = m_RCount-1; y >= 0; y--)
        {
            
            for (int j=y; j>=0; j--) {//three
                if (checkTerrainCode(udUp, x, j)!=0) continue;
                if(cardArr[x][j]->getNumber()>0&&!cardArr[x][j]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    for (int k=j-1; k>=0; k--) {
                        if (checkTerrainCode(udUp, x, k)!=0) break;
                        if(cardArr[x][k]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k-1; m>=0; m--) {
                                if (checkTerrainCode(udUp, x, m)!=0) break;
                                if(cardArr[x][m]->getNumber()>0)
                                {
                                    jjj=m;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
                    else
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if(jj>-1&&!isMerge)//two
                        {
                            if(isMerge)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            }
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
                    if (!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}

bool CellSceneBase::CanDown()
{
    this->clearUseds(udDown);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = 0; y <m_RCount; y++)
        {
            for (int j=y; j<m_RCount; j++) {
                if (checkTerrainCode(udDown, x, j)!=0) continue;
                if(cardArr[x][j]->getNumber()>0&&!cardArr[x][j]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    for (int k=j+1; k<m_RCount; k++) {
                        if (checkTerrainCode(udDown, x, k)!=0) break;
                        if(cardArr[x][k]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k+1; m<m_RCount; m++) {
                                if (checkTerrainCode(udDown, x, m)!=0) break;
                                if(cardArr[x][m]->getNumber()>0)
                                {
                                    jjj=m;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            
                        }
                        if (jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
                    else
                    {
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            
                        }
                        if (jj>-1&&!isMerge)//two
                        {
                            if(isMerge)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            }
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
                    if(!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}


CardSprite* CellSceneBase::createCardNumber(int num)
{
    int emptyCount=this->getEmptyCards();
    if (emptyCount==0) {
        CCLOG("GameOver");
        return NULL;
    }
    //srand(time(0));
    int cardnum=num;
    if (cardnum==0) {
        cardnum=RandomNumber();
    }
    int i = CardRandom()%emptyCount;
    m_EmptyList[i]->setNumber(cardnum);
    CallFuncN* func=CallFuncN::create(CC_CALLBACK_1(CellSceneBase::boneAnimDone, this));
    m_EmptyList[i]->swapBuffer(ShowAniTime,func);
    
    if (m_DispData!=nullptr&&!m_DispData->isLeaf()) {
        CardDispManager::getInstance()->resetLeafSetp(m_DispData);
    }
    m_DispData=CardDispManager::getInstance()->AddOneStep();
    this->saveStepData(m_DispData);
    
    //CardDispManager::getInstance()->writeDataList();
    
    return m_EmptyList[i];
}

void CellSceneBase::boneAnimDone(Node* node)
{
    doCheck();
}

bool CellSceneBase::hasEmptyCard()
{
    for (int i=0; i<m_RCount; i++) {
        for (int j=0; j<m_RCount; j++) {
            if (cardArr[i][j]->getNumber()==0&&checkTerrainCode(udAny, i, j)==0) {
                return true;
            }
        }
    }
    return false;
}

unsigned int  CellSceneBase::getEmptyCards()
{
    m_EmptyList.clear();
    for (int i=0; i<m_RCount; i++) {
        for (int j=0; j<m_RCount; j++) {
            if (cardArr[i][j]->getNumber()==0&&checkTerrainCode(udAny, i, j)==0) {
                m_EmptyList.push_back(cardArr[i][j]);
            }
        }
    }
    return (unsigned int)m_EmptyList.size();
}

unsigned int CellSceneBase::getMaxCardNumber()
{
    int number=0;
    for (int i=0; i<m_RCount; i++) {
        for (int j=0; j<m_RCount; j++) {
            if (number<cardArr[i][j]->getNumber()) {
                number=cardArr[i][j]->getNumber();
            }
        }
    }
    return number;
}



bool SceneMoney::isMerge2(CardSprite* a,CardSprite* b)
{
    return CellSceneBase::isMerge12510(a, b);
}

bool SceneMoney::isMerge3(CardSprite* a,CardSprite* b,CardSprite* c)
{
    return CellSceneBase::isMerge12510(a, b, c);
}

bool SceneMoney::CanMerge2(CardSprite* a,CardSprite* b)
{
    return CellSceneBase::isMerge12510(a, b);
}

bool SceneMoney::CanMerge3(CardSprite* a,CardSprite* b,CardSprite* c)
{
    return CellSceneBase::isMerge12510(a, b, c);
}


int SceneMoney::RandomNumber()
{
    NameList* list=CardDispManager::getInstance()->getTransList(this->getNameList());
    if(list!=nullptr)
    {
        int64_t num1=(*list)[0]->Intnum;
        int64_t num2=(*list)[1]->Intnum;
        return CardRandom()%10 < 1 ? num2 : num1;
    }
    else
    {
        return CardRandom()%10 < 1 ? 2 : 1;
    }
}

bool SceneMoney::isNameTrans()
{
    return true;
}

bool SceneMoneyInBa5::isMerge2(CardSprite* a,CardSprite* b)
{
    return CellSceneBase::isMerge12510(a, b);
}

bool SceneMoneyInBa5::isMerge3(CardSprite* a,CardSprite* b,CardSprite* c)
{
    return CellSceneBase::isMerge12510(a, b, c);
}

bool SceneMoneyInBa5::CanMerge2(CardSprite* a,CardSprite* b)
{
    return CellSceneBase::isMerge12510(a, b);
}

bool SceneMoneyInBa5::CanMerge3(CardSprite* a,CardSprite* b,CardSprite* c)
{
    return CellSceneBase::isMerge12510(a, b, c);
}

int SceneMoneyInBa5::RandomNumber()
{
    int a=CardRandom()%10;
    NameList* list=CardDispManager::getInstance()->getTransList(this->getNameList());
    if (list!=nullptr) {
        int64_t num1=(*list)[0]->Intnum;
        int64_t num2=(*list)[1]->Intnum;
        //int64_t num3=(*list)[2]->Intnum;
        //if(a==5)
        //    return num3;
        return a < 2 ? num2 : num1;
    }else
    {
        if(a==5)
            return 5;
        return a < 2 ? 2 : 1;
    }
}

bool SceneMoneyInBa5::isNameTrans()
{
    return true;
}

bool SceneMoneyInBa5::CanLeft()
{
    this->clearUseds(udLeft);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = 0; x < m_RCount; x++)
        {
            for (int j=x; j<m_RCount; j++) {//three
                if (checkTerrainCode(udLeft, j, y)!=0) continue;
                if(cardArr[j][y]->getNumber()>0&&!cardArr[j][y]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    int j4=-1;
                    int j5=-1;
                    int j6=-1;
                    int j7=-1;
                    int j8=-1;
                    int j9=-1;
                    for (int k=j+1; k<m_RCount; k++) {
                        if (checkTerrainCode(udLeft, k, y)!=0) break;
                        if(cardArr[k][y]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k+1; m<m_RCount; m++) {
                                if (checkTerrainCode(udLeft, m, y)!=0) break;
                                if(cardArr[m][y]->getNumber()>0)
                                {
                                    jjj=m;
                                    for (int l=m+1; l<m_RCount; l++) {
                                        if (checkTerrainCode(udLeft, l, y)!=0) break;
                                        if (cardArr[l][y]->getNumber()>0) {
                                            j4=l;
                                            for (int p=l+1; p<m_RCount; p++) {
                                                if (checkTerrainCode(udLeft, p, y)!=0) break;
                                                if (cardArr[p][y]->getNumber()>0) {
                                                    j5=p;/*
                                                    for (int p6=p+1; p6<m_RCount; p++) {
                                                        if (cardArr[p6][y]->getNumber()>0) {
                                                            j6=p6;
                                                            for (int p7=p6+1; p7<m_RCount; p++) {
                                                                if (cardArr[p7][y]->getNumber()>0) {
                                                                    j7=p7;
                                                                    for (int p8=p7+1; p8<m_RCount; p++) {
                                                                        if (cardArr[p8][y]->getNumber()>0) {
                                                                            j8=p8;
                                                                            for (int p9=p8+1; p9<m_RCount; p++) {
                                                                                if (cardArr[p9][y]->getNumber()>0) {
                                                                                    j9=p9;
                                                                                    break;
                                                                                }
                                                                            }
                                                                            break;
                                                                        }
                                                                    }
                                                                    break;
                                                                }
                                                            }
                                                            break;
                                                        }
                                                    }*/
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                    
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1&&!isMerge)//9
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&!isMerge)//8
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&!isMerge)//7
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&!isMerge)//6
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&!isMerge)//5
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&!isMerge)//4
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                        }
                        else if (jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        else if (jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                    }
                    else
                    {
                        
                        int mergecount=0;
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1)//9
                        {
                            mergecount=9;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1)//8
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            mergecount=8;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1)//7
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            mergecount=7;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1)//6
                        {
                            
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            mergecount=6;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1)//5
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            mergecount=5;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1)//4
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            mergecount=4;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                        }
                        if (jj>-1&&jjj>-1)//three
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                            }
                            mergecount=3;
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1)//two
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                            }
                            else if(mergecount==3)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                            }
                            mergecount=2;
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                        
                        /*
                        if (jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        else if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                        */
                    }
                    if (!isMerge) //one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}

bool SceneMoneyInBa5::CanRight()
{
    this->clearUseds(udRight);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int y = 0; y < m_RCount; y++)
    {
        for (int x = m_RCount-1; x >= 0; x--)
        {
            
            for (int j=x; j>=0; j--) {//three
                if (checkTerrainCode(udLeft, j, y)!=0) continue;
                if(cardArr[j][y]->getNumber()>0&&!cardArr[j][y]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    int j4=-1;
                    int j5=-1;
                    int j6=-1;
                    int j7=-1;
                    int j8=-1;
                    int j9=-1;
                    for (int k=j-1; k>=0; k--) {
                        if (checkTerrainCode(udRight, k, y)!=0) break;
                        if(cardArr[k][y]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k-1; m>=0; m--) {
                                if (checkTerrainCode(udRight, m, y)!=0) break;
                                if(cardArr[m][y]->getNumber()>0)
                                {
                                    jjj=m;
                                    for (int l=m-1; l>=0; l--) {
                                        if (checkTerrainCode(udRight, l, y)!=0) break;
                                        if (cardArr[l][y]->getNumber()>0) {
                                            j4=l;
                                            for (int p=l-1; p>=0; p--) {
                                                if (checkTerrainCode(udRight, p, y)!=0) break;
                                                if (cardArr[p][y]->getNumber()>0) {
                                                    j5=p;/*
                                                    for (int p6=p-1; p6>=0; p--) {
                                                        if (cardArr[p6][y]->getNumber()>0) {
                                                            j6=p6;
                                                            for (int p7=p6-1; p7>=0; p--) {
                                                                if (cardArr[p7][y]->getNumber()>0) {
                                                                    j7=p7;
                                                                    for (int p8=p7-1; p8>=0; p--) {
                                                                        if (cardArr[p8][y]->getNumber()>0) {
                                                                            j8=p8;
                                                                            for (int p9=p8-1; p9>=0; p--) {
                                                                                if (cardArr[p9][y]->getNumber()>0) {
                                                                                    j9=p9;
                                                  
                                                                                    break;
                                                                                }
                                                                            }
                                                                            break;
                                                                        }
                                                                    }
                                                                    break;
                                                                }
                                                            }
                                                            break;
                                                        }
                                                    }*/
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge = false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1&&!isMerge)//9
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&!isMerge)//8
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&!isMerge)//7
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&!isMerge)//6
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&!isMerge)//5
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&!isMerge)//4
                        {
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                        }
                        else if (jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        else if (jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }

                    }
                    else
                    {
                        
                        int mergecount=0;
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1)//9
                        {
                            mergecount=9;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1)//8
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            mergecount=8;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1)//7
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            mergecount=7;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1)//6
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if (mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            mergecount=6;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1)//5
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if (mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if (mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            mergecount=5;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1)//4
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if (mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if (mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if (mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            mergecount=4;
                            isMerge=isMerge12510(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                        }
                        if(jj>-1&&jjj>-1)//three
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if (mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if (mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if (mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            else if (mergecount==4)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                            }
                            mergecount=3;
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        if(jj>-1)//two
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y],cardArr[j9][y]);
                            }
                            else if (mergecount==8)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y],cardArr[j8][y]);
                            }
                            else if (mergecount==7)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y],cardArr[j7][y]);
                            }
                            else if (mergecount==6)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y],cardArr[j6][y]);
                            }
                            else if (mergecount==5)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y],cardArr[j5][y]);
                            }
                            else if (mergecount==4)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y],cardArr[j4][y]);
                            }
                            else if (mergecount==3)
                            {
                                this->clearUsed(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                            }
                            mergecount=2;
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                        /*
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[j][y], cardArr[jj][y], cardArr[jjj][y]);
                        }
                        else if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[j][y], cardArr[jj][y]);
                        }
                        */
                    }
                    if (!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
            
        }
    }
    return isMove;
}

bool SceneMoneyInBa5::CanUp()
{
    this->clearUseds(udUp);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = m_RCount-1; y >= 0; y--)
        {
            
            for (int j=y; j>=0; j--) {//three
                if (checkTerrainCode(udLeft, x, j)!=0) continue;
                if(cardArr[x][j]->getNumber()>0&&!cardArr[x][j]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    int j4=-1;
                    int j5=-1;
                    int j6=-1;
                    int j7=-1;
                    int j8=-1;
                    int j9=-1;
                    for (int k=j-1; k>=0; k--) {
                        if (checkTerrainCode(udUp, x, k)!=0) break;
                        if(cardArr[x][k]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k-1; m>=0; m--) {
                                if (checkTerrainCode(udUp, x, m)!=0) break;
                                if(cardArr[x][m]->getNumber()>0)
                                {
                                    jjj=m;
                                    
                                    for (int l=m-1; l>=0; l--) {
                                        if (checkTerrainCode(udUp, x, l)!=0) break;
                                        if (cardArr[x][l]->getNumber()>0) {
                                            j4=l;
                                            for (int p=l-1; p>=0; p--) {
                                                if (checkTerrainCode(udUp, x, p)!=0) break;
                                                if (cardArr[x][p]->getNumber()>0) {
                                                    j5=p;/*
                                                    for (int p6=p-1; p6>=0; p--) {
                                                        if (cardArr[x][p6]->getNumber()>0) {
                                                            j6=p6;
                                                            for (int p7=p6-1; p7>=0; p--) {
                                                                if (cardArr[x][p7]->getNumber()>0) {
                                                                    j7=p7;
                                                                    for (int p8=p7-1; p8>=0; p--) {
                                                                        if (cardArr[x][p8]->getNumber()>0) {
                                                                            j8=p8;
                                                                            for (int p9=p8-1; p9>=0; p--) {
                                                                                if (cardArr[x][p9]->getNumber()>0) {
                                                                                    j9=p9;
                                                  
                                                                                    break;
                                                                                }
                                                                            }
                                                                            break;
                                                                        }
                                                                    }
                                                                    break;
                                                                }
                                                            }
                                                            break;
                                                        }
                                                    }*/
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1&&!isMerge)//9
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&!isMerge)//8
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&!isMerge)//7
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&!isMerge)//6
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&!isMerge)//5
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&!isMerge)//4
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                        }
                        else if (jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        else if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
                    else
                    {
                        
                        int mergecount=0;
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1)//9
                        {
                            mergecount=9;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1)//8
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            mergecount=8;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1)//7
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            mergecount=7;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1)//6
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            mergecount=6;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1)//5
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            mergecount=5;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                        }
                        if (jj>-1&&jjj>-1&&j4>-1)//4
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            mergecount=4;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                        }
                        if (jj>-1&&jjj>-1)//three
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                            }
                            mergecount=3;
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if(jj>-1)//two
                        {
                            if(mergecount==9)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                            }
                            else if(mergecount==3)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            }
                            mergecount=2;
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                        /*
                        if (jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        else if(jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                        */
                    }
                    if (!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}

bool SceneMoneyInBa5::CanDown()
{
    this->clearUseds(udDown);
    m_DispData->clearMergeList(&m_MergeList);
    bool isMove=false;
    for (int x = 0; x < m_RCount; x++)
    {
        for (int y = 0; y <m_RCount; y++)
        {
            for (int j=y; j<m_RCount; j++) {
                if (checkTerrainCode(udLeft, x, j)!=0) continue;
                if(cardArr[x][j]->getNumber()>0&&!cardArr[x][j]->m_SrcUsed)
                {
                    int jj=-1;
                    int jjj=-1;
                    int j4=-1;
                    int j5=-1;
                    int j6=-1;
                    int j7=-1;
                    int j8=-1;
                    int j9=-1;
                    for (int k=j+1; k<m_RCount; k++) {
                        if (checkTerrainCode(udDown, x, k)!=0) break;
                        if(cardArr[x][k]->getNumber()>0)
                        {
                            jj=k;
                            for (int m=k+1; m<m_RCount; m++) {
                                if (checkTerrainCode(udDown, x, m)!=0) break;
                                if(cardArr[x][m]->getNumber()>0)
                                {
                                    jjj=m;
                                    for (int l=m+1; l<m_RCount; l++) {
                                        if (checkTerrainCode(udDown, x, l)!=0) break;
                                        if (cardArr[x][l]->getNumber()>0) {
                                            j4=l;
                                            for (int p=l+1; p<m_RCount; p++) {
                                                if (checkTerrainCode(udDown, x, p)!=0) break;
                                                if (cardArr[x][p]->getNumber()>0) {
                                                    j5=p;/*
                                                    for (int p6=p+1; p6<m_RCount; p++) {
                                                        if (cardArr[x][p6]->getNumber()>0) {
                                                            j6=p6;
                                                            for (int p7=p6+1; p7<m_RCount; p++) {
                                                                if (cardArr[x][p7]->getNumber()>0) {
                                                                    j7=p7;
                                                                    for (int p8=p7+1; p8<m_RCount; p++) {
                                                                        if (cardArr[x][p8]->getNumber()>0) {
                                                                            j8=p8;
                                                                            for (int p9=p8+1; p9<m_RCount; p++) {
                                                                                if (cardArr[x][p9]->getNumber()>0) {
                                                                                    j9=p9;
                                                  
                                                                                    break;
                                                                                }
                                                                            }
                                                                            break;
                                                                        }
                                                                    }
                                                                    break;
                                                                }
                                                            }
                                                            break;
                                                        }
                                                    }*/
                                                    break;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool isMerge=false;
                    if(CardDispManager::getInstance()->getPriorType()==0)
                    {
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1&&!isMerge)//9
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&!isMerge)//8
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&!isMerge)//7
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&!isMerge)//6
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&!isMerge)//5
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&!isMerge)//4
                        {
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                        }
                        else if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if (jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                    }
					else
                    {
                        int mergecount=0;
                        if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1&&j9>-1)//9
                        {
                            mergecount=9;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1&&j8>-1)//8
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            mergecount=8;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1&&j7>-1)//7
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            mergecount=7;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1&&j6>-1)//6
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            mergecount=6;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1&&j5>-1)//5
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            mergecount=5;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                        }
                        else if (jj>-1&&jjj>-1&&j4>-1)//4
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            mergecount=4;
                            isMerge=isMerge12510(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                        }
                        else if(jj>-1&&jjj>-1)//three
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                            }
                            mergecount=3;
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if (jj>-1)//two
                        {
                            if (mergecount==9) {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8], cardArr[x][j9]);
                            }
                            else if(mergecount==8)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7], cardArr[x][j8]);
                            }
                            else if(mergecount==7)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6], cardArr[x][j7]);
                            }
                            else if(mergecount==6)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5], cardArr[x][j6]);
                            }
                            else if(mergecount==5)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4], cardArr[x][j5]);
                            }
                            else if(mergecount==4)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj], cardArr[x][j4]);
                            }
                            else if(mergecount==3)
                            {
                                this->clearUsed(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                            }
                            mergecount=2;
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                        /*
                        if(jj>-1&&jjj>-1&&!isMerge)//three
                        {
                            isMerge=CanMerge3(cardArr[x][j], cardArr[x][jj], cardArr[x][jjj]);
                        }
                        if (jj>-1&&!isMerge)//two
                        {
                            isMerge=CanMerge2(cardArr[x][j], cardArr[x][jj]);
                        }
                        */
                    }
                    if(!isMerge)//one
                    {
                        
                    }else
                    {
                        isMove=true;
                    }
                }
            }
        }
    }
    return isMove;
}



