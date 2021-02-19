#include "CardLayer.h"
#include "CardData.h"
#include "cnTool.h"
#include "ActionModule.h"

USING_NS_CC;

CardSprite* CardSprite::createCardSprite(int number, int wight, int height, float CardSpriteX, float CardSpriteY, int aliasi, bool isTrans, string Trans)
{
	//这里是创建一个自定义精灵类，格式很规范化
	CardSprite *card = new CardSprite();
	if (card && card->init())
	{
		card->autorelease();
		card->spriteInit(number,wight,height,CardSpriteX,CardSpriteY,isTrans,Trans,aliasi);
		return card;
	}
	CC_SAFE_DELETE(card);

	return NULL;
}

CardSprite::~CardSprite()
{
    this->RemoveToch();
}

//这里也是规范化
bool CardSprite::init()
{
	if (!Layer::init())
	{
		return false;
	}
    m_isTerrain = false;
    m_IsBackColor = true;
	return true;
}

int64_t CardSprite::getOldNumber()
{
    return OldNumber;
}

void CardSprite::setOldNumber(int64_t num)
{
    OldNumber=num;
}

int64_t CardSprite::getDispNumber()
{
    return  DispNumber;
}

Color3B CardSprite::GetNumberColor(int64_t num)
{
    return Color3B(0,0,255);
}

int CardSprite::GetStringFontSizeExt(string name, int cell_width)
{
	int len = cnTools::GetStringWidth(name);
	float fsize = GetStringFontSize(name,cell_width);
	if (len > 0)
	{
		int wlen = len / 2 + len % 2;
		float ww = sqrt(wlen);
		int w = ceil(ww);
		fsize = ((float)cell_width / (w*1.15));
		if (w < 3)
		{
			fsize = fsize*1.1;
		}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        fsize = fsize*0.8;
#endif
        
	}
	return fsize;
}

int CardSprite::GetStringFontSize(string name,int cell_width)
{
    int len=cnTools::GetStringWidth(name);
    int fsize = 0;
    switch (len) {
        case 1:
            fsize=100;
        break;
        case 2:
            fsize=85;
        break;
        case 3:
            fsize=75;
        break;
        case 4:
            fsize=60;
        break;
        case 5:
            fsize=45;
        break;
        case 6:
            fsize=35;
        break;
        case 7:
            fsize=30;
        break;
        case 8:
            fsize=25;
        break;
        default:
            fsize=20;
        break;
    }
    fsize=fsize*((float)cell_width/140.f);
    return fsize;
}

Color3B CardSprite::GetBackColor(int64_t num)
{
    //判断数字的大小来调整颜色
    if(num == 0)
    {
        return Color3B(200,190,180);
    }
    else if (abs(num)>=100000000)
    {
        return Color3B(130,0,0);
    }
    else
    {
        switch (abs(num)) {
            case 2:
                return Color3B(240,130,220);
                break;
            case 4:
                return Color3B(140,230,180);
                break;
            case 8:
                return Color3B(240,230,140);
                break;
            case 16:
                return Color3B(140,230,100);
                break;
            case 32:
                return Color3B(240,230,60);
                break;
            case 64:
                return Color3B(140,230,40);
                break;
            case 128:
                return Color3B(140,130,0);
                break;
            case 256:
                return Color3B(240,190,120);
                break;
            case 512:
                return Color3B(140,150,220);
                break;
            case 1024:
                return Color3B(240,110,220);
                break;
            case 2048:
                return Color3B(140,70,220);
                break;
            case 4096:
                return Color3B(240,30,220);
                break;
            case 8192:
                return Color3B(140,0,220);
                break;
            case 16384:
                return Color3B(240,230,120);
                break;
            case 16384*2:
                return Color3B(200,130,220);
                break;
            case 16384*4:
                return Color3B(160,230,220);
                break;
            case 16384*8:
                return Color3B(120,130,220);
                break;
            case 16384*16:
                return Color3B(80,230,220);
                break;
            case 1:
                return Color3B(40,230,220);
                break;
            case 5:
                return Color3B(0,0,220);
                break;
            case 10:
                return Color3B(240,130,120);
                break;
            case 20:
                return Color3B(100,230,180);
                break;
            case 50:
                return Color3B(160,130,220);
                break;
            case 100:
                return Color3B(120,230,220);
                break;
            case 200:
                return Color3B(80,130,220);
                break;
            case 500:
                return Color3B(40,230,120);
                break;
            case 1000:
                return Color3B(0,130,220);
                break;
            case 2000:
                return Color3B(240,230,120);
                break;
            case 5000:
                return Color3B(200,130,120);
                break;
            case 10000:
                return Color3B(160,230,120);
                break;
            case 20000:
                return Color3B(120,130,120);
                break;
            case 50000:
                return Color3B(80,230,120);
                break;
            case 100000:
                return Color3B(40,130,120);
                break;
            case 200000:
                return Color3B(0,230,120);
                break;
            case 500000:
                return Color3B(120,200,120);
                break;
            case 1000000:
                return Color3B(120,180,220);
                break;
            case 2000000:
                return Color3B(120,160,120);
                break;
            case 5000000:
                return Color3B(120,140,220);
                break;
            case 10000000:
                return Color3B(120,120,120);
                break;
            case 20000000:
                return Color3B(120,100,220);
                break;
            case 50000000:
                return Color3B(120,80,120);
                break;
            case 100000000:
                return Color3B(120,60,220);
                break;
            case 200000000:
                return Color3B(120,40,120);
                break;
            case 500000000:
                return Color3B(120,20,220);
                break;
            case 1000000000:
                return Color3B(120,0,120);
                break;
            case 2000000000:
                return Color3B(120,0,60);
                break;
            default:
                return Color3B(0,88,88);
                break;
        }
    }
}

