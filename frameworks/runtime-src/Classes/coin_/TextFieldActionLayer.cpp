/******************************************************************************** 
 * create£ºcclin
 * date£º2014-11-5
 * ver£º
 * latest update£º
 * function£º
 * update log£º
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/

#include "TextFieldActionLayer.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

const unsigned int DefCharCount=16;

static Rect getRect(Node * node)
{
	Rect rc;
	rc.origin = node->getPosition();
	rc.size = node->getContentSize();
	rc.origin.x -= rc.size.width / 2;
	rc.origin.y -= rc.size.height / 2;
	return rc;
}

KeyboardLayer::KeyboardLayer()
: _trackNode(0)
{
    // Register Touch Event
    auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(KeyboardLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(KeyboardLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	this->setAnchorPoint(Vec2(0,0));
}

void KeyboardLayer::keyboardWillShow(IMEKeyboardNotificationInfo& info)
{
    CCLOG("KeyboardLayer:keyboardWillShowAt(origin:%f,%f, size:%f,%f)",
        info.end.origin.x, info.end.origin.y, info.end.size.width, info.end.size.height);

    if (! _trackNode)
    {
        return;
    }

    auto rectTracked = getRect(_trackNode);
    CCLOG("KeyboardLayer:trackingNodeAt(origin:%f,%f, size:%f,%f)",
        rectTracked.origin.x, rectTracked.origin.y, rectTracked.size.width, rectTracked.size.height);

    // if the keyboard area doesn't intersect with the tracking node area, nothing need to do.
    if (! rectTracked.intersectsRect(info.end))
    {
        return;
    }

    //assume keyboard at the bottom of screen, calculate the vertical adjustment.
    float adjustVert = info.end.getMaxY() - rectTracked.getMinY();
    CCLOG("KeyboardLayer:needAdjustVerticalPosition(%f)", adjustVert);

    //move all the children node of KeyboardNotificationLayer
    auto& children = getChildren();
    Node * node = 0;
    ssize_t count = children.size();
    Vec2 pos;
    for (int i = 0; i < count; ++i)
    {
        node = children.at(i);
        pos = node->getPosition();
        pos.y += adjustVert;
        node->setPosition(pos);
    }
}
// Layer function
bool KeyboardLayer::onTouchBegan(Touch  *touch, Event  *event)
{
    CCLOG("++++++++++++++++++++++++++++++++++++++++++++");
    _beginPos = touch->getLocation();    
    return true;
}

void KeyboardLayer::onTouchEnded(Touch  *touch, Event  *event)
{
    if (! _trackNode)
    {
        return;
    }
    
    auto endPos = touch->getLocation();    

    float delta = 5.0f;
    if (::abs(endPos.x - _beginPos.x) > delta
        || ::abs(endPos.y - _beginPos.y) > delta)
    {
        // not click
        _beginPos.x = _beginPos.y = -1;
        return;
    }

    // decide the trackNode is clicked.
    Rect rect;
    auto point = convertTouchToNodeSpaceAR(touch);	
    CCLOG("KeyboardLayer:clickedAt(%f,%f)", point.x, point.y);

    rect = getRect(_trackNode);
    CCLOG("KeyboardLayer:TrackNode at(origin:%f,%f, size:%f,%f)",
        rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

    this->onClickTrackNode(rect.containsPoint(point));
    CCLOG("----------------------------------");
}


void TextFieldLayer::onClickTrackNode(bool bClicked)
{
	auto pTextField = (TextFieldTTF*)_trackNode;
	if (bClicked)
	{
		// TextFieldTTFTest be clicked
		CCLOG("TextFieldLayer:TextFieldTTF attachWithIME");
		pTextField->attachWithIME();
	}
	else
	{
		// TextFieldTTFTest not be clicked
		CCLOG("TextFieldLayer:TextFieldTTF detachWithIME");
		pTextField->detachWithIME();
	}
}

void TextFieldLayer::onEnter()
{
	KeyboardLayer::onEnter();

	_charLimit = DefCharCount;

	_textFieldAction = RepeatForever::create(
		Sequence::create(
		FadeOut::create(0.25),
		FadeIn::create(0.25),
		nullptr
		));
	_textFieldAction->retain();
	_action = false;

	// add TextFieldTTF
	auto s = Director::getInstance()->getWinSize();

	_textField = TextFieldTTF::textFieldWithPlaceHolder("<click here for input>",_FontName,_FontSize);
	TTFConfig config=_textField->getTTFConfig();
	config.fontFilePath = _FontName;
	config.fontSize = _FontSize;	
	_textField->setTTFConfig(config);

	TextField* text=TextField::create("<click here for input>",_FontName,_FontSize);
	addChild(text);
	//_textField->setColorSpaceHolder(Color3B::WHITE);
	addChild(_textField);

	_textField->setDelegate(this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)    
	// on android, TextFieldTTF cannot auto adjust its position when soft-keyboard pop up
	// so we had to set a higher position
	_textField->setPosition(Vec2(s.width / 2, s.height/2 + 50));
#else
	_textField->setPosition(Vec2(s.width / 2, s.height / 2+15));
#endif
	text->setPosition(Vec2(s.width / 2, s.height / 2-45));

// 	testlabel=Text::create(_textField->getString(),_FontName,_FontSize);
// 	addChild(testlabel);	
// 	testlabel->setPosition(Vec2(s.width / 2, s.height / 2-85));

	_trackNode = _textField;
}

void TextFieldLayer::onExit()
{
	KeyboardLayer::onExit();
	_textFieldAction->release();
}

// TextFieldDelegate protocol
bool TextFieldLayer::onTextFieldAttachWithIME(TextFieldTTF * sender)
{
	if (! _action)
	{
		_textField->runAction(_textFieldAction);
		_action = true;
	}
	return false;
}

bool TextFieldLayer::onTextFieldDetachWithIME(TextFieldTTF * sender)
{
	if (_action)
	{
		_textField->stopAction(_textFieldAction);
		_textField->setOpacity(255);
		_action = false;
	}
	return false;
}

bool TextFieldLayer::onTextFieldInsertText(TextFieldTTF * sender, const char * text, size_t nLen)
{
	// if insert enter, treat as default to detach with ime
	if ('\n' == *text)
	{
		return false;
	}

	// if the textfield's char count more than _charLimit, doesn't insert text anymore.
	if (sender->getCharCount() >= _charLimit)
	{
		return true;
	}

	// create a insert text sprite and do some action
	//auto label = Label::createWithSystemFont(text, _FontName, _FontSize);
	//auto label = Label::createWithTTF(text, _FontName, _FontSize);
	auto label=Text::create(text,_FontName,_FontSize);
	this->addChild(label);	
	//label->setColor(_FontColor);

	// move the sprite from top to position
	auto endPos = sender->getPosition();
	if (sender->getCharCount())
	{
		endPos.x += sender->getContentSize().width / 2;
	}
	auto inputTextSize = label->getContentSize();
	Vec2 beginPos(endPos.x, Director::getInstance()->getWinSize().height - inputTextSize.height * 2); 

	float duration = 0.5;
	label->setPosition(beginPos);
	label->setScale(8);

	auto seq = Sequence::create(
		Spawn::create(
			MoveTo::create(duration, endPos),
			ScaleTo::create(duration, 1),
			FadeOut::create(duration),
			nullptr),
		CallFuncN::create(CC_CALLBACK_1(TextFieldLayer::callbackRemoveNodeWhenDidAction, this)),
		nullptr);
	label->runAction(seq);

	//testlabel->setText(_textField->getString());
	//testlabel->setContentSize(CCSizeMake(300,30));
	//testlabel->ignoreContentAdaptWithSize(false);
	return false;
}

bool TextFieldLayer::onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen)
{
	// create a delete text sprite and do some action
	//auto label = Label::createWithSystemFont(delText, _FontName, _FontSize);
	//auto label = Label::createWithTTF(delText, _FontName, _FontSize);
	auto label=Text::create(delText,_FontName,_FontSize);
	this->addChild(label);

	// move the sprite to fly out
	auto beginPos = sender->getPosition();
	auto textfieldSize = sender->getContentSize();
	auto labelSize = label->getContentSize();
	beginPos.x += (textfieldSize.width - labelSize.width) / 2.0f;

	auto winSize = Director::getInstance()->getWinSize();
	Vec2 endPos(- winSize.width / 4.0f, winSize.height * (0.5 + (float)rand() / (2.0f * RAND_MAX)));

	float duration = 1;
	float rotateDuration = 0.2f;
	int repeatTime = 5; 
	label->setPosition(beginPos);

	auto seq = Sequence::create(
		Spawn::create(
			MoveTo::create(duration, endPos),
			Repeat::create(RotateBy::create(rotateDuration, (rand()%2) ? 360 : -360),repeatTime),
			FadeOut::create(duration),
			nullptr
			),
		CallFuncN::create(CC_CALLBACK_1(TextFieldLayer::callbackRemoveNodeWhenDidAction, this)),
		nullptr
		);

	label->runAction(seq);

	return false;
}

bool TextFieldLayer::onDraw(TextFieldTTF * sender)
{
	return false;
}

void TextFieldLayer::callbackRemoveNodeWhenDidAction(Node * node)
{
	this->removeChild(node, true);
}

bool TextFieldLayer::onVisit(TextFieldTTF * sender,Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	return false;
}

bool TextFieldLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}
	//TODO:initilize
	_FontName ="fonts/FZDHTJW_1.ttf";
	_FontSize = 20*1.1f;
	return true;
}

void TextFieldLayer::setDefaultFontName(string name)
{
	_FontName = name;
}

void TextFieldLayer::setDefaultFontSize(unsigned int size)
{
	_FontSize = size*1.1f;
}

void TextFieldLayer::setDefaultColor(Color3B color)
{
	_FontColor = color;
}

void TextFieldLayer::setDefaultLength(unsigned int length)
{
	_charLimit = length;
}

TextFieldTTF* TextFieldLayer::getTextField()
{
	return _textField;
}
