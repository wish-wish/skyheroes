/*
//使用
auto alien = SpriteWithHue::create("res/coin/1fen.png");
alien->setPosition(Vec2((visibleSize.width + origin.x) * CCRANDOM_0_1(), (visibleSize.height + origin.y) * CCRANDOM_0_1()));
alien->setHue(2 * M_PI * CCRANDOM_0_1());
*/

#ifndef __SpriteWithHue__
#define __SpriteWithHue__

#include "cocos2d.h"

USING_NS_CC;

class SpriteWithHue;

typedef std::function<bool(SpriteWithHue*, int, Touch *, Event *)> CallBack;

class SpriteWithHue: public Sprite
{
public:
	SpriteWithHue();
	virtual ~SpriteWithHue();

    static SpriteWithHue* create(const std::string& filename);
    static SpriteWithHue* create(const std::string& filename, const Rect& rect);
    
    static SpriteWithHue* createWithTexture(Texture2D *texture);
    static SpriteWithHue* createWithTexture(Texture2D *texture, const Rect& rect, bool rotated=false);
    
    static SpriteWithHue* createWithSpriteFrame(SpriteFrame *spriteFrame);
    static SpriteWithHue* createWithSpriteFrameName(const std::string& spriteFrameName);
    
    float getHue();
    void setHue(float hue);

	bool isClick(Touch* touch);
	bool hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const;
	bool isEnable(){ return _enable; };
	
	void setEnable(bool is);
	
	bool isSelect(){ return _selected; };
	void setSelect(bool is);
	bool isDefineInherite(){ return _defineInherite; };
	void setDefineInherite(bool is){ _defineInherite = is; }

	void setCallBack(CallBack cb){ _callback = cb; };

	bool isInherite(Event* event);
	virtual bool onTouchBegan(Touch *touch, Event *aevent);
	virtual void onTouchMoved(Touch *touch, Event *aevent);
	virtual void onTouchEnded(Touch *touch, Event *aevent);
	virtual void onTouchCancelled(Touch *touch, Event *aevent);
    
CC_CONSTRUCTOR_ACCESS:
	bool _enable;
	bool _selected;
	bool _defineInherite;
	CallBack _callback;
	EventListenerTouchOneByOne* _touchListener;

	float _hue;
    GLfloat _mat[3][3];
    
    bool initWithTexture(Texture2D *texture);
    bool initWithTexture(Texture2D *texture, const Rect& rect);
    virtual bool initWithTexture(Texture2D *texture, const Rect &rect, bool rotated);
    virtual bool initWithSpriteFrame(SpriteFrame *spriteFrame);
    
    void setupDefaultSettings();
    void initShader();
    const GLchar* shaderBody();
    virtual void updateColor();
    void updateColorMatrix();
    void updateAlpha();
    GLfloat getAlpha();
    
    void hueUniformCallback(GLProgram *p, Uniform *u);
private:
	bool old_selected;
};

class MenuItemHue :public MenuItemImage
{
public:
	static MenuItemHue* create();
	bool initWithHueImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ccMenuCallback& callback);
};

class CoinHue :public MenuItemHue
{
public:
	static CoinHue* create(std::string coinfile);
	SpriteWithHue* getSprite(){ return m_hue; };
	bool initWithHueImage(const std::string& normalImage, const std::string& selectedImage, const std::string& disabledImage, const ccMenuCallback& callback);
private:
	void setCoinSprite(std::string coinfile);
	SpriteWithHue* m_hue;	
	std::string m_file;
};

#endif /* defined(__SpriteWithHue__) */