void CardSprite::ShowHintAnim()
{
    
    if (m_Sprite!=nullptr&&m_Sprite->isVisible()) {
        Sequence* seq=ActionModule::getInstance()->NewHintAnim(1.0,0.85*m_Sprite->getScale(),nullptr,m_Sprite->getScale());
        RepeatForever* rep=RepeatForever::create(seq);
        m_Sprite->runAction(rep);
    }
    else
    {
        Sequence* seq=ActionModule::getInstance()->NewHintAnim(1.0,0.85);
        RepeatForever* rep=RepeatForever::create(seq);
        colorBackground->runAction(rep);
    }
}

void CardSprite::StopHintAnim()
{
    if (m_Sprite!=nullptr&&m_Sprite->isVisible()) {
        m_Sprite->stopAllActions();
        m_Sprite->setScale(m_SpriteScale);
    }else
    {
        if(colorBackground!=nullptr)
        {
            colorBackground->stopAllActions();
            colorBackground->setScale(1.0f);
        }
    }
}

void CardSprite::clearTerrain()
{
    m_Number=0;
    m_DispStr="";
}


Sprite* CardSprite::createPicture(TransNumber* number, float *scale)
{
	Sprite* sprite = nullptr;
	if (this->isPicSprite(number))
	{
		sprite = Sprite::create();
// 		SpriteFrame* frame=SpriteFrameCache::getInstance()->getSpriteFrameByName(number->picture);
// 		if (frame != nullptr)
// 		{
// 			sprite->initWithSpriteFrame(frame->clone());
// 		}
// 		else
// 		{
// 			sprite->setTexture(number->picture);
// 		}
		sprite->setTexture(number->picture);
		*scale = this->adapterCoord(sprite);
	}
	return sprite;
}


float CardSprite::adapterCoord(Sprite* sprite)
{
	Size size = sprite->getContentSize();
	float scalex = m_Width / size.width;
	float scaley = m_Height / size.height;
	float scale = 0;
	if (scalex < scaley)
	{
		scale = scalex;
	}
	else
	{
		scale = scaley;
	}
	sprite->setPosition(m_Width / 2, m_Height / 2);
	sprite->setScale(scale);
	return scale;
}

bool CardSprite::isPicSprite(TransNumber* number)
{
	if (number!=nullptr&&FileUtils::getInstance()->isFileExist(number->picture))
	{
		return true;
	}
	return false;
}


