
#ifndef __sunmap__Style__
#define __sunmap__Style__

#include "cocos2d.h"

USING_NS_CC;

class Style
{
public:
    Style();
    Style(const Style& mStyle);
	Style(GLfloat lineWidth, Color4F linecolor, Color4F fillcolor);
    
    Style& operator=(const Style& mStyle);
public:
	Color4F linecolor;
	Color4F fillcolor;
    
    GLfloat lineWidth;
};

#endif /* defined(__sunmap__Style__) */
