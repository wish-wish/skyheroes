#pragma once

#include "cocos2d.h"
#include "CardData.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;
using namespace std;

#define GridBrder 50
#define EffectZorder 9999

typedef vector<Vec2> VecList;

class CardSprite : public Layer
{
public:
	static CardSprite* createCardSprite(int number, int wight, int height, float CardSpriteX, float CardSpriteY, int aliasi = -1, bool isTrans = false, string Trans = "");
	virtual bool init();
	CREATE_FUNC(CardSprite);
    ~CardSprite();
	//获取数据
	int64_t getNumber();
	//设置数据
	void setNumber(int64_t num);
    
    int64_t getOldNumber();
    void setOldNumber(int64_t num);
    
    int64_t getDispNumber();
    void setDispNumber(int64_t num);
    
    VecList m_SrcPos;
    bool m_SrcUsed;
    
    bool m_DstUsed;
    Vec2 m_DstPos;
    
    Vec2 GetCardPostion();
    void swapBuffer(float livets=0.0,CallFuncN* done=nullptr);
    
    bool addSrcPos(int x,int y);
    void setGridPos(int x,int y);
    bool IsInSrcPos(int x,int y);
	int getIndex(int rowcount);
    
    int getGridPosX();
    int getGridPosY();
    float getPixelsPosX(){return m_PixelPosX;};
    float getPixelsPosY(){return m_PixelPosY;};
    
    static Color3B GetNumberColor(int64_t num);
    static Color3B GetBackColor(int64_t num);
    static int GetStringFontSize(string name,int cell_width);

	static int GetStringFontSizeExt(string name, int cell_width);

    LayerColor* getBackground();
    Label* getLabel(){return labelCardNumber;};
    Sprite*  getSprite(){return m_Sprite;};
    Rect& getRect(){return m_Rect;};


    void dispPicture(TransNumber* number);
    void hidePicture();
	Sprite* createPicture(TransNumber* number,float *scale);
	float adapterCoord(Sprite* sprite);
	bool isPicSprite(TransNumber* number);
	void cardBlendActionFade(CardSprite* destcard,float delta=0.2);
	void removeTemp();
	void removeSprite(Sprite* b);
	void cardBlendActionShader(CardSprite* destcard, float delta = 0.2);

    void ShowHintAnim();
    void StopHintAnim();
    
    void clearTerrain();
    
    bool getIsTerrain(){return m_isTerrain;};
    bool getIsNumCard(){return !m_isTerrain&&this->getNumber()!=0;};
    void setIsTerrain(bool is){m_isTerrain=is;};
	int     m_AliasIndex;
	int     m_OldIndex;
protected:
	//显示的数字
	int64_t m_Number;
    int64_t OldNumber;
    int64_t DispNumber;
	
    bool m_isTerrain;
    
    int m_GridPosX;
    int m_GridPosY;
    int m_PixelPosX;
    int m_PixelPosY;
    
    int m_Width;
    int m_Height;
    
    bool m_IsTrans;
    bool m_IsBackColor;
    
    float m_SpriteScale;
    
    Sprite* m_Sprite;
    
    Rect m_Rect;
    
    string m_Trans;
    string m_DispStr;
    
    EventListenerTouchOneByOne* touchListener;
    
	//初始化
	void spriteInit(int64_t number, int wight, int height, float CardSpriteX, float CardSpriteY,bool isTrans,string Trans,int aliasi=-1);
	//显示数字的控件
	Label *labelCardNumber;
	Label *labelMax;
	//背景
	LayerColor *colorBackground;
    
    virtual void afterInit(int number, int wight, int height, float CardSpriteX, float CardSpriteY){};
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    void AddTouch();
    void RemoveToch();
};

class CardButton:public CardSprite
{
public:
    CREATE_FUNC(CardButton);
    
    virtual bool init();
    Button* getCardBtn();
    void setText(string txt);
    string getText();
    static CardButton* createCardSprite(int num, int wight, int height, float CardSpriteX, float CardSpriteY,bool isTrans=false,string Trans="");
private:
    Button* m_CardBtn;
    void afterInit(int64_t number, int wight, int height, float CardSpriteX, float CardSpriteY);
};