void CardSprite::cardBlendActionFade(CardSprite* destcard, float delta)
{
	TransNumber* tnum = CardDispManager::getInstance()->getTransNumber(m_Trans, this->getOldNumber());
	TransNumber* dnum = CardDispManager::getInstance()->getTransNumber(m_Trans, destcard->getNumber());
	Color3B dstcolor = CardSprite::GetBackColor(destcard->getNumber());
	bool isSprite = this->isPicSprite(tnum);
	bool dstIsSprite = destcard->isPicSprite(dnum);
	float scale;
	if (isSprite)
	{
		if (dstIsSprite)
		{
			Sprite* dsts = this->createPicture(dnum, &scale);
			this->addChild(dsts, 6);
			dsts->runAction(Sequence::create(FadeIn::create(delta), RemoveSelf::create(true), nullptr));
			this->getSprite()->runAction(FadeOut::create(delta));
		}
		else
		{
			Sprite* dsts = this->createPicture(dnum, &scale);
			this->addChild(dsts, 6);
			dsts->runAction(Sequence::create(FadeIn::create(delta), RemoveSelf::create(true), nullptr));
			this->getSprite()->runAction(FadeOut::create(delta));

			destcard->getBackground()->runAction(TintTo::create(delta, dstcolor));
		}
	}
	else
	{
		if (dstIsSprite)
		{
			Sprite* dsts = this->createPicture(dnum, &scale);
			destcard->addChild(dsts, 6);

			Sprite* sprite = destcard->createPicture(dnum, &scale);
			destcard->addChild(sprite, 5);
			destcard->m_Sprite = sprite;
			destcard->m_SpriteScale = scale;

			destcard->getSprite()->runAction(FadeOut::create(delta));
			dsts->runAction(Sequence::create(FadeIn::create(delta), RemoveSelf::create(true), nullptr));

			this->getBackground()->runAction(TintTo::create(delta, dstcolor));
		}
		else
		{
			destcard->getBackground()->runAction(TintTo::create(delta, dstcolor));
			this->getBackground()->runAction(TintTo::create(delta, dstcolor));
		}
	}
	this->runAction(Sequence::create(MoveTo::create(delta, destcard->GetCardPostion()), nullptr));
}

void CardSprite::removeTemp()
{
	if (m_Sprite)
	{
		m_Sprite->removeFromParentAndCleanup(true);
		m_Sprite = nullptr;
	}
	Vector<Node*> childern = this->getChildren();
	for (int i = 0; i < childern.size();i++)
	{
		CCLOG("Tag %d", childern.at(i)->getTag());
		if (childern.at(i)->getTag() > 1000)
		{
			childern.at(i)->removeFromParentAndCleanup(true);
		}
	}	
}

void CardSprite::removeSprite(Sprite* b)
{
	//b->removeFromParent();
	Vector<Node*> childern=this->getChildren();
	Vector<Node*>::iterator it = childern.begin();
	while (it!=childern.end())
	{
		if ((*it)->getTag() > 1000)
		{
			(*it)->removeFromParentAndCleanup(true);
		}
		it++;
	}
}

