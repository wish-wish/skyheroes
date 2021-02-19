/******************************************************************************** 
 * create：cclin
 * date：2014-11-5
 * ver：
 * latest update：
 * function：
 * update log：
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#ifndef __Action__OvalInterval__
#define __Action__OvalInterval__
  
#include "2d/CCActionInterval.h"//包含系统延时类动作头文件
  
USING_NS_CC;


#define  OvalPI  3.1415926535
  
// 定义一个结构来包含确定椭圆的参数
typedef struct OvalConfig {
    //中心点坐标
    Vec2 centerPosition;
    //椭圆a长半轴
    float a;
    //椭圆b短半轴
    float b;
    //是否逆时针运动
    bool moveInAnticlockwise;
    //two zOrder
    std::pair<int, int> zOrder;
} lOvalConfig;  


//以椭圆方式移动  
class MoveOvalBy : public ActionInterval{
public:
    MoveOvalBy();
    //用“动作持续时间”和“椭圆控制参数”初始化动作
    bool initWithDuration(float t, const OvalConfig& c,float byAngle=OvalPI*2);
    virtual MoveOvalBy* clone() const override;
    virtual MoveOvalBy* reverse() const override;
    virtual void update(float t);//利用update函数来不断的设定坐标
    virtual void startWithTarget(Node *target) override;
public:
    //用“动作持续时间”和“椭圆控制参数”创建动作
    static MoveOvalBy *create(float t, const OvalConfig& c,float byAngle=OvalPI*2);
  
protected:
    OvalConfig _config;
	float _angle;
  
    //x = a * cos(t)  t = [0, 2Pi]
    inline float getPositionXAtOval(float t);//返回X坐标
    //y = b * sin(t)  t = [0, 2Pi]
    inline float getPositionYAtOval(float t);//返回Y坐标

	inline float getAngleAtPos(Vec2 pos);
private:
    CC_DISALLOW_COPY_AND_ASSIGN(MoveOvalBy);
};



class MoveOvalTo : public MoveOvalBy
{
public:
    static MoveOvalTo* create(float t, const OvalConfig& c,float ToAngle);


    virtual void startWithTarget(Node *target) override;
    virtual MoveOvalTo* clone() const override;
	virtual MoveOvalTo* reverse(void) const override;
    
CC_CONSTRUCTOR_ACCESS:
    MoveOvalTo() {}
    virtual ~MoveOvalTo() {}

    bool initWithDuration(float t, const OvalConfig &c,float ToAngle);
	
protected:
    float _ToAngle;	

private:
    CC_DISALLOW_COPY_AND_ASSIGN(MoveOvalTo);
};
  
// 定义一个结构来包含确定椭圆的参数  
typedef struct _lrTuoyuanConfig {  
    //中心点坐标  
    Vec2 centerPosition;  
    //椭圆a长，三角斜边  
    float aLength;  
    //椭圆c长，三角底边  
    float cLength;  
} lrTuoyuanConfig; 

class LRTuoyuanBy : public ActionInterval  
{  
public:  
    //用“动作持续时间”和“椭圆控制参数”初始化动作  
    bool initWithDuration(float t, const lrTuoyuanConfig& c);  
    virtual void update(float time);//利用update函数来不断的设定坐标  
public:  
    //用“动作持续时间”和“椭圆控制参数”创建动作  
    static LRTuoyuanBy *create(float t, const lrTuoyuanConfig& c);  
private:
	float tuoyuanXat(float a, float bx, float c, float t);
	float tuoyuanYat(float a, float by, float c, float t);

	virtual LRTuoyuanBy* reverse() const override;
	virtual LRTuoyuanBy* clone() const override;	
protected:  
    lrTuoyuanConfig m_sConfig;  
    Vec2 m_startPosition;  
    Vec2 s_startPosition;  
};

  
#endif
