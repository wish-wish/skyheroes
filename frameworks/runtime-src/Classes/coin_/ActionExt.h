/********************************************************************************
* create£ºcclin
* date£º2017-1-16
* ver£º
* latest update£º
* function£º
* update log£º
* <author>  <time>           <version >   <desc>
*********************************************************************************/
#ifndef __Action__EXT__
#define __Action__EXT__

#include "2d/CCActionInterval.h"
#include "2d/CCSprite.h"

USING_NS_CC;

class TextureTo : public ActionInterval
{
public:
	static TextureTo* create(float duration, Node* sprite);

	virtual TextureTo* clone() const override;
	virtual TextureTo* reverse(void) const override;
	virtual void startWithTarget(Node *target) override;

	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	TextureTo() {};
	virtual ~TextureTo();

	bool initWithDuration(float duration, Node* sprite);

protected:
	Node* _to;
	Node* _from;
	GLProgramState* glprogramstate;
private:
	CC_DISALLOW_COPY_AND_ASSIGN(TextureTo);
};

class TextureToColor : public ActionInterval
{
public:
	static TextureToColor* create(float duration, Color4F color);

	virtual TextureToColor* clone() const override;
	virtual TextureToColor* reverse(void) const override;
	virtual void startWithTarget(Node *target) override;

	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	TextureToColor() {};
	virtual ~TextureToColor();

	bool initWithDuration(float duration, Color4F color);

protected:
	Color4F _to;
	Node* _from;
	GLProgramState* glprogramstate;
private:
	CC_DISALLOW_COPY_AND_ASSIGN(TextureToColor);
};

class ColorToTexture : public ActionInterval
{
public:
	static ColorToTexture* create(float duration, Color4F color);

	virtual ColorToTexture* clone() const override;
	virtual ColorToTexture* reverse(void) const override;
	virtual void startWithTarget(Node *target) override;

	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	ColorToTexture() {};
	virtual ~ColorToTexture();

	bool initWithDuration(float duration, Color4F color);

protected:
	Node* _to;
	Color4F _from;
	GLProgramState* glprogramstate;
private:
	CC_DISALLOW_COPY_AND_ASSIGN(ColorToTexture);
};

class ColorToColor : public ActionInterval
{
public:
	static ColorToColor* create(float duration, Color4F from,Color4F to);

	virtual ColorToColor* clone() const override;
	virtual ColorToColor* reverse(void) const override;
	virtual void startWithTarget(Node *target) override;

	virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
	ColorToColor() {};
	virtual ~ColorToColor();

	bool initWithDuration(float duration, Color4F from, Color4F to);

protected:
	Color4F _to;
	Color4F _from;
	GLProgramState* glprogramstate;
private:
	CC_DISALLOW_COPY_AND_ASSIGN(ColorToColor);
};

#endif