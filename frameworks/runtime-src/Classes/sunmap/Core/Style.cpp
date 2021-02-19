
#include "Style.h"

Style::Style()
{
    lineWidth = 1.5;
    linecolor.r = 255;
    linecolor.g = linecolor.b = 0;
    linecolor.a = 128;
    
    fillcolor.r = 255;
    fillcolor.g = fillcolor.b = 0;
    fillcolor.a = 128;
    
}

Style::Style(const Style& mStyle)
{
    lineWidth = mStyle.lineWidth;
    linecolor = mStyle.linecolor;
    fillcolor = mStyle.fillcolor;
}

Style& Style::operator=(const Style& mStyle)
{
    if(this == &mStyle)
        return *this;
    
    lineWidth = mStyle.lineWidth;
    linecolor = mStyle.linecolor;
    fillcolor = mStyle.fillcolor;
    
    return *this;
}

Style::Style(GLfloat mlineWidth, Color4F mlinecolor, Color4F mfillcolor)
{
    lineWidth = mlineWidth;
    linecolor = mlinecolor;
    fillcolor = mfillcolor;
}