void CardSprite::cardBlendActionShader(CardSprite* destcard, float delta)
{
	TransNumber* tnum = CardDispManager::getInstance()->getTransNumber(m_Trans, this->getOldNumber());
	TransNumber* dnum = CardDispManager::getInstance()->getTransNumber(m_Trans, destcard->getNumber());
	Color3B dstcolor = CardSprite::GetBackColor(destcard->getNumber());
	Color4F color(dstcolor.r / 255.0, dstcolor.g / 255.0, dstcolor.b / 255.0, 1);
	Color3B srccolor = CardSprite::GetBackColor(this->getOldNumber());
	Color4F scolor(srccolor.r / 255.0, srccolor.g / 255.0, srccolor.b / 255.0, 1);

	bool isSprite = this->isPicSprite(tnum);
	bool dstIsSprite = destcard->isPicSprite(dnum);
	float scale;
	if (isSprite)
	{
		if (dstIsSprite)
		{
			Sprite* dsts = this->createPicture(dnum, &scale);
			dsts->setTag(12532);
			CallFunc* func = CallFunc::create(CC_CALLBACK_0(CardSprite::removeSprite, this, dsts));
			this->getSprite()->runAction(Sequence::createWithTwoActions(TextureTo::create(delta,dsts),func));
		}
		else
		{
			//Sprite* dsts = Sprite::create("res/coin/alpha.png");
			//this->adapterCoord(dsts);
			
			//DrawNode* draw = DrawNode::create();
			//Vec2 posstart(this->getPixelsPosX(), this->getPixelsPosY());
			//Vec2 posend(this->getPixelsPosX()+m_Width, this->getPixelsPosY()+m_Height);
			//draw->drawSolidRect(posstart, posend, color);

			this->getSprite()->runAction(TextureToColor::create(delta, color));

			//this->getBackground()->runAction(TintTo::create(delta, dstcolor));
			//destcard->getBackground()->runAction(TintTo::create(delta,dstcolor));
		}
	}
	else
	{
		if (dstIsSprite)
		{
			//Sprite* srcs = Sprite::create("res/coin/alpha.png");
			//this->addChild(srcs, 6);
			//this->adapterCoord(srcs);
			
			//DrawNode* draw = DrawNode::create();
			//Vec2 posstart(this->getPixelsPosX(), this->getPixelsPosY());
			//Vec2 posend(this->getPixelsPosX() + m_Width, this->getPixelsPosY() + m_Height);
			//draw->drawSolidRect(posstart, posend, color);
			Sprite* dsts = Sprite::create();
// 			SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(dnum->picture);
// 			if (frame != nullptr)
// 			{
// 				dsts->initWithSpriteFrame(frame->clone());
// 			}
// 			else
// 			{
// 				dsts->setTexture(dnum->picture);
// 			}
			dsts->setTexture(dnum->picture);
			dsts->setTag(12532);
			this->adapterCoord(dsts);
			this->addChild(dsts, 5);
			CallFunc* func = CallFunc::create(CC_CALLBACK_0(CardSprite::removeSprite, this, dsts));
			dsts->runAction(Sequence::createWithTwoActions(ColorToTexture::create(delta, color),func));

			//this->getBackground()->runAction(TintTo::create(delta,dstcolor));
		}
		else
		{
			//destcard->getBackground()->runAction(TintTo::create(delta, dstcolor));
			//this->getBackground()->runAction(TintTo::create(delta, dstcolor));
			std::string alphafile = "res/coin/alpha.png";
			Sprite* srcs = Sprite::create();
// 			SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(alphafile);
// 			if (frame != nullptr)
// 			{
// 				srcs->initWithSpriteFrame(frame->clone());
// 			}
// 			else
// 			{
// 				srcs->setTexture(alphafile);
// 			}
			srcs->setTexture(alphafile);
			this->addChild(srcs, 6);
			this->adapterCoord(srcs);
			srcs->setTag(12532);
			CallFunc* func = CallFunc::create(CC_CALLBACK_0(CardSprite::removeSprite, this, srcs));
			srcs->runAction(Sequence::createWithTwoActions(ColorToColor::create(delta, color, scolor), RemoveSelf::create(true)));
		}
	}
	//destcard->runAction(Sequence::create(DelayTime::create(delta), Show::create(), nullptr));
}

void CardSprite::dispPicture(TransNumber* number)
{
	if (this->isPicSprite(number))
	{
		if (m_Sprite == nullptr) {
			float scale = 1.0;
			m_Sprite = this->createPicture(number, &scale);
			m_SpriteScale = scale;
			this->addChild(m_Sprite, 5);
		}
		m_Sprite->setTexture(number->picture);
		this->adapterCoord(m_Sprite);
		m_Sprite->setVisible(true);
		labelCardNumber->setColor(Color3B::GREEN);
		labelCardNumber->setVisible(false);
		colorBackground->setVisible(false);
	}
	else
	{
		labelCardNumber->setColor(Color3B::WHITE);
		labelCardNumber->setVisible(true);
		this->hidePicture();
	}
}

void CardSprite::hidePicture()
{
    if(m_Sprite!=nullptr)
    {
        m_Sprite->setVisible(false);
    }
	colorBackground->setVisible(true);
	labelCardNumber->setVisible(true);
}

