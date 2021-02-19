#pragma once

#include "cocos2d.h"
#include "CardLayer.h"
#include "CardData.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "ActionModule.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;
using namespace ui;
using namespace cocostudio;

class BaseScene : public Layer
{
public:
	virtual bool init();

	//触摸事件监听回调函数
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    static Scene* createTNScene(float t,Scene* scene);
protected:
    float beginX,beginY,endX,endY;
    
    int m_CardSize;
    int m_RowCount;
    int m_ColCount;
    CardButton *cardArr[maxRCount][maxRCount];

    //根据屏幕大小创建卡片
    void destoryCardSprite();
    void createCardSprite(Size size,Vec2 offset=Vec2::ZERO);
	//私有变了记录触摸开始于结束时的坐标
	
    virtual void onClick(Ref* ref,Widget::TouchEventType type)=0;
    virtual string getWordsName()=0;
    virtual int getRows()=0;
    virtual int getCols()=0;
};



