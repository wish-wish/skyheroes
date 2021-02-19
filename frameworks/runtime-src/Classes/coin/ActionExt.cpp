/********************************************************************************
* create£ºcclin
* date£º2017-1-16
* ver£º
* latest update£º
* function£º
* update log£º
* <author>  <time>           <version >   <desc>
*********************************************************************************/
#include "ActionExt.h"
#include "renderer/CCGLProgram.h"
//#include "OvalActionInterval.h"

static GLProgram* program = nullptr;

void initProgram()
{
	program = GLProgram::createWithFilenames("res/shaders/CellMerge.vsh", "res/shaders/CellMerge.fsh");
	program->retain();
}

TextureTo* TextureTo::create(float duration, Node* sprite)
{
	TextureTo *textureTo = new (std::nothrow) TextureTo();
	textureTo->initWithDuration(duration, sprite);
	textureTo->autorelease();
	return textureTo;
}

TextureTo::~TextureTo() 
{
// 	glprogram->release();
// 	glprogramstate->release();
}


bool TextureTo::initWithDuration(float duration, Node* sprite)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_to = sprite;
		
		return true;
	}


	return false;
}

TextureTo* TextureTo::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) TextureTo();
	a->initWithDuration(_duration, _to);
	a->autorelease();
	return a;
}

TextureTo* TextureTo::reverse() const
{
	CCASSERT(false, "reverse() not supported in TextureTo");
	return nullptr;
}

void TextureTo::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
	if (_target)
	{
		_from = (Sprite*)_target;

		if (!program){ initProgram(); }
		glprogramstate = GLProgramState::create(program);

		_from->setGLProgramState(glprogramstate);

		glprogramstate->setUniformTexture("u_texture1", ((Sprite*)_to)->getTexture());
		glprogramstate->setUniformFloat("u_interpolate", 0.0);
		glprogramstate->setUniformInt("u_type", 0);
		glprogramstate->setUniformVec4("u_src_color", Vec4(0, 0, 0, 0));
		glprogramstate->setUniformVec4("u_dst_color", Vec4(0, 0, 0, 0));
	}
}

void TextureTo::update(float time)
{
	if (_target)
	{
		float interpolate = _elapsed / _duration;
		_target->getGLProgramState()->setUniformFloat("u_interpolate", interpolate);
	}
}

TextureToColor* TextureToColor::create(float duration, Color4F color)
{
	TextureToColor *textureTo = new (std::nothrow) TextureToColor();
	textureTo->initWithDuration(duration, color);
	textureTo->autorelease();
	return textureTo;
}

TextureToColor::~TextureToColor()
{
// 	glprogram->release();
// 	glprogramstate->release();
}


bool TextureToColor::initWithDuration(float duration, Color4F color)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_to = color;
		return true;
	}

	return false;
}

TextureToColor* TextureToColor::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) TextureToColor();
	a->initWithDuration(_duration, _to);
	a->autorelease();
	return a;
}

TextureToColor* TextureToColor::reverse() const
{
	CCASSERT(false, "reverse() not supported in TextureTo");
	return nullptr;
}

void TextureToColor::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
	if (_target)
	{
		_from = (Sprite*)_target;

		if (!program){ initProgram(); }
		glprogramstate = GLProgramState::create(program);

		_target->setGLProgramState(glprogramstate);

		glprogramstate->setUniformTexture("u_texture1", ((Sprite*)_target)->getTexture());
		glprogramstate->setUniformFloat("u_interpolate", 0.0);
		glprogramstate->setUniformInt("u_type", 1);
		glprogramstate->setUniformVec4("u_src_color",Vec4(_to.r, _to.g, _to.b, _to.a));
		glprogramstate->setUniformVec4("u_dst_color", Vec4(0, 0, 0, 0));
	}
}

void TextureToColor::update(float time)
{
	if (_target)
	{
		float interpolate = _elapsed / _duration;
		_target->getGLProgramState()->setUniformFloat("u_interpolate", interpolate);
	}
}

ColorToTexture* ColorToTexture::create(float duration, Color4F color)
{
	ColorToTexture *textureTo = new (std::nothrow) ColorToTexture();
	textureTo->initWithDuration(duration, color);
	textureTo->autorelease();
	return textureTo;
}

ColorToTexture::~ColorToTexture()
{
// 	glprogram->release();
// 	glprogramstate->release();
}

bool ColorToTexture::initWithDuration(float duration, Color4F color)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_from = color;
		return true;
	}

	return false;
}

ColorToTexture* ColorToTexture::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) ColorToTexture();
	a->initWithDuration(_duration, _from);
	a->autorelease();
	return a;
}

ColorToTexture* ColorToTexture::reverse() const
{
	CCASSERT(false, "reverse() not supported in TextureTo");
	return nullptr;
}

void ColorToTexture::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
	if (_target)
	{
		_to = (Sprite*)_target;

		if (!program){ initProgram(); }
		glprogramstate = GLProgramState::create(program);

		_target->setGLProgramState(glprogramstate);

		glprogramstate->setUniformTexture("u_texture1", ((Sprite*)_target)->getTexture());
		glprogramstate->setUniformFloat("u_interpolate", 0.0);
		glprogramstate->setUniformInt("u_type", 2);
		glprogramstate->setUniformVec4("u_src_color", Vec4(0, 0, 0, 0));
		glprogramstate->setUniformVec4("u_dst_color", Vec4(_from.r, _from.g, _from.b, _from.a));
	}
}

void ColorToTexture::update(float time)
{
	if (_target)
	{
		float interpolate = _elapsed / _duration;
		_target->getGLProgramState()->setUniformFloat("u_interpolate", interpolate);
	}
}

ColorToColor* ColorToColor::create(float duration, Color4F from, Color4F to)
{
	ColorToColor *textureTo = new (std::nothrow) ColorToColor();
	textureTo->initWithDuration(duration, from,to);
	textureTo->autorelease();
	return textureTo;
}

ColorToColor::~ColorToColor()
{
// 	glprogram->release();
// 	glprogramstate->release();
}

bool ColorToColor::initWithDuration(float duration, Color4F from, Color4F to)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_from = from;
		_to = to;
		return true;
	}

	return false;
}

ColorToColor* ColorToColor::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) ColorToColor();
	a->initWithDuration(_duration, _from, _to);
	a->autorelease();
	return a;
}

ColorToColor* ColorToColor::reverse() const
{
	CCASSERT(false, "reverse() not supported in TextureTo");
	return nullptr;
}

void ColorToColor::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
	if (_target)
	{
		if (!program){ initProgram(); }
		glprogramstate = GLProgramState::create(program);

		_target->setGLProgramState(glprogramstate);

		glprogramstate->setUniformTexture("u_texture1", ((Sprite*)_target)->getTexture());
		glprogramstate->setUniformFloat("u_interpolate", 0.0);
		glprogramstate->setUniformInt("u_type", 3);
		glprogramstate->setUniformVec4("u_src_color", Vec4(_from.r, _from.g, _from.b, _from.a));
		glprogramstate->setUniformVec4("u_dst_color", Vec4(_to.r, _to.g, _to.b, _to.a));
	}
}

void ColorToColor::update(float time)
{
	if (_target)
	{
		float interpolate = _elapsed / _duration;
		_target->getGLProgramState()->setUniformFloat("u_interpolate", interpolate);
	}
}