void CardSprite::setDispNumber(int64_t num)
{
    DispNumber = num;
    //更新显示的数字
    if (m_isTerrain)//terrain display
    {
		int index = -1;
		m_DispStr = CardDispManager::getInstance()->getTransName("terrain", DispNumber, &index);
		CCLOG("Terrain DispNumber:%s,%d", m_DispStr.c_str(), DispNumber);
        labelCardNumber->setString(m_DispStr);
        colorBackground->setColor(Color3B::BLACK);
        this->hidePicture();
    }else
    {
        if(DispNumber==0)
        {
            this->hidePicture();
            labelCardNumber->setString("");
            m_DispStr = "";
            colorBackground->setColor(Color3B(200, 190, 180));
        }
        else
        {
            string dispstr;
            if (!m_IsTrans) {
                char s[64];
                sprintf(s,"%lld",DispNumber);
                string str=string(s);
                dispstr=str;
            }
            else
            {
                dispstr=CardDispManager::getInstance()->getTransName(m_Trans, DispNumber,&m_AliasIndex);
            }
            m_DispStr = dispstr;
            labelCardNumber->setString(dispstr);
            TransNumber* tnum=CardDispManager::getInstance()->getTransNumber(m_Trans, DispNumber);
            this->dispPicture(tnum);
        }
		
		TransNumber* tnum = CardDispManager::getInstance()->getTransNumber(m_Trans, DispNumber);
		labelMax->setVisible(false);
		if (tnum!=nullptr&&tnum->Intnum == tnum->Max)
		{
			labelMax->setVisible(true);
		}

        if (m_IsBackColor) {
            colorBackground->setColor(CardSprite::GetBackColor(DispNumber));
        }
    }
	Size size=labelCardNumber->getDimensions();

	labelCardNumber->setSystemFontSize(GetStringFontSizeExt(m_DispStr, m_Width));

}

//获取数据
int64_t CardSprite::getNumber()
{
	return m_Number;
}

//设置数据
void CardSprite::setNumber(int64_t num)
{
	m_Number = num;
}

void CardSprite::swapBuffer(float livets,CallFuncN* done)
{
    this->setDispNumber(getNumber());
    if (livets>0.01) {
        colorBackground->setScale(0.1);
        labelCardNumber->setScale(0.1);
        float timets=livets;
        if(timets<=0)
            timets=0.5;
        
        Sequence* seq=ActionModule::getInstance()->NewBornAnim(timets);
        labelCardNumber->runAction(seq);
        if (m_Sprite!=nullptr&&m_Sprite->isVisible()) {
            seq=ActionModule::getInstance()->NewBornAnim(timets,done,m_Sprite->getScale());
            m_Sprite->runAction(seq);
        }
        else
        {
            seq=ActionModule::getInstance()->NewBornAnim(timets,done);
            colorBackground->runAction(seq);
        }
		
        //this->setScale(0.1);
        //this->runAction(ScaleTo::create(0.5, 1));
    }
}

Vec2 CardSprite::GetCardPostion()
{
    return Point(m_PixelPosX,m_PixelPosY);// this->getPosition();
}

bool CardSprite::addSrcPos(int x,int y)
{
    if (IsInSrcPos(x, y)) {
        CCLOG("Pos already in list %d %d",x,y);
        return false;
    }
    m_SrcPos.push_back(Vec2(x,y));
    return true;
}

bool CardSprite::IsInSrcPos(int x,int y)
{
    for (int i=0; i<m_SrcPos.size(); i++) {
        int xx=m_SrcPos[i].x;
        int yy=m_SrcPos[i].y;
        if (xx==x&&yy==y) {
            return true;
        }
    }
    return false;
}


int CardSprite::getIndex(int rowcount)
{
	return m_GridPosY*rowcount + m_GridPosX;
}

void CardSprite::setGridPos(int x, int y)
{
    m_GridPosX = x;
    m_GridPosY = y;
}

int CardSprite::getGridPosX()
{
    return  m_GridPosX;
}

int CardSprite::getGridPosY()
{
    return m_GridPosY;
}

LayerColor* CardSprite::getBackground()
{
    return colorBackground;
}

