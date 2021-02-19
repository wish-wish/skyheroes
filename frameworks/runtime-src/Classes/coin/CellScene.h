#pragma once

#include "cocos2d.h"
#include "CardLayer.h"
#include "CardData.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "cocostudio/Cocostudio.h"

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;
using namespace ui;
using namespace cocostudio;

#define MoveAniTime 0.4
#define ShowAniTime 0.4

typedef vector<CardSprite*> CardList;

enum UserDirection
{
    udLeft=0
    ,udRight
    ,udUp
    ,udDown
    ,udAny
    ,udUpDown
    ,udLeftRight
    ,udLeftUp
    ,udLeftDown
    ,udRightUp
    ,udRightDown
};

class CellSceneBase : public Scene
{
public:
    ~CellSceneBase();
	static Scene* createScene(string SceneName);
	virtual bool init();

	//触摸事件监听回调函数
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void changeRowCount(int count);
    bool hasEmptyCard();
    unsigned int getEmptyCards();
    unsigned int getMaxCardNumber();
protected:
    float beginX,beginY,endX,endY;
    
    int m_CardSize;
    int m_RCount;
    bool m_isDirty;
    bool m_isMove;
    bool m_isChange;
    //储存卡片类
    CardSprite *cardArr[maxRCount][maxRCount];
    //分数
    
    //显示分数控件
    Label *cardNumberTTF;
    //创建生成随机卡片
    
    Label *maxNumber;
    Label *timeNumber;
    Label *maxRecordNumber;
    Label *aboutText;
    
    CardList m_EmptyList;
    bool m_isAnimating;
    
    CardDispData* m_DispData;
    MergeList  m_MergeList;
    EventListenerTouchOneByOne* touchListener;
    CardGameData* m_GameData;
protected:
	//根据屏幕大小创建卡片
    void destoryCardSprite();
    void createCardSprite(Size size,Vec2 offset=Vec2::ZERO);
	Vec3 calcCardSize(Size size);
    void zeroArray();
	//私有变了记录触摸开始于结束时的坐标
	
	void doCheck();
    
    void doBackNumbers();
    void clearUseds(UserDirection ud);
    
    void doDisplayNumbers();
    
	//设置分数
	void updateScore();
    
    void removeCard(CardSprite* b);
    void swapCard(CardSprite* b);
    
    void boneAnimDone(Node* node);
protected:
    
    bool isMerge2048(CardSprite* a,CardSprite* b);
    
    bool isMerge12510(CardSprite* a,CardSprite* b);
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c);
    
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d);
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e);
    
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f);
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g);
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h);
    bool isMerge12510(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h,CardSprite* i);
    
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h,CardSprite* i);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g,CardSprite* h);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f,CardSprite* g);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e,CardSprite* f);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d,CardSprite* e);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c,CardSprite* d);
    void clearUsed(CardSprite* a,CardSprite* b,CardSprite* c);
    void clearUsed(CardSprite* a,CardSprite* b);
    
    virtual bool CanLeft();
    virtual bool CanRight();
    virtual bool CanUp();
    virtual bool CanDown();
    
    void logic();
    
    CardSprite* createCardNumber(int num=0);
    
    void saveStepData(CardDispData* data);
    void showCardsNumber(CardDispData* data);
    
    void writeHistoryData();
    
    void RenderFunc(float delta);
    
    RepeatForever* HingAnim();
protected:
    void Logic();
    void doAnimations();
    
    bool MMLeft();
    bool MMRight();
    bool MMUp();
    bool MMDown();
    
protected:
    void onNext(Ref* ref,Widget::TouchEventType type);
    void onPrior(Ref* ref,Widget::TouchEventType type);
    void onShare(Ref* ref,Widget::TouchEventType type);
    void onReset(Ref* ref,Widget::TouchEventType type);
    void onEffectDone(Armature* armature,MovementEventType movementType,const std::string& movementID);
    
protected:
    virtual bool isMerge2(CardSprite* a,CardSprite* b)=0;
    virtual bool isMerge3(CardSprite* a,CardSprite* b,CardSprite* c)=0;
    
    virtual bool CanMerge2(CardSprite* a,CardSprite* b)=0;
    virtual bool CanMerge3(CardSprite* a,CardSprite* b,CardSprite* c)=0;
    
    virtual int RandomNumber()=0;
    virtual bool isNameTrans()=0;
    virtual string getNameList()=0;
    virtual string GetSceneName()=0;
    virtual int getMinNumber()=0;
	virtual int checkTerrainCode(UserDirection ud,int x,int y){return 0;};
    virtual int64_t AddScore(int num);
    virtual int64_t getGoal(int64_t num){return num*2;};
};


class SceneMoney:public CellSceneBase
{
public:
    CREATE_FUNC(SceneMoney);
protected:
    virtual bool isMerge2(CardSprite* a,CardSprite* );
    virtual bool isMerge3(CardSprite* a,CardSprite* b,CardSprite* c);
    
    virtual bool CanMerge2(CardSprite* a,CardSprite* b);
    virtual bool CanMerge3(CardSprite* a,CardSprite* b,CardSprite* c);
    
    virtual int RandomNumber();
    virtual bool isNameTrans();
    virtual string getNameList(){return "coin_renminbi";};
    virtual string GetSceneName(){return "SceneMoney";};
    virtual int getMinNumber(){return 1;};
};

class SceneMoneyInBa5:public CellSceneBase
{
public:
    CREATE_FUNC(SceneMoneyInBa5);
protected:
    virtual bool isMerge2(CardSprite* a,CardSprite* );
    virtual bool isMerge3(CardSprite* a,CardSprite* b,CardSprite* c);
    
    virtual bool CanMerge2(CardSprite* a,CardSprite* b);
    virtual bool CanMerge3(CardSprite* a,CardSprite* b,CardSprite* c);
    
    virtual int RandomNumber();
    virtual bool isNameTrans();
    virtual string getNameList(){return "coin_renminbi";};
    virtual string GetSceneName(){return "SceneMoneyInBa5";};
    
    virtual bool CanLeft();
    virtual bool CanRight();
    virtual bool CanUp();
    virtual bool CanDown();
    virtual int getMinNumber(){return 1;};
};

class SceneMoneyIb127 :public SceneMoneyInBa5
{
public:
	CREATE_FUNC(SceneMoneyIb127);
protected:
	virtual string getNameList(){ return "coin_127"; };
	virtual string GetSceneName(){ return "SceneMoneyIb127"; };
};

