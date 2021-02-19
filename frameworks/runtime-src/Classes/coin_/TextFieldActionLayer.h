/******************************************************************************** 
 * create£ºcclin
 * date£º2014-11-5
 * ver£º
 * latest update£º
 * function£º
 * update log£º
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/

#pragma once

#include "cocos2d.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;

extern const unsigned int DefCharCount;

class KeyboardLayer : public Layer, public IMEDelegate
{
public:
    KeyboardLayer();
    
    virtual void onClickTrackNode(bool bClicked) = 0;
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo& info);
    
    bool onTouchBegan(Touch  *touch, Event  *event);
    void onTouchEnded(Touch  *touch, Event  *event);

protected:
    Node * _trackNode;
    Vec2  _beginPos;
};


class TextFieldLayer : public KeyboardLayer, public TextFieldDelegate
{
private:
	TextFieldTTF *    _textField;
	Action *          _textFieldAction;
	bool              _action;
	int               _charLimit;       //the textfield max char limit
	string            _FontName;
	int				  _FontSize;
	Color3B           _FontColor;
public:
	CREATE_FUNC(TextFieldLayer);
	void setDefaultFontName(string name);
	void setDefaultFontSize(unsigned int size);
	void setDefaultColor(Color3B color);
	void setDefaultLength(unsigned int length);

	TextFieldTTF* getTextField();
private:
	virtual bool init();
	void callbackRemoveNodeWhenDidAction(Node * node);

	// KeyboardLayer	
	virtual void onClickTrackNode(bool bClicked);

	// Layer
	virtual void onEnter() override;
	virtual void onExit() override;

	// TextFieldDelegate
	virtual bool onTextFieldAttachWithIME(TextFieldTTF * sender);
	virtual bool onTextFieldDetachWithIME(TextFieldTTF * sender);
	virtual bool onTextFieldInsertText(TextFieldTTF * sender, const char * text, size_t nLen) override;
	virtual bool onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen) override;
	virtual bool onVisit(TextFieldTTF * sender,Renderer *renderer, const Mat4 &transform, uint32_t flags);
	virtual bool onDraw(TextFieldTTF * sender);
private:
	Text* testlabel;
	
};