//初始化
void CardSprite::spriteInit(int64_t number, int wight, int height, float CardSpriteX, float CardSpriteY, bool isTrans, string Trans, int aliasi)
{
	//初始化数字
    m_Sprite = nullptr;
	OldNumber = number;
	m_Number = number;
    m_IsTrans = isTrans;
    m_Trans = Trans;
	m_AliasIndex = aliasi;
    touchListener = nullptr;
    m_PixelPosX = CardSpriteX;
    m_PixelPosY = CardSpriteY;
    //this->AddTouch();
	m_DispStr = CardDispManager::getInstance()->getTransName(m_Trans, m_Number, &m_AliasIndex);
	//加入卡片背景颜色
    int border = wight/40;
    m_Width = wight;
    m_Height = height;
    this->setPosition(Vec2(CardSpriteX,CardSpriteY));
    this->setContentSize(Size(wight,height));
    
    m_Rect = Rect(CardSpriteX+border, CardSpriteY+border, wight-border*2, height-border*2);
    
    LayerColor * broder=LayerColor::create(Color4B(200,190,180,255),wight-border*2,height-border*2);
    this->addChild(broder);
    broder->setPosition(Vec2(border,border));
    
    colorBackground = LayerColor::create(Color4B(200,190,180,255),wight-border*2,height-border*2);
	colorBackground->setPosition(Vec2(border,border));
    
    this->addChild(colorBackground);
    
    labelCardNumber = Label::create();
    labelCardNumber->setPosition(Point(colorBackground->getContentSize().width/2, colorBackground->getContentSize().height/2));
    labelCardNumber->setTag(8);
	labelCardNumber->setDimensions(wight - border * 2, height - border * 2);
	labelCardNumber->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    labelCardNumber->setSystemFontName("Helvetica-Bold");
    labelCardNumber->setSystemFontSize(100);
    //labelCardNumber->setVerticalAlignment(TextVAlignment::CENTER);
    this->addChild(labelCardNumber,10000);

	labelMax = Label::create();
	labelMax->setString("max");
	labelMax->setSystemFontSize(20);
	labelMax->setPosition(20, 10);
	this->addChild(labelMax, 20);
	labelMax->setVisible(false);
	labelMax->setSystemFontName("Helvetica-Bold");
	labelMax->setColor(Color3B::RED);

    this->afterInit(m_Number, m_Width, m_Height, CardSpriteX, CardSpriteY);
}

void CardSprite::AddTouch()
{
    //add touch listener
    if (touchListener==nullptr) {
        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(CardSprite::onTouchBegan, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(CardSprite::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
        //_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 100);
    }
}
void CardSprite::RemoveToch()
{
    _eventDispatcher->removeEventListener(touchListener);
    touchListener=nullptr;
}

bool CardSprite::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (m_Rect.containsPoint(touch->getLocation())) {
        //CCLOG("Touch Sprite Began %s",m_DispStr.c_str());
        touchListener->setSwallowTouches(true);
    }else
    {
        touchListener->setSwallowTouches(false);
    }
    return true;
}

void CardSprite::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    if (m_Rect.containsPoint(touch->getLocation())) {
        //CCLOG("Touch Sprite Ended %s",m_DispStr.c_str());
    }
}

bool CardButton::init()
{
    if (!CardSprite::init())
    {
        return false;
    }
    return true;
}

Button* CardButton::getCardBtn()
{
    return m_CardBtn;
}

void CardButton::setText(string txt)
{
    m_DispStr = txt;
    m_CardBtn->setTitleText(txt);
}

string CardButton::getText()
{
    return m_DispStr;
}

CardButton* CardButton::createCardSprite(int num, int wight, int height, float CardSpriteX, float CardSpriteY,bool isTrans,string Trans)
{
    CardButton *button = new CardButton();
    if (button && button->init())
    {
        button->autorelease();
        button->spriteInit(num,wight,height,CardSpriteX,CardSpriteY,isTrans,Trans);
        return button;
    }
    CC_SAFE_DELETE(button);
    
    return NULL;
}

void CardButton::afterInit(int64_t number, int wight, int height, float CardSpriteX, float CardSpriteY)
{
    m_CardBtn = Button::create();
    int border = wight/40;
    Size s=Size(wight-border*2,height-border*2);
    int fsize=this->GetStringFontSize(m_DispStr, s.width);
    m_CardBtn->setTitleFontName("Consolas");
    m_CardBtn->setTitleFontSize(fsize);
    m_CardBtn->setTitleText(m_DispStr);
    m_CardBtn->setPosition(Vec2(colorBackground->getContentSize().width/2, colorBackground->getContentSize().height/2));
    this->addChild(m_CardBtn,100);
    this->swapBuffer();
    colorBackground->setColor(Color3B(200,190,180));
}

